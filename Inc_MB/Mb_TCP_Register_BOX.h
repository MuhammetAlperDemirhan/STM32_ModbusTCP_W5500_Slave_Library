/*
 * Mb_TCP_Register_BOX.h
 *
 *  Created on: Aug 18, 2026
 *      Author: hanal
 */

#ifndef INC_MB_TCP_REGISTER_BOX_H_
#define INC_MB_TCP_REGISTER_BOX_H_

#include "main.h"
#include <stdint.h>

extern const uint16_t CoilStatus_Size ;
extern const uint16_t InputStatus_Size ;
extern const uint16_t InputRegisters_Size ;
extern const uint16_t HoldingRegisters_Size;

extern uint8_t MB_Coils[]; // 0xxxx , R/W , 1-bit
extern uint8_t MB_DiscreteInput[]; // 1xxxx , R , 1-bit
extern uint16_t MB_InputRegisters[]; //3xxxx , R , 16-bit/word/2-byte
extern uint16_t MB_HoldingRegisters[]; //4xxxx , R/W , 16-bit/word/2-byte

typedef struct
{
	GPIO_TypeDef* Port ;
	uint16_t Pin ;
}Coil_Degisken;

typedef struct
{
	GPIO_TypeDef* Port ;
	uint16_t Pin ;
}Discrete_Input_Degisken;

extern const Coil_Degisken Coil_Map[];
extern const Discrete_Input_Degisken Discrete_Input_Map[];
extern const uint16_t Coil_Map_Size;
extern const uint16_t Discrete_Input_Size;

#endif /* INC_MB_TCP_REGISTER_BOX_H_ */
