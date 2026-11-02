/*
 * Mb_TCP_Init_Core.h
 *
 *  Created on: Jul 23, 2026
 *      Author: hanal
 */

#ifndef INC_MB_TCP_W5500_SETTINGS_CORE_H_
#define INC_MB_TCP_W5500_SETTINGS_CORE_H_



#include "main.h"
#include "wizchip_conf.h"
#include "socket.h"
#include "W5500/w5500.h"
#include <string.h>



void CS_PIN_SET(void);
void CS_PIN_RESET(void);
void W5500_INIT_CS_Settings(GPIO_TypeDef* CS_GPIO_PORT, uint16_t CS_GPIO_NUMBER);

uint8_t SPI_Data_Recive(void);
void SPI_Data_Transmit(uint8_t TX_Data);
void W5500_INIT_SPI_Settings(SPI_HandleTypeDef* SPI_PORT);

void SPI_DMA_DATA_RECIVE(uint8_t* pBUFFER , uint16_t LEN);
void SPI_DMA_DATA_TRANSMIT(uint8_t* pBUFFER , uint16_t LEN);
void W5500_INIT_SPI_DMA_Settings(SPI_HandleTypeDef* SPI_PORT);
uint8_t W5500_INIT_RxTx_Buffer_Settings(void);
uint8_t W5500_INIT_TIMEOUT_Settings(uint16_t timeout_time, uint8_t timeout_retry);
uint8_t W5500_INIT_IP_Settings(uint8_t* Mac, uint8_t* Ip, uint8_t* Sn, uint8_t* Gw);
uint8_t W5500_Check_Health(void);

#endif /* INC_MB_TCP_W5500_SETTINGS_CORE_H_ */
