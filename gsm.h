#ifndef GSM_H
#define GSM_H

#include "Uart.h"
#include "gps_gsm_buffer.h"

/* Definitions */
#define GSM_DELAY 100
#define GSM_AT_COMMAND "AT"

#ifndef AT_OK_CMD
#define AT_OK_CMD "OK"
#endif

#define GSM_IS_RDY_CMD "AT+CPIN?"
#define GSM_FULL_FUN_CMD "AT+CFUN=1"
#define GSM_VERBOSE_ERRS_ON_CMD "AT+CMEE=2"
#define GSM_ENABLE_BATTERY_CHECKS_CMD "AT+CBATCHK=1"
#define GMS_IS_NETWORK_REG_CMD "AT+CREG?"
#define GSM_SMS_TXT_MODE_CMD "AT+CMGF=1"
#define GSM_CLOSE_BEARER_CMD "AT+SAPBR=0,1"
#define GSM_OPEN_BEARER_CMD "AT+SAPBR=1,1"

/* 3 for setting bearer paramaters
 * Contype: Type of Internet connection
 * GPRS: GPRS connection 
 */
#define GSM_SET_BEARER_PARAMS_CMD "AT+SAPBR=3,1,\"Contype\",\"GPRS\""

#define GSM_SET_APN_SETTINGS_CMD "AT+SAPBR=3,1,\"APN\",\"internet.vodafone.net\""
#define GSM_SET_APN_USER_CMD "AT+SAPBR=3,1,\"USER\",\"internet\""
#define GSM_SET_APN_PWD_CMD "AT+SAPBR=3,1,\"PWD\",\"internet\""
#define GSM_QUERY_BEARER_CMD "AT+SAPBR=2,1"

/* HTTP Request related defintions */
#define GSM_HTTP_INIT "AT+HTTPINIT"
#define GSM_BEARER_PROFILE_IDENTIFIER "AT+HTTPPARA=\"CID\",1"
#define GSM_HTTP_CLIENT_URL "AT+HTTPPARA=\"URL\","
#define GSM_HTTP_JSON_CONTENT "AT+HTTPPARA=\"CONTENT\",\"application/json\""
#define GSM_HTTP_DATA_LENGTH "AT+HTTPDATA="
#define GSM_HTTP_POST_ACTION "AT+HTTPACTION=1"
#define GSM_HTTP_TERMINATE "AT+HTTPTERM"

/* Function Prototypes */
void GSM_sendCommand(uint8 * command);
void GSM_clearBuffer(void);
void GSM_setBuffer(void);
void GSM_init();
void GSM_connectGPRS(void);
void GSM_sendHTTPRequest(uint8 * url, uint8 dataLength, uint8 * data);


#endif