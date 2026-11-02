/**
 * @file    Mb_TCP_USER_BOX.h
 * @author  Muhammet Alper DEMIRHAN - hanalper9@gmail.com
 * @brief   Modbus TCP/IP Slave (Server) library for STM32 MCUs.
 * @date    Sep 02, 2026
 * @version v1.1.0
 */

#ifndef INC_MB_TCP_USER_BOX_H_
#define INC_MB_TCP_USER_BOX_H_

#include "Mb_TCP_W5500_Settings_Core.h"
#include "Mb_TCP_Register_BOX.h"
extern uint32_t MB_FlashStart_Address;
extern uint8_t NEW_DATA_FLAG[];
extern SPI_HandleTypeDef* Global_hspiX;
extern GPIO_TypeDef* Global_CS_Port;
extern uint16_t Global_CS_PIN;
extern uint16_t Global_timeout_tiM;
extern uint8_t Global_timeout_reT;
extern GPIO_TypeDef* Global_W5500_Rst_GPIOx;
extern uint16_t Global_W5500_Rst_PIN_x;
#define MB_TCP_BUFFER_SIZE 260
#define MB_TCP_MBAP_SIZE 7

extern uint8_t  Reading_MAC_Address[6];
extern uint8_t  Reading_IP_Address[4];
extern uint8_t  Reading_Subnet_Address[4];
extern uint8_t  Reading_Gateway_Address[4];
extern uint16_t Reading_TCP_Port;

/* ========================================================================== */
/*                         TARGET MCU SELECTION                               */
/* ========================================================================== */

/**
 * @brief Just uncomment the MCU family you will use.
 *
 * @note This section customizes the library according to your processor.
 *
 * @warning You MUST select exactly ONE microcontroller. If no MCU is
 * selected, or multiple are selected, the library will not compile.
 */

//#define STM32F0
//#define STM32F1
//#define STM32F2
//#define STM32F3
#define STM32F4
//#define STM32F7
//#define STM32G0
//#define STM32G4
//#define STM32L0
//#define STM32L1
//#define STM32L4
//#define STM32L5
//#define STM32C0


/* ========================================================================== */
/*                      DEVICE IDENTIFICATION SETTINGS                        */
/* ========================================================================== */

/**
 * @brief Edit the strings below to customize your device identification.
 *
 * @note These values are returned when an FC 0x2B (Read Device Identification)
 * request is received by the server.
 */

#define VENDOR_NAME " MUHAMMET ALPER DEMIRHAN "
#define PRODUCT_CODE " TCP LIBRARY "
#define VERSION " v1.1.0 "

/* ========================================================================== */
/*                     MODBUS TCP SOCKET CONFIGURATIONS                       */
/* ========================================================================== */

/**
 * @brief Defines the maximum number of simultaneous Modbus TCP sockets.
 *
 * @note This value directly affects the RAM usage of the STM32 and the
 * hardware buffer allocation of the W5500 chip.
 *    - Valid range: 1 to 8.
 *    - Default: 4 (Balances memory and multi-client capabilities).
 */

#define MODBUS_MAX_SOCKETS  4

/* ========================================================================== */
/*                      DEFAULT NETWORK CONFIGURATIONS                        */
/* ========================================================================== */

/**
 * @brief Default network settings for the W5500 Ethernet module.
 *
 * @note These are default startup values. Network parameters can be
 *       dynamically read or changed via Modbus TCP commands:
 *       - MAC Addr: Read-Only via Input Registers (0, 1, 2)
 *       - Flash Flag : Read-Only via Input Register (3) (0xFFFF: OK, 0x0000: Fail)
 *       - IP Addr : R/W via Holding Registers (0, 1)
 *       - Subnet  : R/W via Holding Registers (2, 3)
 *       - Gateway : R/W via Holding Registers (4, 5)
 *       - TCP Port: R/W via Holding Register (6)
 *
 * @warning If updated via Modbus TCP, the device MUST be rebooted
 * (power cycled) for the new network settings to take effect!
 */

#define MAC_ADDR        {0x00, 0x08, 0xDC, 0x11, 0x22, 0x33}
#define IP_ADDR         {192, 168, 1, 150}
#define Subnet_ADDR     {255, 255, 255, 0}
#define Gateway_ADDR    {192, 168, 1, 1}
#define TCP_PORT         502


/* ========================================================================== */
/*                          CORE LIBRARY FUNCTIONS                            */
/* ========================================================================== */

/**
 * @brief Initializes the W5500 hardware and Modbus TCP Server.
 *
 * @note This function must be called in main() before the while(1) loop (e.g. USER CODE BEGIN 2).
 *
 * @param  - CS_Port: SPI Chip Select Port (e.g. GPIOA).
 * @param  - CS_PIN:  SPI Chip Select Pin (e.g. GPIO_PIN_2).
 *         -> HW Config: Output Level High, mode Push Pull, Pull-Up, Speed HIGH.
 *
 * @param  - W5500_Rst_GPIOx: W5500 Hardware Reset Port (e.g., GPIOA).
 * @param  - W5500_Rst_PIN_x: W5500 Hardware Reset Pin (e.g., GPIO_PIN_1).
 *         -> HW Config: Output Level High, mode Push Pull, Pull-Up, Speed HIGH or Medium.
 *
 * @param  - hspiX: Pointer to the SPI handle (e.g. &hspi1).
 *
 * @param  - timeout_tiM: Network connection timeout in ms (e.g., 2000).
 * @param  - timeout_reT: Number of timeout retries (e.g., 3).
 *
 * @param  - Flash_Address: Starting flash address for settings (e.g., 0x080E0000).
 *                          @warning The library writes exactly 16 bytes (4 Words).
 *                          Do not select an address too close to the sector end!
 *                          @warning During an update, the entire Sector/Page will be
 *                          ERASED. Do not share this sector with other data!
 *
 * @param  - RstBtn_GPIOx: Factory Reset Port (e.g., GPIOB or NULL).
 * @param  - RstBtn_GPIO_PIN_x: Factory Reset Pin (e.g., GPIO_PIN_5 or 0).
 *                           -> HW Config: Input Mode, Pull-Down Resistor.
 *                           @note Button must switch VDD to GPIO. If
 *                           unused, pass (NULL, 0). Restores default network
 *                           settings if pressed during power-up.
 *
 * @param  - Led_GPIOx: Factory Reset LED Port (e.g., GPIOD or NULL).
 * @param  - Led_GPIO_PIN_x: Factory Reset LED Pin (e.g., GPIO_PIN_15 or 0).
 *                           -> HW Config: Output Mode, Push-Pull.
 *                           @note Linked to the Reset button. Indicates
 *                           the update to default settings if the button is
 *                           held during startup. If unused, pass (NULL, 0).
 *                           Sequence: Blinks 2s -> Solid -> Off (Complete).
 *
 * @retval  - uint8_t:  0: Success, 1: Initialization Failed.
 */

uint8_t ModbusTCP_IP_INIT_Function(GPIO_TypeDef* CS_Port,
		                           uint16_t CS_PIN,
								   GPIO_TypeDef* W5500_Rst_GPIOx,
								   uint16_t W5500_Rst_PIN_x,
								   SPI_HandleTypeDef* hspiX,
								   uint16_t timeout_tiM,
								   uint8_t timeout_reT,
								   uint32_t Flash_Address,
		                           GPIO_TypeDef* RstBtn_GPIOx,
								   uint16_t RstBtn_GPIO_PIN_x,
								   GPIO_TypeDef* Led_GPIOx,
								   uint16_t Led_GPIO_PIN_x);


/**
 * @brief Main Modbus TCP server polling and processing function.
 *
 * @note This function must be called continuously inside the main
 *       while(1) loop to handle incoming client requests.
 *
 * @param  - Socket: W5500 hardware socket number for the server (e.g., 0).
 *                   @note Valid range is 0 to 7.
 *
 * @warning Do NOT use blocking delays (e.g., HAL_Delay) in the while(1) loop,
 *          as it will cause Modbus TCP connection timeouts.
 *
 * @retval  - None
 */

void ModbusTCP_IP_WHILE_Function(void);


/**
 * @brief Generates or assigns a user-specific custom MAC address.
 *
 * @note OPTIONAL AUTOMATION: Instead of manually editing MAC addresses in
 *       the code for each new device, you can write an algorithm here
 *       (e.g. using STM32's unique UID) to assign a dynamic MAC automatically.
 *
 * @note HOW TO USE: Open the "Mb_TCP_USER_BOX.c" file, find this
 *       empty function, and write your custom MAC generation logic inside it.
 *
 * @note If unused, you can use the default static MAC defined at the top
 *       of the file. This function is called automatically during initialization.
 *
 * @warning DO NOT modify anything else in the "Mb_TCP_USER_BOX.c" file!
 *          Write your MAC algorithm ONLY inside the body of this function.
 *
 * @retval  - None
 */

void USER_SPECIFIC_MAC_ADDR_ALGORITHM(uint8_t *Mac_ADDR);



/* ========================================================================== */
/*                          Go to Mb_TCP_Register_BOX.c                       */
/* ========================================================================== */





#endif /* INC_MB_TCP_USER_BOX_H_ */
