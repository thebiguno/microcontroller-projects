#include "SerialHAL.h"
#include <dcutil/delay.h>

using namespace digitalcave;

SerialHAL* SerialHAL::instances[SERIAL_HAL_MAX_INSTANCES] = {NULL};

SerialHAL::SerialHAL(UART_HandleTypeDef* huart, uint8_t bufferSize):
	rxBuffer(bufferSize),
	huart(huart)
{
	for (uint8_t i = 0; i < SERIAL_HAL_MAX_INSTANCES; i++){
		if (instances[i] == NULL){
			instances[i] = this;
			break;
		}
	}

	HAL_UART_Receive_IT(huart, &incomingByte, 1);	//Start listening
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
	HAL_UART_Transmit(huart, &b, 1, 1);
	HAL_UART_Receive_IT(huart, &incomingByte, 1);	//Restart listening in case something bad happened...
	return 1;
}

void SerialHAL::isr(){
	//HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_1);
	rxBuffer.write(incomingByte);
	HAL_UART_Receive_IT(huart, &incomingByte, 1);
}

void SerialHAL::error(){
	// HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_2);
	// uint8_t error = huart->ErrorCode;
	// if (error == HAL_UART_ERROR_PE) HAL_UART_Transmit(huart, (uint8_t*) "Error PE; ", 10, 1);
	// if (error == HAL_UART_ERROR_NE) HAL_UART_Transmit(huart, (uint8_t*) "Error NE; ", 10, 1);
	// if (error == HAL_UART_ERROR_FE) HAL_UART_Transmit(huart, (uint8_t*) "Error FE; ", 10, 1);
	// if (error == HAL_UART_ERROR_ORE) HAL_UART_Transmit(huart, (uint8_t*) "Error ORE; ", 11, 1);
	// if (error == HAL_UART_ERROR_DMA) HAL_UART_Transmit(huart, (uint8_t*) "Error DMA; ", 11, 1);
	// if (error == HAL_UART_ERROR_NONE) HAL_UART_Transmit(huart, (uint8_t*) "No Error; ", 10, 1);
	//
	// uint8_t rxState = huart->RxState;
	// if (rxState == HAL_UART_STATE_RESET) HAL_UART_Transmit(huart, (uint8_t*) "State Reset; ", 13, 1);
	// if (rxState == HAL_UART_STATE_READY) HAL_UART_Transmit(huart, (uint8_t*) "State Ready; ", 13, 1);
	// if (rxState == HAL_UART_STATE_BUSY) HAL_UART_Transmit(huart, (uint8_t*) "State Busy; ", 12, 1);
	// if (rxState == HAL_UART_STATE_BUSY_TX) HAL_UART_Transmit(huart, (uint8_t*) "State Busy TX; ", 15, 1);
	// if (rxState == HAL_UART_STATE_BUSY_RX) HAL_UART_Transmit(huart, (uint8_t*) "State Busy RX; ", 15, 1);
	// if (rxState == HAL_UART_STATE_BUSY_TX_RX) HAL_UART_Transmit(huart, (uint8_t*) "State Busy TX RX; ", 18, 1);
	// if (rxState == HAL_UART_STATE_TIMEOUT) HAL_UART_Transmit(huart, (uint8_t*) "State Timeout; ", 15, 1);
	// if (rxState == HAL_UART_STATE_ERROR) HAL_UART_Transmit(huart, (uint8_t*) "State Error; ", 13, 1);
	//
	// //See https://my.st.com/public/STe2ecommunities/mcu/Lists/cortex_mx_stm32/Flat.aspx?RootFolder=https%3a%2f%2fmy%2est%2ecom%2fpublic%2fSTe2ecommunities%2fmcu%2fLists%2fcortex_mx_stm32%2fUART%20framing%20error%20on%20STM32F0%20device&FolderCTID=0x01200200770978C69A1141439FE559EB459D7580009C4E14902C3CDE46A77F0FFD06506F5B for instructions on what I did here
	// __HAL_UART_CLEAR_OREFLAG(huart);
	// __HAL_UART_CLEAR_NEFLAG(huart);
	// __HAL_UART_CLEAR_FEFLAG(huart);
	//
	// __HAL_UART_DISABLE_IT(huart, UART_IT_ERR);

	HAL_UART_Receive_IT(huart, &incomingByte, 1);
}


//Delegate rx events to the correct serial instance
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	for (uint8_t i = 0; i < SERIAL_HAL_MAX_INSTANCES; i++){
		if (SerialHAL::instances[i] == NULL){
			return;
		}
		else if (SerialHAL::instances[i]->getHandleTypeDef() == huart){
			SerialHAL::instances[i]->isr();
		}
	}
}

//Delegate error events to the correct serial instance
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart){
	for (uint8_t i = 0; i < SERIAL_HAL_MAX_INSTANCES; i++){
		if (SerialHAL::instances[i] == NULL){
			return;
		}
		else if (SerialHAL::instances[i]->getHandleTypeDef() == huart){
			SerialHAL::instances[i]->error();
		}
	}
}
