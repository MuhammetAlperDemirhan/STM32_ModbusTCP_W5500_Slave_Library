/*
 * Mb_TCP_Register_BOX.c
 *
 *  Created on: Aug 18, 2026
 *      Author: hanal
 */

#include "Mb_TCP_Register_BOX.h"

/* ========================================================================== */
/*                      MODBUS TCP REGISTER & COIL SIZE                       */
/* ========================================================================== */

/**
 * @brief Modbus TCP Register & Coil Array Size Settings.
 *
 * @note Set these sizes to your project's exact needs. If a Master
 *       attempts to read/write an address beyond these limits, the server
 *       will safely return a Modbus Exception Code (Illegal Data Address).
 *
 * @warning MINIMUM SIZE CONSTRAINTS & SYSTEM MEMORY MAP:
 *          - USER_HoldingRegisters_Size MUST be >= 7:
 *            [0-1]: IP Addr, [2-3]: Subnet, [4-5]: Gateway, [6]: TCP Port.
 *          - USER_InputRegisters_Size MUST be >= 4:
 *            [0-2]: MAC Address.
 *            [3]: Network Update (Flash Write) Flag (0xFFFF: Default/Success, 0xAAAA: Busy / Writing..., 0x0000: Update Failed).
 */

#define USER_CoilStatus_Size 1999       /* -> Min:0, Max:2000 */
#define USER_InputStatus_Size 1999      /* -> Min:0, Max:2000 */
#define USER_InputRegisters_Size 125    /* -> Min:4, Max:125  */
#define USER_HoldingRegisters_Size 125  /* -> Min:7, Max:125  */



/* ========================================================================== */
/*                           MODBUS COILS MAPPING                             */
/* ========================================================================== */
 /**
  * @brief Physical and Virtual Coil (Read/Write 1-bit) Mapping.
  *
  * @note HARDWARE COILS: Map physical outputs (e.g., Relays, LEDs) by
  *       assigning the Port and Pin to a specific Modbus address index.
  *       These pins MUST be configured as GPIO_Output in CubeMX!
  *
  * @note VIRTUAL COILS (MARKERS): Any unassigned/skipped index in this
  *       array automatically acts as a 1-bit Virtual Coil (Internal Marker).
  *       They are fully Read/Write accessible via Modbus but have no
  *       physical hardware connection. Ideal for internal logic flags.
  */
const Coil_Degisken Coil_Map[USER_CoilStatus_Size] =
{
		//FORMAT: [Index] = {GPIO_Port, GPIO_Pin},

		//(e.g.)
		/* [0]  = {GPIOD, GPIO_PIN_15}, (e.g., I1)
		 * [1]  = {GPIOD, GPIO_PIN_12}, (e.g., I2)
		 * [2]  = {GPIOD, GPIO_PIN_11}, (e.g., I3)
		 *
		 * [6] = {GPIOD, GPIO_PIN_13},  (e.g., I7)*/

		//(0, 1, 2, 6) Coil (Physical Hardware Output)
		//(3, 4, 5) 1-bit Virtual Coils (Markers)
		[0] = {GPIOD, GPIO_PIN_15},
		[1] = {GPIOD, GPIO_PIN_14},
		[2] = {GPIOD, GPIO_PIN_13},
		[3] = {GPIOD, GPIO_PIN_12},


};
const uint16_t Coil_Map_Size = (sizeof(Coil_Map) / sizeof(Coil_Map[0])) ;



/* ========================================================================== */
/*                      MODBUS DISCRETE INPUTS MAPPING                        */
/* ========================================================================== */

/**
 * @brief Physical Discrete Input (Read-Only 1-bit) Mapping.
 *
 * @note HARDWARE INPUTS: Map physical inputs (e.g., Buttons, Sensors)
 *       by assigning the Port and Pin. These pins MUST be configured
 *       as GPIO_Input in CubeMX (Pull-Up/Pull-Down depends on hardware).
 */

const Discrete_Input_Degisken Discrete_Input_Map[USER_InputStatus_Size] =
{

		//FORMAT: [Index] = {GPIO_Port, GPIO_Pin},

		//(e.g.)
		/* [0]  = {GPIOD, GPIO_PIN_15}, (e.g., Q1)
		 * [1]  = {GPIOD, GPIO_PIN_12}, (e.g., Q2)
		 * [2]  = {GPIOD, GPIO_PIN_11}, (e.g., Q3) */

		//(0, 1, 2) Discrete Input (Physical Hardware Input)

		[0] = {GPIOA, GPIO_PIN_0},

};
const uint16_t  Discrete_Input_Size = (sizeof(Discrete_Input_Map) / sizeof(Discrete_Input_Map[0]));



/* ========================================================================== */
/*                   MODBUS INPUT REGISTERS (16-BIT ANALOG)                   */
/* ========================================================================== */

/**
 * @brief Input Registers (Read-Only 16-bit) Data Mapping.
 *
 * @warning SYSTEM RESERVED ADDRESSES: Indices [0], [1], [2], and [3]
 *          are reserved for MAC Address and Network Update Flag.
 *          Your custom data MUST start from index [4].
 * @warning [0, 1, 2]: MAC Address.
 *          [3]: Network Update (Flash Write) Flag (0xFFFF: Default/Success, 0xAAAA: Busy / Writing..., 0x0000: Update Failed).
 *
 * @note HOW TO USE: Assign your dynamic 16-bit variables (e.g., ADC readings, sensor data)
 *       directly to this array inside your main() callbacks.
 *
 * @note Usage Example in main.c:
 *       @code
 *       // Read ADC value and send it to the Master via Input Register 4
 *       MB_InputRegisters[4] = HAL_ADC_GetValue(&hadc1);
 *       MB_InputRegisters[5] = System_Temperature_Variable;
 *       @endcode
 */



/* ========================================================================== */
/*                 MODBUS HOLDING REGISTERS (16-BIT R/W)                      */
/* ========================================================================== */

/**
 * @brief Holding Registers (Read/Write 16-bit) Data Mapping.
 *
 * @warning SYSTEM RESERVED ADDRESSES: Indices [0] to [6] are strictly
 *          reserved for network settings (IP, Subnet, Gateway, TCP Port).
 *          Your custom data MUST start from index [7].
 * @warning [0-1]: IP Address.
 *          [2-3]: Subnet Address.
 *          [4-5]: Gateway Address.
 *          [6]: TCP Port.
 *
 * @note HOW TO USE: Holding registers are bidirectional. You can write
 *       data to them for the Master to read, or read data from them that
 *       the Master has written (e.g., Setpoints, PID Parameters).
 *
 * @note Usage Example in main.c:
 *       @code
 *       // 1. Writing data to be read by the Master
 *       MB_HoldingRegisters[7] = System_Running_Time;
 *
 *       // 2. Reading a Setpoint configured by the Master
 *       Target_Voltage = MB_HoldingRegisters[8];
 *       @endcode
 */



/* ========================================================================== */
/* ========================================================================== */
/*                 SYSTEM INTERNAL VARIABLES (DO NOT MODIFY!)                 */
/* ========================================================================== */
/* ========================================================================== */
const uint16_t CoilStatus_Size = USER_CoilStatus_Size ;
const uint16_t InputStatus_Size = USER_InputStatus_Size;
const uint16_t InputRegisters_Size = USER_InputRegisters_Size;
const uint16_t HoldingRegisters_Size = USER_HoldingRegisters_Size;

uint8_t MB_Coils[USER_CoilStatus_Size]; // 0xxxx , R/W , 1-bit
uint8_t MB_DiscreteInput[USER_InputStatus_Size]; // 1xxxx, R , 1-bit
uint16_t MB_InputRegisters[USER_InputRegisters_Size]; //3xxxx, R , 16-bit/word/2-byte
uint16_t MB_HoldingRegisters[USER_HoldingRegisters_Size]; //4xxxx , R/W , 16-bit/word/2-byte
