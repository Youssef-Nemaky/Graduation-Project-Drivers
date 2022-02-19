#ifndef UART_H
#define UART_H

#include "UART_Cfg.h"
#include "std_types.h"
#include "uart_regs.h"
#include "MCU_Config.h"

/*******************************************************************************
 *                              Module Data Types                              *
 *******************************************************************************/
#define NUMBER_OF_UART_MODULES       (8U)

#define CLEAR_WORD_LENGTH_BITS_MASK  (0xFF9F)
#define UART_WORD_LENGTH_SHIFT_MASK  (4)

typedef enum
{
    UART0_MODULE_NUMBER,
    UART1_MODULE_NUMBER,
    UART2_MODULE_NUMBER,
    UART3_MODULE_NUMBER,
    UART4_MODULE_NUMBER,
    UART5_MODULE_NUMBER,
    UART6_MODULE_NUMBER,
    UART7_MODULE_NUMBER,
} Uart_ModuleNumber;

typedef enum{
    UART_OFF,
    UART_ON
} Uart_StatusMode;

/* Enumeration to define the character size */
typedef enum
{
	UART_FIVE_BITS, UART_SIX_BITS, UART_SEVEN_BITS, UART_EIGHT_BITS
}Uart_DataBits;

/* Enumeration to define the parity type */
typedef enum
{
	UART_NO_PARITY, UART_ODD_PARITY, UART_EVEN_PARITY
}Uart_Parity;

/* Enumeration to define the number of stop bits */
typedef enum
{
	UART_ONE_STOP_BIT, UART_TWO_STOP_BIT
}Uart_StopBits;

typedef enum
{
    UART_POLLING, UART_INTERRUPT
}Uart_Mode;

typedef struct
{
    Uart_StatusMode statusMode;
	Uart_DataBits noDataBits;
	Uart_Parity parityType;
	Uart_StopBits noStopBits;   /*Number of stop bits*/
    Uart_Mode mode;
	uint32 baudRate;
}Uart_ConfigModule;

/* Data Structure required for initializing the UART Driver */
typedef struct UART_ConfigType
{
	const Uart_ConfigModule UARTModules[NUMBER_OF_UART_MODULES];
} Uart_ConfigType;



/*******************************************************************************
 *                       External Variables                                    *
 *******************************************************************************/
/* Extern PB structures to be used by I2c and other modules */
extern const Uart_ConfigType Uart_Configuration;

/**************************************DON'T KNOW WHAT THESE ARE*******************************/
extern uint8 flag;
extern uint8 RecDataBuffer;


/*Functions Prototypes*/


/********************************

 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const Uart_ConfigType * ConfigPtr);
/*
 * Description :
 * Functional responsible for send byte to another device.
 */
void UART_sendByte(const uint8 data);
/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void);
/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str);
/*
 * Description :
 * receive the required string from another UART device.
 */
void UART_receiveString(uint8 *Str);
/*
 *Description :
 * Enable one of Uart Modules from UART0 to UART7
 * */
void UART_moduleEnable(uint8 module);
#endif
