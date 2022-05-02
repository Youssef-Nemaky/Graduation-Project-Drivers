#include "Dio.h"
#include "Port.h"
#include "Sw_Delay.h"
#include "gsm.h"
#include "gps.h"

uint8 httpMessageBody[100];
uint8 latitude[10]  = "";
uint8 longtide[10] = "";

#define URL "api.thingspeak.com/update"
#define API_KEY "C5ZRHJHSN2BZPF48"

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
}


int main()
{
    Dio_Init(&Dio_Configuration);
    Port_Init(&Port_Configuration);
    GPS_init();
    GSM_init();

    while(1){
        GPS_getLocation(latitude, longtide);
        updateLocation();
        Delay_Ms(10000);
    }
    return 0;
}
