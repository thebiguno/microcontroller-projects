#include "ph42121rgb16s.h"
// see http://www.batsocks.co.uk/readme/art_bcm_3.htm for information on BAM
// see https://esdblog.org/how-to-drive-4096-rgb-leds-each-24bit-rgb-brightness-with-one-stm32-microcontroller-without-much-cpu-load/


// 8 levels BAM of [x:x:x:b1:g1:r1:ck:b0:g0:r0:oe:st:d:c:b:a]
// this maps directly onto GPIOC and is transferred using DMA
// ck is driven by the same timer as DMA and not part of the BAM codes
// two rows (top half, bottom half) are driven at a time
// there are 8 unique values spread across 256 time slices
uint16_t bam[BAM_SIZE * 8]; // 1056 * 16 = 8448 bytes
uint8_t time_index;         // there are 256 time slices
TIM_HandleTypeDef htim3;

// 2.2 gamma: round(((i/31)^2.2)*255)
// 32 gamma values since the buffer contains 5-bits RGB values
const uint8_t gamma[32] = {
      0,   0,   1,   2,   3,   5,   7,  10,
     13,  17,  21,  26,  32,  38,  44,  52,
     60,  68,  77,  87,  97, 108, 120, 132,
    145, 159, 173, 188, 204, 220, 237, 255
};

void ph42121rgb16s_flush(uint16_t* buffer) {
    for (uint16_t x = 0; x < MATRIX_WIDTH + 1; x++) {
        for (uint16_t y = 0; y < MATRIX_HEIGHT / 2; y++) {
            if (x == MATRIX_WIDTH) {
                // add a latch at the end of every row
                for (uint8_t t = 0; t < 8; t++) {
                    bam[x*y*t] = 0x0010;        // strobe
                }
            } else {
                uint16_t c0 = buffer[x*y];
                uint16_t c1 = buffer[x*y*2];
                uint8_t r0 = gamma[(c0 >> 10) & 0x1f];
                uint8_t g0 = gamma[(c0 >> 5) & 0x1f];
                uint8_t b0 = gamma[(c0 >> 0) & 0x1f];
                uint8_t r1 = gamma[(c1 >> 10) & 0x1f];
                uint8_t g1 = gamma[(c1 >> 5) & 0x1f];
                uint8_t b1 = gamma[(c1 >> 0) & 0x1f];

                for (uint8_t t = 0; t < 8; t++) {
                    uint8_t bvt = 1 << t;

                    // [x:x:x:b1:g1:r1:ck:b0:g0:r0:oe:st:d:c:b:a]
                    bam[x*y*t] = x             // x == abcd
                                               // strobe is 0
                    | 0x0020                   // output enable
                    | ((bvt & r0) >> t) << 6   // red 0
                    | ((bvt & g0) >> t) << 7   // green 0
                    | ((bvt & b0) >> t) << 8   // blue 0
                                               // clock is driven directly by the timer
                    | ((bvt & r1) >> t) << 10  // red 1
                    | ((bvt & g1) >> t) << 11  // green 1
                    | ((bvt & b1) >> t) << 12; // blue 1

                    // each bam time index maps to a bit in the color value
                    // bam[x*y*0] has bit 1 of the color
                    // bam[x*y*7] has bit 8 of the color
                }
            }
        }
    }
}

void ph42121rgb16s_init() {
    // Timer 3 is used to drive DMA Str

    GPIO_InitTypeDef GPIO_InitStruct;
    DMA_HandleTypeDef hdma_tim3_ch4_up;

    // Prepare GPIOC
    __HAL_RCC_GPIOC_CLK_ENABLE();
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_All, GPIO_PIN_RESET);

    // Configure GPIOC as output
    //RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // Configure Timer 3 to drive DMA and GPIOC9
    __HAL_RCC_TIM3_CLK_ENABLE();
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 0;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 0;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.RepetitionCounter = 0;
    HAL_TIM_Base_Init(&htim3);

    // Configure GPIOC pin 9 as output from tim3 clock source
    // this is so we don't have to calculate the clock source in the BAM array construction
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // Configure DMA Stream 2 Channel 4 to transfer from BAM buffer to GPIOC
    __HAL_RCC_DMA1_CLK_ENABLE();
    hdma_tim3_ch4_up.Instance = DMA1_Stream2;
    hdma_tim3_ch4_up.Init.Channel = DMA_CHANNEL_4;
    hdma_tim3_ch4_up.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_tim3_ch4_up.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim3_ch4_up.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim3_ch4_up.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_tim3_ch4_up.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_tim3_ch4_up.Init.Mode = DMA_CIRCULAR;
    hdma_tim3_ch4_up.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_tim3_ch4_up.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_tim3_ch4_up.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_tim3_ch4_up.Init.PeriphBurst = DMA_PBURST_SINGLE;
    hdma_tim3_ch4_up.Init.Priority = DMA_PRIORITY_HIGH;
    HAL_DMA_Init(&hdma_tim3_ch4_up);

    //__HAL_DMA2_REMAP(HAL_DMA1_CH4_TIM3_CH1);  // TODO is this needed, if so how is it done with the latest libs?
    __HAL_LINKDMA(&htim3, hdma[TIM_DMA_ID_UPDATE], hdma_tim3_ch4_up);
    HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);

    // Configure DMA callbacks
    htim3.hdma[TIM_DMA_ID_UPDATE]->XferCpltCallback = ph42121rgb16s_data_transmitted_handler_m0;
    htim3.hdma[TIM_DMA_ID_UPDATE]->XferM1CpltCallback = ph42121rgb16s_data_transmitted_handler_m1;
    htim3.hdma[TIM_DMA_ID_UPDATE]->XferErrorCallback = ph42121rgb16s_transmit_error_handler;

    // Enable DMA
    // transmit a single BAM, starting with time index 0 (bit 1); when the transfer is complete the callbacks
    // below will set up for the next time index
    HAL_DMAEx_MultiBufferStart_IT(&hdma_tim3_ch4_up, (uint32_t) &bam[0], (uint32_t) &GPIOC->ODR, (uint32_t) &bam[BAM_SIZE], BAM_SIZE);

    // Start Timer
    __HAL_TIM_ENABLE(&htim3);
}

uint8_t ph4212rgb16s_msb0(uint8_t b) {
    // msb0(0) = 0; msb0(1) = 0; msb0(8) = 3; msb0(255) = 7;
    uint8_t r = 0;
    while (b > 1) {
        b = b >> 1;
        r++;
    }
    return r;
}

void ph42121rgb16s_data_transmitted_handler_m0(DMA_HandleTypeDef* handle) {
    uint8_t i = ph4212rgb16s_msb0(time_index++);
    HAL_DMAEx_ChangeMemory(handle, (uint32_t) &bam[BAM_SIZE * i], MEMORY0);
}

void ph42121rgb16s_data_transmitted_handler_m1(DMA_HandleTypeDef* handle) {
    uint8_t i = ph4212rgb16s_msb0(time_index++);
    HAL_DMAEx_ChangeMemory(handle, (uint32_t) &bam[BAM_SIZE * i], MEMORY1);
}

void ph42121rgb16s_transmit_error_handler(DMA_HandleTypeDef* hdma) {
    __HAL_TIM_DISABLE(&htim3);
}
