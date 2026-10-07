/*
 * Mb_TCP_Connectivity_Core.c
 *
 *  Created on: Aug 16, 2026
 *      Author: hanal
 */

#include "Mb_TCP_Connectivity_Core.h"
#include "Mb_TCP_USER_BOX.h"
#include "Mb_TCP_Register_BOX.h"

static uint32_t FLASH_DEFAULT_DATA_PACKET[4];

uint32_t Connectivity_Update(void) //**********************************************************************************************************************************************
{
	uint8_t Connectiviy_IP[4];
	uint8_t Connectiviy_SUBNET[4];
	uint8_t Connectiviy_GATEWAY[4];
	uint16_t Connectiviy_TCP_PORT;

	Connectiviy_IP[0] = (uint8_t)(MB_HoldingRegisters[0] >> 8);
	Connectiviy_IP[1] = (uint8_t)(MB_HoldingRegisters[0] & 0xFF);
	Connectiviy_IP[2] = (uint8_t)(MB_HoldingRegisters[1] >> 8);
	Connectiviy_IP[3] = (uint8_t)(MB_HoldingRegisters[1] & 0xFF);

	Connectiviy_SUBNET[0] = (uint8_t)(MB_HoldingRegisters[2] >> 8);;
	Connectiviy_SUBNET[1] = (uint8_t)(MB_HoldingRegisters[2] & 0xFF);;
	Connectiviy_SUBNET[2] = (uint8_t)(MB_HoldingRegisters[3] >> 8);;
	Connectiviy_SUBNET[3] = (uint8_t)(MB_HoldingRegisters[3] & 0xFF);;

	Connectiviy_GATEWAY[0] = (uint8_t)(MB_HoldingRegisters[4] >> 8);;
	Connectiviy_GATEWAY[1] = (uint8_t)(MB_HoldingRegisters[4] & 0xFF);;
	Connectiviy_GATEWAY[2] = (uint8_t)(MB_HoldingRegisters[5] >> 8);;
	Connectiviy_GATEWAY[3] = (uint8_t)(MB_HoldingRegisters[5] & 0xFF);;

	Connectiviy_TCP_PORT = MB_HoldingRegisters[6];

	return Flash_Write(Connectiviy_IP, Connectiviy_SUBNET, Connectiviy_GATEWAY, Connectiviy_TCP_PORT);
}

void Connectivity_Default_Update(GPIO_TypeDef* RstBtn_GPIOx,uint16_t RstBtn_GPIO_PIN,GPIO_TypeDef* Led_GPIOx,uint16_t Led_GPIO_PIN) //*********************************************
{
	if(HAL_GPIO_ReadPin(RstBtn_GPIOx, RstBtn_GPIO_PIN) == GPIO_PIN_SET)
	{
		HAL_Delay(2000);

		if(HAL_GPIO_ReadPin(RstBtn_GPIOx, RstBtn_GPIO_PIN) == GPIO_PIN_SET)
		{
			for(int i = 0; i < 10; i++)
			{
			   HAL_GPIO_TogglePin(Led_GPIOx, Led_GPIO_PIN);
			   HAL_Delay(100);

			   if(HAL_GPIO_ReadPin(RstBtn_GPIOx, RstBtn_GPIO_PIN) != GPIO_PIN_SET)
			   {
			       HAL_GPIO_WritePin(Led_GPIOx, Led_GPIO_PIN, GPIO_PIN_RESET);
			       return;
			   }
			}

			HAL_GPIO_WritePin(Led_GPIOx, Led_GPIO_PIN, GPIO_PIN_SET);

			Flash_Default_Write();

			HAL_GPIO_WritePin(Led_GPIOx, Led_GPIO_PIN, GPIO_PIN_RESET);
			return;
		}
	}
}



void Connectivity_Init_Update(uint8_t* Mac , uint8_t* IP, uint8_t* Subnet, uint8_t* Gateway, uint16_t* Port) //********************************************************************
{
	uint8_t Default_MAC[6] = MAC_ADDR ;
	USER_SPECIFIC_MAC_ADDR_ALGORITHM(Default_MAC);
	for(uint8_t i = 0 ; i < 6 ; i++)
	{
		Mac[i] = Default_MAC[i] ;
	}
	MB_InputRegisters[0] = ((uint16_t)Default_MAC[0] << 8) | Default_MAC[1];
	MB_InputRegisters[1] = ((uint16_t)Default_MAC[2] << 8) | Default_MAC[3];
	MB_InputRegisters[2] = ((uint16_t)Default_MAC[4] << 8) | Default_MAC[5];


	uint32_t Test_FLASH = 0 ;
	Test_FLASH = Flash_Read(MB_FlashStart_Address);
	if(Test_FLASH == (0xFFFFFFFF))
	{
		Flash_Default_Write();
	}


	uint32_t IP_IN_FLASH = Flash_Read(MB_FlashStart_Address);
	uint32_t SUBNET_IN_FLASH = Flash_Read(MB_FlashStart_Address + 4);
	uint32_t GATEWAY_IN_FLASH = Flash_Read(MB_FlashStart_Address + 8);
	uint32_t TCP_PORT_IN_FLASH = Flash_Read(MB_FlashStart_Address + 12);

	IP[0] = (IP_IN_FLASH >> 24) & 0xFF;
	IP[1] = (IP_IN_FLASH >> 16) & 0xFF;
	IP[2] = (IP_IN_FLASH >> 8) & 0xFF;
	IP[3] = IP_IN_FLASH & 0xFF;

	Subnet[0] = (SUBNET_IN_FLASH >> 24) & 0xFF;
	Subnet[1] = (SUBNET_IN_FLASH >> 16) & 0xFF;
	Subnet[2] = (SUBNET_IN_FLASH >> 8) & 0xFF;
	Subnet[3] = SUBNET_IN_FLASH & 0xFF;

	Gateway[0] = (GATEWAY_IN_FLASH >> 24) & 0xFF;
	Gateway[1] = (GATEWAY_IN_FLASH >> 16) & 0xFF;
	Gateway[2] = (GATEWAY_IN_FLASH >> 8) & 0xFF;
	Gateway[3] = GATEWAY_IN_FLASH & 0xFF;

	*Port = (uint16_t)(TCP_PORT_IN_FLASH >> 16);

	MB_HoldingRegisters[0] = (uint16_t)(IP_IN_FLASH >> 16);
	MB_HoldingRegisters[1] = (uint16_t)(IP_IN_FLASH & 0xFFFF);
	MB_HoldingRegisters[2] = (uint16_t)(SUBNET_IN_FLASH >> 16);
	MB_HoldingRegisters[3] = (uint16_t)(SUBNET_IN_FLASH & 0xFFFF);
	MB_HoldingRegisters[4] = (uint16_t)(GATEWAY_IN_FLASH >> 16);
	MB_HoldingRegisters[5] = (uint16_t)(GATEWAY_IN_FLASH & 0xFFFF);
	MB_HoldingRegisters[6] = (uint16_t)(TCP_PORT_IN_FLASH >> 16);
	MB_InputRegisters[3] = 0xFFFF;

}




uint32_t Flash_Write(  uint8_t ip[4], uint8_t subnet[4], uint8_t gateway[4], uint16_t port) // ************************************************************************************
{
	uint32_t addressW = MB_FlashStart_Address ;
	uint32_t data[4];

	data[0] = ((uint32_t)ip[0] << 24)  | ((uint32_t)ip[1] << 16)  | ((uint32_t)ip[2] << 8)  | ip[3];
	data[1] = ((uint32_t)subnet[0] << 24) | ((uint32_t)subnet[1] << 16) | ((uint32_t)subnet[2] << 8) | subnet[3];
	data[2] = ((uint32_t)gateway[0] << 24) | ((uint32_t)gateway[1] << 16) | ((uint32_t)gateway[2] << 8) | gateway[3];
	data[3] = ((uint32_t)(port >> 8) & 0xFF) << 24 | ((uint32_t)(port & 0xFF)) << 16;



	FLASH_EraseInitTypeDef SettingsFlash = {0} ;
	uint32_t ErrorCODE =0 ;
	uint32_t BANK_NO ;
	uint32_t PAGE_NO;
	uint32_t SECTOR_NO;
	uint32_t SECTOR_BANK_NO;

	PAGE_BANK_CALCULATE(MB_FlashStart_Address, &PAGE_NO, &BANK_NO) ;
	SECTOR_CALCULATE(MB_FlashStart_Address, &SECTOR_NO, &SECTOR_BANK_NO);

	HAL_FLASH_Unlock();



#if defined (MB_Sector)
	SettingsFlash.TypeErase = FLASH_TYPEERASE_SECTORS ;
	SettingsFlash.Sector = SECTOR_NO ;
	SettingsFlash.NbSectors = 1 ;
	SettingsFlash.VoltageRange = FLASH_VOLTAGE_RANGE_3 ;
#if defined (FLASH_BANK_1)
	SettingsFlash.Banks = SECTOR_BANK_NO ;
#endif

#elif defined (MB_Page) && defined (Page_ADDRESS)
	SettingsFlash.TypeErase = FLASH_TYPEERASE_PAGES ;
	SettingsFlash.PageAddress = MB_FlashStart_Address ;
	SettingsFlash.NbPages = 2 ;
#elif defined (MB_Page) && defined (Page_NO) && defined (BANK_YOK)
	SettingsFlash.TypeErase = FLASH_TYPEERASE_PAGES ;
	SettingsFlash.Page = PAGE_NO ;
	SettingsFlash.NbPages = 2 ;
#elif defined (MB_Page) && defined (Page_NO) && defined (BANK_VAR)
	SettingsFlash.TypeErase = FLASH_TYPEERASE_PAGES ;
	SettingsFlash.Page = PAGE_NO ;
	SettingsFlash.NbPages = 2 ;
	SettingsFlash.Banks = BANK_NO ;
#endif

	HAL_FLASHEx_Erase(&SettingsFlash, &ErrorCODE);


#if defined (MB_Word)

	for(uint8_t i = 0 ; i<4 ; i++)
	{
		if((HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD , addressW , data[i])) == HAL_OK)
		{
			addressW += 4;
		}
		else
		{
			return HAL_FLASH_GetError();
		}
	}
#elif defined (MB_DoubleWord)

	uint64_t *DoubleWord_Data_Trans = (uint64_t*)data ;

	for(uint8_t i = 0 ; i<2 ; i++)
	{
		if((HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD , addressW , DoubleWord_Data_Trans[i])) == HAL_OK)
		{
			addressW += 8;
		}
		else
		{
			return HAL_FLASH_GetError();
		}
	}
#endif


	HAL_FLASH_Lock();

	return 0 ;
}


uint32_t Flash_Default_Write() //**************************************************************************************************************************************************
{
	uint32_t addressWDef = MB_FlashStart_Address ;

	uint8_t Transform_ip[4]      = IP_ADDR;
	uint8_t Transform_subnet[4]  = Subnet_ADDR;
	uint8_t Transform_gateway[4] = Gateway_ADDR;

	FLASH_DEFAULT_DATA_PACKET[0] = ((uint32_t)Transform_ip[0] << 24) | ((uint32_t)Transform_ip[1] << 16) | ((uint32_t)Transform_ip[2] << 8) | ((uint32_t)Transform_ip[3]);

	FLASH_DEFAULT_DATA_PACKET[1] = ((uint32_t)Transform_subnet[0]<<24)|((uint32_t)Transform_subnet[1]<<16)|((uint32_t)Transform_subnet[2]<<8)|((uint32_t)Transform_subnet[3]);

	FLASH_DEFAULT_DATA_PACKET[2] = ((uint32_t)Transform_gateway[0]<<24)|((uint32_t)Transform_gateway[1]<<16)|((uint32_t)Transform_gateway[2]<<8)|((uint32_t)Transform_gateway[3]);

	FLASH_DEFAULT_DATA_PACKET[3] = ((((uint32_t)TCP_PORT >> 8) & 0xFF) << 24) | ((uint32_t)TCP_PORT & 0xFF) << 16 ;


	uint32_t PAGE_BANK_NO ;
	uint32_t PAGE_NO;
	uint32_t SECTOR_NO;
	uint32_t SECTOR_BANK_NO;

	PAGE_BANK_CALCULATE(MB_FlashStart_Address, &PAGE_NO, &PAGE_BANK_NO);
	SECTOR_CALCULATE(MB_FlashStart_Address, &SECTOR_NO, &SECTOR_BANK_NO);

	FLASH_EraseInitTypeDef SettingsFlash = {0} ;
	uint32_t ErrorCODE =0 ;

	HAL_FLASH_Unlock();

#if defined (MB_Sector)
	SettingsFlash.TypeErase = FLASH_TYPEERASE_SECTORS ;
	SettingsFlash.Sector = SECTOR_NO ;
	SettingsFlash.NbSectors = 1 ;
	SettingsFlash.VoltageRange = FLASH_VOLTAGE_RANGE_3 ;
#if defined (FLASH_BANK_1)
	SettingsFlash.Banks = SECTOR_BANK_NO ;
#endif

#elif defined (MB_Page) && defined (Page_ADDRESS)
	SettingsFlash.TypeErase = FLASH_TYPEERASE_PAGES ;
	SettingsFlash.PageAddress = MB_FlashStart_Address;
	SettingsFlash.NbPages = 2 ;
#elif defined (MB_Page) && defined (Page_NO) && defined (BANK_YOK)
	SettingsFlash.TypeErase = FLASH_TYPEERASE_PAGES ;
	SettingsFlash.Page = PAGE_NO ;
	SettingsFlash.NbPages = 2 ;
#elif defined (MB_Page) && defined (Page_NO) && defined (BANK_VAR)
	SettingsFlash.TypeErase = FLASH_TYPEERASE_PAGES ;
	SettingsFlash.Page = PAGE_NO ;
	SettingsFlash.NbPages = 2 ;
	SettingsFlash.Banks = PAGE_BANK_NO ;
#endif

	if(HAL_FLASHEx_Erase(&SettingsFlash, &ErrorCODE) != HAL_OK)
	{
		HAL_FLASH_Lock();
		return HAL_FLASH_GetError() ;
	}

#if defined (MB_Word)

	for(uint8_t i = 0 ; i<4 ; i++)
	{
		if((HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD , addressWDef , FLASH_DEFAULT_DATA_PACKET[i])) == HAL_OK)
		{
			addressWDef += 4;
		}
		else
		{
			return HAL_FLASH_GetError();
		}
	}
#elif defined (MB_DoubleWord)
	uint64_t *DoubleWord_Data_Trans = (uint64_t*)FLASH_DEFAULT_DATA_PACKET ;

	for(uint8_t i = 0 ; i<2 ; i++)
	{
		if((HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD , addressWDef , DoubleWord_Data_Trans[i])) == HAL_OK)
		{
			addressWDef += 8;
		}
		else
		{
			return HAL_FLASH_GetError();
		}
	}
#endif

	HAL_FLASH_Lock();

	return 0 ;
}



uint32_t Flash_Read(uint32_t addressR) //******************************************************************************************************************************************
{
	return *(__IO uint32_t*)addressR ;
}




void PAGE_BANK_CALCULATE(uint32_t Address , uint32_t *PAGE , uint32_t *BANK) //****************************************************************************************************
{
#ifndef FLASH_PAGE_SIZE
#define FLASH_PAGE_SIZE 2048
#endif

#if defined (STM32G4) || defined (STM32L5) || defined (STM32L4)
	uint32_t Flash_Size = ((*((uint16_t*)FLASHSIZE_BASE)) * 1024);
	uint32_t Bank2_Start = (FLASH_BASE + (Flash_Size /2));
	if(Address >= Bank2_Start)
	{
		*BANK = FLASH_BANK_2 ;
		*PAGE = ((Address - Bank2_Start) / FLASH_PAGE_SIZE );
	}
	else
	{
		*BANK = FLASH_BANK_1 ;
		*PAGE = ((Address - FLASH_BASE) / FLASH_PAGE_SIZE );
	}
#elif defined (STM32G0) || defined (STM32C0)
	*PAGE = ((Address - FLASH_BASE) / FLASH_PAGE_SIZE) ;
	*BANK = 0 ;
#endif
}



void SECTOR_CALCULATE(uint32_t Address , uint32_t *SECTOR , uint32_t *BANK) //*****************************************************************************************************
{
	uint32_t Bank_YES = 0 ;
	uint32_t Bank_NO = 0 ;

#if defined (FLASH_OPTCR_nDBANK)
	if((FLASH->OPTCR & FLASH_OPTCR_nDBANK) == 0)
	{
		Bank_YES = 1 ;
	}
#elif defined (FLASH_OPTCR_DB1M)
	if((FLASH->OPTCR & FLASH_OPTCR_DB1M) == 1)
	{
		Bank_YES = 1 ;
	}
#else
	Bank_NO = 1 ;
#endif

	if(Bank_NO)
	{
#if defined (STM32F2) || defined (STM32F4)
		*BANK = FLASH_BANK_1 ;

		if((Address >= 0x08000000) && (Address <= 0x08003FFF )){ *SECTOR = FLASH_SECTOR_0 ; }
		else if ((Address >= 0x08004000) && (Address <= 0x08007FFF )){ *SECTOR = FLASH_SECTOR_1 ; }
		else if ((Address >= 0x08008000) && (Address <= 0x0800BFFF )){ *SECTOR = FLASH_SECTOR_2 ; }
		else if ((Address >= 0x0800C000) && (Address <= 0x0800FFFF )){ *SECTOR = FLASH_SECTOR_3 ; }
		else if ((Address >= 0x08010000) && (Address <= 0x0801FFFF )){ *SECTOR = FLASH_SECTOR_4 ; }
		else if ((Address >= 0x08020000) && (Address <= 0x0803FFFF )){ *SECTOR = FLASH_SECTOR_5 ; }
#if defined (FLASH_SECTOR_6)
		else if ((Address >= 0x08040000) && (Address <= 0x0805FFFF )){ *SECTOR = FLASH_SECTOR_6 ; }
		else if ((Address >= 0x08060000) && (Address <= 0x0807FFFF )){ *SECTOR = FLASH_SECTOR_7 ; }
#endif
#if defined (FLASH_SECTOR_8)
		else if ((Address >= 0x08080000) && (Address <= 0x0809FFFF )){ *SECTOR = FLASH_SECTOR_8 ; }
		else if ((Address >= 0x080A0000) && (Address <= 0x080BFFFF )){ *SECTOR = FLASH_SECTOR_9 ; }
		else if ((Address >= 0x080C0000) && (Address <= 0x080DFFFF )){ *SECTOR = FLASH_SECTOR_10; }
		else if ((Address >= 0x080E0000) && (Address <= 0x080FFFFF )){ *SECTOR = FLASH_SECTOR_11; }
#endif
#endif

#if defined (STM32F7)
		if ((Address >= 0x08000000) && (Address <= 0x08007FFF )){ *SECTOR = FLASH_SECTOR_0 ; }
		else if ((Address >= 0x08008000) && (Address <= 0x0800FFFF )){ *SECTOR = FLASH_SECTOR_1 ; }
		else if ((Address >= 0x08010000) && (Address <= 0x08017FFF )){ *SECTOR = FLASH_SECTOR_2 ; }
		else if ((Address >= 0x08018000) && (Address <= 0x0801FFFF )){ *SECTOR = FLASH_SECTOR_3 ; }
		else if ((Address >= 0x08020000) && (Address <= 0x0803FFFF )){ *SECTOR = FLASH_SECTOR_4 ; }
		else if ((Address >= 0x08040000) && (Address <= 0x0807FFFF )){ *SECTOR = FLASH_SECTOR_5 ; }
#if defined (FLASH_SECTOR_6)
		else if ((Address >= 0x08080000) && (Address <= 0x080BFFFF )){ *SECTOR = FLASH_SECTOR_6 ; }
		else if ((Address >= 0x080C0000) && (Address <= 0x080FFFFF )){ *SECTOR = FLASH_SECTOR_7 ; }
#endif
#if defined (FLASH_SECTOR_8)
		else if ((Address >= 0x08100000) && (Address <= 0x0813FFFF )){ *SECTOR = FLASH_SECTOR_8 ; }
		else if ((Address >= 0x08140000) && (Address <= 0x0817FFFF )){ *SECTOR = FLASH_SECTOR_9 ; }
		else if ((Address >= 0x08180000) && (Address <= 0x081BFFFF )){ *SECTOR = FLASH_SECTOR_10; }
		else if ((Address >= 0x081C0000) && (Address <= 0x081FFFFF )){ *SECTOR = FLASH_SECTOR_11; }
#endif
#endif
	}

	else if(Bank_YES)
	{
#if defined (STM32F7) || defined (STM32F42xxx) || defined (STM32F43xxx)
#if defined (FLASH_BANK_2)
		if((Address >= 0x08100000) && (Address <= 0x081FFFFF)){ *BANK = FLASH_BANK_2 ; }
		else { *BANK = FLASH_BANK_1 ; }
#elif defined (FLASH_BANK_1)
		*BANK = FLASH_BANK_1 ;
#else
		*BANK = 0 ;
#endif

		if ((Address >= 0x08000000) && (Address <= 0x08003FFF)){ *SECTOR = FLASH_SECTOR_0 ; }
		else if ((Address >= 0x08004000) && (Address <= 0x08007FFF)){ *SECTOR = FLASH_SECTOR_1 ; }
		else if ((Address >= 0x08008000) && (Address <= 0x0800BFFF)){ *SECTOR = FLASH_SECTOR_2 ; }
		else if ((Address >= 0x0800C000) && (Address <= 0x0800FFFF)){ *SECTOR = FLASH_SECTOR_3 ; }
		else if ((Address >= 0x08010000) && (Address <= 0x0801FFFF)){ *SECTOR = FLASH_SECTOR_4 ; }
		else if ((Address >= 0x08020000) && (Address <= 0x0803FFFF)){ *SECTOR = FLASH_SECTOR_5 ; }
#if defined (FLASH_SECTOR_6)
		else if ((Address >= 0x08040000) && (Address <= 0x0805FFFF)){ *SECTOR = FLASH_SECTOR_6 ; }
		else if ((Address >= 0x08060000) && (Address <= 0x0807FFFF)){ *SECTOR = FLASH_SECTOR_7 ; }
#endif
#if defined (FLASH_SECTOR_8)
		else if ((Address >= 0x08080000) && (Address <= 0x0809FFFF)){ *SECTOR = FLASH_SECTOR_8 ; }
		else if ((Address >= 0x080A0000) && (Address <= 0x080BFFFF)){ *SECTOR = FLASH_SECTOR_9 ; }
		else if ((Address >= 0x080C0000) && (Address <= 0x080DFFFF)){ *SECTOR = FLASH_SECTOR_10; }
		else if ((Address >= 0x080E0000) && (Address <= 0x080FFFFF)){ *SECTOR = FLASH_SECTOR_11; }
#endif
#if defined (FLASH_SECTOR_12)
		else if ((Address >= 0x08100000) && (Address <= 0x08103FFF)){ *SECTOR = FLASH_SECTOR_12; }
		else if ((Address >= 0x08104000) && (Address <= 0x08107FFF)){ *SECTOR = FLASH_SECTOR_13; }
		else if ((Address >= 0x08108000) && (Address <= 0x0810BFFF)){ *SECTOR = FLASH_SECTOR_14; }
		else if ((Address >= 0x0810C000) && (Address <= 0x0810FFFF)){ *SECTOR = FLASH_SECTOR_15; }
		else if ((Address >= 0x08110000) && (Address <= 0x0811FFFF)){ *SECTOR = FLASH_SECTOR_16; }
		else if ((Address >= 0x08120000) && (Address <= 0x0813FFFF)){ *SECTOR = FLASH_SECTOR_17; }
		else if ((Address >= 0x08140000) && (Address <= 0x0815FFFF)){ *SECTOR = FLASH_SECTOR_18; }
		else if ((Address >= 0x08160000) && (Address <= 0x0817FFFF)){ *SECTOR = FLASH_SECTOR_19; }
		else if ((Address >= 0x08180000) && (Address <= 0x0819FFFF)){ *SECTOR = FLASH_SECTOR_20; }
		else if ((Address >= 0x081A0000) && (Address <= 0x081BFFFF)){ *SECTOR = FLASH_SECTOR_21; }
		else if ((Address >= 0x081C0000) && (Address <= 0x081DFFFF)){ *SECTOR = FLASH_SECTOR_22; }
		else if ((Address >= 0x081E0000) && (Address <= 0x081FFFFF)){ *SECTOR = FLASH_SECTOR_23; }
#endif
#endif
	}
}
