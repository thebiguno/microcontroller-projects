#include "Matrix.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_dma_ex.h"

using namespace digitalcave;

Matrix::Matrix() {
}

Matrix::~Matrix() {
}

void Matrix::setColor(Rgb rgb) {
	setColor(rgb.getRed(), rgb.getGreen(), rgb.getBlue());
}

void Matrix::setColor(uint8_t r, uint8_t g, uint8_t b) {
    // use only the top nibble
	uint16_t red = (r & 0xf0) >> 4;
	uint8_t green = (b & 0xf0) >> 4;
	uint8_t blue = (b & 0xf0) >> 4;

	color = (red << 8) | (green << 4) | blue
}

void Matrix::setPixel(int16_t x, int16_t y) {
	int16_t i = (x & 0x01) ? (x * MATRIX_WIDTH + MATRIX_HEIGHT - 1 - y) : (x * MATRIX_HEIGHT + y);
	//int16_t i = (x * 12) + y;
	if (i >= MATRIX_WIDTH * MATRIX_HEIGHT || i < 0) return;

    uint16_t current = buffer[i];

	uint8_t overlay = getOverlay();
	if (overlay == DRAW_OVERLAY_REPLACE){
		buffer[i] = color;
	}
	else if (overlay == DRAW_OVERLAY_OR){
		buffer[i] |= color;
	}
	else if (overlay == DRAW_OVERLAY_NAND){
		buffer[i] &= ~color;
	}
	else if (overlay == DRAW_OVERLAY_XOR){
		buffer[i] ^= color;
	}

	if (buffer[i] != current) changed = 1;
}

void Matrix::flush(){
	if (changed) {
		buffer_to_bam();
		changed = 0;
	}
}

uint8_t time_index_to_bam_index(uint8_t time_index) {
    if (time_index == 0) return 0;       //         0 = 1
    else if (time_index < 3) return 1;   //   1 -   2 = 2
    else if (time_index < 7) return 2;   //   3 -   6 = 4
    else if (time_index < 15) return 3;  //   7 -  14 = 8
    else if (time_index < 31) return 4;  //  15 -  30 = 16
    else if (time_index < 63) return 5;  //  31 -  62 = 32
    else if (time_index < 127) return 6; //  64 - 126 = 64
    else return 7;                       // 127 - 255 = 128
}

void buffer_to_bam() {
    for (uint16_t x = 0; x < MATRIX_WIDTH + 1; x++) {
        for (uint16_t y = 0; y < MATRIX_HEIGHT / 2; y++) {
            if (x == MATRIX_WIDTH) {
                // add a latch at the end of every row
                for (uint8_t t = 0; t < 16; i++) {
                    bam[x*y*t] = _bv(ST);
                }
            } else {
                uint16_t c0 = buffer[x*y];
                uint16_t c1 = buffer[x*y*2];
                uint8_t r0 = gamma[(c0 >> 8) & 0xf];
                uint8_t g0 = gamma[(c0 >> 4) & 0xf];
                uint8_t b0 = gamma[(c0 >> 0) & 0xf];
                uint8_t r1 = gamma[(c1 >> 8) & 0xf];
                uint8_t g1 = gamma[(c1 >> 4) & 0xf];
                uint8_t b1 = gamma[(c1 >> 0) & 0xf];

                for (uint8_t t = 0; t < 16; i++) {
                    uint8_t bvt = _bv(t);

                    bam[x*y*t] = x              // x == abcd
                    | ((bvt & r0) v) << R0
                    | ((bvt & g0) v} << G0
                    | ((bvt & b0) v} << B0
                    | ((bvt & r1) v} << R1
                    | ((bvt & g1) v} << G1
                    | ((bvt & b1) v} << B1;
                }
            }
        }
    }
}

void initHardware() {
    // Timer 8 is used to drive DMA Str

    GPIO_InitTypeDef GPIO_InitStruct;
    TIM_HandleTypeDef htim4;
    DMA_HandleTypeDef hdma_gpioc;

    // Prepare GPIOC
    __HAL_RCC_GPIOC_CLK_ENABLE();
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_All, GPIO_PIN_RESET);

    // Configure GPIOC to output push/pull open drain
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    GPIO_InitStruct.Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // Configure Timer 4 to drive DMA and GPIOC9
    __HAL_RCC_TIM8_CLK_ENABLE();
    htim4.Instance = TIM4;
    htim4.Init.Prescaler = 0;
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = 9;
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim4.Init.RepetitionCounter = 0;
    HAL_TIM_Base_Init(&htim4)

    // Configure GPIOC pin 9 as a clock source
    // this is so we don't have to calculate the clock source in the BAM array construction
    GPIO_InitStruct.Pin = GPIO_Pin_9;
    GPIO_InitStruct.Mode = GPIO_Mode_AF;
    GPIO_InitStruct.Pull = GPIO_PUSH_PULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF_TIM3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // Configure DMA Stream 1 Channel 7 to transfer from BAM buffer to GPIOC
    DMA_HandleTypeDef *hdma_gpioc;
    hdma_gpioc.Instance = DMA1_Stream2;
    hdma_gpioc.Init.Channel = DMA_Channel_4;
    hdma_gpioc.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_gpioc.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_gpioc.Init.MemInc = DMA_MINC_ENABLE;
    hdma_gpioc.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_gpioc.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_gpioc.Init.Mode = DMA_CIRCULAR;
    hdma_gpioc.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_gpioc.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_gpioc.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_gpioc.Init.PeriphBurst = DMA_PBURST_SINGLE;
    hdma_gpioc.Init.Priority = DMA_PRIORITY_HIGH;
    HAL_DMA_Init(&hdma_gpioc);
    __HAL_DMA2_REMAP(HAL_DMA1_CH4_TIM3_CH1);
    __HAL_LINKDMA(&htim4, hdma[TIM_DMA_ID_UPDATE], hdma_gpioc);
    HAL_NVIC_SetPriority(DMA1_Ch7_3_DMA2_Ch7_2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Ch7_3_DMA2_Ch1_2_IRQn);

    // Configure DMA callbacks
    htim->hdma[TIM_DMA_ID_UPDATE]->XferCpltCallback = data_transmitted_handler_0;
    htim->hdma[TIM_DMA_ID_UPDATE]->XferCpltM1Callback = data_transmitted_handler_1;
    htim->hdma[TIM_DMA_ID_UPDATE]->XferErrorCallback = transmit_error_handler;

    // Enable DMA
    HAL_DMAEx_MultiBufferStart_IT(&hdma_gpioc, (uint32_t) &dma[0], (uint32_t) &GPIOC->ODR, (uint32_t) &dma[BAM_SIZE], BAM_SIZE);

    // Start Timer
    __HAL_TIM_ENABLE(&htim8);
}

void data_transmitted_handler_0(UART_HandleTypeDef *handle) {
    uint8_t i = time_index_to_bam_index(time_index++);
    HAL_DMAEx_ChangeMemory(&handle, (uint32_t) &dma[BAM_SIZE * i], MEMORY0);
}

void data_transmitted_handler_1(UART_HandleTypeDef *handle) {
    uint8_t i = time_index_to_bam_index(time_index++);
    HAL_DMAEx_ChangeMemory(&handle, (uint32_t) &dma[BAM_SIZE * i], MEMORY1);
}

void transmit_error_handler(DMA_HandleTypeDef *hdma) {
    __HAL_TIM_DISABLE(&htim8);
}

