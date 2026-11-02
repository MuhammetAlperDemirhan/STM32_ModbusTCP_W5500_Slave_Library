/*
 * Mb_TCP_Function_Codes_Core.h
 *
 *  Created on: Aug 22, 2026
 *      Author: hanal
 */

#ifndef INC_MB_TCP_FUNCTION_CODES_CORE_H_
#define INC_MB_TCP_FUNCTION_CODES_CORE_H_

#include "main.h"
#include <stdint.h>

extern uint8_t NEW_IP_FLAG;

uint16_t Create_TX_Package(uint8_t* MBAP_Header, uint8_t* RX_BUFFER, uint8_t* TX_BUFFER);
uint16_t Exception_Codes(uint8_t* Request , uint8_t* Response, uint8_t Exception_Code);
uint16_t Read_Coils(uint8_t* Request , uint8_t* Response);
uint16_t Read_Discrete_Inputs(uint8_t* Request , uint8_t* Response );
uint16_t Read_Holding_Registers(uint8_t* Request, uint8_t* Response);
uint16_t Read_Input_Registers(uint8_t* Request, uint8_t* Response);
uint16_t Write_Single_Coil(uint8_t* Request, uint8_t* Response);
uint16_t Write_Single_Register(uint8_t* Request, uint8_t* Response);
uint16_t Write_Multiple_Coils(uint8_t* Request, uint8_t* Response);
uint16_t Write_Multiple_Registers(uint8_t* Request, uint8_t* Response);
uint16_t Encapsulated_Interface_Transport(uint8_t* Request, uint8_t* Response);
uint16_t Mask_Write_Register(uint8_t* Request, uint8_t* Response);
uint16_t Read_Write_Multiple_Registers(uint8_t* Request, uint8_t* Response);

#endif /* INC_MB_TCP_FUNCTION_CODES_CORE_H_ */
