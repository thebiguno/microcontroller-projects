#ifndef RDA5807_h
#define RDA5807_h

#include <stdlib.h>
#include <dcutil/delay.h>
#include <I2C.h>

#define RDA5807_ADDRESS			0x11

#define MIN_STATION				870
#define MAX_STATION				1080

typedef struct {
	//Each byte is arranged in little endian ordering, at least in avr-gcc.  So, bit 0 of a byte comes first.  The byte ordering is in proper array-indexed order.
	uint8_t r0_chip_id : 8;
	uint8_t r0_unused1 : 8;

	uint8_t r1_unused1 : 8;
	uint8_t r1_unused2 : 8;

	uint8_t r2_seek : 1;
	uint8_t r2_seekup : 1;
	uint8_t r2_rclk_di : 1;
	uint8_t r2_rclk : 1;
	uint8_t r2_bass : 1;
	uint8_t r2_mono : 1;
	uint8_t r2_dmute : 1;
	uint8_t r2_dhiz : 1;

	uint8_t r2_enable : 1;
	uint8_t r2_soft_reset : 1;
	uint8_t r2_new_method : 1;
	uint8_t r2_rds_en : 1;
	uint8_t r2_clk_mode : 3;
	uint8_t r2_seek_mode : 1;

	//You need to be careful here; this 10 bit value is left shifted 6 bits.  This is the only place
	// in this struct where this is an issue; everywhere else the msb / lsb fields are right shifted.
	uint8_t r3_channel_msb : 8;

	uint8_t r3_space : 2;
	uint8_t r3_band : 2;
	uint8_t r3_tune : 1;
	uint8_t r3_direct : 1;
	uint8_t r3_channel_lsb : 2;

	uint8_t r4_afcd : 1;
	uint8_t r4_softmute_en : 1;
	uint8_t r4_rsvd3 : 1;
	uint8_t r4_de : 1;
	uint8_t r4_rsvd2 : 2;
	uint8_t r4_unused1 : 1;	//Not shown in the data sheet, but there is a gap in bit 14.
	uint8_t r4_rsvd1 : 1;

	uint8_t r4_unused2 : 8;

	uint8_t r5_seek_snr : 4;
	uint8_t r5_rsvd1 : 3;
	uint8_t r5_int_mode : 1;

	uint8_t r5_volume : 4;
	uint8_t r5_rsvd2 : 2;
	uint8_t r5_unused1 : 2;

	uint8_t r6_unused1 : 5;
	uint8_t r6_open_mode : 2;
	uint8_t r6_rsvd1 : 1;

	uint8_t r6_unused2 : 8;

	uint8_t r7_rsvd2 : 1;
	uint8_t r7_65m_50m_mode : 1;
	uint8_t r7_th_softblend : 5;
	uint8_t r7_rsvd1 : 1;

	uint8_t r7_freq_mode : 1;
	uint8_t r7_softblend_en : 1;
	uint8_t r7_seek_th_old : 6;

	uint8_t r8_unused1 : 8;
	uint8_t r8_unused2 : 8;

	uint8_t r9_unused1 : 8;
	uint8_t r9_unused2 : 8;

	uint8_t rA_read_chan_msb : 2;
	uint8_t rA_st : 1;
	uint8_t rA_blk_e : 1;
	uint8_t rA_rdss : 1;
	uint8_t rA_sf : 1;
	uint8_t rA_stc : 1;
	uint8_t rA_rdsr : 1;

	uint8_t rA_read_chan_lsb : 8;

	uint8_t rB_fm_true : 1;
	uint8_t rB_rssi : 7;

	uint8_t rB_blerb : 2;
	uint8_t rB_blera : 2;
	uint8_t rB_abcd_e : 1;
	uint8_t rB_rsvd1 : 2;
	uint8_t rB_fm_ready : 1;

	uint8_t rC_rdsa_msb : 8;
	uint8_t rC_rdsa_lsb : 8;

	uint8_t rD_rdsb_msb : 8;
	uint8_t rD_rdsb_lsb : 8;

	uint8_t rE_rdsc_msb : 8;
	uint8_t rE_rdsc_lsb : 8;

	uint8_t rF_rdsd_msb : 8;
	uint8_t rF_rdsd_lsb : 8;
} rda5807_register_fields_t;

typedef union {
	rda5807_register_fields_t fields;
	uint8_t data[32];
} rda5807_registers_t;

namespace digitalcave {

	class RDA5807 {
		private:
			I2C* i2c;

			rda5807_registers_t r;

			//Returns the 16 bit value at the specified register number from the register mapping.  Returns 0x0000 if the register number is invalid.
			uint16_t getRegister(uint8_t registerNumber);


			//Read / Write the specified register, using / updating the register values
			void readRegister(uint8_t registerNumber);
			void writeRegister(uint8_t registerNumber);

			void setRegister(uint8_t registerNumber, uint16_t value);

		public:
			rda5807_registers_t* getRaw();

			//Reads all registers into the struct
			void readRegisters();

			//Writes registers 2-7 from the struct into the chip
			void writeRegisters();

			uint8_t getChipId();

			//Inits the RDA5807 control object and sends the power up commands to the chip.
			RDA5807(I2C* i2c);

			//Gets / Sets the current station.  Station is specified in 100kHz; e.g. 102.1 is shown as 1021.
			uint16_t getStation();
			void setStation(uint16_t station);

			//Starts scanning.  Poll getStation() to get results.
			void doScan();

			//Gets / Sets the current volume, from 0x00 to 0x0F.
			uint8_t getVolume();
			void setVolume(uint8_t volume);

			//Gets / Sets hard mute
			uint8_t getMute();
			void setMute(uint8_t mute_on);
	};
}
#endif
