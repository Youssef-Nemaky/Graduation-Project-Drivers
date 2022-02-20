
#include "Uart.h"
#include "Common_Macros.h"

STATIC const Uart_ConfigModule * Uart_ModulePtr = NULL_PTR;

/********************************

 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */

void Uart_Init(const Uart_ConfigType * ConfigPtr)
{
    /* loop on the configuration structure array */
    /* Enable the clock for the registers */
    uint8 uartCounter = 0;
    volatile uint32 * uartBaseAddressPtr = NULL_PTR;
    uint16 integerBaudRateDivisor = 0;
    uint8 fractionalBaurdRateDivisor = 0;

    Uart_ModulePtr = ConfigPtr->UARTModules;

    for(uartCounter = 0; uartCounter < NUMBER_OF_UART_MODULES; uartCounter++){
        /* Skip the configuration if it is a disabled (NOT USED) UART */
        if(Uart_ModulePtr[uartCounter].statusMode == UART_OFF){
            continue;
        }

        switch (uartCounter)
        {
        case UART0_MODULE_NUMBER:
            uartBaseAddressPtr = (volatile uint32 *)UART0_BASE_ADDRESS;
            break;
        case UART1_MODULE_NUMBER:
            uartBaseAddressPtr = (volatile uint32 *)UART1_BASE_ADDRESS;
            break;
        case UART2_MODULE_NUMBER:
            uartBaseAddressPtr = (volatile uint32 *)UART2_BASE_ADDRESS;
            break;
        case UART3_MODULE_NUMBER:
            uartBaseAddressPtr = (volatile uint32 *)UART3_BASE_ADDRESS;
            break;
        case UART4_MODULE_NUMBER:
            uartBaseAddressPtr = (volatile uint32 *)UART4_BASE_ADDRESS;
            break;
        case UART5_MODULE_NUMBER:
            uartBaseAddressPtr = (volatile uint32 *)UART5_BASE_ADDRESS;
            break;
        case UART6_MODULE_NUMBER:
            uartBaseAddressPtr = (volatile uint32 *)UART6_BASE_ADDRESS;
            break;
        case UART7_MODULE_NUMBER:
            uartBaseAddressPtr = (volatile uint32 *)UART7_BASE_ADDRESS;
            break;
        default:
            break;
        }

        /* Enable the clock for the uart registers */
        SET_BIT(UART_RCGC_REG, uartCounter); 

        /* Disable the uart module by clearing the UARTEN BIT IN UARTCTL Register*/
        *(volatile uint32 *)((volatile uint8 *)uartBaseAddressPtr + UART_CTL_REG_OFFSET) &= ~(1<<UART_EN_BIT);

        integerBaudRateDivisor = F_CPU / (16 * Uart_ModulePtr[uartCounter].baudRate);
        *(volatile uint32 *)((volatile uint8 *)uartBaseAddressPtr + UART_IBRD_REG_OFFSET) = integerBaudRateDivisor;

        fractionalBaurdRateDivisor = (uint8)((F_CPU / (16.0f * Uart_ModulePtr[uartCounter].baudRate) - integerBaudRateDivisor) * 64 + 0.5);
        *(volatile uint32 *)((volatile uint8 *)uartBaseAddressPtr + UART_FBRD_REG_OFFSET) = fractionalBaurdRateDivisor;

        /* Set the setting of the frame of the UART */

        /* Setting the parity bits */
        if(Uart_ModulePtr[uartCounter].parityType == UART_NO_PARITY){
            /* Parity is disabled and no parity bit is added to the data frame */
            *(volatile uint32 *)((volatile uint8 *)uartBaseAddressPtr + UART_LCRH_REG_OFFSET) &= ~(1<<UART_PARITY_EN_BIT);
        } else {
            /* Parity checking and generation is enabled */
            *(volatile uint32 *)((volatile uint8 *)uartBaseAddressPtr + UART_LCRH_REG_OFFSET) |= (1<<UART_PARITY_EN_BIT);

            /* Check whether it's even or odd parity */
            if(Uart_ModulePtr[uartCounter].parityType == UART_EVEN_PARITY){
                /* Even parity generation and checking is performed during transmission and reception,
                 which checks for an even number
                 of 1s in data and parity bits */
                *(volatile uint32 *)((volatile uint8 *)uartBaseAddressPtr + UART_LCRH_REG_OFFSET) |= (1<<UART_EVEN_PARITY_SELECT_BIT);
            } else {
                /* Odd parity is performed, which checks for an odd number of 1s */
                *(volatile uint32 *)((volatile uint8 *)uartBaseAddressPtr + UART_LCRH_REG_OFFSET) &= ~(1<<UART_EVEN_PARITY_SELECT_BIT);
            }
        }

        /* Setting the stop bits */
        if(Uart_ModulePtr[uartCounter].noStopBits == UART_TWO_STOP_BIT){
            /* Two stop bits are transmitted at the end of a frame. The receive
               logic does not check for two stop bits being received. */
            *(volatile uint32 *)((volatile uint8 *)uartBaseAddressPtr + UART_LCRH_REG_OFFSET) |= (1<<UART_TWO_STOP_BITS_SELECT_BIT);
        } else {
            *(volatile uint32 *)((volatile uint8 *)uartBaseAddressPtr + UART_LCRH_REG_OFFSET) &= ~(1<<UART_TWO_STOP_BITS_SELECT_BIT);
        }

        /* Setting the number of data bits */
        *(volatile uint32 *)((volatile uint8 *)uartBaseAddressPtr + UART_LCRH_REG_OFFSET) &= UART_CLEAR_WORD_LENGTH_BITS_MASK;
        *(volatile uint32 *)((volatile uint8 *)uartBaseAddressPtr + UART_LCRH_REG_OFFSET) |= (Uart_ModulePtr[uartCounter].noDataBits)<<UART_WORD_LENGTH_SHIFT_MASK;

        /* intializing the clock by giving it zero value to use system clock */
        *(volatile uint32 *)((volatile uint8 *)uartBaseAddressPtr + UART_CC_REG_OFFSET) = 0;

        /* Enable the uart module by setting the UARTEN BIT IN UARTCTL Register*/
        *(volatile uint32 *)((volatile uint8 *)uartBaseAddressPtr + UART_CTL_REG_OFFSET) |= (1<<UART_EN_BIT);


    }
}

void Uart_SendByte(Uart_ModuleNumber uartModuleNumber ,const uint8 byteToSend){
    volatile uint32 * uartBaseAddressPtr = NULL_PTR;

    switch (uartModuleNumber){
    case UART0_MODULE_NUMBER:
        uartBaseAddressPtr = (volatile uint32*)UART0_BASE_ADDRESS;
        break;
    case UART1_MODULE_NUMBER:
        uartBaseAddressPtr = (volatile uint32*)UART1_BASE_ADDRESS;
        break;
    case UART2_MODULE_NUMBER:
        uartBaseAddressPtr = (volatile uint32*)UART2_BASE_ADDRESS;
        break;
    case UART3_MODULE_NUMBER:
        uartBaseAddressPtr = (volatile uint32*)UART3_BASE_ADDRESS;
        break;
    case UART4_MODULE_NUMBER:
        uartBaseAddressPtr = (volatile uint32*)UART4_BASE_ADDRESS;
        break;
    case UART5_MODULE_NUMBER:
        uartBaseAddressPtr = (volatile uint32*)UART5_BASE_ADDRESS;
        break;
    case UART6_MODULE_NUMBER:
        uartBaseAddressPtr = (volatile uint32*)UART6_BASE_ADDRESS;
        break;
    case UART7_MODULE_NUMBER:
        uartBaseAddressPtr = (volatile uint32*)UART7_BASE_ADDRESS;
        break;
    default:
        break;
    }

    /* wait until Tx buffer is not full before giving it another byte */
    while((*(volatile uint32 *)((volatile uint8 *)uartBaseAddressPtr + UART_F_REG_OFFSET)) & UART_TXFF_MASK);
    
    /* Write the byte to the data register of the uart to start the transmission */
    *(volatile uint32 *)((volatile uint8 *)uartBaseAddressPtr + UART_D_REG_OFFSET) = byteToSend;
}

sint8 Uart_ReceiveByte(Uart_ModuleNumber uartModuleNumber){
    sint8 dataReceived = 0;
    volatile uint32 * uartBaseAddressPtr = NULL_PTR;

    switch (uartModuleNumber){
    case UART0_MODULE_NUMBER:
        uartBaseAddressPtr = (volatile uint32*)UART0_BASE_ADDRESS;
        break;
    case UART1_MODULE_NUMBER:
        uartBaseAddressPtr = (volatile uint32*)UART1_BASE_ADDRESS;
        break;
    case UART2_MODULE_NUMBER:
        uartBaseAddressPtr = (volatile uint32*)UART2_BASE_ADDRESS;
        break;
    case UART3_MODULE_NUMBER:
        uartBaseAddressPtr = (volatile uint32*)UART3_BASE_ADDRESS;
        break;
    case UART4_MODULE_NUMBER:
        uartBaseAddressPtr = (volatile uint32*)UART4_BASE_ADDRESS;
        break;
    case UART5_MODULE_NUMBER:
        uartBaseAddressPtr = (volatile uint32*)UART5_BASE_ADDRESS;
        break;
    case UART6_MODULE_NUMBER:
        uartBaseAddressPtr = (volatile uint32*)UART6_BASE_ADDRESS;
        break;
    case UART7_MODULE_NUMBER:
        uartBaseAddressPtr = (volatile uint32*)UART7_BASE_ADDRESS;
        break;
    default:
        break;
    }

    /* wait until Rx buffer is not empty */
    while(((*(volatile uint32 *)((volatile uint8 *)uartBaseAddressPtr + UART_F_REG_OFFSET)) & UART_RXEF_MASK) != 0);

    /* Read the data from the uart data register and save it into dataReceived variable */
    dataReceived = *(volatile uint32 *)((volatile uint8 *)uartBaseAddressPtr + UART_D_REG_OFFSET);

    /* return the read register */
    return dataReceived;

} 