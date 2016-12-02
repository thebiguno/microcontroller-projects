#include "SerialHAL.h"
#include <dcutil/delay.h>

using namespace digitalcave;

SerialHAL::SerialHAL(UART_HandleTypeDef* huart, uint8_t bufferSize):
	rxBuffer(bufferSize),
	huart(huart)
{
	HAL_StatusTypeDef ret = HAL_UART_Receive_IT(huart, &incomingByte, 1);	//Start listening
	while (ret != HAL_OK) {
		delay_ms(100);
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_2);
	}
}

uint8_t SerialHAL::read(uint8_t *c){
	if (!rxBuffer.isEmpty()){
		if (rxBuffer.read(c)){
			return 1;
		}
	}
	return 0;
}

uint8_t SerialHAL::write(uint8_t b){
	HAL_UART_Transmit(huart, &b, 1, 10);
	return 1;
}

void SerialHAL::isr(){
	rxBuffer.write(incomingByte);
	HAL_StatusTypeDef ret = HAL_UART_Receive_IT(huart, &incomingByte, 1);	//Keep on listening
	while (ret != HAL_OK) {
		delay_ms(100);
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_2);
	}
}
