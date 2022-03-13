 /******************************************************************************
 *
 * Module: Main Source File for The Control Block
 *
 * File Name: Control_ECU.h
 *
 * Description: Header File for Control ECU Block Functions
 *
 * Created on: Dec 9, 2021
 *
 * Authors: Mourad Eldin Nash'at , Abdelaziz Abu Zaid
 *
 *******************************************************************************/


#ifndef CONTROL_ECU_H_
#define CONTROL_ECU_H_


/*******************************************************************************
 *                                  Includes                                   *
 *******************************************************************************/

#include "Std_Types.h"

/*******************************************************************************
 *                                Definitions                                 *
 *******************************************************************************/

#define NUMBER_OF_ITERATIONS_PER_ONE_MILI_SECOND 762

#define HMI_BLOCK_UART UART7_MODULE_NUMBER /* Add me to uart.h later*/
#define LOCK_TIME 60000
#define INCORRECT 0
#define CORRECT 1
#define LOCK 2

#define RFID_UNIQUE_ID_LENGTH (8u)


/*******************************************************************************
 *                         		Types Declaration                              *
 *******************************************************************************/

/* Typedef Enumeration to make Sytsem_State Date Type with values BLOCKED and FREE */
typedef enum
{
	BLOCKED , FREE

}System_State;



/*******************************************************************************
 *                     		  Functions Prototypes                             *
 *******************************************************************************/

/*******************************************************************************************************
 * [Name]: Delay_ms
 * [Parameters]: uint64 n
 * [Return]: void (none)
 * [Description]: The Function responsible for The Delay in the internal Operations of the peripherals
 * and Hardware Components that don't need accurate timing using Timers like (Systic or GPT).
 ********************************************************************************************************/
void Delay_ms(uint64 n);


/*******************************************************************************************************
 * [Name]: Drivers_Init
 * [Parameters]: void (none)
 * [Return]: void (none)
 * [Description]: The Function responsible for Initializing of (MCAL) Drivers like UART & I2C and (HAL)
 * Drivers like GPS/GSM Modules and Enabling Global Interrupt (I-Bit) in Control Block.
 ********************************************************************************************************/
void Drivers_Init(void);


/*******************************************************************************************************
 * [Name]: Send_First_Time_Check
 * [Parameters]: void (none)
 * [Return]: void (none)
 * [Description]: he Function responsible for Sending a certain Byte ('Y' or 'N') to HMI Block to check
 * whether its the first time use of the system or not.
 ********************************************************************************************************/
void Send_First_Time_Check(void);


/*******************************************************************************************************
 * [Name]: Passcode_Receive_and_Check
 * [Parameters]: void (none)
 * [Return]: void (none)
 * [Description]: The Function responsible for checking that both Pass Codes entered by the user are
 * the same and receiving it via UART from HMI Block.
 ********************************************************************************************************/
void Passcode_Receive_and_Check(void);


/*******************************************************************************************************
 * [Name]: Save_Passcode
 * [Parameters]: uint8 *pass
 * [Return]: void (none)
 * [Description]: Function Responsible for Saving The Passcode in The EEPROM in Specific locations
 ********************************************************************************************************/
void Save_Passcode(uint8 *pass);


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
uint8 Passcode_Check(void);


/*******************************************************************************************************
 * [Name]: Block_System
 * [Parameters]: void (none)
 * [Return]: void (none)
 * [Description]: The Function responsible for Blocking The System at a certain Point
 ********************************************************************************************************/
void Block_System(void);


/*******************************************************************************************************
 * [Name]: Free_System
 * [Parameters]: void (none)
 * [Return]: void (none)
 * [Description]: The Function responsible for getting The System back from Blocked state to Free State.
 ********************************************************************************************************/
void Free_System(void);


void rfidSave(uint8 * rfidTag, uint8);

#endif /* CONTROL_ECU_H_ */

