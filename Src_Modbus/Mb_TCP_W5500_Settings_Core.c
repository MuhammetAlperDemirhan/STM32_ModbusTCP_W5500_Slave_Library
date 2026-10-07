/*
 * Mb_TCP_Init_Core.c
 *
 *  Created on: Jul 16, 2026
 *      Author: hanal
 */

#include "Mb_TCP_W5500_Settings_Core.h"
#include "Mb_TCP_USER_BOX.h"

static GPIO_TypeDef* CS_PIN_PORT;
static uint16_t CS_PIN_NUMBER;
static SPI_HandleTypeDef* SPI_Handle;


void CS_PIN_SET(void)
{
	HAL_GPIO_WritePin( CS_PIN_PORT, CS_PIN_NUMBER, GPIO_PIN_SET);
}


void CS_PIN_RESET(void)
{
	HAL_GPIO_WritePin( CS_PIN_PORT, CS_PIN_NUMBER, GPIO_PIN_RESET);
}


void W5500_INIT_CS_Settings(GPIO_TypeDef* CS_GPIO_PORT, uint16_t CS_GPIO_NUMBER)
{
	CS_PIN_PORT = CS_GPIO_PORT;
	CS_PIN_NUMBER = CS_GPIO_NUMBER;
	reg_wizchip_cs_cbfunc(CS_PIN_RESET, CS_PIN_SET);
}

//*********************************************************************************************************************************************************************************

uint8_t SPI_Data_Recive(void)
{

	uint8_t Tx_Idle = 0xFF;
	uint8_t Rx_Data = 0 ;

	if(HAL_SPI_TransmitReceive(SPI_Handle, &Tx_Idle, &Rx_Data, 1, HAL_MAX_DELAY) != HAL_OK)
	{
		HAL_SPI_Abort(SPI_Handle);
		Rx_Data = 0x00 ;
	}

	return Rx_Data ;
}

void SPI_Data_Transmit(uint8_t TX_Data)
{
	if(HAL_SPI_Transmit(SPI_Handle, &TX_Data, 1, HAL_MAX_DELAY) != HAL_OK)
	{
		HAL_SPI_Abort(SPI_Handle);
	}
}


void W5500_INIT_SPI_Settings(SPI_HandleTypeDef* SPI_PORT)
{
	SPI_Handle = SPI_PORT;
	reg_wizchip_spi_cbfunc(SPI_Data_Recive , SPI_Data_Transmit);
}

//*********************************************************************************************************************************************************************************


void SPI_DMA_DATA_RECIVE(uint8_t* pBUFFER , uint16_t LEN)
{
	HAL_SPI_Receive_DMA(SPI_Handle, pBUFFER, LEN);
	uint32_t Time_StarT= HAL_GetTick();
	uint16_t timeout = 50 ;
	while(HAL_SPI_GetState(SPI_Handle) != HAL_SPI_STATE_READY)
	{
		if((HAL_GetTick() - Time_StarT) >= timeout)
		{
			HAL_SPI_Abort(SPI_Handle);
			break;
		}
	}
}

void SPI_DMA_DATA_TRANSMIT(uint8_t* pBUFFER , uint16_t LEN)
{
	HAL_SPI_Transmit_DMA(SPI_Handle, pBUFFER, LEN);
	uint32_t Time_StarT= HAL_GetTick();
	uint16_t timeout = 50 ;
	while(HAL_SPI_GetState(SPI_Handle) != HAL_SPI_STATE_READY)
	{
		if((HAL_GetTick() - Time_StarT) >= timeout)
		{
			HAL_SPI_Abort(SPI_Handle);
			break;
		}
	}
}



void W5500_INIT_SPI_DMA_Settings(SPI_HandleTypeDef* SPI_PORT)
{
	SPI_Handle = SPI_PORT;
	reg_wizchip_spiburst_cbfunc(SPI_DMA_DATA_RECIVE, SPI_DMA_DATA_TRANSMIT);
}

//*********************************************************************************************************************************************************************************

uint8_t W5500_INIT_RxTx_Buffer_Settings(void)
{
	uint8_t bufSize[] = {2, 2, 2, 2 , 2 , 2 , 2 , 2};
	if(wizchip_init(bufSize, bufSize) == -1)
	{
		return 1 ; //ERROR
	}

	else
	{
		return 0 ;
	}

}

//*********************************************************************************************************************************************************************************

uint8_t W5500_INIT_TIMEOUT_Settings(uint16_t timeout_time, uint8_t timeout_retry)
{
	wiz_NetTimeout Write_nettime ;
	Write_nettime.time_100us = timeout_time ;
	Write_nettime.retry_cnt = timeout_retry;
	wizchip_settimeout(&Write_nettime);
	wiz_NetTimeout Read_nettime ;
	wizchip_gettimeout(&Read_nettime);
	if((Write_nettime.time_100us == Read_nettime.time_100us) && (Write_nettime.retry_cnt == Read_nettime.retry_cnt))
	{
		return 0 ;
	}
	else
	{
		return 1;  //ERROR
	}
}

//*********************************************************************************************************************************************************************************

uint8_t W5500_INIT_IP_Settings(uint8_t* Mac, uint8_t* Ip, uint8_t* Sn, uint8_t* Gw)
{
	wiz_NetInfo Device_Network;
	for(uint8_t a = 0  ; a < 6 ; a++)
	{
		Device_Network.mac[a] = Mac[a] ;
	}

	for(uint8_t i = 0 ; i<4 ; i++)
	{
		Device_Network.ip[i] = Ip[i];
		Device_Network.sn[i] = Sn[i];
		Device_Network.gw[i] = Gw[i];
	}
	Device_Network.dns[0] = 8;
	Device_Network.dns[1] = 8;
	Device_Network.dns[2] = 8;
	Device_Network.dns[3] = 8;
    Device_Network.dhcp = NETINFO_STATIC;


	wizchip_setnetinfo(&Device_Network);
	wiz_NetInfo Read_Network;
	wizchip_getnetinfo(&Read_Network);
	if(memcmp(Device_Network.mac , Read_Network.mac , 6) == 0 )
	{
		return 0 ;
	}

	else
	{
		return 1 ; //ERROR
	}

}













