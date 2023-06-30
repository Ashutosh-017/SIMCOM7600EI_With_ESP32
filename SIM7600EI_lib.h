#include <stdio.h>
#include <stdio.h>
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_log.h"
#include "stdlib.h"
#include "UART_driver.h"

/* Define all posiable response */
#define OK "OK"
#define ERROR_type1 "+CME ERROR"
#define ERROR_type2 "ERROR"
#define COPS "+COPS: " // SIM operator 
#define CREG "+CREG: 0,1" // SIM Registration 
#define CPIN "+CPIN: " // SIM status 
#define CSQ  "+CSQ: " // Signal Quality 
#define CGMI "CGMI " // Request manufacturer identification
#define CGMM "CGMM " // Request model identification
#define REVISION "+CGMR: " //  Request revision identification
#define CGSN "CGSN " //  Request product serial number identification
#define CFUN "+CFUN: 1" // Module functionality 
#define CICCID "+ICCID: " // SIM ICCID
#define CSPN "+CSPN: " // service provider name from SIM
#define CNMP "+CNMP:" // SIM Mode 
#define CGREG "+CGREG: 0,1" // GPRS
#define CGACT "+CGACT " // PDP context
#define CGPS "+CGPS: 1" // GPS 
#define CGPSINFO "+CGPSINFO: " // GPS Information
#define SIM_GPRS_Waiting_reg "+CGREG: 0,2"
#define SIM_GPRS_reg_denied "+CGREG: 0,3"


/* SIM registration related response */
#define SIM_READY "READY"
#define SIM_NOT_READY "NOT READY"
#define SIM_LOCKED "SIM PIN"
#define SIM_NOT_INSERTED "NOT INSERTED"
#define SIM_PUK "SIM PUK"
#define AIRTEL_OPERATOR  "\"IND airtel\""
#define SIM_Waiting_reg "+CREG: 0,2"
#define SIM_reg_denied "+CREG: 0,3"
#define SIM_waiting "+CREG: 0,0"
#define MAX_VALUES 10
#define MAX_VALUE_LENGTH 50

enum PARSE_TYPE
{
    OK_TYPE         = 1,
    CREG_TYPE       = 3,
    COPS_TYPE       = 4,
    CPIN_TYPE       = 2,
    CGMI_TYPE       = 5,
    CGMM_TYPE       = 6,
    CGMR_TYPE       = 7,
    CGSN_TYPE       = 8,
    CFUN_TYPE       = 9,
    CICCID_TYPE     = 10,
    CSPN_TYPE       = 11,
    CSQ_TYPE        = 12,
    CNMP_TYPE       = 13,
    CGREG_TYPE      = 14,
    CGACT_TYPE      = 15,
    CGPS_TYPE       = 16,
    CGPSINFO_TYPE   = 17,


};

enum SimStatus {
  sim_error            = 0,
  sim_ready            = 1,
  sim_locked           = 2,
  sim_not_inserted     = 3,
  sim_not_ready        = 4,
};

enum Sim_Network_Reg_Status{
	NETWORK_NOT_REGISTERED      = 0,
	NETWORK_REGISTERED          = 1,
	NETWORK_WAITING_TO_Reg      = 2,
	NETWORK_REG_DENIED          = 3,
	NETWORK_UNKNOWN             = 4,
	NETWORK_REGISTERED_ROAMING  = 5,
};

enum Operator_Mode {
    AUTOMATIC           = 0,
    MANUAL              = 1,
    FORCE_DEREGISTER    = 2,
    SET_ONLY            = 3,
    MANUAL_AUTOMATIC    = 4,

};

enum SIM7600EI_Network_Access_Mode {

     AUTOMATIC_MODE                             = 2,
     GSM_ONLY                                   = 13,
     WCDMA_ONLY                                 = 14,
     LTE_ONLY                                   = 38,
     CDMA_ONLY                                  =  9,
     EVDO_ONLY                                  = 10,
     GSM_WCDMA_ONLY                             = 19,
     CDMA_EVDO_ONLY                             = 22,
     GSM_TDSCDMA_Only                           = 60,
     GSM_WCDMA_TDSCDMA_ONLY                     = 63,
     CDMA_EVDO_GSM_WCDMA_TDSCDMA_ONLY           = 67,
     GSM_WDMA_LTE_ONLY                          = 39,
     WDMA_LTE_ONLY                              = 54,
};

enum Sim_Internet_Reg_Status{
	INTERNET_NOT_REGISTERED         = 0,
	INTERNET_REGISTERED             = 1,
	INTERNET_SEARCHING              = 2,
	INTERNET_REG_DENIED             = 3,
	INTERNET_REG_UNKNOWN            = 4,
	INTERNET_REGISTERED_ROAMING     = 5,
};

enum  PDP_context_activation{
    PDP_DEACTTVATED     = 0,
    PDP_ACTTVATED       = 1,
};

enum GPS_activation{
    GPS_DEACTTVATED     = 0,
    GPS_ACTTVATED       = 1,
    GPS_INFO_GET        = 3,
    GPS_INFO_Fail       = 4,    
};

enum GPRS_activation{
    GPRS_DEACTTVATED         = 0,
    GPRS_ACTTVATED           = 1,
    GPRS_Waiting_to_connect  = 3,
    GPRS_registration_denied = 4, 
     
};

enum SIM7600EI_Functionality{
    Minimum_functionalit                   = 0,
    Full_functionality_online_mode         = 1,
    Disable_transmit_receive_RF_circuits   = 4,
    Factory_Test_Mode                      = 5,
    Reset                                  = 6,
    Offline_Mode                           = 7,
};

typedef struct {

	char data[15];
	int size;

}pass_data;

enum SIM7600EI_Oper_Sel_Mode{
	Automatic_Oper_Sel = 0,
	Mannual_Oper_Sel = 1,
	Mannual_Dereg = 2,
	Read_Oper_Only = 3,
	Mannual_Automatic_Oper_Sel = 4,
    Oprator_Info_get           = 5,
    Oprator_Info_fail          = 6,
};

typedef struct{
	char oper_name[30];   //oper = operator
	SIM7600EI_Network_Access_Mode oper_net_type;
	SIM7600EI_Oper_Sel_Mode oper_sel_mode;

}oper_detail_type;

enum SIM7600EI_state_t {
    SIM7600EI_START_FAILED = -1,
    SIM7600EI_STATE_POWER_ON = 0,
    SIM7600EI_STATE_POWER_OFF = 1,
    SIM7600EI_STATE_START_SUCCESS =2,
    SIM7600EI_STATE_START_FAILED =3,
    SIM7600EI_STATE_GNSS = 4,

};

typedef struct {

    SimStatus sim_sts;
    oper_detail_type  oper_detail;
    Sim_Network_Reg_Status  sim_net_reg_sts;
    GPS_activation Module_GPS;
    GPRS_activation GPRS_Status;
    PDP_context_activation PDP_Status;
    SIM7600EI_state_t Module_state;
    SIM7600EI_Functionality Module_functionality;

}SIM7600EI_data_t;

typedef struct GNSS_data{
	uint8_t gnss_data_found;
	double latitude;
	double longitude;
	uint32_t GPS_utc_time;
	uint32_t GPS_date;
	double hdop;
	double altitude;
	uint8_t fix_type;
	uint16_t speed_kmph;
	uint16_t speed_knot;
	uint8_t no_of_satellites;
};

typedef struct Module_SIM7600EI_data 
{
    uint8_t SIM_REG_Data1;
    uint8_t SIM_REG_Data2;
    uint8_t SIM_Mode_Data;
    char SIM_operator_Name[20];
    uint8_t SIM_operator_mode;
    uint8_t SIM_Access_technology_selected;
    uint8_t SIM_Signal_Data1;
    uint8_t SIM_Signal_Data2;

};

class SIM7600EI_Modem 
{
    public:
    void SIM7600EI_module_init();
    void SIM7600EI_AT_OFF_Echo();
    void getSIM_Status();
    void SIM7600EI_SIM_REG_STATUS();
    void SIM7600EI_get_operator();
    int parser_data(char data1[],  const char* cmd_type, Module_SIM7600EI_data* DATA ,pass_data data2[15] = {0}, pass_data data3[15] = {0}, char data4[20] = NULL , char data5[15] = NULL , char data6[15] = NULL , char data7[15] = NULL);
    bool parser(int parse_type, uint32_t timeout_ms);
    int parser_location_data(char dataResponse[], const char* GPSINFO_CMD , GNSS_data *DATA ,char GPS_data1[20] ,char GPS_data2[20] ,char GPS_data3[20] ,char GPS_data4[20] ,char GPS_data5[20] ,char GPS_data6[20] ,char GPS_data7[20] ,char GPS_data8[20] ,char GPS_data9[20] ,char GPS_data10[20]);
    void SIM7600EI_get_signal_quality();
    void SIM7600EI_Request_manufacturer_identification();
    void SIM7600EI_Request_model_identification();
    void SIM7600EI_Request_revision_identification();
    void SIM7600EI_Request_product_serial_number();
    void SIM7600EI_Module_functionality();
    void SIM7600EI_getSIM_ICCID();
    void SIM7600EI_getSIM_provider();
    void SIM7600EI_SIM_Mode();
    void SIM7600EI_Change_SIM_Mode();
    void SIM7600EI_getGPRS_status();
    void SIM7600EI_Activate_PDP_Context();
    void SIM7600EI_Activate_GPS();
    void SIM7600EI_getGPS_INFO();
    void SIM7600EI_START_MQTT();
    void SIM7600EI_ACCQ_MQTT_CLIENT();
    static SimStatus simstatus; 
	   
};

void module_init();

void module_MQTT_init();
