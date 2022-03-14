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


/*******************************************************************************
 *                              Global Variables                               *
 *******************************************************************************/

/* Global Variable to check whether this the first use of The System or not */
uint8 first_time;

/* Global Variable of type System_State used to block The System at a certain point for an amount of time */
System_State state = BLOCKED;

uint8 numOfUsedAuthMethods = 3;

uint8 (*authArray[numOfAvAuthMethods])(void) = {passwordAuth, rfidAuth, faceAuth};




/*******************************************************************************
 *                       	  Functions Definitions                            *
 *******************************************************************************/

uint8  passwordAuth(void){
    uint8 pass1[NUMBER_OF_CHARACTERS_IN_PASSCODE];
    uint8 receive_new , passwordCounter = 0;

    /* Clear The Screen to add new Data */
    LCD_clearScreen();

    /* Enter your new created Passcode */
    LCD_displayStringRowColumn(0, 3, "Enter Passcode");
    LCD_moveCursor(1, 7);

    /* Execute Theses Lines at least once */
    do {
        /* Enter your new created Passcode */
        Enter_Passcode(pass1);

        /* Send a Certain Character to Control Block to inform it to read the saved Passcode on The EEPROM */
        Uart_SendByte(CONTROL_BLOCK_UART, 'B');

        /* Send The Entered Passcode to Control Block via UART */
        for (passwordCounter = 0; passwordCounter < NUMBER_OF_CHARACTERS_IN_PASSCODE; passwordCounter++)
        {
            Uart_SendByte(CONTROL_BLOCK_UART, pass1[passwordCounter] + '0');
        }

        /* Receive a certain Macro from Control Block to know if the Passcode is Correct or not */
        receive_new = Uart_ReceiveByte(CONTROL_BLOCK_UART);

        /* Case The Received Macro is INCORRECT */
        if (receive_new == INCORRECT)
        {
            /* Clear The Screen to add new Data */
            LCD_clearScreen();

            LCD_displayStringRowColumn(0, 3, "Wrong Passcode");
            Delay_ms(LCD_MESSAGE_DELAY);

            /* Clear The Screen to add new Data */
            LCD_clearScreen();

            LCD_displayStringRowColumn(0, 0, "Enter Passcode Again");
            LCD_moveCursor(1, 7);
        }

        /* As long as The Received Macro is still INCORRECT */
    } while (receive_new == INCORRECT);

    /* Case The Received Macro is CORRECT */
    if (receive_new == CORRECT)
    {
        /* Clear The Screen to add new Data */
        LCD_clearScreen();

        return TRUE;
    }

    /* MOVE ME PLEASE SENPAI */
    /* Case The Received Macro is LOCK */
    else if (receive_new == LOCK)
    {
        return FALSE;
    }
    return FALSE;
}

void systemAuth(void){
    uint8 authCounter = 0;
    boolean authResult = FALSE;
    for(authCounter = 0; authCounter < numOfUsedAuthMethods; authCounter++){
        authResult = (*authArray[authCounter])();
        if(authResult == FALSE){
            /* Clear The Screen to add new Data */
            LCD_clearScreen();

            LCD_displayString("7ramy");

            Delay_ms(LOCK_TIME);
            return;
        }
    }
    LCD_clearScreen();
    
    LCD_displayStringRowColumn(0, 2, "Access Granted!");

    Delay_ms(LCD_MESSAGE_DELAY);
}


uint8 isFirstTime(){
    uint8 firstTime = 0;
    Uart_SendByte(CONTROL_BLOCK_UART, 'F');
    firstTime = Uart_ReceiveByte(CONTROL_BLOCK_UART);
    return firstTime;
}

/*******************************************************************************************************
 * [Name]: Drivers_Init
 * [Parameters]: void (none)
 * [Return]: void (none)
 * [Description]: The Function responsible for Initializing of (MCAL) Drivers like UART and (HAL) Drivers
 * like LCD and Enabling Global Interrupt (I-Bit) in HMI Block.
 ********************************************************************************************************/
void Drivers_Init(void){
    Port_Init(&Port_Configuration);
    Dio_Init(&Dio_Configuration);
    Uart_Init(&Uart_Configuration);
    Spi_Init(&Spi_Configuration);
    RFID_Init();
    LCD_init();
}


void passwordSetup(void){
    /* Arrays to hold The whole Passcode */
    uint8 pass1[NUMBER_OF_CHARACTERS_IN_PASSCODE] = { 0 };
    uint8 pass2[NUMBER_OF_CHARACTERS_IN_PASSCODE] = { 0 };

    /* Loop Counter */
    uint8 i;

    /* Setup New Passcode for The System */
    Setup_New_Passcode(pass1, pass2);

    /* Send a Certain Character to Control Block to inform it to save the sent Passcode on The EEPROM */
    Uart_SendByte(CONTROL_BLOCK_UART, 'A');

    /* In case of match Send The Passcode element by element to Control-ECU */
    for (i = 0; i < NUMBER_OF_CHARACTERS_IN_PASSCODE; i++)
    {
        Uart_SendByte(CONTROL_BLOCK_UART, pass1[i] + '0');
    }

}

void rfidReadTag(uint8 * a_rfid_tag){
    uint8 byte;

    byte = RFID_Read(ComIEnReg);
    RFID_Write(ComIEnReg, byte | 0x20);
    byte = RFID_Read(DivIEnReg);
    RFID_Write(DivIEnReg, byte | 0x80);

    while (1) {
        byte = RFID_Request(PICC_REQALL, a_rfid_tag);
        if (byte == CARD_FOUND)
        {
            byte = RFID_getCardSerial(a_rfid_tag);
            /* Str has the UID of the rfid tag */

                 /* Delete me later */
            if (byte == CARD_FOUND)
            {
                LCD_clearScreen();
                for (byte = 0;byte < 4;byte++) {
                    LCD_displayHex(a_rfid_tag[byte]);
                }
                Delay_ms(2500);
                return;
            }
        }
        Delay_ms(1000);
    }
}


void rfidSetup(void){
    uint8 uidCounter = 0;
    uint8 rfidTag1[RFID_UNIQUE_ID_LENGTH];
    uint8 rfidTag2[RFID_UNIQUE_ID_LENGTH];

    LCD_clearScreen();
    LCD_displayString("Enter your 1st tag");
    rfidReadTag(rfidTag1);

    LCD_clearScreen();
    LCD_displayString("Enter your 2nd tag");
    rfidReadTag(rfidTag2);

    /* store the RFID tags */
    /* Send a Certain Character to Control Block to inform it to save the sent Passcode on The EEPROM */
    Uart_SendByte(CONTROL_BLOCK_UART, 'S');

    /* In case of match Send The Passcode element by element to Control-ECU */

    /* First Tag */
    for (uidCounter = 0; uidCounter < RFID_UNIQUE_ID_LENGTH; uidCounter++)
    {
        Uart_SendByte(CONTROL_BLOCK_UART, rfidTag1[uidCounter]);
    }

    if(Uart_ReceiveByte(CONTROL_BLOCK_UART) == 'Z'){
        /* Second Tag */
        for (uidCounter = 0; uidCounter < RFID_UNIQUE_ID_LENGTH; uidCounter++)
        {
            Uart_SendByte(CONTROL_BLOCK_UART, rfidTag2[uidCounter]);
        }
    }
}


uint8 rfidAuth(void){
    uint8 rfidTag[RFID_UNIQUE_ID_LENGTH] = {0};
    uint8 receive_new, rfidCounter = 0;

    /* Clear The Screen to add new Data */
    LCD_clearScreen();

    /* Enter your new created Passcode */
    LCD_displayStringRowColumn(0, 3, "Scan your tag");

    /* Execute Theses Lines at least once */
    do {
        rfidReadTag(rfidTag);

        /* Send a Certain Character to Control Block to inform it to read the saved Passcode on The EEPROM */
        Uart_SendByte(CONTROL_BLOCK_UART , 'R');

        /* Send The Entered Passcode to Control Block via UART */
        for (rfidCounter = 0; rfidCounter < RFID_UNIQUE_ID_LENGTH; rfidCounter++)
        {
            Uart_SendByte(CONTROL_BLOCK_UART, rfidTag[rfidCounter]);
        }

        /* Receive a certain Macro from Control Block to know if the Passcode is Correct or not */
        receive_new = Uart_ReceiveByte(CONTROL_BLOCK_UART);

        /* Case The Received Macro is INCORRECT */
        if (receive_new == INCORRECT)
        {
            /* Clear The Screen to add new Data */
            LCD_clearScreen();

            LCD_displayStringRowColumn(0, 5, "Tag Failed");
            Delay_ms(LCD_MESSAGE_DELAY);

            /* Clear The Screen to add new Data */
            LCD_clearScreen();

            LCD_displayStringRowColumn(0, 2, "Re-scan your tag");
        }

        /* As long as The Received Macro is still INCORRECT */
    } while (receive_new == INCORRECT);

    /* Case The Received Macro is CORRECT */
    if (receive_new == CORRECT)
    {
        /* Clear The Screen to add new Data */
        LCD_clearScreen();
        return TRUE;
    }

    /* MOVE ME PLEASE SENPAI */
    /* Case The Received Macro is LOCK */
    else if (receive_new == LOCK)
    {
        return FALSE;
    }
    return FALSE;
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
	switch(first_time)
	{
		/* Case First Time Use */
	case 'Y':

        passwordSetup();
        rfidSetup();
        faceSetup();

		/* Calling Options Menu in case of Match */
		Options_Menu();
		break;

		/* Case Not The First Time Use */
	case 'N':

		/* Go to Options Menu Directly without The Setup Process */
		Options_Menu();
                break;
        default: break;
	}
}

void faceSetup(void){
    uint8 faceRespond = 0;
    LCD_clearScreen();
    LCD_displayString("Smile to the camera!");
    Uart_SendByte(CONTROL_BLOCK_UART,'U');
    while(1){
        faceRespond =  Uart_ReceiveByte(CONTROL_BLOCK_UART);
        LCD_clearScreen();
        if (faceRespond == 'W') {
            LCD_displayString("Face ID Saved");
            break;
        } else {
            LCD_displayString("Error, Retry!");
            Delay_ms(LCD_MESSAGE_DELAY);
            LCD_displayString("Smile to the camera!");
        }
    }
}

uint8 faceAuth(void){
    uint8 faceRespond = 0;
    LCD_clearScreen();
    LCD_displayString("Smile to the camera!");
    Uart_SendByte(CONTROL_BLOCK_UART,'X');
    while(1){
        faceRespond =  Uart_ReceiveByte(CONTROL_BLOCK_UART);
        LCD_clearScreen();
        if (faceRespond == CORRECT) {
            return TRUE;
        } else if (faceRespond == INCORRECT){
            LCD_displayString("Error, Retry!");
            Delay_ms(LCD_MESSAGE_DELAY);
            LCD_displayString("Smile to the camera!");
        } else if(faceRespond == LOCK){
            return FALSE;
        }
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
            Delay_ms(LCD_MESSAGE_DELAY);
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
	uint8 i = 0;

	for(i = 0; i < NUMBER_OF_CHARACTERS_IN_PASSCODE; i++)
	{
		if(pass1[i] != pass2[i]){
            return FALSE;
        }
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
	uint8 i = 0;

	/* Variable to hold The value of the Pressed Key */
	uint8 pressed_key = 0;

	/* Repeat for 5 Times only (5 Numbers Passcode) */
	for(i = 0; i < NUMBER_OF_CHARACTERS_IN_PASSCODE;)
	{
		/* Assign the value of the function to the varable */
		pressed_key = KEYPAD_getPressedKey();

		/* Check that The Entered Key is a Numeric Value */
		if((pressed_key >= 0) && (pressed_key <= 9))
		{
			/* Get the pressed button from keypad */
			pass[i++] = pressed_key;

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
  uint8 pass1[NUMBER_OF_CHARACTERS_IN_PASSCODE] = {0};
  uint8 pass2[NUMBER_OF_CHARACTERS_IN_PASSCODE] = {0};

	/* Variable To Hold The Value of The Received Byte (INCORRECT , CORRECT , LOCK) */
	uint8 receive_new;

	/* Loops Counter */
	uint8 i;

	/* Clear The Screen to add new Data */
	LCD_clearScreen();

	/* Variable to hold the value of "+" or "-" for Options Menu */
	uint8 option;

	/* Display Available Options on The Screen */
	LCD_displayString("1: Unlock The Engine");
	LCD_moveCursor(1 , 0);
	LCD_displayString("2: Change Passcode");

	/* Extra Options*/
	/* - Locking and Unlocking The Engine for long Periods of time
	 - Share The access of the car
	   every month the user must authenticate his identity using the three authentication methods */

	/* Enter an Option */
    			/* keep looping until the pressed key is either 1 or 2 */
    do {
        option = KEYPAD_getPressedKey();
        Delay_ms(KEYPAD_DELAY);
    } while (option != 1 && option != 2);
	

	switch(option)
	{
		/* Case Open The Door */
	case 1:

        systemAuth();

		/* Clear The Screen to add new Data */
		LCD_clearScreen();
        
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
				Uart_SendByte(CONTROL_BLOCK_UART, pass1[i] + '0');
			}

			/* Receive a certain Macro from Control Block to know if the Passcode is Correct or not */
			receive_new = Uart_ReceiveByte(CONTROL_BLOCK_UART);

			/* Case The Received Macro is INCORRECT */
			if(receive_new == INCORRECT)
			{
				/* Clear The Screen to add new Data */
				LCD_clearScreen();

				LCD_displayStringRowColumn(0 , 1 , "Wrong Passcode");
				Delay_ms(LCD_MESSAGE_DELAY);

				/* Clear The Screen to add new Data */
				LCD_clearScreen();
                                LCD_displayString("Enter Passcode Again");
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
				Uart_SendByte(CONTROL_BLOCK_UART, pass2[i] + '0');
			}
		}

		/* Case The Received Macro is LOCK */
        else if (receive_new == LOCK)
        {
            /* Clear The Screen to add new Data */
            LCD_clearScreen();

            LCD_displayString("Smile To Camera :)");
            
            Delay_ms(LOCK_TIME);
        }
        break;
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
        LCD_displayString("2looo");
        Delay_ms(1000);
        LCD_clearScreen();
	/* Receive a certain Byte (Character) from Control Block to know if that is the System First Use or not */
	
    
	while(1)
	{
        
                first_time = isFirstTime();
		/* Calling The Program Start Function for HMI Block */
		Receive_First_Time_Check();
	}
}










