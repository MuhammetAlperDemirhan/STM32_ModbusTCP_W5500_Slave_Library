/*
 * Mb_TCP_Function_Codes_Core.c
 *
 *  Created on: Aug 22, 2026
 *      Author: hanal
 */

#include "Mb_TCP_Function_Codes_Core.h"
#include "Mb_TCP_Register_BOX.h"
#include "Mb_TCP_Connectivity_Core.h"
#include "Mb_TCP_USER_BOX.h"

uint8_t NEW_IP_FLAG = 0 ;

const char VendorName[] = VENDOR_NAME ;
const char ProductCode[] = PRODUCT_CODE;
const char Version[] = VERSION;


uint16_t Create_TX_Package(uint8_t* MBAP_Header, uint8_t* RX_BUFFER, uint8_t* TX_BUFFER) //****************************************************************************************
{
	uint16_t Tx_Buffer_LEN = 0 ;

	uint16_t Protocol_Identifier = ((uint16_t)MBAP_Header[2] << 8) | MBAP_Header[3];
	if(Protocol_Identifier != 0x0000)
	{
		return 0 ;
	}

	TX_BUFFER[0] = MBAP_Header[0] ;
	TX_BUFFER[1] = MBAP_Header[1] ;
	TX_BUFFER[2] = MBAP_Header[2] ;
	TX_BUFFER[3] = MBAP_Header[3] ;
	TX_BUFFER[6] = MBAP_Header[6] ;

	uint8_t Function_Code = RX_BUFFER[0] ;
	switch(Function_Code)
	{
	case 0x01 : // Read Coils
		Tx_Buffer_LEN = Read_Coils(RX_BUFFER, TX_BUFFER);
		break;
	case 0x02 : // Read Discrete Inputs
		Tx_Buffer_LEN = Read_Discrete_Inputs(RX_BUFFER, TX_BUFFER);
		break ;
	case 0x03 : // Read Holding Registers
		Tx_Buffer_LEN = Read_Holding_Registers(RX_BUFFER, TX_BUFFER);
		break ;
	case 0x04 : // Read Input Registers
		Tx_Buffer_LEN = Read_Input_Registers(RX_BUFFER, TX_BUFFER);
		break ;
	case 0x05 : // Write Single Coil
		Tx_Buffer_LEN = Write_Single_Coil(RX_BUFFER, TX_BUFFER);
		break ;
	case 0x06 : // Write Single Register
		Tx_Buffer_LEN = Write_Single_Register(RX_BUFFER, TX_BUFFER);
		break ;
	case 0x0F : // Write Multiple Coils
		Tx_Buffer_LEN = Write_Multiple_Coils(RX_BUFFER, TX_BUFFER);
		break ;
	case 0x10 : //Write Multiple Registers
		Tx_Buffer_LEN = Write_Multiple_Registers(RX_BUFFER, TX_BUFFER);
		break ;
	case 0x16 : //Mask Write Register
		Tx_Buffer_LEN = Mask_Write_Register(RX_BUFFER, TX_BUFFER);
		break ;
	case 0x17 : //Read_Write_Multiple_Registers
		Tx_Buffer_LEN =Read_Write_Multiple_Registers(RX_BUFFER, TX_BUFFER);
		break;
	case 0x2B : //Encapsulated Interface Transport
		Tx_Buffer_LEN = Encapsulated_Interface_Transport(RX_BUFFER, TX_BUFFER);
		break ;
	// ......
	// FUNCTİON CODELER , BU KISMIN DEVAMINA case-break YAPISI İLE EKLENEBİLİR. EKLENEN FUNCTİON CODELER TX_PAKET LERİN UZUNLUK (PAKET SİZE) BİLGİSİNİ return ETMELİDİR.

	default :
		Tx_Buffer_LEN = Exception_Codes(RX_BUFFER, TX_BUFFER, 0x01);
		break;

	}
	return Tx_Buffer_LEN ;
}

uint16_t Exception_Codes(uint8_t* Request , uint8_t* Response, uint8_t Exception_Code) //******************************************************************************************
{
	Response[4] = 0x00 ;
	Response[5] = 0x03 ;
	Response[7] = (Request[0] | 0x80);
	Response[8] = Exception_Code;
	return 9 ;
}

uint16_t Read_Coils(uint8_t* Request , uint8_t* Response) //0x01 ******************************************************************************************************************
{
	uint16_t Start_Address = ((uint16_t)Request[1] << 8) | Request[2];
	uint16_t Quantity = ((uint16_t)Request[3] << 8) | Request[4];
	uint8_t Byte_Count = (Quantity / 8) ;

	if (Quantity < 1 || Quantity > 2000)
	{
		return Exception_Codes(Request, Response, 0x03);
	}

	if((Quantity % 8) > 0)
	{
		Byte_Count++;
	}

	if((Quantity + Start_Address) > Coil_Map_Size)
	{
		return Exception_Codes(Request, Response, 0x02) ;
	}

	for(uint16_t i = Start_Address; i < (Start_Address + Quantity) ; i++)
	{
		if (Coil_Map[i].Port != NULL)
		{
			MB_Coils[i] = HAL_GPIO_ReadPin(Coil_Map[i].Port, Coil_Map[i].Pin);
		}
	}

	Response[7] = 0x01 ;
	Response[8] = Byte_Count;

	for (uint16_t k = 0; k < Byte_Count; k++)
	{
		Response[9 + k] = 0x00 ;
	}

	uint8_t Target_bit = 0;
	uint8_t Target_byte = 0 ;
	for(uint16_t i = Start_Address ; i < (Start_Address + Quantity) ; i++)
	{
		if(MB_Coils[i] > 0)
		{
			Response[Target_byte + 9] = Response[Target_byte + 9] | (1 << Target_bit);
		}
		Target_bit++ ;
		if(Target_bit > 7)
		{
			Target_byte++;
			Target_bit = 0 ;
		}
	}

	uint16_t PDU_SizE = Byte_Count + 3 ;
	Response[4] = (PDU_SizE >> 8) & 0xFF ;
	Response[5] = PDU_SizE & 0xFF ;

	return (PDU_SizE + 6);
}


uint16_t Read_Discrete_Inputs(uint8_t* Request , uint8_t* Response ) //0x02 *******************************************************************************************************
{
	uint16_t Start_Address = ((uint16_t)Request[1] << 8) | Request[2];
	uint16_t Quantity = ((uint16_t)Request[3] << 8) | Request[4];
	uint8_t Byte_Count = (Quantity / 8) ;

	if (Quantity < 1 || Quantity > 2000)
	{
		return Exception_Codes(Request, Response, 0x03);
	}

	if((Quantity % 8) > 0)
	{
		Byte_Count++;
	}

	if((Quantity + Start_Address) > Discrete_Input_Size)
	{
		return Exception_Codes(Request, Response, 0x02) ;
	}

	for(uint16_t i = Start_Address; i < (Start_Address + Quantity) ; i++)
	{
		MB_DiscreteInput[i] = HAL_GPIO_ReadPin(Discrete_Input_Map[i].Port, Discrete_Input_Map[i].Pin);
	}

	Response[7] = 0x02 ;
	Response[8] = Byte_Count;

	for (uint16_t k = 0; k < Byte_Count; k++)
	{
		Response[9 + k] = 0x00 ;
	}

	uint8_t Target_bit = 0;
	uint8_t Target_byte = 0 ;
	for(uint16_t i = Start_Address ; i < (Start_Address + Quantity) ; i++)
	{
		if(MB_DiscreteInput[i] > 0)
		{
			Response[Target_byte + 9] = Response[Target_byte + 9] | (1 << Target_bit);
		}
		Target_bit++ ;
		if(Target_bit > 7)
		{
			Target_byte++;
			Target_bit = 0 ;
		}
	}

	uint16_t PDU_SizE = Byte_Count + 3 ;
	Response[4] = (PDU_SizE >> 8) & 0xFF ;
	Response[5] = PDU_SizE & 0xFF ;

	return (PDU_SizE + 6);
}

uint16_t Read_Holding_Registers(uint8_t* Request, uint8_t* Response) //0x03 *******************************************************************************************************
{
	uint16_t Start_Address = ((uint16_t)Request[1] << 8) | Request[2];
	uint16_t Quantity = ((uint16_t)Request[3] << 8) | Request[4];
	uint8_t Byte_Count = (Quantity * 2);

	if (Quantity < 1 || Quantity > 125)
	{
		return Exception_Codes(Request, Response, 0x03);
	}

	if((Start_Address + Quantity) > HoldingRegisters_Size)
	{
		return Exception_Codes(Request, Response, 0x02) ;
	}

	Response[7] = 0x03;
	Response[8] = Byte_Count;

	for (uint16_t k = 0; k < Byte_Count; k++)
	{
		Response[9 + k] = 0x00 ;
	}

	for(uint16_t i = 0 ; i < Quantity ; i++)
	{
		uint16_t Register_Value = MB_HoldingRegisters[Start_Address + i];
		Response[9 + (i*2)] = (uint8_t)(Register_Value >> 8) ;
		Response[10 + (i*2)] = (uint8_t)Register_Value ;
	}

	uint16_t PDU_SizE = Byte_Count + 3;
	Response[4] = (PDU_SizE >> 8) & 0xFF;
	Response[5] = PDU_SizE & 0xFF;

	return (PDU_SizE + 6);
}

uint16_t Read_Input_Registers(uint8_t* Request, uint8_t* Response) //0x04 *********************************************************************************************************
{
	uint16_t Start_Address = ((uint16_t)Request[1] << 8) | Request[2];
	uint16_t Quantity = ((uint16_t)Request[3] << 8) | Request[4];
	uint8_t Byte_Count = (Quantity * 2);

	if (Quantity < 1 || Quantity > 125)
	{
		return Exception_Codes(Request, Response, 0x03);
	}

	if((Start_Address + Quantity) > InputRegisters_Size)
	{
		return Exception_Codes(Request, Response, 0x02) ;
	}

	Response[7] = 0x04;
	Response[8] = Byte_Count;

	for (uint16_t k = 0; k < Byte_Count; k++)
	{
		Response[9 + k] = 0x00 ;
	}

	for(uint16_t i = 0 ; i < Quantity ; i++)
	{
		uint16_t Register_Value = MB_InputRegisters[Start_Address + i];
		Response[9 + (i*2)] = (uint8_t)(Register_Value >> 8) ;
		Response[10 + (i*2)] = (uint8_t)Register_Value ;
	}

	uint16_t PDU_SizE = Byte_Count + 3;
	Response[4] = (PDU_SizE >> 8) & 0xFF;
	Response[5] = PDU_SizE & 0xFF;

	return (PDU_SizE + 6);
}

uint16_t Write_Single_Coil(uint8_t* Request, uint8_t* Response) //0x05 ************************************************************************************************************
{
	uint16_t Write_Addreess = ((uint16_t)Request[1] << 8) | Request[2];
	uint16_t Address_Value = ((uint16_t)Request[3] << 8) | Request[4];

	if(Write_Addreess >= Coil_Map_Size)
	{
		return Exception_Codes(Request, Response, 0x02);
	}

	if(Address_Value == 0xFF00)
	{
		MB_Coils[Write_Addreess] = 1;
		if(Coil_Map[Write_Addreess].Port != NULL)
		{
			HAL_GPIO_WritePin(Coil_Map[Write_Addreess].Port, Coil_Map[Write_Addreess].Pin, GPIO_PIN_SET);
		}
	}

	else if(Address_Value == 0x0000)
	{
		MB_Coils[Write_Addreess] = 0 ;
		if(Coil_Map[Write_Addreess].Port != NULL)
		{
			HAL_GPIO_WritePin(Coil_Map[Write_Addreess].Port, Coil_Map[Write_Addreess].Pin, GPIO_PIN_RESET);
		}
	}

	else
	{
		return Exception_Codes(Request, Response, 0x03);
	}

	Response[7] = 0x05;
	Response[8] = Request[1];
	Response[9] = Request[2];
	Response[10] = Request[3];
	Response[11] = Request[4];

	Response[4] = 0x00;
	Response[5] = 0x06;

	return 12;
}

uint16_t Write_Single_Register(uint8_t* Request, uint8_t* Response) // 0x06 *******************************************************************************************************
{
	uint16_t Write_Address = ((uint16_t)Request[1] << 8) | Request[2];
	uint16_t Address_Value = ((uint16_t)Request[3] << 8) | Request[4];

	if(Write_Address >= HoldingRegisters_Size)
	{
		return Exception_Codes(Request, Response, 0x02);
	}

	MB_HoldingRegisters[Write_Address] = Address_Value ;

	if (Write_Address <= 6)
	{
		uint32_t Success_OR_Fail = Connectivity_Update();
		if(Success_OR_Fail != 0)
		{
			return Exception_Codes(Request, Response, 0x04);
		}
	}

	Response[7] = 0x06;
	Response[8] = Request[1];
	Response[9] = Request[2];
	Response[10] = Request[3];
	Response[11] = Request[4];

	Response[4] = 0x00;
	Response[5] = 0x06;

	return 12;
}

uint16_t Write_Multiple_Coils(uint8_t* Request, uint8_t* Response) //0x0F *********************************************************************************************************
{
	uint16_t Start_Address = ((uint16_t)Request[1] << 8) | Request[2];
	uint16_t Coil_Count = ((uint16_t)Request[3] << 8) | Request[4];
	uint8_t Byte_Count =Request[5] ;

	if (Coil_Count < 1 || Coil_Count > 1968)
	{
		return Exception_Codes(Request, Response, 0x03);
	}

	uint8_t Exception_Check = (Coil_Count / 8);
	if((Coil_Count % 8) > 0)
	{
		Exception_Check ++;
	}
	if(Byte_Count != Exception_Check)
	{
		return Exception_Codes(Request, Response, 0x03);
	}

	if((Start_Address + Coil_Count) > Coil_Map_Size)
	{
		return Exception_Codes(Request, Response, 0x02);
	}

	uint16_t Coil_No = 0 ;
	for(uint16_t a = 0 ; a < Byte_Count ; a++)
	{
		uint8_t Bit_Count;
		if(((a + 1) * 8) <= Coil_Count)
		{
			Bit_Count = 8 ;
		}
		else
		{
			Bit_Count = (Coil_Count % 8);
		}

		for(uint8_t i = 0 ; i < Bit_Count ; i++)
		{
			uint8_t Data_Coil_Value = (Request[6 + a] >> i) ;

			if((Data_Coil_Value & 0x01) == 0x01)
			{
				MB_Coils[Start_Address + Coil_No] = 0x01 ;
				if(Coil_Map[Start_Address + Coil_No].Port != NULL)
				{
					HAL_GPIO_WritePin(Coil_Map[Start_Address + Coil_No].Port, Coil_Map[Start_Address + Coil_No].Pin, GPIO_PIN_SET);
				}
			}
			else
			{
				MB_Coils[Start_Address + Coil_No] = 0x00 ;
				if(Coil_Map[Start_Address + Coil_No].Port != NULL)
				{
					HAL_GPIO_WritePin(Coil_Map[Start_Address + Coil_No].Port, Coil_Map[Start_Address + Coil_No].Pin, GPIO_PIN_RESET);
				}
			}

			Coil_No++;
		}
	}

	Response[7] = 0x0F;
	Response[8] = Request[1];
	Response[9] = Request[2];
	Response[10] = Request[3];
	Response[11] = Request[4];

	Response[4] = 0x00;
	Response[5] = 0x06;

	return 12;
}

uint16_t Write_Multiple_Registers(uint8_t* Request, uint8_t* Response) //0x10 *****************************************************************************************************
{
	uint16_t Start_Address = ((uint16_t)Request[1] << 8) | Request[2];
	uint16_t Register_Count = ((uint16_t)Request[3] << 8) | Request[4];
	uint8_t Byte_Count = Request[5] ;

	if (Register_Count < 1 || Register_Count > 123)
	{
		return Exception_Codes(Request, Response, 0x03);
	}

	if((Register_Count * 2) != Byte_Count)
	{
		return Exception_Codes(Request, Response, 0x03);
	}

	if((Start_Address + Register_Count) > HoldingRegisters_Size)
	{
		return Exception_Codes(Request, Response, 0x02);
	}


	for(uint16_t i = 0 ; i < Register_Count ; i++)
	{
		uint16_t Register_Value = ((uint16_t)Request[6 + (i*2)] << 8) | Request[7 + (i*2)];
		MB_HoldingRegisters[Start_Address + i] = Register_Value ;
	}

	if (Start_Address <= 6)
	{
		NEW_IP_FLAG = 1 ;
	}

	Response[7] = 0x10;
	Response[8] = Request[1];
	Response[9] = Request[2];
	Response[10] = Request[3];
	Response[11] = Request[4];

	Response[4] = 0x00;
	Response[5] = 0x06;

	return 12;
}

uint16_t Mask_Write_Register(uint8_t* Request, uint8_t* Response) //0x16 **********************************************************************************************************
{
	uint16_t Reference_Address = ((uint16_t)Request[1] << 8) | Request[2];
	uint16_t AND_Mask = ((uint16_t)Request[3] << 8) | Request[4];
	uint16_t OR_Mask = ((uint16_t)Request[5] << 8) | Request[6];

	if(Reference_Address >= HoldingRegisters_Size)
	{
		return Exception_Codes(Request, Response, 0x02);
	}

	uint16_t Old_Value = MB_HoldingRegisters[Reference_Address];

	MB_HoldingRegisters[Reference_Address] = (Old_Value & AND_Mask) | (OR_Mask & (~AND_Mask));

	Response[7] = 0x16;
	Response[8]  = Request[1];
	Response[9]  = Request[2];
	Response[10] = Request[3];
	Response[11] = Request[4];
	Response[12] = Request[5];
	Response[13] = Request[6];

	uint16_t Length_Field = 8 ;
	Response[4] = (Length_Field >> 8) & 0xFF;
	Response[5] = Length_Field & 0xFF;

	return 14;
}

uint16_t Read_Write_Multiple_Registers(uint8_t* Request, uint8_t* Response) //0x17 ************************************************************************************************
{
	uint16_t Read_Start_Address = ((uint16_t)Request[1] << 8) | Request[2];
	uint16_t Read_Quantity = ((uint16_t)Request[3] << 8) | Request[4];
	uint16_t Write_Start_Address = ((uint16_t)Request[5] << 8) | Request[6];
	uint16_t Write_Quantity = ((uint16_t)Request[7] << 8) | Request[8];
	uint8_t Write_Byte_Count = Request[9];

	if(Write_Byte_Count != (Write_Quantity * 2))
	{
		return Exception_Codes(Request, Response, 0x03);
	}

	if((Read_Start_Address + Read_Quantity) > HoldingRegisters_Size)
	{
		return Exception_Codes(Request, Response, 0x02);
	}

	if((Write_Start_Address + Write_Quantity) > HoldingRegisters_Size)
	{
		return Exception_Codes(Request, Response, 0x02);
	}

	if((Read_Quantity < 1) || (Read_Quantity > 125))
	{
		return Exception_Codes(Request, Response, 0x03);
	}

	if((Write_Quantity < 1) || (Write_Quantity > 121))
	{
		return Exception_Codes(Request, Response, 0x03);
	}

	for(uint16_t i = 0; i < Write_Quantity; i++)
	{
	MB_HoldingRegisters[Write_Start_Address + i] = ((uint16_t)Request[10 + (i*2)] << 8) | Request[11 + (i*2)];
	}

	Response[7] = 0x17;
	uint16_t Read_Byte_Count = (Read_Quantity * 2) ;
	Response[8] = Read_Byte_Count;

	for (uint16_t k = 0; k < Read_Byte_Count; k++)
	{
		Response[9 + k] = 0x00;
	}

	for(uint16_t i = 0 ; i < Read_Quantity ; i++)
	{
		uint16_t Register_Value = MB_HoldingRegisters[Read_Start_Address + i];
		Response[9 + (i*2)] = (uint8_t)(Register_Value >> 8);
		Response[10 + (i*2)] = (uint8_t)Register_Value;
	}

	uint16_t PDU_SizE = Read_Byte_Count + 3;
	Response[4] = (PDU_SizE >> 8) & 0xFF;
	Response[5] = PDU_SizE & 0xFF;

	return (PDU_SizE + 6);
}

uint16_t Encapsulated_Interface_Transport(uint8_t* Request, uint8_t* Response) //0x2B *********************************************************************************************
{
	uint8_t MEI_Type = Request[1];
	uint8_t Read_Device_ID_Code = Request[2];
	uint8_t Object_ID = Request[3];

	if(MEI_Type != 0x0E)
	{
		return Exception_Codes(Request, Response, 0x01);
	}

	if(Read_Device_ID_Code != 0x01)
	{
		return Exception_Codes(Request, Response, 0x03);
	}

	if(Object_ID > 0x02)
	{
		return Exception_Codes(Request, Response, 0x02);
	}

	Response[7] = 0x2B;
	Response[8] = 0x0E;
	Response[9] = Read_Device_ID_Code;
	Response[10] = 0x01;
	Response[11] = 0x00;
	Response[12] = 0x00;

	uint16_t Position = 14;
	uint8_t Number_Of_Object = 0;

	switch(Object_ID)
	{
	case 0x00 :
		Response[Position] = 0x00;
		Position++;

		uint8_t Vendor_SIZE = sizeof(VendorName) - 1;
		Response[Position] = Vendor_SIZE;
		Position++;

		for(int8_t i = 0 ; i < Vendor_SIZE ; i++)
		{
			Response[Position] = VendorName[i];
			Position++;
		}
		Number_Of_Object++;

	case 0x01 :
		Response[Position] = 0x01;
		Position++;

		uint8_t Product_SIZE = sizeof(ProductCode) -1;
		Response[Position] = Product_SIZE;
		Position++;

		for(int8_t i = 0 ; i < Product_SIZE ; i++)
		{
			Response[Position] = ProductCode[i];
			Position++;
		}
		Number_Of_Object++;

	case 0x02 :
		Response[Position] = 0x02;
		Position++;

		uint8_t Version_SIZE =sizeof(Version) - 1;
		Response[Position] = Version_SIZE;
		Position++;

		for(int8_t i = 0 ; i < Version_SIZE ; i++)
		{
			Response[Position] = Version[i];
			Position++;
		}
		Number_Of_Object++;

		break;
	}

	Response[13] = Number_Of_Object;

	uint16_t Length_Field = Position - 6;
	Response[4] = (Length_Field >> 8) & 0xFF ;
	Response[5] = Length_Field & 0xFF;

	return Position;
}











