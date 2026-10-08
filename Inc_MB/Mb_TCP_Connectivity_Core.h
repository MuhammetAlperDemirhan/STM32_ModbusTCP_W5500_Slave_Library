/*
 * Mb_TCP_Connectivity_Core.h
 *
 *  Created on: Aug 16, 2026
 *      Author: hanal
 */

#ifndef INC_MB_TCP_CONNECTIVITY_CORE_H_
#define INC_MB_TCP_CONNECTIVITY_CORE_H_

#include "main.h"
#include <stdint.h>
/*=============================== HAFIZA DURUMU =============================== */
#if defined (STM32F1) || defined (STM32F0) || defined (STM32F3) || defined (STM32G0) || defined (STM32G4) || defined (STM32L0) ||\
    defined (STM32L4) || defined (STM32L5) || defined (STM32C0) || defined (STM32L1)
#define MB_Page 1

#elif defined (STM32F4) || defined (STM32F7) || defined (STM32F2) //VOLTAGE RANGE AYARI VAR. FLASH_VOLTAGE_RANGE_3 YAPILIRSA WORD YAZMA YAPILIR.
#define MB_Sector 1
#endif
/*=============================== YAZMA BİT DURUMU =============================== */

#if defined (STM32G0) || defined (STM32G4) || defined (STM32L4) || defined (STM32L5) || defined (STM32C0)
#define MB_DoubleWord 1

#elif defined (STM32F4) || defined (STM32F7) || defined (STM32F2) || defined (STM32F0) || defined (STM32F1) || defined (STM32F3) || defined (STM32L0) || defined (STM32L1)
#define MB_Word 1
#endif
/*=============================== PAGE DURUMU =============================== */

#if defined (STM32L1) || defined (STM32L0) || defined (STM32F1) || defined (STM32F0) || defined (STM32F3)
#define Page_ADDRESS 1

#elif defined (STM32G0) || defined (STM32G4) || defined (STM32L4) || defined (STM32L5) || defined (STM32C0)
#define Page_NO 1
#endif
/*=============================== BANK DURUMU ================================  */

#if defined (STM32G4) || defined (STM32L5) || defined (STM32L4)
#define BANK_VAR 1

#elif defined (STM32C0) || defined (STM32G0)
#define BANK_YOK 1
#endif

uint32_t Connectivity_Update(void);
void Connectivity_Default_Update(GPIO_TypeDef* RstBtn_GPIOx,uint16_t RstBtn_GPIO_PIN,GPIO_TypeDef* Led_GPIOx,uint16_t Led_GPIO_PIN);
void Connectivity_Init_Update(uint8_t* Mac , uint8_t* IP, uint8_t* Subnet, uint8_t* Gateway, uint16_t* Port);
uint32_t Flash_Write(uint8_t ip[4], uint8_t subnet[4], uint8_t gateway[4], uint16_t port);
uint32_t Flash_Default_Write();
uint32_t Flash_Read(uint32_t addressR);
void PAGE_BANK_CALCULATE(uint32_t Address , uint32_t *PAGE , uint32_t *BANK);
void SECTOR_CALCULATE(uint32_t Address , uint32_t *SECTOR , uint32_t *BANK);



#endif /* INC_MB_TCP_CONNECTIVITY_CORE_H_ */
