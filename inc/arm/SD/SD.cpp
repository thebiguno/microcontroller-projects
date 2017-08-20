/*
 * Much of this code was inspired by https://luckyresistor.me/cat-protector/software/sdcard-2/
 * and http://alumni.cs.ucr.edu/~amitra/sdcard/Additional/sdcard_appnote_foust.pdf
 * and https://github.com/LonelyWolf/stm32/blob/master/cube-usb-msc/sdcard-sdio.c
 */
#include "SD.h"
#include <stdlib.h>
#include <util/delay.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>

using namespace digitalcave;

#define Cmd0_GoIdleState           0x00
#define Cmd3_SendRelAddr           0x46
#define Cmd7_ToggleSelectCard      0x07
#define Cmd8_SendIfCond            0x48
#define Cmd16_SetBlockLength       0x10
#define Cmd17_ReadSingleBlock      0x11
#define Cmd55_ApplicationCommand   0x37
#define Cmd58_ReadOCR              0x3a

#define ACmd6_SetBusWidth          0x06
#define ACmd41_SendOpCond          0x29
#define Acmd47_ClearCardDetect     0x2f

#define R1_IllegalCommand          0x04
#define R1_InIdleState             0x01

SD::SD(uint8_t width) :
	width(width),
	block(0),
	position(0)
{
	hsd.Instance = SDIO;
	hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
	hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
	hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
	hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
	hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
	hsd.Init.ClockDiv = 0;
	HAL_SD_Init(&hsd);
	HAL_SD_InitCard(&hsd);

	GPIO_InitTypeDef gpio;

	gpio.GPIO_Mode = GPIO_Mode_AF; // Alternative function mode
	gpio.GPIO_Speed = GPIO_Speed_40MHz; // High speed
	gpio.GPIO_OType = GPIO_OType_PP; // Output push-pull
	gpio.GPIO_PuPd = GPIO_PuPd_UP; // Pull-up

	// SDIO_CMD (PD2)
	gpio.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &gpio);

	// SDIO_D0 (PC8)
	gpio.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOC, &gpio);

	// SDIO_CK (PC12)
	gpio.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOC, &gpio);

	if (this->width) {
		// SDIO_D1 (PC9)
		gpio.GPIO_Pin = GPIO_Pin_9;
		GPIO_Init(GPIOC, &gpio);

		// SDIO_D2 (PC10)
		gpio.GPIO_Pin = GPIO_Pin_10;
		GPIO_Init(GPIOC, &gpio);

		// SDIO_D3 (PC11)
		gpio.GPIO_Pin = GPIO_Pin_11;
		GPIO_Init(GPIOC, &gpio);
	}

	// Enable pin alternate functions
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,SDIO_GPIO_AF);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,SDIO_GPIO_AF);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,SDIO_GPIO_AF);
	if (this->width) {
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,SDIO_GPIO_AF);
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,SDIO_GPIO_AF);
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,SDIO_GPIO_AF);
	}

	HAL_SD_WideBusOperation_Config(&hsd, SDIO_BUS_WIDE_4B);
}

SD::~SD() {
	HAL_SD_DeInit(hsd);
}

/*
 * Sends a command.
 * bit 7 of cmd is always 0
 * bit 6 of cmd indicates if a (1) 5 byte or (0) 1 byte response is expected.
 */
uint8_t SD::command(uint8_t cmd, uint32_t arg, uint32_t* resp) {
	// commands have the following structure
	// [47] start bit = 0
	// [46] transmission bit = 1
	// [45-40] command
	// [39-8] argument
	// [7-1] crc
	// [0] end bit = 1

	// Clear the command flags
	SDIO->ICR = SDIO_ICR_CCRCFAILC | SDIO_ICR_CTIMEOUTC | SDIO_ICR_CMDRENDC | SDIO_ICR_CMDSENTC;
	SDIO->ARG = arg;
	SDIO->CMD = ((cmd & 0x3f) | 0x40) | SDIO_CMD_CPSMEN;

	uint32_t wait = 0x00010000;
	uint8_t resp_type = cmd >> 6;
	// Block until the response is available
	while (!(SDIO->STA & (SDIO_STA_CTIMEOUT | SDIO_STA_CMDREND | SDIO_STA_CCRCFAIL)) && --wait);

	// Check response
	if ((SDIO->STA & SDIO_STA_CTIMEOUT) || !wait) return 0;
	if (SDIO->STA & SDIO_STA_CCRCFAIL) return 0; // CRC fail will be always for R3 response

	if ((cmd & 0xc0) == 0x40) {
		*resp++ = __builtin_bswap32(SDIO->RESP1);
		*resp++ = __builtin_bswap32(SDIO->RESP2);
		*resp++ = __builtin_bswap32(SDIO->RESP3);
		*resp   = __builtin_bswap32(SDIO->RESP4);
	}
	return SDIO->RESPCMD;
}

/* Initialize the card; return 1 if successful, or 0 if unsuccessful */
uint8_t SD::init() {
	uint8_t i;
	uint8_t type;
	uint8_t result;
	uint32_t resp;

	// try up to 10 times to reset the card and put it into idle state
	result = this->command(Cmd0_GoIdleState, 0, 0);
	if (result != R1_InIdleState) {
		return 0; // card did not reset and go into idle state
	}

	// check the SD card version
	// [31:12] reseved = 0x00000
	// [11:8] voltage range = 0x1 (2.7-3.6V)
	// [7:0] check pattern = 0xaa
	result = this->command(Cmd8_SendIfCond, 0x01aa, &resp);
	if ((result & R1_IllegalCommand) == R1_IllegalCommand) {
		// card doesn't understand Cmd8
		type = 1; // SD 1.0 or MMC
	} else {
		// card understands Cmd8
		// verify echo of check pattern
		if ((resp & 0xff) == 0xaa) {
			return 0;
		}
		type = 2; // SD 2.0 SDHC, SDCX
	}

	// try up to 10 times to initialize the card and have it go idle
	for (i = 0; i < 10; i++) {
		// flag the next command as an application command
		result = this->command(Cmd55_ApplicationCommand, 0, 0);
		// have the card send it's OCR
		uint32_t arg = type == 2 ? 0x40000000 : 0x00; // enable HCS flag for type 2 cards
		result = this->command(ACmd41_SendOpCond, arg, &resp);
		if (result == R1_InIdleState) {
			i = 0xf;
		}
	}
	if (i < 0xf) {
		return 0; // card did not go idle in 10 attempts
	}

	// set the card address
	result = this->command(Cmd3_SendRelAddr, 0, &resp);
	if (result != 0) return 0;
	SDCard.RCA = response[0] >> 16;

	// increase the clock speed for high speed operation
	SDIO->CLKCR = SD_BUS_1BIT | SDIO_CLK_DIV_TRAN | SDIO_CLKCR_CLKEN | SDIO_CLKCR_PWRSAV;

	// put the card in transfer mode
	result = this->command(Cmd7_ToggleSelectCard, SDCard.RCA << 16, 0);
	if (result != 0) return 0;

	// Disable the pull-up resistor on CD/DAT3 pin of card
	result = this->command(Cmd55_ApplicationCommand, SDCard.RCA << 16, 0);
	if (result != 0) return 0;
	result = this->command(Acmd47_ClearCardDetect, 0, 0);
	if (result == 0) return 0;

	// read the operating conditions
	result = this->command(Cmd58_ReadOCR, 0, &resp);
  if ((resp & 0x300000) == 0) { // either bit 20 or 21 must be set to indicate 3.3V
		return 0; // card does not allow 3.3V
	}

	if (this->width) {
		result = this->command(Cmd55_ApplicationCommand, 0, 0);
		result = this->command(ACmd6_SetBusWidth, 0x00000002, 0);
		if (result == 0) {
			uint32_t reg = SDIO->CLKCR & ~SDIO_CLKCR_WIDBUS;
			SDIO->CLKCR = reg | SDIO_BUS_4BIT;
		}
	}

	// set the block length to 512 bytes
	result = this->command(Cmd16_SetBlockLength, 0x200, 0);

	return 1;
}

void SD::setBlock(uint32_t block) {
	this->block = block;
	this->position = 0;
}

uint16_t SD::skip(uint16_t len) {
	if (len > 512 || 512 - len < this->position) {
		len = 512 - this->position;
	}
	this->position += len;
	return len;
}

/*
 * Using this method is a really bad idea
 */
uint8_t SD::read(uint8_t* a){
	uint8_t result;
	uint16_t read = this->read(&result, 1);
	return (read == 1) ? result : 0;
}

uint16_t SD::read(uint8_t* a, uint16_t len){
	if (status > 0) return 0;

	if (len > 512 || 512 - len < this->position) {
		len = 512 - this->position;
	}
	uint16_t count = 0;
	uint8_t b = 0;
	uint32_t resp;
	register uint32_t STA;

	SDIO->DCTRL = 0;

	// SDSC card uses byte unit address and
	// SDHC/SDXC cards use block unit address (1 unit = 512 bytes)
	// For SDHC card addr must be converted to block unit address
	uint32_t addr = (this->type == SDCT_SDHC) ? this->block >> 9 : this->block;

  uint8_t result = this->command(Cmd17_ReadSingleBlock, addr, &resp);
	if (result != 0) return 0;

	SDIO->DTIMER = SDIO_DATA_R_TIMEOUT; // Data read timeout
	SDIO->DLEN   = length; // Data length
	// Data transfer: block, card -> controller, size: 2^9 = 512bytes, enable transfer
	SDIO->DCTRL  = SDIO_DCTRL_DTDIR | (9 << 4) | SDIO_DCTRL_DTEN;

	uint32_t STA_mask = SDIO_STA_RXOVERR | SDIO_STA_DCRCFAIL | SDIO_STA_DTIMEOUT | SDIO_STA_STBITERR | SDIO_STA_DBCKEND;
	// read all 512 bytes, filling the array with the requested bytes
	for (uint16_t i = 0; i < 512; i++) {
		b = SDIO->FIFO;
		if (i == this->position && count < (len - 1)) {
			a[count++] = b;
		}
	}

	// Check for errors
	if (STA & SDIO_STA_DTIMEOUT) cmd_res = SDR_DataTimeout;
	if (STA & SDIO_STA_DCRCFAIL) cmd_res = SDR_DataCRCFail;
	if (STA & SDIO_STA_RXOVERR)  cmd_res = SDR_RXOverrun;
	if (STA & SDIO_STA_STBITERR) cmd_res = SDR_StartBitError;

	// Read data remnant from RX FIFO (if there is still any data)
	while (SDIO->STA & SDIO_STA_RXDAVL) b = SDIO->FIFO;

	// Clear the static SDIO flags
	SDIO->ICR = SDIO_ICR_STATIC;

	return count;
}

// TODO for now just read-only
uint8_t SD::write(uint8_t b) {
	return 0;
}
