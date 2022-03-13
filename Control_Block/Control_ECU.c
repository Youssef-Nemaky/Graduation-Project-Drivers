 /******************************************************************************
 *
 * Module: Main Source File for The Control Block
 *
 * File Name: Control_ECU.c
 *
 * Description: Source File for Control ECU Block Functions
 *
 * Created on: Dec 9, 2021
 *
 * Authors: Mourad Eldin Nash'at , Abdelaziz Abu Zaid
 *
 *******************************************************************************/

/*******************************************************************************
 *                                  Includes                                   *
 *******************************************************************************/

#include "Dio.h"
#include "Port.h"
#include "Control_ECU.h"
#include "Uart.h"
#include "I2c.h"
#include "external_eeprom.h"
#include "Sw_Delay.h"



/*******************************************************************************
 *                              Global Variables                               *
 *******************************************************************************/

/* Global Variable of type System_State used to block The System at a certain point for an amount of time */
System_State state = BLOCKED;

/* Global Variable that holds The Value of Times The Passcode is entered in case it was entered wrong */
uint8 passWrongAttempts = 0;

uint8 rfidWrongAttempts = 0;


/*******************************************************************************
 *                       	  Functions Definitions                            *
 *******************************************************************************/


/*******************************************************************************************************
 * [Name]: Drivers_Init
 * [Parameters]: void (none)
 * [Return]: void (none)
 * [Description]: The Function responsible for Initializing of (MCAL) Drivers like UART & I2C and (HAL)
 * Drivers like GPS/GSM Modules and Enabling Global Interrupt (I-Bit) in Control Block.
 ********************************************************************************************************/
void Drivers_Init(void)
{
    Port_Init(&Port_Configuration);
    Dio_Init(&Dio_Configuration);
    Uart_Init(&Uart_Configuration);
    I2c_Init(&I2c_Confiuration);
}


/*******************************************************************************************************
 * [Name]: Send_First_Time_Check
 * [Parameters]: void (none)
 * [Return]: void (none)
 * [Description]: he Function responsible for Sending a certain Byte ('Y' or 'N') to HMI Block to check
 * whether its the first time use of the system or not.
 ********************************************************************************************************/
void Send_First_Time_Check(void)
{
	/* Variable Holds a specific value to inform the system that this a The First Time Use or not */
	uint8 first_time_use = 0;

	EEPROM_readByte(0x0321 , &first_time_use);

	/* Send a certain Byte (Character) to HMI Block to know if that is the System's First Use or not */
	if(first_time_use == 0xFF)
	{
		/* First Time Use */
		Uart_SendByte(HMI_BLOCK_UART,'Y');
	}
	else
	{
		/* Not The First Time Use */
		Uart_SendByte(HMI_BLOCK_UART,'N');
	}
}


/*******************************************************************************************************
 * [Name]: Passcode_Receive_and_Check
 * [Parameters]: void (none)
 * [Return]: void (none)
 * [Description]: The Function responsible for checking that both Pass Codes entered by the user are
 * the same and receiving it via UART from HMI Block.
 ********************************************************************************************************/
void Passcode_Receive_and_Check(void)
{
	/* Arrays to hold The whole Passcode */
	uint8 pass1[5];
	uint8 pass2[5];

    uint8 rfidTag1[RFID_UNIQUE_ID_LENGTH] = {0};
    uint8 rfidTag2[RFID_UNIQUE_ID_LENGTH] = {0};
    uint8 rfidTag3[RFID_UNIQUE_ID_LENGTH] = {0};

    uint8 uniqueIdCounter = 0;

	/* Variable to hold The Value of The Received Byte from HMI Block to decide what to do  */
	uint8 receive_new = 0;
        
        
        receive_new = Uart_ReceiveByte(HMI_BLOCK_UART);

	/* A: Character Received from HMI Block to Save The Passcode for The first Time.
	 * B: Character Received from HMI Block to Check The Passcode and Start The Door Operation.
	 * C: Character Received from HMI Block to Save The Passcode When it's changed by the user.
	 */

	/* Case The Received Character is A */

        switch (receive_new) {
        case 'A':
            /* Save The New Created Passcode in The EEPROM */
            Save_Passcode(pass1);

            /* Writing location of first_time to make sure that its default value is changed */
            EEPROM_writeByte(0x0321, 0x00);
            break;
        case 'B':
            /* Case The Check Condition is True */
            if (Passcode_Check()) {
                /* Send CORRECT Macro to HMI-ECU Indicating Correct Passcode */
                Uart_SendByte(HMI_BLOCK_UART, CORRECT);

                /* Unlock The Engine Door */

            }

            /* Case The Check Condition is False */
            else {
                /* Increment passWrongAttempts by 1 each time The Passcode is entered wrong */
                passWrongAttempts++;

                /* Case passWrongAttempts reached it's Maximum Value 3 Times */
                if (passWrongAttempts >= 3) {
                    /* Send LOCK Macro to HMI-ECU Indicating Incorrect Passcode for 3 passWrongAttempts */
                    Uart_SendByte(HMI_BLOCK_UART, LOCK);

                    /* Send Notification via The Mobile Application */


                    /* Rest passWrongAttempts back to 0 */
                    passWrongAttempts = 0;
                    Delay_ms(LOCK_TIME);
                }

                else
                {
                    /* Send INCORRECT Macro to HMI-ECU Indicating Incorrect Passcode for 1 time */
                    Uart_SendByte(HMI_BLOCK_UART, INCORRECT);
                }
            }
            break;
        case 'C':
            /* Case The Check Condition is True */
            if (Passcode_Check()) {
                /* Send CORRECT Macro to HMI-ECU Indicating Correct Passcode */
                Uart_SendByte(HMI_BLOCK_UART, CORRECT);

                /* Save The New Changed Passcode in The EEPROM */
                Save_Passcode(pass2);
            }

            /* Case The Check Condition is False */
            else
            {
                /* Increment passWrongAttempts by 1 each time The Passcode is not correct */
                passWrongAttempts++;

                /* Case passWrongAttempts reached it's Maximum Value 3 Times */
                if (passWrongAttempts >= 3) {
                    /* Send LOCK Macro to HMI-ECU Indicating Incorrect Passcode for 3 passWrongAttempts */
                    Uart_SendByte(HMI_BLOCK_UART, LOCK);
                    Delay_ms(LOCK_TIME);
                    /* Send Notification via The Mobile Application */


                    /* Rest passWrongAttempts back to 0 */
                    passWrongAttempts = 0;
                }

                else {
                    /* Send INCORRECT Macro to HMI-ECU Indicating Incorrect Passcode for 1 time */
                    Uart_SendByte(HMI_BLOCK_UART, INCORRECT);
                }
            }
            break;
        case 'F':
            Send_First_Time_Check();
            break;
        case 'S':
            rfidSave(rfidTag1, 1);
            Uart_SendByte(HMI_BLOCK_UART, 'Z');
            rfidSave(rfidTag2, 2);
            break;
        case 'R':
            /* get the uid from the HMI */
            for (uniqueIdCounter = 0; uniqueIdCounter < RFID_UNIQUE_ID_LENGTH; uniqueIdCounter++) {
                rfidTag1[uniqueIdCounter] = Uart_ReceiveByte(HMI_BLOCK_UART);
            }

            /* get the uid from the HMI */
            for (uniqueIdCounter = 0; uniqueIdCounter < RFID_UNIQUE_ID_LENGTH; uniqueIdCounter++) {
                EEPROM_readByte(0x0316 + uniqueIdCounter, &rfidTag2[uniqueIdCounter]);
                EEPROM_readByte(0x0316 + uniqueIdCounter + RFID_UNIQUE_ID_LENGTH, &rfidTag3[uniqueIdCounter]);
                if(( rfidTag1[uniqueIdCounter] != rfidTag2[uniqueIdCounter] ) && ( rfidTag1[uniqueIdCounter] != rfidTag3[uniqueIdCounter] )){
                    rfidWrongAttempts++;

                    if(rfidWrongAttempts >= 3){
                        /* Send LOCK Macro to HMI-ECU Indicating Incorrect Passcode for 3 passWrongAttempts */
                        Uart_SendByte(HMI_BLOCK_UART, LOCK);
                        /* Rest passWrongAttempts back to 0 */
                        rfidWrongAttempts = 0;
                        Delay_ms(LOCK_TIME);
                        return;
                    } else {
                        /* Send INCORRECT Macro to HMI-ECU Indicating Incorrect Passcode for 1 time */
                        Uart_SendByte(HMI_BLOCK_UART, INCORRECT);
                    }
                }
            }
            Uart_SendByte(HMI_BLOCK_UART, CORRECT);
            break;
        default:
            break;
        }
}


/*******************************************************************************************************
 * [Name]: Save_Passcode
 * [Parameters]: uint8 *pass
 * [Return]: void (none)
 * [Description]: Function Responsible for Saving The Passcode in The EEPROM in Specific locations
 ********************************************************************************************************/
void Save_Passcode(uint8 *pass)
{
	/* Loop Counter */
	uint8 i;

	for(i = 0; i < 5; i++)
	{
		/* Receive The New Pass Code from HMI-ECU through UART */
		pass[i] = Uart_ReceiveByte(HMI_BLOCK_UART);
	}

	/* Write The Passcode values in the external EEPROM in Locations from (0x311) to (0x315) */

    for(i = 0; i < 5; i++){
        EEPROM_writeByte(0x0311 + i, pass[i]);
        Delay_ms(10);
    }
    
}


void rfidSave(uint8 * rfidTag, uint8 tagNumber){
    uint8 uniqueIdCounter = 0;

    /* get the uid from the HMI */
    for(uniqueIdCounter = 0; uniqueIdCounter < RFID_UNIQUE_ID_LENGTH; uniqueIdCounter++){
        rfidTag[uniqueIdCounter] = Uart_ReceiveByte(HMI_BLOCK_UART);
    }

    /* store the uid into the EEPROM */
    for(uniqueIdCounter = 0; uniqueIdCounter < RFID_UNIQUE_ID_LENGTH; uniqueIdCounter++){
        if(tagNumber == 1){
            EEPROM_writeByte(0x0316 + uniqueIdCounter, rfidTag[uniqueIdCounter]);
        } else {
            EEPROM_writeByte(0x0316 + uniqueIdCounter + RFID_UNIQUE_ID_LENGTH, rfidTag[uniqueIdCounter]);
        }
        Delay_ms(10);
    }
}
/*******************************************************************************************************
 * [Name]: Passcode_Check
 * [Parameters]: void (none)
 * [Return]: void (none)
 * [Description]: The Function responsible for Checking that every element in Pass1 equals its correspondent
 * in Pass2. in case of True The System will Unlock The Engine for the user as long as there is no other
 * authentication method is required. in case of False for Three Times The System will send a warning
 * Notification to the owner via Mobile Application and a certain Message will be displayed on
 * The LCD screen.
 ********************************************************************************************************/
uint8 Passcode_Check(void)
{
	/* Arrays to hold The whole Pass Code */
	uint8 pass1[5];
	uint8 pass2[5];

	/* Loop Counter */
	uint8 i;

	for(i = 0; i < 5; i++)
	{
		/* Receive The Entered Pass Code from HMI-ECU through UART and Store it in Array pass2[5] */
		pass2[i] = Uart_ReceiveByte(HMI_BLOCK_UART);
	}

	/* Reading date from addresses (0x311) to (0x315) and assigning it to pass1[5] array */

	EEPROM_readByte(0x0311 , &pass1[0]);
	EEPROM_readByte(0x0312 , &pass1[1]);
	EEPROM_readByte(0x0313 , &pass1[2]);
	EEPROM_readByte(0x0314 , &pass1[3]);
	EEPROM_readByte(0x0315 , &pass1[4]);

	/* Check Condition */
	for(i = 0; i < 5; i++)
	{
		if(pass1[i] != pass2[i])
		{
			return FALSE;
		}
	}
	return TRUE;
}


/*******************************************************************************************************
 * [Name]: Block_System
 * [Parameters]: void (none)
 * [Return]: void (none)
 * [Description]: The Function responsible for Blocking The System at a certain Point
 ********************************************************************************************************/
void Block_System(void)
{
	if(state == BLOCKED)
	{
		while(1)
		{
			if(state == FREE)
			break;
		}
	}
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

	/* Sending The Byte to HMI to decide the course of action */

	while(1)
	{
		/* Calling The Program Start Function for Control Block */
		Passcode_Receive_and_Check();
	}
}





