/*
 * Mb_TCP_USER_BOX.c
 *
 *  Created on: Jul 23, 2026
 *      Author: hanal
 */


#include "Mb_TCP_USER_BOX.h"
#include "Mb_TCP_Connectivity_Core.h"
#include "Mb_TCP_Function_Codes_Core.h"
#include "Mb_TCP_Register_BOX.h"

__attribute__((aligned(4))) static uint8_t MB_TCP_RX_Buffer[MB_TCP_BUFFER_SIZE] = {0};
__attribute__((aligned(4))) static uint8_t MB_TCP_TX_Buffer[MB_TCP_BUFFER_SIZE] = {0};
__attribute__((aligned(4))) static uint8_t MBAP[MB_TCP_MBAP_SIZE] = {0};
static uint16_t PDU_LEN = 0 ;
static uint8_t NEW_DATA_FLAG = 1 ;
static uint32_t PDU_MESSAGE_TIMEOUT = 0 ;
static uint8_t SOCKET_BUSY_RETURN_VALUE = 0 ;
static uint8_t SOCKET_BUSY_RETURN_VALUE_2 = 0 ;
uint8_t Reading_MAC_Address[6];
uint8_t Reading_IP_Address[4];
uint8_t Reading_Subnet_Address[4];
uint8_t Reading_Gateway_Address[4];
uint16_t Reading_TCP_Port;
static SPI_HandleTypeDef* Global_hspiX;
static GPIO_TypeDef* Global_CS_Port;
static uint16_t Global_CS_PIN;
static uint16_t Global_timeout_tiM;
static uint8_t Global_timeout_reT;
static GPIO_TypeDef* Global_W5500_Rst_GPIOx;
static uint16_t Global_W5500_Rst_PIN_x;
uint32_t MB_FlashStart_Address = 0 ;


/* ========================================================================== */
/*        Generates or assigns a user-specific custom MAC address.            */
/* ========================================================================== */

/**
 * @brief Generates MAC Address in Function.
 *
 * @note OUI & U/L Bit Requirement:
 *       According to IEEE 802 standards, if you do not possess a registered
 *       Organizationally Unique Identifier (OUI), your custom MAC address MUST be
 *       designated as "Locally Administered". This means the Universal/Local (U/L) bit
 *       must be set to 1. To fulfill this requirement, the Least Significant Nibble (LSN)
 *       of the first byte must be 2, 6, A, or E (e.g., 0x02, 0x06, 0x0A, or 0x0E).
 *       Please strictly observe this rule when generating your MAC address.
 *
 * @note Utilizing the STM32 UID:
 *       For the remaining 5 bytes of the MAC address, it is highly recommended to use
 *       the STM32 MCUs Unique Device ID (UID). The UID is a silicon-embedded
 *       factory-programmed 96-bit (12-byte) unique identification number. By implementing
 *       an algorithm (e.g., XOR hashing) on this UID, you can generate a robust,
 *       collision-free, and entirely unique local MAC address for each individual device.
 */
void USER_SPECIFIC_MAC_ADDR_ALGORITHM(uint8_t *Mac_ADDR)
{
/*	uint8_t *Unique_ID = (uint8_t*)UID_BASE;

	Mac_ADDR[0] = 0x02;

	Mac_ADDR[1] = Unique_ID[0] ^ Unique_ID[5] ^ Unique_ID[10] ;
    Mac_ADDR[2] = Unique_ID[1] ^ Unique_ID[6] ^ Unique_ID[11];
	Mac_ADDR[3] = Unique_ID[2] ^ Unique_ID[7] ^ Unique_ID[0];
	Mac_ADDR[4] = Unique_ID[3] ^ Unique_ID[8] ^ Unique_ID[1];
	Mac_ADDR[5] = Unique_ID[4] ^ Unique_ID[9] ^ Unique_ID[2]; */

}
/* ========================================================================== */
/*           If can complete in the Mb_TCP_Register_BOX.c file;               */
/*           If you have already done so, Good Luck :)                        */
/* ========================================================================== */


uint8_t ModbusTCP_IP_INIT_Function(GPIO_TypeDef* CS_Port, uint16_t CS_PIN, GPIO_TypeDef* W5500_Rst_GPIOx, uint16_t W5500_Rst_PIN_x, SPI_HandleTypeDef* hspiX, uint16_t timeout_tiM,
		uint8_t timeout_reT, uint32_t Flash_Address,GPIO_TypeDef* RstBtn_GPIOx,uint16_t RstBtn_GPIO_PIN_x,GPIO_TypeDef* Led_GPIOx,uint16_t Led_GPIO_PIN_x)
{
	MB_FlashStart_Address = Flash_Address;
	Global_hspiX = hspiX;
	Global_CS_Port = CS_Port;
	Global_CS_PIN = CS_PIN;
	Global_timeout_tiM = timeout_tiM;
	Global_timeout_reT = timeout_reT;
	Global_W5500_Rst_GPIOx = W5500_Rst_GPIOx;
	Global_W5500_Rst_PIN_x = W5500_Rst_PIN_x;

	Connectivity_Default_Update(RstBtn_GPIOx, RstBtn_GPIO_PIN_x, Led_GPIOx, Led_GPIO_PIN_x);
	Connectivity_Init_Update(Reading_MAC_Address, Reading_IP_Address, Reading_Subnet_Address, Reading_Gateway_Address, &Reading_TCP_Port);

	uint8_t Number_Configuration = 0 ;
	uint8_t Max_Configuration = 3 ;

	while(Number_Configuration < Max_Configuration)
	{
		HAL_GPIO_WritePin(Global_W5500_Rst_GPIOx, Global_W5500_Rst_PIN_x, GPIO_PIN_RESET);
		HAL_Delay(50);
		HAL_GPIO_WritePin(Global_W5500_Rst_GPIOx, Global_W5500_Rst_PIN_x, GPIO_PIN_SET);
		HAL_Delay(200);

		W5500_INIT_CS_Settings(Global_CS_Port, Global_CS_PIN);
		W5500_INIT_SPI_Settings(Global_hspiX);
		W5500_INIT_SPI_DMA_Settings(Global_hspiX);

		if(W5500_INIT_TIMEOUT_Settings(Global_timeout_tiM, Global_timeout_reT) != 0)
		{
			Number_Configuration++;
			continue;
		}
		if (W5500_INIT_RxTx_Buffer_Settings() != 0 )
		{
			Number_Configuration++;
			continue;
		}
		if (W5500_INIT_IP_Settings(Reading_MAC_Address, Reading_IP_Address, Reading_Subnet_Address, Reading_Gateway_Address) != 0)
		{
			Number_Configuration++;
			continue;
		}
		return 0 ;
	}
	return 1 ; //ERROR
}


void ModbusTCP_IP_WHILE_Function(uint8_t Socket )
{
	static uint32_t Old_Heartbeat_Time = 0 ;
	static uint8_t W5500_Error_Count = 0 ;
	static uint32_t Pin_Reset_State = 0;
	static uint8_t Pin_Reset_Time = 0 ;

	if((Pin_Reset_State == 0) && ((HAL_GetTick() - Old_Heartbeat_Time) >= 1000))
	{
		Old_Heartbeat_Time = HAL_GetTick() ;

		if(getVERSIONR() != 0X04)
		{
			W5500_Error_Count++;
		}
		else
		{
			W5500_Error_Count = 0 ;
		}

		if(W5500_Error_Count >= 3)
		{
			HAL_GPIO_WritePin(Global_W5500_Rst_GPIOx, Global_W5500_Rst_PIN_x, GPIO_PIN_RESET);
			Pin_Reset_Time = HAL_GetTick();
			Pin_Reset_State = 1;
			NEW_DATA_FLAG = 1 ;
		}
	}
	else if (Pin_Reset_State == 1)
	{
		if((HAL_GetTick() - Pin_Reset_Time) >= 2)
		{
			HAL_GPIO_WritePin(Global_W5500_Rst_GPIOx, Global_W5500_Rst_PIN_x, GPIO_PIN_SET);
			Pin_Reset_Time = HAL_GetTick();
			Pin_Reset_State = 2;
		}
	}
	else if(Pin_Reset_State == 2)
	{
		if((HAL_GetTick() - Pin_Reset_Time) >= 150)
		{
			uint8_t Init_error = 0 ;
			W5500_INIT_CS_Settings(Global_CS_Port, Global_CS_PIN);
			W5500_INIT_SPI_Settings(Global_hspiX);
			W5500_INIT_SPI_DMA_Settings(Global_hspiX);

			if(W5500_INIT_TIMEOUT_Settings(Global_timeout_tiM, Global_timeout_reT) != 0)
			{
				Init_error = 1 ;
			}
			if (W5500_INIT_RxTx_Buffer_Settings() != 0 )
			{
				Init_error = 1 ;
			}
			if (W5500_INIT_IP_Settings(Reading_MAC_Address, Reading_IP_Address, Reading_Subnet_Address, Reading_Gateway_Address) != 0)
			{
				Init_error = 1 ;
			}

			if(Init_error == 1)
			{
				HAL_GPIO_WritePin(Global_W5500_Rst_GPIOx, Global_W5500_Rst_PIN_x, GPIO_PIN_RESET);
				Pin_Reset_Time = HAL_GetTick();
				Pin_Reset_State = 1;
			}
			else
			{
				W5500_Error_Count = 0 ;
				Old_Heartbeat_Time = HAL_GetTick();
				Pin_Reset_State = 0;
			}
		}
	}

	if(Pin_Reset_State != 0)
	{
		return;
	}

	switch(getSn_SR(Socket))
	{
		case SOCK_CLOSED :

			if(socket(Socket, Sn_MR_TCP, Reading_TCP_Port, SF_TCP_NODELAY) == Socket)
			{
				setSn_KPALVTR(Socket , 3 ) ;
				break ;
			}
			break ;

		case SOCK_INIT :

			int32_t listen_Return_Value = listen(Socket);
			if(listen_Return_Value == SOCK_OK)
			{
				break ;
			}

			else if ((listen_Return_Value == SOCKERR_SOCKINIT) || (listen_Return_Value == SOCKERR_SOCKCLOSED))
			{
				close(Socket);
				break ;
			}
			break ;

		case SOCK_LISTEN :

			break ;

		case SOCK_ESTABLISHED :

			uint8_t IR_FLAG = getSn_IR(Socket);
			if(IR_FLAG & Sn_IR_TIMEOUT)
						{
							close(Socket);
							setSn_IR(Socket, Sn_IR_TIMEOUT);
							break;
						}

			if(IR_FLAG & Sn_IR_DISCON)
			{
				int32_t disconnect_RETURN_VALUE = disconnect(Socket) ;
				if(disconnect_RETURN_VALUE == SOCK_OK)
				{
					SOCKET_BUSY_RETURN_VALUE = 0 ;
					setSn_IR(Socket, Sn_IR_DISCON);
					break;
				}
				else if(disconnect_RETURN_VALUE == SOCKERR_TIMEOUT )
				{
					SOCKET_BUSY_RETURN_VALUE = 0 ;
					setSn_IR(Socket, Sn_IR_DISCON);
					close(Socket);
					break;
				}
				else if(disconnect_RETURN_VALUE == SOCK_BUSY)
				{
					SOCKET_BUSY_RETURN_VALUE ++ ;

					if(SOCKET_BUSY_RETURN_VALUE >= 10)
					{
						SOCKET_BUSY_RETURN_VALUE = 0 ;
						setSn_IR(Socket, Sn_IR_DISCON);
						close(Socket);
					}
					break;
				}
				else
				{
					SOCKET_BUSY_RETURN_VALUE = 0 ;
					setSn_IR(Socket, Sn_IR_DISCON);
					break;
				}
			}

			if( IR_FLAG & Sn_IR_CON)
				{
				   setSn_IR(Socket, Sn_IR_CON);
				}

			if(IR_FLAG & Sn_IR_RECV)
			{
				if(NEW_DATA_FLAG == 1)
				{
					if(getSn_RX_RSR(Socket) >= 7)
					{
						if(recv(Socket, MBAP, 7) == 7)
						{
							PDU_LEN = ((MBAP[4] << 8) | (MBAP[5])) ;
							NEW_DATA_FLAG = 0 ;
							PDU_MESSAGE_TIMEOUT = HAL_GetTick();
						}
						else
						{
							break;
						}
					}
					else
					{
						break;
					}
				}

				if(NEW_DATA_FLAG == 0)
				{
					if(getSn_RX_RSR(Socket) >= (PDU_LEN - 1))
					{
						if (recv(Socket, MB_TCP_RX_Buffer, (PDU_LEN - 1)) == (PDU_LEN - 1))
						{
							uint32_t TX_Function_Return_Value = Create_TX_Package(MBAP, MB_TCP_RX_Buffer, MB_TCP_TX_Buffer);

							if(TX_Function_Return_Value > 0)
							{
								uint16_t Successfully_Sent_Bytes = 0 ;
								int32_t Return_Send_Value = 0 ;

								while(TX_Function_Return_Value != Successfully_Sent_Bytes)
								{
									Return_Send_Value = send(Socket, (MB_TCP_TX_Buffer + Successfully_Sent_Bytes), (TX_Function_Return_Value - Successfully_Sent_Bytes)) ;

									if(Return_Send_Value < 0)
									{
										close(Socket);
										break;
									}
									Successfully_Sent_Bytes += (uint16_t) Return_Send_Value ;
								}
							}

							NEW_DATA_FLAG = 1 ;
							if(getSn_RX_RSR(Socket) == 0)
							{
								setSn_IR(Socket , Sn_IR_RECV);
							}

							if(NEW_IP_FLAG == 1)
							{
								NEW_IP_FLAG = 0 ;
								MB_InputRegisters[3] = 0xAAAA;

								HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
								uint32_t Success_OR_Fail = Connectivity_Update();
								HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
								if(Success_OR_Fail != 0)
								{
									MB_InputRegisters[3] = 0x0000;
								}
								else
								{
									MB_InputRegisters[3] = 0xFFFF;
								}
							}
						}
						else
						{
							break;
						}
					}
					else
					{
						if((HAL_GetTick() - PDU_MESSAGE_TIMEOUT) >= 2000)
						{
							NEW_DATA_FLAG = 1;
							close(Socket);
							break;
						}
						else
						{
							break;
						}
					}
				}
			}

			break;

		case SOCK_CLOSE_WAIT :

			int32_t discon_VALUE = disconnect(Socket) ;
			if(discon_VALUE == SOCK_OK)
			{
				SOCKET_BUSY_RETURN_VALUE_2 = 0 ;
				break;
			}
			else if(discon_VALUE == SOCKERR_TIMEOUT )
			{
				SOCKET_BUSY_RETURN_VALUE_2 = 0 ;
				close(Socket);
				break;
			}
			else if(discon_VALUE == SOCK_BUSY)
			{
				SOCKET_BUSY_RETURN_VALUE_2 ++ ;
				if(SOCKET_BUSY_RETURN_VALUE_2 >= 10)
				{
					SOCKET_BUSY_RETURN_VALUE_2 = 0 ;
					setSn_IR(Socket, Sn_IR_DISCON);
					close(Socket);
				}
				break;
			}
			else
			{
				SOCKET_BUSY_RETURN_VALUE_2 = 0 ;
				break;
			}
  }
}



