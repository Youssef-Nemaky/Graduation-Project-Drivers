#include "Dio.h"
#include "Port.h"
#include "lcd.h"
#include "keypad.h"
#include "sw_delay.h"
#include "Uart.h"
#include "external_eeprom.h"


int main()
{

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
/*
    LCD_moveCursor(0,0);
    pressedKey = KEYPAD_getPressedKey();
    if((pressedKey >= 0) && (pressedKey <= 9)){
        LCD_displayCharacter(('0' + pressedKey));
    } else {
        LCD_displayCharacter(pressedKey);
    }
  */   
    
        LCD_moveCursor(0,0);
        receivedByte = Uart_ReceiveByte(UART7_MODULE_NUMBER);
        Uart_SendByte(UART7_MODULE_NUMBER , '#');
        LCD_displayCharacter(receivedByte);
    }

    return 0;
}
