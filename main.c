#include "Dio.h"
#include "Port.h"
#include "lcd.h"

void Delay_Ms(unsigned long long n);
int main()
{
    Dio_Init(&Dio_Configuration);
    Port_Init(&Port_Configuration);
    LCD_init();
    uint8 counter = 0;
    LCD_displayCharacter('S');
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
    
    LCD_moveCursor(0,0);
    LCD_displayCharacter('A' + counter++);
    if(counter >= 27){
        counter = 0;
    }
    Delay_Ms(1000);
    }
    return 0;
}
