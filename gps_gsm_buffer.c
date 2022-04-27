#include "gps_gsm_buffer.h"

uint8 g_gps_gsm_buffer[GPS_GSM_BUFFER_SIZE] = {0};
uint8 g_gps_gsm_buffer_index = 0;
uint8 g_gps_gsm_rx_complete_flag = 0;
static uint8 counter = 0;

void gps_gsm_populate_buffer(uint8 uartData){
    /*
     * Commands are usually followed by a response that includes.
     * "<CR><LF><response><CR><LF>"
     */
    if(uartData == '\n' && g_gps_gsm_buffer_index != 1){
        g_gps_gsm_rx_complete_flag = 1;
    } else {
        if(uartData != '\r'){
            g_gps_gsm_buffer[g_gps_gsm_buffer_index++] = uartData;
        }
    }
}

void gps_gsm_reset_buffer(void){
    uint8 bufferCounter = 0; 

    /* Reset the buffer itself */
    for(bufferCounter = 0; bufferCounter < g_gps_gsm_buffer_index; bufferCounter++){
        g_gps_gsm_buffer[bufferCounter] = 0;
    }
    
    /* Reset the buffer index */
    g_gps_gsm_buffer_index = 0;

    /* Reset the buffer receive complete flag */
    g_gps_gsm_rx_complete_flag = 0;
}