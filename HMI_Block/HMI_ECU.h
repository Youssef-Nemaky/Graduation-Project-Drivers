 /******************************************************************************
 *
 * Module: Main Source File for The HMI Block
 *
 * File Name: HMI_ECU.h
 *
 * Description: Header File for HMI ECU Block Functions
 *
 * Created on: Dec 9, 2021
 *
 * Authors: Mourad Eldin Nash'at , Abdelaziz Abu Zaid
 *
 *******************************************************************************/


#ifndef HMI_ECU_H_
#define HMI_ECU_H_


/*******************************************************************************
 *                                  Includes                                   *
 *******************************************************************************/

#include "Std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define KEYPAD_DELAY 250
#define LCD_MESSAGE_DELAY 500


#define NUMBER_OF_CHARACTERS_IN_PASSCODE 5

#define INCORRECT 0
#define CORRECT 1
#define PASSWORD_OR_LOCK 3
#define LOCK 2
#define FREE_SYSTEM 5
#define LOCK_TIME 60000
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

uint8 isFirstTime();

/*******************************************************************************************************
 * [Name]: Drivers_Init
 * [Parameters]: void (none)
 * [Return]: void (none)
 * [Description]: The Function responsible for Initializing of (MCAL) Drivers like UART and (HAL) Drivers
 * like LCD and Enabling Global Interrupt (I-Bit) in HMI Block.
 ********************************************************************************************************/
void Drivers_Init(void);


/*******************************************************************************************************
 * [Name]: Receive_First_Time_Check
 * [Parameters]: void (none)
 * [Return]: void (none)
 * [Description]: The Function responsible for Receiving a certain Byte ('Y' or 'N') from The Control Block
 * to to check whether its the first time use of the system or not.
 ********************************************************************************************************/
void Receive_First_Time_Check(void);


/*******************************************************************************************************
 * [Name]: Setup_New_Passcode
 * [Parameters]: uint8 *pass1 , uint8 *pass2
 * [Return]: void (none)
 * [Description]: The Function responsible for Creating a new Passcode for The System for the first time
 * use or when changing The Passcode and check it's correctness.
 ********************************************************************************************************/
void Setup_New_Passcode(uint8 *pass1 , uint8 *pass2);


/*******************************************************************************************************
 * [Name]: Passcode_Check_Condition
 * [Parameters]: uint8 *pass1 , uint8 *pass2
 * [Return]: Boolean Value (0 or 1)
 * [Description]: The Function responsible for The Condition to check the two Entered Passcodes at
 * The Setup or Change Processes.
 ********************************************************************************************************/
boolean Passcode_Check_Condition(uint8 *pass1 , uint8 *pass2);


/*******************************************************************************************************
 * [Name]: Enter_Passcode
 * [Parameters]: uint8 *pass
 * [Return]: void (none)
 * [Description]: The Function responsible for Entering a Passcode of certain number of Numeric
 * Characters using The Keypad.
 ********************************************************************************************************/
void Enter_Passcode(uint8 *pass);


/*******************************************************************************************************
 * [Name]: Options_Menu
 * [Parameters]: void (none)
 * [Return]: void (none)
 * [Description]: The Function responsible for Displaying Options Menu on The LCD to decide what action
 * to do and display it's result.
 ********************************************************************************************************/
void Options_Menu(void);


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


#endif /* HMI_ECU_H_ */


