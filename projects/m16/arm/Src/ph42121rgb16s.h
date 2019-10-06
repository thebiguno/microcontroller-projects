#ifndef ph42121rgb16c_h
#define ph42121rgb16c_h

#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_rcc_ex.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_dma_ex.h"

#if defined (__cplusplus)
extern "C" {
#endif

#define MATRIX_WIDTH 64
#define MATRIX_HEIGHT 32
#define BAM_SIZE (MATRIX_WIDTH + 1) * (MATRIX_HEIGHT / 2)

/* setup the hardware */
void ph42121rgb16s_init();
/* translate a 15 bpp buffer into bam codes to be sent using dma */
void ph42121rgb16s_flush(uint16_t* buffer);
/* returns a value between 0 and 8 inclusive for the highest bit set, or 0 if no bit is set */
uint8_t ph42121rgb16s_msb(uint8_t b);

// callbacks
void ph42121rgb16s_data_transmitted_handler_m0(DMA_HandleTypeDef *handle);
void ph42121rgb16s_data_transmitted_handler_m1(DMA_HandleTypeDef *handle);
void ph42121rgb16s_transmit_error_handler(DMA_HandleTypeDef *hdma);

#if defined (__cplusplus)
}
#endif

#endif
