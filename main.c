#include "Dio.h"
#include "Port.h"
#include "Sw_Delay.h"
#include "gsm.h"
#include "gps.h"

/* Enable IRQ Interrupts ... This Macro enables IRQ interrupts by clearing the I-bit in the PRIMASK. */
#define Enable_Interrupts()    __asm("CPSIE I")

/* Disable IRQ Interrupts ... This Macro disables IRQ interrupts by setting the I-bit in the PRIMASK. */
#define Disable_Interrupts()   __asm("CPSID I")

/* Go to low power mode while waiting for the next interrupt */
#define Wait_For_Interrupt()   __asm("WFI")
/* Enable IRQ Interrupts ... This Macro enables IRQ interrupts by clearing the I-bit in the PRIMASK. */
#define Enable_Interrupts()    __asm("CPSIE I")

/* Disable IRQ Interrupts ... This Macro disables IRQ interrupts by setting the I-bit in the PRIMASK. */
#define Disable_Interrupts()   __asm("CPSID I")

/* Go to low power mode while waiting for the next interrupt */
#define Wait_For_Interrupt()   __asm("WFI")

#define NVIC_EN1_REG              (*((volatile unsigned long *)0xE000E104))


uint8 httpMessageBody[100];
uint8 latitude[10] = "";
uint8 longtide[10] = "";

#define URL "api.thingspeak.com/update"
#define API_KEY "api_key=C5ZRHJHSN2BZPF48"

void updateLocation(void){
    if( ( strcmp(latitude, "0") == 0 ) && ( strcmp(longtide, "0") == 0 ) ){
        return;
    }
    GSM_connectGPRS();
    strcat(httpMessageBody, API_KEY);
    strcat(httpMessageBody, "&field1=");
    strcat(httpMessageBody, latitude);
    strcat(httpMessageBody, "&field2=");
    strcat(httpMessageBody, longtide);
    GSM_sendHTTPRequest(URL, strlen(httpMessageBody), httpMessageBody, GSM_URL_ENCODED_CONTENT);
    httpMessageBody[0] = '\0';
}


int main()
{
    Port_Init(&Port_Configuration);
    Dio_Init(&Dio_Configuration);
    Uart_Init(&Uart_Configuration);
    
    GPS_init();
    GSM_init();
    NVIC_EN1_REG |= (1<<27);
    Enable_Interrupts();
    while(1){
        GPS_getLocation(latitude, longtide);
        updateLocation();
        Delay_Ms(10000);
    }
    return 0;
}
