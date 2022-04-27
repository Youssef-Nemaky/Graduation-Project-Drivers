#ifndef GPS_GSM_BUFFER_H
#define GPS_GSM_BUFFER_H

/* Include files */
#include "Uart.h"
#include <string.h>

/* Definitions */
#define GPS_GSM_BUFFER_SIZE 250


/* Functions prototypes */
void gps_gsm_populate_buffer(uint8 uartData);
void gps_gsm_reset_buffer(void);

/* Externing variables */
extern uint8 g_gps_gsm_buffer[GPS_GSM_BUFFER_SIZE];
extern uint8 g_gps_gsm_buffer_index;
extern uint8 g_gps_gsm_rx_complete_flag;
#endif