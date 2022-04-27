#include "gsm.h"

/* Static functions prototypes */
static void integerToString(sint8 * dest, uint8 number);

void GSM_init(){
    uint8 counter = 0;
    /* Send multiple AT command to let the module configure its baud rate automatically */
    for(counter = 0; counter < 5; counter++){
        Gps_sendCommand(GSM_AT_COMMAND);
    }

    /* Enable Full functionality for GSM (both transmit and receive) */
    GSM_sendCommand(GSM_FULL_FUN_CMD);

    /* Set SMS mode to text mode */
    GSM_sendCommand(GSM_SMS_TXT_MODE_CMD);
    
    /* Close the bearer */
    GSM_sendCommand(GSM_CLOSE_BEARER_CMD);
    
    /* Set bearer parameters */
    /* 3 for setting bearer paramaters
     * Contype: Type of Internet connection
     * GPRS: GPRS connection
     */
    GSM_sendCommand(GSM_SET_BEARER_PARAMS_CMD);

    /* Set bearer APN settings */
    GSM_sendCommand(GSM_SET_APN_SETTINGS_CMD);

    /* Set bearer APN username */
    GSM_sendCommand(GSM_SET_APN_USER_CMD);

    /* Set bearer APN password */
    GSM_sendCommand(GSM_SET_APN_PWD_CMD);
    
    /* Open bearer */
    GSM_sendCommand(GSM_OPEN_BEARER_CMD);  
}

void GSM_connectGPRS(void){
    GSM_sendCommand(GSM_CLOSE_BEARER_CMD);
    GSM_sendCommand(GSM_SET_BEARER_PARAMS_CMD);
    GSM_sendCommand(GSM_SET_APN_SETTINGS_CMD);
    GSM_sendCommand(GSM_SET_APN_USER_CMD);
    GSM_sendCommand(GSM_SET_APN_PWD_CMD);
    GSM_sendCommand(GSM_OPEN_BEARER_CMD);
}

void GSM_sendHTTPRequest(uint8 * url, uint8 dataLength, uint8 * data){
    sint8 urlCommand[200] = "";
    sint8 dataLengthCommand[60] = "";
    sint8 dataLengthStr[10] = "";

    GSM_connectGPRS();
    GSM_sendCommand(GSM_HTTP_INIT);
    GSM_sendCommand(GSM_BEARER_PROFILE_IDENTIFIER);
    
    strcat(urlCommand, GSM_HTTP_CLIENT_URL);
    strcat(urlCommand, url);
    GSM_sendCommand(urlCommand);

    
    GSM_sendCommand(GSM_HTTP_JSON_CONTENT);
    
    strcat(dataLengthCommand, GSM_HTTP_DATA_LENGTH);
    integerToString(dataLengthStr, dataLength);
    strcat(dataLengthCommand, dataLengthStr);
    strcat(dataLengthCommand, "1000");
    GSM_sendCommand(dataLengthCommand);

    GSM_sendCommand(data);

    GSM_sendCommand(GSM_HTTP_POST_ACTION);

    GSM_sendCommand(GSM_HTTP_TERMINATE);
}

void GSM_sendCommand(uint8 * command){
    uint8 commandCounter = 0; 
    
    /* Reset the buffer before sending any commands to get a proper response from the module */
    gps_gsm_reset_buffer();
    
    while(command[commandCounter] != '\0'){
        Uart_SendByte(GSM_MODULE_UART, command[commandCounter]);
    }
    Uart_SendByte('\r');
    Delay_Ms(GSM_DELAY); // maybe replace me with while(!g_gps_gsm_rx_complete_flag) 
}

static void integerToString(sint8 * dest, uint8 number){
    uint8 counter = 0;
    uint8 tempNumber = number;
    if(number == 0){
        dest[0] = '0';
        return;
    }
    /* Get the length of the number */
    while(number != 0){
        counter++;
        number /= 10;
    }
    
    while(tempNumber != 0){
        dest[--counter] = (tempNumber%10) + '0';
        tempNumber /= 10;
    }
}