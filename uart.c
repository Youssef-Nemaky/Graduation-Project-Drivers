
#include "uart.h"
#include"common_macros.h"

STATIC const Uart_ConfigModule * Uart_ModulePtr = NULL_PTR;

/********************************

 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */

void UART_init(const Uart_ConfigType * ConfigPtr)
{
    /* loop on the configuration structure array */
    /* Enable the clock for the registers */
    /* y3ml al 7gat al gamda al anta 3mltha */
    uint8 uart_counter = 0;
    volatile uint32 * uartBaseAddressPtr = NULL_PTR;
    uint16 integerBaudRateDivisor = 0;
    uint8 fractionalBaurdRateDivisor = 0;

    Uart_ModulePtr = ConfigPtr->UARTModules;

    for(uart_counter = 0; uart_counter < NUMBER_OF_UART_MODULES; uart_counter++){
        /* Skip the configuration if it is a disabled (NOT USED) UART */
        if(Uart_ModulePtr[uart_counter].statusMode == UART_OFF){
            continue;
        }

        switch (uart_counter)
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
        SET_BIT(UART_RCGC_REG, uart_counter); 

        /* Disable the uart module by clearing the UARTEN BIT IN UARTCTL Register*/
        *(((uint32 *)(uint8 *)uartBaseAddressPtr + UART_CTL_REG_OFFSET)) &= ~(1<<UART_EN_BIT);
        integerBaudRateDivisor = F_CPU / (16 * Uart_ModulePtr[uart_counter].baudRate);
        *(volatile uint32 *)((volatile uint8 *)uartBaseAddressPtr + UART_IBRD_REG_OFFSET) = integerBaudRateDivisor;

        fractionalBaurdRateDivisor = (uint8)((F_CPU / (16.0f * Uart_ModulePtr[uart_counter].baudRate) - integerBaudRateDivisor) * 64 + 0.5);
        *(volatile uint32 *)((volatile uint8 *)uartBaseAddressPtr + UART_FBRD_REG_OFFSET) = fractionalBaurdRateDivisor;

        /* Set the setting of the frame of the UART */

        /* Setting the parity bits */
        if(Uart_ModulePtr[uart_counter].parityType == UART_NO_PARITY){
            /* Parity is disabled and no parity bit is added to the data frame */
            *(volatile uint32 *)((volatile uint8 *)uartBaseAddressPtr + UART_LCRH_REG_OFFSET) &= ~(1<<UART_PARITY_EN_BIT);
        } else {
            /* Parity checking and generation is enabled */
            *(volatile uint32 *)((volatile uint8 *)uartBaseAddressPtr + UART_LCRH_REG_OFFSET) |= (1<<UART_PARITY_EN_BIT);

            /* Check whether it's even or odd parity */
            if(Uart_ModulePtr[uart_counter].parityType == UART_EVEN_PARITY){
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
        if(Uart_ModulePtr[uart_counter].noStopBits == UART_TWO_STOP_BIT){
            /* Two stop bits are transmitted at the end of a frame. The receive
               logic does not check for two stop bits being received. */
            *(volatile uint32 *)((volatile uint8 *)uartBaseAddressPtr + UART_LCRH_REG_OFFSET) |= (1<<UART_TWO_STOP_BITS_SELECT_BIT);
        } else {
            *(volatile uint32 *)((volatile uint8 *)uartBaseAddressPtr + UART_LCRH_REG_OFFSET) &= ~(1<<UART_TWO_STOP_BITS_SELECT_BIT);
        }

        /* Setting the number of data bits */
        *(volatile uint32 *)((volatile uint8 *)uartBaseAddressPtr + UART_LCRH_REG_OFFSET) &= CLEAR_WORD_LENGTH_BITS_MASK;
        *(volatile uint32 *)((volatile uint8 *)uartBaseAddressPtr + UART_LCRH_REG_OFFSET) |= (Uart_ModulePtr[uart_counter].noDataBits)<<UART_WORD_LENGTH_SHIFT_MASK;

        /* intializing the clock by giving it zero value to use system clock */
        *(volatile uint32 *)((volatile uint8 *)uartBaseAddressPtr + UART_CC_REG_OFFSET) = 0;

        /* Enable the uart module by clearing the UARTEN BIT IN UARTCTL Register*/
        *(((uint32 *)(uint8 *)uartBaseAddressPtr + UART_CTL_REG_OFFSET)) |= (1<<UART_EN_BIT);


    }
}

#ifdef d7k
/*
 * Description :
 * Functional responsible for send byte to another device.
 */
void UART_sendByte(const uint8 data)
{

	while((UARTFR&(1<<3)));          /*wait if the uart busy transmitting data*/
	/*bit 1 is for UART Parity Enable
	 * bit 2 is selecting even parity
	 *bit 3 is for UART Parity Enable
	 *bit 4 is UART Enable FIFOs
	 *bit 5 and bit 6 should be set to let the length of the word be 8 bits  */
	UARTLCRH=0x7E;          /*Enable bits from 1 to 6 in this register*/

	SET_BIT(UARTCTL,RTSEN);         /*this bit in the control register enables request to send)*/
	SET_BIT(UARTFR,5);              /* The fifo full */




	SET_BIT(UARTCTL,8);
	/*UART Transmit Enable*/
	SET_BIT(UARTDMACTL,1);
	/*�DMA for the transmit FIFO is enabled*/

	UARTDR=(data&0xFF);
	/*The data register is placed in first 8 bits */
	SET_BIT(UARTCTL,4);
	/*This bit is set only after all transmitted data, including
stop bits, have cleared the serializer*/
	SET_BIT(UARTFR,7)
	/*The transmit FIFO is empty*/

}
/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void)
{
	while(UARTFR&(1<<4));                    /*Till UART Receive FIFO Empty*/
	SET_BIT(UARTLCRH,4);                     /* UART Enable FIFOs*/
	SET_BIT(UARTCTL,9);                      /*UART Receive Enable*/


	SET_BIT(UARTDMACTL,0);                    /*Receive DMA Enable*/
	CLEAR_BIT(UARTFR,6);                        /*The reciever can recieve data by clearing this bit*/

}

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */

void UART_sendString(const uint8 *Str)
{
	uint8 i=0;                      /*i is the counter value */
	while( Str[i]!='/0')             /*till meeting the null character*/
	{

		UART_sendByte(Str[i]);       /*the uart send a byte which is first character in the string */
		i++;                        /* counter value increments */
	}
}
/*
 * Description :
 * receive the required string from another UART device.
 */
void UART_receiveString(uint8 *Str)
{
	uint8 j=0;                        /*j is the counter value */
	Str[j]=UART_recieveByte;           /*The string will receive first byte*/
	while(Str[j]!='#')                  /*till meeting the null character*/
	{
		j++;                            /* counter value increments */
		UART_recieveByte(Str[j]);       /*receive byte then another byte*/



	}
	Str[j] = '\0';/*null character is adjusted at the end of the received string*/
}/*This Function Enables any Uart Module from 0 to 7*/
void UART_moduleEnable(uint8 module)
{

	if(module==0)
	{



		SET_BIT(RCGCUART ,0);  /* enable clock to UART0 */
		SET_BIT(RCGCGPIO,0);  /* enable clock to PORTA for PA1/Rx */

		Delay(1);


		/* UART5 TX5 and RX5 use PA0 and PA1. Configure them digital and enable alternate function */

		GPIO_PORTA_PCTL_REG  &= 0xFFFFFFF0;  	    /* Clear PMCx bits for PA0 to use it as GPIO pin */
		GPIO_PORTA_PCTL_REG  &= 0xFFFFFF0F;       	    /* Clear PMCx bits for PA1 to use it as GPIO pin */

		GPIO_PORTA_DIR_REG  &= ~(1<<0);      /* Configure PA0 as input pin */
		GPIO_PORTA_AFSEL_REG |= (1<<0);      /* enable alternative function on PA0 */
		GPIO_PORTA_DIR_REG  |= (1<<1);      /* Configure PA1 as output pin */
		GPIO_PORTA_AFSEL_REG |= (1<<1);      /* enable alternative function on PA1 */ 

		GPIO_PORTA_PUR_REG   |= (1<<0);       /* Enable pull-up on PA0 */
		GPIO_PORTA_DEN_REG   |= (1<<0);/* Enable Digital I/O on PA0 *
			    GPIO_PORTA_PUR_REG   |= (1<<1);       /* Enable pull-up on PA1 */
		GPIO_PORTA_DEN_REG   |= (1<<1); /* Enable Digital I/O on PA1 */
	}
	else if(module==1)
	{

		SET_BIT(RCGCUART ,1);  /* enable clock to UART1 */
		SET_BIT(RCGCGPIO,2);  /* enable clock to PORTC for PA1/Rx */


		/* UART4 PC4 and PC5 Configure them digital and enable alternate function */

		GPIO_PORTC_PCTL_REG  &= 0xFFF0FFFF;  	    /* Clear PMCx bits for PC4 to use it as GPIO pin */
		GPIO_PORTC_PCTL_REG  &= 0xFF0FFFFF;       	    /* Clear PMCx bits for PC5 to use it as GPIO pin */

		GPIO_PORTC_DIR_REG  &= ~(1<<4);      /* Configure PC4 as input pin */
		GPIO_PORTC_AFSEL_REG |= (1<<4);      /* enable alternative function on PC4 */
		GPIO_PORTC_DIR_REG  |= (1<<5);      /* Configure PC5 as output pin */
		GPIO_PORTC_AFSEL_REG |= (1<<5);      /* enable alternative function on PC5 */ 

		GPIO_PORTC_PUR_REG   |= (1<<4);       /* Enable pull-up on PC4 */
		GPIO_PORTC_DEN_REG   |= (1<<4);/* Enable Digital I/O on PC4 */
		GPIO_PORTC_PUR_REG   |= (1<<5);       /* Enable pull-up on PC5 */
		GPIO_PORTC_DEN_REG   |= (1<<5); /* Enable Digital I/O on PC5 */


	}

	else if(module==2)
	{
		SET_BIT(RCGCUART ,2);  /* enable clock to UART2 */
		SET_BIT(RCGCGPIO,3);  /* enable clock to PORTD*/


		/* UART1 PD6 and PD7 Configure them digital and enable alternate function */

		GPIO_PORTD_PCTL_REG  &= 0xF0FFFFFF;  	    /* Clear PMCx bits for PD6 to use it as GPIO pin */
		GPIO_PORTD_PCTL_REG  &= 0x0FFFFFFF;       	    /* Clear PMCx bits for PD7 to use it as GPIO pin */

		GPIO_PORTD_DIR_REG  &= ~(1<<6);      /* Configure PD6 as input pin */
		GPIO_PORTD_AFSEL_REG |= (1<<6);      /* enable alternative function on PD6 */
		GPIO_PORTD_DIR_REG  |= (1<<7);      /* Configure PD7 as output pin */
		GPIO_PORTD_AFSEL_REG |= (1<<7);      /* enable alternative function on PD7 */ 

		GPIO_PORTD_PUR_REG   |= (1<<6);       /* Enable pull-up on PD6 */
		GPIO_PORTD_DEN_REG   |= (1<<6);/* Enable Digital I/O on PD6 */
		GPIO_PORTD_PUR_REG   |= (1<<7);       /* Enable pull-up on PD7 */
		GPIO_PORTD_DEN_REG   |= (1<<7); /* Enable Digital I/O on PD7 */
	}
	else if(module==3)
	{
		SET_BIT(RCGCUART ,3);  /* enable clock to UART3 */
		SET_BIT(RCGCGPIO,2);  /* enable clock to PORTC */


		/* UART3 PC6 and PC7 Configure them digital and enable alternate function */

		GPIO_PORTD_PCTL_REG  &= 0xF0FFFFFF;  	    /* Clear PMCx bits for PC6 to use it as GPIO pin */
		GPIO_PORTD_PCTL_REG  &= 0x0FFFFFFF;       	    /* Clear PMCx bits for PC7 to use it as GPIO pin */

		GPIO_PORTC_DIR_REG  &= ~(1<<6);      /* Configure PC6 as input pin */
		GPIO_PORTC_AFSEL_REG |= (1<<6);      /* enable alternative function on PC6 */
		GPIO_PORTC_DIR_REG  |= (1<<7);      /* Configure PD7 as output pin */
		GPIO_PORTC_AFSEL_REG |= (1<<7);      /* enable alternative function on PC7 */ 

		GPIO_PORTC_PUR_REG   |= (1<<6);       /* Enable pull-up on PC6 */
		GPIO_PORTC_DEN_REG   |= (1<<6);/* Enable Digital I/O on PC6 */
		GPIO_PORTC_PUR_REG   |= (1<<7);       /* Enable pull-up on PC7 */
		GPIO_PORTC_DEN_REG   |= (1<<7); /* Enable Digital I/O on PC7 */
	}
	else if(module==4)
	{

		SET_BIT(RCGCUART ,4);  /* enable clock to UART4 */
		SET_BIT(RCGCGPIO,2);  /* enable clock to PORTC  */


		/* UART4 PC4 and PC5 Configure them digital and enable alternate function */

		GPIO_PORTC_PCTL_REG  &= 0xFFF0FFFF;  	    /* Clear PMCx bits for PC4 to use it as GPIO pin */
		GPIO_PORTC_PCTL_REG  &= 0xFF0FFFFF;       	    /* Clear PMCx bits for PC5 to use it as GPIO pin */

		GPIO_PORTC_DIR_REG  &= ~(1<<4);      /* Configure PC4 as input pin */
		GPIO_PORTC_AFSEL_REG |= (1<<4);      /* enable alternative function on PC4 */
		GPIO_PORTC_DIR_REG  |= (1<<5);      /* Configure PC5 as output pin */
		GPIO_PORTC_AFSEL_REG |= (1<<5);      /* enable alternative function on PC5 */ 

		GPIO_PORTC_PUR_REG   |= (1<<4);       /* Enable pull-up on PC4 */
		GPIO_PORTC_DEN_REG   |= (1<<4);/* Enable Digital I/O on PC4 */
		GPIO_PORTC_PUR_REG   |= (1<<5);       /* Enable pull-up on PC5 */
		GPIO_PORTC_DEN_REG   |= (1<<5); /* Enable Digital I/O on PC5 */

	}
	else if(module==5)
	{

		SET_BIT(RCGCUART ,5);  /* enable clock to UART5 */
		SET_BIT(RCGCGPIO,4);  /* enable clock to PORTE for PA1/Rx */

		/* UART4 PE4 and PE5 Configure them digital and enable alternate function */

		GPIO_PORTE_PCTL_REG  &= 0xFFF0FFFF;  	    /* Clear PMCx bits for PE4 to use it as GPIO pin */
		GPIO_PORTE_PCTL_REG  &= 0xFF0FFFFF;       	    /* Clear PMCx bits for PE5 to use it as GPIO pin */

		GPIO_PORTE_DIR_REG  &= ~(1<<4);      /* Configure PE4 as input pin */
		GPIO_PORTE_AFSEL_REG |= (1<<4);      /* enable alternative function on PE4 */
		GPIO_PORTE_DIR_REG  |= (1<<5);      /* Configure PE5 as output pin */
		GPIO_PORTE_AFSEL_REG |= (1<<5);      /* enable alternative function on PE5 */ 

		GPIO_PORTE_PUR_REG   |= (1<<4);       /* Enable pull-up on PE4 */
		GPIO_PORTE_DEN_REG   |= (1<<4);/* Enable Digital I/O on PE4 */
		GPIO_PORTE_PUR_REG   |= (1<<5);       /* Enable pull-up on PE5 */
		GPIO_PORTE_DEN_REG   |= (1<<5); /* Enable Digital I/O on PE5 */

	}
	else if(module==6)

	{

		SET_BIT(RCGCUART ,6);  /* enable clock to UART6 */
		SET_BIT(RCGCGPIO,3);  /* enable clock to PORTD */

		/* UART6 PD4 and PD5 Configure them digital and enable alternate function */

		GPIO_PORTD_PCTL_REG  &= 0xFFF0FFFF;  	    /* Clear PMCx bits for PD4 to use it as GPIO pin */
		GPIO_PORTD_PCTL_REG  &= 0xFF0FFFFF;       	    /* Clear PMCx bits for PD5 to use it as GPIO pin */

		GPIO_PORTD_DIR_REG  &= ~(1<<4);      /* Configure PD4 as input pin */
		GPIO_PORTD_AFSEL_REG |= (1<<4);      /* enable alternative function on PD4 */
		GPIO_PORTD_DIR_REG  |= (1<<5);      /* Configure PD5 as output pin */
		GPIO_PORTD_AFSEL_REG |= (1<<5);      /* enable alternative function on PD5 */ 

		GPIO_PORTD_PUR_REG   |= (1<<4);       /* Enable pull-up on PD4 */
		GPIO_PORTD_DEN_REG   |= (1<<4);        /* Enable Digital I/O on PD4 */
		GPIO_PORTD_PUR_REG   |= (1<<5);       /* Enable pull-up on PD5 */
		GPIO_PORTD_DEN_REG   |= (1<<5);        /* Enable Digital I/O on PD5 */

	}
	else if(module==7)
	{
		SET_BIT(RCGCUART ,7);  /* enable clock to UART6 */
		SET_BIT(RCGCGPIO,4);  /* enable clock to PORTE */



		/* UART4 PE4 and PE5 Configure them digital and enable alternate function */

		GPIO_PORTE_PCTL_REG  &= 0xFFFFFFF0;  	    /* Clear PMCx bits for PE0 to use it as GPIO pin */
		GPIO_PORTE_PCTL_REG  &= 0xFFFFFF0F;       	    /* Clear PMCx bits for PE1 to use it as GPIO pin */

		GPIO_PORTE_DIR_REG  &= ~(1<<0);      /* Configure PE0as input pin */
		GPIO_PORTE_AFSEL_REG |= (1<<0);      /* enable alternative function on PE0 */
		GPIO_PORTE_DIR_REG  |= (1<<1);      /* Configure PE1 as output pin */
		GPIO_PORTE_AFSEL_REG |= (1<<1);      /* enable alternative function on PE1 */ 

		GPIO_PORTE_PUR_REG   |= (1<<0);       /* Enable pull-up on PE0 */
		GPIO_PORTE_DEN_REG   |= (1<<0);/* Enable Digital I/O on PE0 */
		GPIO_PORTE_PUR_REG   |= (1<<1);       /* Enable pull-up on PE1 */
		GPIO_PORTE_DEN_REG   |= (1<<1); /* Enable Digital I/O on PE1 */
	}
	else
	{
		/*Do Nothing*/
	}
}

#endif
