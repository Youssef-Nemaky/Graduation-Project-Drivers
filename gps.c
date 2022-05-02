#include "gps.h"



void GPS_sendCommand(uint8 * command){
    uint8 commandCounter = 0; 

    /* Reset the buffer before sending any commands to get a proper response from the module */
    gps_gsm_reset_buffer();

    while(command[commandCounter] != '\0'){
        Uart_SendByte(GPS_MODULE_UART, command[commandCounter]);
    }
    Uart_SendByte(GPS_MODULE_UART,'\r');
    Delay_Ms(GPS_DELAY);
}

void GPS_init(){
    uint8 counter = 0;
    /* Send multiple AT command to let the module configure its baud rate automatically */
    for(counter = 0; counter < 5; counter++){
        GPS_sendCommand(GPS_AT_COMMAND);
    }

    /* Turn off command echos */
    GPS_sendCommand(GPS_ECHO_OFF_CMD);
    
    /* Turn on GPS & GNS */
    GPS_sendCommand(GNS_PWR_ON_CMD);

    /* Set RMC NMEA */
    GPS_sendCommand(GNS_RMC_NMEA_CMD);

    Uart_setRXCallBack(gps_gsm_populate_buffer);
}

void GPS_handler(uint8 uartData){
    g_gps_gsm_buffer[g_gps_gsm_buffer_index] = uartData;
}


void GPS_getLocation(uint8 * latitude ,uint8 * longitude){
    uint8 buffer_counter = 0;
    uint8 latitudeCounter = 0, longitudeCounter = 0;
    uint8 numberOfCommas = 0;

    /* send get location command to the gps module */
    GPS_sendCommand(GNS_GET_LOCATION_CMD);

    /* Wait to receive the reponse and populate the buffer */
    while(!g_gps_gsm_rx_complete_flag);

    /* Now we have a buffer that has the response from the module
     * time to do some processing on that buffer 
     */
    for(buffer_counter = 0; buffer_counter < g_gps_gsm_buffer_index; buffer_counter++){
        if(g_gps_gsm_buffer[buffer_counter] == ','){
            numberOfCommas++;
        }
        if (numberOfCommas == 3) {
            latitude[latitudeCounter++] = g_gps_gsm_buffer[buffer_counter];
        } else if (numberOfCommas == 4) {
            longitude[longitudeCounter++] = g_gps_gsm_buffer[buffer_counter];
        } else if (numberOfCommas == 5) {
            /* No need to continue as we've already read the latitude and longtide */
            break;
        }
    }
}

