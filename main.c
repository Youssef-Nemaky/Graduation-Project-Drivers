#include "Dio.h"
#include "Port.h"
#include "lcd.h"
#include "keypad.h"
#include "sw_delay.h"
#include "external_eeprom.h"

int main()
{
    uint8 pressedKey = 0;
    uint8 byteToRead = 0;
    Dio_Init(&Dio_Configuration);
    Port_Init(&Port_Configuration);
    I2c_Init(&I2c_Confiuration);
    LCD_init();
    LCD_displayString("TESTING");
    Delay_Ms(2000);
    LCD_clearScreen();
    LCD_displayInteger(9845);
    Delay_Ms(2000);
    LCD_clearScreen();
    LCD_displayCharacter('S');
    Delay_Ms(2000);
    LCD_clearScreen();
    LCD_displayString("Content of 0x000");
    LCD_displayStringRowColumn(1,0,"is ");
    EEPROM_readByte(0x0000, &byteToRead);
    LCD_displayInteger(byteToRead);
    EEPROM_writeByte(0x0000, 99);
    Delay_Ms(5000);
    LCD_clearScreen();
    LCD_displayString("After changing it");
    Delay_Ms(2000);
    LCD_clearScreen();
    LCD_displayString("Content of 0x000");
    LCD_displayStringRowColumn(1,0,"is ");
    EEPROM_readByte(0x0000, &byteToRead);
    LCD_displayInteger(byteToRead);
    Delay_Ms(5000);
    
    while(1){

        /*
        Dio_WriteChannelGroup(DioConf_LEDS_GRP_PTR, (1<<counter));
        counter++;
        if(counter >= 4){
            counter = 0;
        }
        Delay_MS(1000);
        */
        /*

       if(Dio_ReadChannel(DioConf_SW1_CHANNEL_ID_INDEX) == STD_LOW){
           Dio_WriteChannel(DioConf_LED1_CHANNEL_ID_INDEX, STD_HIGH);
       } else {
           Dio_WriteChannel(DioConf_LED1_CHANNEL_ID_INDEX, STD_LOW);
       }
*/
    //LCD_displayCharacter('A');
    /*
    LCD_moveCursor(0,0);
    LCD_displayCharacter('A' + counter++);
    if(counter >= 27){
        counter = 0;
    }
    Delay_Ms(1000);
*/
    LCD_moveCursor(0,0);
    pressedKey = KEYPAD_getPressedKey();
    if((pressedKey >= 0) && (pressedKey <= 9)){
        LCD_displayCharacter(('0' + pressedKey));
    } else {
        LCD_displayCharacter(pressedKey);
    }
    
    
    }

    return 0;
}
