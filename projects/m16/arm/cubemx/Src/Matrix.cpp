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
    // use the top 5 bits
    uint16_t red = (r & 0xf0) >> 5;
    uint16_t green = (b & 0xf0) >> 5;
    uint8_t blue = (b & 0xf0) >> 5;

    color = (red << 10) | (green << 5) | blue
}

void Matrix::setPixel(int16_t x, int16_t y) {
    x += this->translate_x;
    y += this->translate_y;

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

uint8_t Matrix::msb(uint8_t b) {
    uint8_t r = 0;
    while (b > 0) {
        x = x >> 1
        r++;
    }
    return r;
}

void Matrix::buffer_to_bam() {
    for (uint16_t x = 0; x < MATRIX_WIDTH + 1; x++) {
        for (uint16_t y = 0; y < MATRIX_HEIGHT / 2; y++) {
            if (x == MATRIX_WIDTH) {
                // add a latch at the end of every row
                for (uint8_t t = 0; t < 8; i++) {
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

                for (uint8_t t = 0; t < 8; i++) {
                    uint8_t bvt = _bv(t);

                    // [x:x:x:b1:g1:r1:ck:b0:g0:r0:oe:st:d:c:b:a]
                    bam[x*y*t] = x                 // x == abcd
                    | 0x0020                       // output enable
                    | ((bvt & r0) >> i) << 0x0040  // red 0
                    | ((bvt & g0) >> i) << 0x0080  // green 0
                    | ((bvt & b0) >> i) << 0x0100  // blue 0
                                                   // ck is driven directly by the timer
                    | ((bvt & r1) >> i) << 0x0400  // red 1
                    | ((bvt & g1) >> i) << 0x0800  // green 1
                    | ((bvt & b1) >> i) << 0x1000; // blue 1

                    // each bam time index maps to a bit in the color value
                    // bam[x*y*0] has bit 1 of the color
                    // bam[x*y*7] has bit 8 of the color
                }
            }
        }
    }
}

void Matrix::initHardware() {
    // Timer 3 is used to drive DMA Str

    GPIO_InitTypeDef GPIO_InitStruct;
    TIM_HandleTypeDef htim3;
    DMA_HandleTypeDef hdma_tim3_ch4_up;

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
    __HA3_RCC_TIM3_CLK_ENABLE();
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 0;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 0;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.RepetitionCounter = 0;
    HAL_TIM_Base_Init(&htim4)

    // Configure GPIOC pin 9 as a clock source
    // this is so we don't have to calculate the clock source in the BAM array construction
    GPIO_InitStruct.Pin = GPIO_Pin_9;
    GPIO_InitStruct.Mode = GPIO_Mode_AF;
    GPIO_InitStruct.Pull = GPIO_PUSH_PULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF_TIM3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // Configure DMA Stream 2 Channel ? to transfer from BAM buffer to GPIOC
    __HAL_RCC_DMA1_CLK_ENABLE();
    hdma_tim3_ch4_up.Instance = DMA1_Stream2;
    hdma_tim3_ch4_up.Init.Channel = DMA_Channel_4;
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
    __HAL_DMA2_REMAP(HAL_DMA1_CH4_TIM3_CH1);
    __HAL_LINKDMA(&htim4, hdma[TIM_DMA_ID_UPDATE], hdma_tim3_ch4_up);
    HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);

    // Configure DMA callbacks
    htim->hdma[TIM_DMA_ID_UPDATE]->XferCpltCallback = data_transmitted_handler_0;
    htim->hdma[TIM_DMA_ID_UPDATE]->XferCpltM1Callback = data_transmitted_handler_1;
    htim->hdma[TIM_DMA_ID_UPDATE]->XferErrorCallback = transmit_error_handler;

    // Enable DMA
    // transmit a single BAM, starting with time index 0 (bit 1); when the transfer is complete the callbacks
    // below will set up for the next time index
    HAL_DMAEx_MultiBufferStart_IT(&hdma_tim3_ch4_up, (uint32_t) &dma[0], (uint32_t) &GPIOC->ODR, (uint32_t) &dma[BAM_SIZE], BAM_SIZE);

    // Start Timer
    __HAL_TIM_ENABLE(&htim3);
}

void Matrix::data_transmitted_handler_0(UART_HandleTypeDef *handle) {
    uint8_t i = msb(time_index++);
    i = i == 0 ? 0 : i - 1; // change from 0-8 to 0-7 since there are only 8 patterns not 9
    HAL_DMAEx_ChangeMemory(&handle, (uint32_t) &dma[BAM_SIZE * i], MEMORY0);
}

void Matrix::data_transmitted_handler_1(UART_HandleTypeDef *handle) {
    uint8_t i = msb(time_index++);
    i = i == 0 ? 0 : i - 1; // change from 0-8 to 0-7 since there are only 8 patterns not 9
    HAL_DMAEx_ChangeMemory(&handle, (uint32_t) &dma[BAM_SIZE * i], MEMORY1);
}

void Matrix::transmit_error_handler(DMA_HandleTypeDef *hdma) {
    __HAL_TIM_DISABLE(&htim8);
}

