 /******************************************************************************
 *
 * Module: Main Source File for The HMI Block
 *
 * File Name: HMI_ECU.c
 *
 * Description: Source File for HMI ECU Block Functions
 *
 * Created on: Dec 9, 2021
 *
 * Authors: Mourad Eldin Nash'at , Abdelaziz Abu Zaid
 *
 *******************************************************************************/

/*******************************************************************************
 *                                  Includes                                   *
 *******************************************************************************/

#include "HMI_ECU.h"
#include "lcd.h"
#include "keypad.h"
#include "uart.h"
#include "Port.h"
#include "Sw_Delay.h"


/*******************************************************************************
 *                              Global Variables                               *
 *******************************************************************************/

/* Global Variable to check whether this the first use of The System or not */
uint8 first_time;

/* Global Variable of type System_State used to block The System at a certain point for an amount of time */
System_State state = BLOCKED;



/*******************************************************************************
 *                       	  Functions Definitions                            *
 *******************************************************************************/

/*******************************************************************************************************
 * [Name]: Drivers_Init
 * [Parameters]: void (none)
 * [Return]: void (none)
 * [Description]: The Function responsible for Initializing of (MCAL) Drivers like UART and (HAL) Drivers
 * like LCD and Enabling Global Interrupt (I-Bit) in HMI Block.
 ********************************************************************************************************/
void Drivers_Init(void){
    Dio_Init(&Dio_Configuration);
    Port_Init(&Port_Configuration);
    Uart_Init(&Uart_Configuration);
    LCD_init();
}


/*******************************************************************************************************
 * [Name]: Receive_First_Time_Check
 * [Parameters]: void (none)
 * [Return]: void (none)
 * [Description]: The Function responsible for Receiving a certain Byte ('Y' or 'N') from The Control Block
 * to to check whether its the first time use of the system or not.
 ********************************************************************************************************/
void Receive_First_Time_Check(void)
{
	/* Arrays to hold The whole Passcode */
	uint8 pass1[NUMBER_OF_CHARACTERS_IN_PASSCODE];
	uint8 pass2[NUMBER_OF_CHARACTERS_IN_PASSCODE];

	/* Loop Counter */
	uint8 i;

	switch(first_time)
	{
		/* Case First Time Use */
	case 'Y':

		/* Setup New Passcode for The System */
		Setup_New_Passcode(pass1 , pass2);

		/* Send a Certain Character to Control Block to inform it to save the sent Passcode on The EEPROM */
		Uart_SendByte(CONTROL_BLOCK_UART, 'A');

		/* In case of match Send The Passcode element by element to Control-ECU */
		for(i = 0; i < NUMBER_OF_CHARACTERS_IN_PASSCODE; i++)
		{
            Uart_SendByte(CONTROL_BLOCK_UART, pass1[i]);
		}

		/* Calling Options Menu in case of Match */
		Options_Menu();
		break;

		/* Case Not The First Time Use */
	case 'N':

		/* Go to Options Menu Directly without The Setup Process */
		Options_Menu();
	}
}


/*******************************************************************************************************
 * [Name]: Setup_New_Passcode
 * [Parameters]: uint8 *pass1 , uint8 *pass2
 * [Return]: void (none)
 * [Description]: The Function responsible for Creating a new Passcode for The System for the first time
 * use or when changing The Passcode and check it's correctness.
 ********************************************************************************************************/
void Setup_New_Passcode(uint8 *pass1 , uint8 *pass2)
{
	/* Execute Theses Lines at least once */
    boolean passwordIsCorrect = FALSE;
	do
	{
		/* Create a new Passcode for The System */
		/* Clear The Screen to add new Data */
		LCD_clearScreen();

		/* Enter Passcode for The First Time */
		LCD_displayString("Plz Enter Pass 1");
		LCD_moveCursor(1 , 7);

		/* Enter Passcode for The First Time*/
		Enter_Passcode(pass1);

		/* Clear The Screen to add new Data */
		LCD_clearScreen();

		/* Enter the same Passcode for The Second Time */
		LCD_displayString("Plz Enter Pass 2");
		LCD_moveCursor(1 , 7);

		/* Enter the same Passcode for The Second Time*/
		Enter_Passcode(pass2);

        passwordIsCorrect = Passcode_Check_Condition(pass1 , pass2);

        if(!passwordIsCorrect){
            /* Clear the LCD screen */
            LCD_clearScreen();
            /* Display an error message */
            LCD_displayString("Passwords Mismatch!");
            /* A short delay to give the user time to read the error message */
            Delay_Ms(LCD_MESSAGE_DELAY);
        }

		/*
		 * Description :
		 * Check that every element in Pass1 equals its correspondent in Pass2.
		 * in case of True the loop will exit immediately.
		 * in case of False the loop will iterate again until Pass1 matches Pass2.
		 */
	}while(!passwordIsCorrect);
}


/*******************************************************************************************************
 * [Name]: Passcode_Check_Condition
 * [Parameters]: uint8 *pass1 , uint8 *pass2
 * [Return]: Boolean Value (0 or 1)
 * [Description]: The Function responsible for The Condition to check the two Entered Passcodes at
 * The Setup or Change Processes.
 ********************************************************************************************************/
boolean Passcode_Check_Condition(uint8 *pass1 , uint8 *pass2)
{
	/* Loop Counter */
	uint8 i;

	for(i = 0; i < NUMBER_OF_CHARACTERS_IN_PASSCODE; i++)
	{
		if(pass1[i] != pass2[i])
		return FALSE;
	}
	return TRUE;
}


/*******************************************************************************************************
 * [Name]: Enter_Passcode
 * [Parameters]: uint8 *pass
 * [Return]: void (none)
 * [Description]: The Function responsible for Entering a Passcode of certain number of Numeric
 * Characters using The Keypad.
 ********************************************************************************************************/
void Enter_Passcode(uint8 *pass)
{
	/* Loop Counter */
	uint8 i;

	/* Variable to hold The value of the Pressed Key */
	uint8 pressed_key;

	/* Repeat for 5 Times only (5 Numbers Passcode) */
	for(i = 0; i < NUMBER_OF_CHARACTERS_IN_PASSCODE;)
	{
		/* Assign the value of the function to the varable */
		pressed_key = KEYPAD_getPressedKey();

		/* Check that The Entered Key is a Numeric Value */
		if((pressed_key >= 0) && (pressed_key <= 9))
		{
			/* Increment in case of number only */
			i++;

			/* Get the pressed button from keypad */
			pass[i] = pressed_key;
			LCD_displayCharacter('*');

			/* Wait 250msec before getting a new press from the keypad buttons, Press time is 250msec */
			Delay_ms(KEYPAD_DELAY);
		}
	}
}


/*******************************************************************************************************
 * [Name]: Options_Menu
 * [Parameters]: void (none)
 * [Return]: void (none)
 * [Description]: The Function responsible for Displaying Options Menu on The LCD to decide what action
 * to do and display it's result.
 ********************************************************************************************************/
void Options_Menu(void)
{
	/* Arrays to hold The whole Passcode */
	uint8 pass1[NUMBER_OF_CHARACTERS_IN_PASSCODE];
	uint8 pass2[NUMBER_OF_CHARACTERS_IN_PASSCODE];

	/* Variable To Hold The Value of The Received Byte (INCORRECT , CORRECT , LOCK) */
	uint8 receive_new;

	/* Loops Counter */
	uint8 i;

	uint8 times = 0;

	/* Clear The Screen to add new Data */
	LCD_clearScreen();

	/* Variable to hold the value of "+" or "-" for Options Menu */
	uint8 option;

	/* Display Available Options on The Screen */
	LCD_displayString("1: Unlock The Engine");
	LCD_moveCursor(1 , 0);
	LCD_displayString("2: Change Passcode");

	/* Extra Options
	/* - Locking and Unlocking The Engine for long Periods of time
	 - Share The access of the car
	   every month the user must authenticate his identity using the three authentication methods */

	/* Enter an Option */
    			/* keep looping until the pressed key is either 1 or 2 */
    do {
        option = KEYPAD_getPressedKey();
        Delay_Ms(KEYPAD_DELAY);
    } while (option != 1 && option != 2);
	
	switch(option)
	{
		/* Case Open The Door */
	case 1:

		/* Clear The Screen to add new Data */
		LCD_clearScreen();

		/* Enter your new created Passcode */
		LCD_displayStringRowColumn(0 , 3 , "Enter Passcode");
		LCD_moveCursor(1 , 7);

		/* Execute Theses Lines at least once */
		do{
			/* Enter your new created Passcode */
			Enter_Passcode(pass1);

			/* Send a Certain Character to Control Block to inform it to read the saved Passcode on The EEPROM */
			UART_sendByte('B');

			/* Send The Entered Passcode to Control Block via UART */
			for(i = 0; i < NUMBER_OF_CHARACTERS_IN_PASSCODE; i++)
			{
                Uart_SendByte(CONTROL_BLOCK_UART, pass1[i]);
			}

			/* Receive a certain Macro from Control Block to know if the Passcode is Correct or not */
			receive_new = Uart_ReceiveByte(CONTROL_BLOCK_UART);
            
			/* Case The Received Macro is INCORRECT */
			if(receive_new == INCORRECT)
			{
				/* Clear The Screen to add new Data */
				LCD_clearScreen();

				LCD_displayStringRowColumn(0 , 3 , "Wrong Passcode");
				Delay_Ms(LCD_MESSAGE_DELAY);

				/* Clear The Screen to add new Data */
				LCD_clearScreen();

				LCD_displayStringRowColumn(0 , 0 , "Enter Passcode Again");
				LCD_moveCursor(1 , 7);
			}

			/* As long as The Received Macro is still INCORRECT */
		}while(receive_new == INCORRECT);

		/* Case The Received Macro is CORRECT */
		if(receive_new == CORRECT)
		{
			/* Clear The Screen to add new Data */
			LCD_clearScreen();

			LCD_displayString("The Engine is Ready");

            Delay_Ms(LCD_MESSAGE_DELAY);

		}

		/* Case The Received Macro is LOCK */
		else if(receive_new == PASSWORD_OR_LOCK)
		{
			/* Clear The Screen to add new Data */
			LCD_clearScreen();

            /* Enter the random generated number */

            LCD_displayString("Enter Sent Passcode");
            LCD_moveCursor(1,7);

            Enter_Passcode(pass1);
            UART_sendByte(CONTROL_BLOCK_UART, 'N');

            for(i = 0 ; i < NUMBER_OF_CHARACTERS_IN_PASSCODE; i++){
                UART_sendByte(CONTROL_BLOCK_UART, pass1[i]);
            }

            if(Uart_ReceiveByte(CONTROL_BLOCK_UART) == LOCK){
                LCD_displayString("Smile To Camera :)");
                /* Block The System after Executing all Options until it is freed again */
                Block_System();
            } else {
                break;
            }



		}
		break;

		/* Change The Passcode */
	case 2:

		/* Clear The Screen to add new Data */
		LCD_clearScreen();

		/* Enter Your old Passcode */
		LCD_displayString("Enter old Passcode");
		LCD_moveCursor(1 , 7);

		/* Execute Theses Lines at least once */
		do{
			/* Send a Certain Character to Control Block to inform it to save the sent Passcode on The EEPROM */
            Uart_SendByte(CONTROL_BLOCK_UART, 'C');

			/* Enter your old Passcode */
			Enter_Passcode(pass1);

			/* Send The Entered Passcode to Control Block via UART */
			for(i = 0; i < NUMBER_OF_CHARACTERS_IN_PASSCODE; i++)
			{
				Uart_SendByte(CONTROL_BLOCK_UART, pass1[i]);
			}

			/* Receive a certain Macro from Control Block to know if the Passcode is Correct or not */
			receive_new = UART_receiveByte();

			/* Case The Received Macro is INCORRECT */
			if(receive_new == INCORRECT)
			{
				/* Clear The Screen to add new Data */
				LCD_clearScreen();

				LCD_displayStringRowColumn(0 , 1 , "Wrong Passcode");
				Delay_ms(LCD_MESSAGE_DELAY);

				/* Clear The Screen to add new Data */
				LCD_clearScreen();

				LCD_displayStringRowColumn(0 , 1 , "Enter Passcode Again");
				LCD_moveCursor(1 , 7);
			}

			/* As long as The Received Macro is still INCORRECT */
		}while(receive_new == INCORRECT);

		/* Case The Received Macro is CORRECT */
		if(receive_new == CORRECT)
		{
			/* Setup New Passcode for The System */
			Setup_New_Passcode(pass1 , pass2);

			/* Send The New Passcode to Control Block via UART */
			for(i = 0; i < NUMBER_OF_CHARACTERS_IN_PASSCODE; i++)
			{
				UART_sendByte(pass2[i]);
			}
		}

		/* Case The Received Macro is LOCK */
		else if(receive_new == PASSWORD_OR_LOCK)
		{
			/* Clear The Screen to add new Data */
			LCD_clearScreen();

            /* Enter the random generated number */

            LCD_displayString("Enter Sent Passcode");
            LCD_moveCursor(1,7);

            Enter_Passcode(pass1);
            UART_sendByte(CONTROL_BLOCK_UART, 'N');

            for(i = 0 ; i < NUMBER_OF_CHARACTERS_IN_PASSCODE; i++){
                UART_sendByte(CONTROL_BLOCK_UART, pass1[i]);
            }

            if(Uart_ReceiveByte(CONTROL_BLOCK_UART) == LOCK){
                LCD_displayString("Smile To Camera :)");
                /* Block The System after Executing all Options until it is freed again */
                Block_System();
            } else {
                break;
            }

		}
	}

	/* Set first_time to N indicating that The next iteration won't be The First Time Use and There is a saved
	 * Passcode Already.
	 */
	first_time = 'N';


}

//******************************************** 3dlny PLEASE *************************/
/*******************************************************************************************************
 * [Name]: Block_System
 * [Parameters]: void (none)
 * [Return]: void (none)
 * [Description]: The Function responsible for Blocking The System at a certain Point
 ********************************************************************************************************/
void Block_System(void)
{
    while(Uart_ReceiveByte(CONTROL_BLOCK_UART) != FREE_SYSTEM);
}


/*******************************************************************************************************
 * [Name]: Free_System
 * [Parameters]: void (none)
 * [Return]: void (none)
 * [Description]: The Function responsible for getting The System back from Blocked state to Free State.
 ********************************************************************************************************/
void Free_System(void)
{
	/* Set The System_State back to Free */
	state = FREE;
}



/*******************************************************************************
 *                     		      Main Function                                *
 *******************************************************************************/

int main(void)
{
	/* Initialize Drivers to be ready to use when it is needed */
	Drivers_Init();

	/* Receive a certain Byte (Character) from Control Block to know if that is the System First Use or not */
	first_time = Uart_ReceiveByte(CONTROL_BLOCK_UART);
    
	while(1)
	{
		/* Calling The Program Start Function for HMI Block */
		Receive_First_Time_Check();
	}
}









