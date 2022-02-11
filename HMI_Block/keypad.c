#include "keypad.h"

#define NUMBER_OF_ITERATIONS_PER_ONE_MILI_SECONDs 762

void Delay2_Ms(unsigned long long n)
{
    volatile unsigned long long count = 0;
    while(count++ < (NUMBER_OF_ITERATIONS_PER_ONE_MILI_SECONDs * n) );
}
#if (KEYPAD_NUM_COLS == 3)
/*
 * Function responsible for mapping the switch number in the keypad to
 * its corresponding functional number in the proteus for 4x3 keypad
 */
static uint8 KEYPAD_4x3_adjustKeyNumber(uint8 button_number);
#elif (KEYPAD_NUM_COLS == 4)
/*
 * Function responsible for mapping the switch number in the keypad to
 * its corresponding functional number in the proteus for 4x4 keypad
 */
static uint8 KEYPAD_4x4_adjustSwitchNumber(uint8 button_number); // function responsible fo 3*3 keypad to know the key pressed
#endif

#define keypad_index 3
uint8 KEYPAD_getPressedKey(void)
{
    uint8 row, col = 0; /* 2 variables to loop on rows and columns*/
    while (1)
    {
        for (col = 0; col < KEYPAD_NUM_COLS; col++)
        {
            Dio_WriteChannel(4+keypad_index+col, STD_HIGH);
            //Port_SetPinDirection((4 + col + 8), PORT_PIN_OUT); /*we use this function in port driver to set column by column as output pin*/
            for (row = 0; row < KEYPAD_NUM_ROWS; row++)
            {                                                 /*looping on rows to know if any of them is pressed */
                if (Dio_ReadChannel(row + keypad_index)) /*read rows from first to last one*/
                {
                    Dio_WriteChannel(4+keypad_index+col, STD_LOW);
                    //Port_SetPinDirection((4 + col + 8), PORT_PIN_IN); /*we use this function in port driver to reset the used column as input  pin*/

#if (KEYPAD_NUM_COLS == 4)
                    return KEYPAD_4x4_adjustSwitchNumber((row * KEYPAD_NUM_COLS) + (col + 1)); /*passing this equation to the corresponding function to know the pressed key*/

#elif (KEYPAD_NUM_COLS == 3)

                    return KEYPAD_4x3_adjustSwitchNumber((row * KEYPAD_NUM_COLS) + (col + 1)); /*passing this equation to the corresponding function to know the pressed key*/
#endif
                }
            }
            Dio_WriteChannel(4+keypad_index+col, STD_LOW);
            //Port_SetPinDirection((4 + col + 8), PORT_PIN_IN); /*we use this function in port driver to reset the used column as input  pin*/
        }
    }
}

#if (KEYPAD_NUM_COLS == 3)
static uint8 KEYPAD_4x3_adjustSwitchNumber(uint8 a_switchNumber){
    uint8 pressedKey;
    switch (a_switchNumber){
        case 10: pressedKey = '*';            break;
        case 11: pressedKey = 0;              break;
        case 12: pressedKey = '#';            break;
        default: pressedKey = a_switchNumber; break;
    }
    return pressedKey;
}
#elif (KEYPAD_NUM_COLS == 4)

static uint8 KEYPAD_4x4_adjustSwitchNumber(uint8 a_switchNumber){
    uint8 pressedKey;
    switch (a_switchNumber){
    case 4:   pressedKey = 'A';              break;
    case 5:   pressedKey = 4;                break;
    case 6:   pressedKey = 5;                break;
    case 7:   pressedKey = 6;                break;
    case 8:   pressedKey = 'B';              break;            
    case 9:   pressedKey = 7;                break;
    case 10:  pressedKey = 8;                break;
    case 11:  pressedKey = 9;                break;
    case 12:  pressedKey = 'C';              break;
    case 13:  pressedKey = '*';              break;
    case 14:  pressedKey = 0;                break;
    case 15:  pressedKey = '#';              break;
    case 16:  pressedKey = 'D';              break;
    default: pressedKey = a_switchNumber;    break;
    }
    return pressedKey;
}
#endif
