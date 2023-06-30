
#include "SIM7600EI_lib.h"

SIM7600EI_Modem SIM7600EI_Modem_;
SIM7600EI_data_t Module_data;
SimStatus SIM7600EI_Modem::simstatus = sim_not_ready;
oper_detail_type Network_mode;
bool SIM_registration_done_flag = false;
/****************************************************** SENDIND AT COMMANDS *******************************************************************************************/
void SIM7600EI_Modem ::SIM7600EI_module_init()
{
    uart_write_bytes(UART_NUM_0, "AT\r\n", 11);
}

void SIM7600EI_Modem ::SIM7600EI_AT_OFF_Echo()
{
    uart_write_bytes(UART_NUM_0, "ATE0\r\n", 11);
}

void SIM7600EI_Modem ::getSIM_Status()
{
    uart_write_bytes(UART_NUM_0, "AT+CPIN?\r\n", 11);
}

void SIM7600EI_Modem::SIM7600EI_SIM_REG_STATUS()
{
    uart_write_bytes(UART_NUM_0, "AT+CREG?\r\n", 11);
}

void SIM7600EI_Modem::SIM7600EI_get_operator()
{
    uart_write_bytes(UART_NUM_0, "AT+COPS?\r\n", 11);
}

void SIM7600EI_Modem::SIM7600EI_get_signal_quality()
{
    uart_write_bytes(UART_NUM_0, "AT+CSQ\r\n", 11);
}
void SIM7600EI_Modem::SIM7600EI_Request_manufacturer_identification()
{
    uart_write_bytes(UART_NUM_0, "AT+CGMI\r\n", 11);
}
void SIM7600EI_Modem::SIM7600EI_Request_model_identification()
{
    uart_write_bytes(UART_NUM_0, "AT+CGMM\r\n", 11);
}
void SIM7600EI_Modem::SIM7600EI_Request_revision_identification()
{
    uart_write_bytes(UART_NUM_0, "AT+CGMR\r\n", 11);
}
void SIM7600EI_Modem::SIM7600EI_Request_product_serial_number()
{
    uart_write_bytes(UART_NUM_0, "AT+CGSN\r\n", 11);
}
void SIM7600EI_Modem::SIM7600EI_Module_functionality()
{
    uart_write_bytes(UART_NUM_0, "AT+CFUN?\r\n", 11);
}
void SIM7600EI_Modem::SIM7600EI_getSIM_ICCID()
{
    uart_write_bytes(UART_NUM_0, "AT+CICCID\r\n", 11);
}
void SIM7600EI_Modem::SIM7600EI_getSIM_provider()
{
    uart_write_bytes(UART_NUM_0, "AT+CSPN?\r\n", 11);
}
void SIM7600EI_Modem::SIM7600EI_SIM_Mode()
{
    uart_write_bytes(UART_NUM_0, "AT+CNMP?\r\n", 11);
}

void SIM7600EI_Modem::SIM7600EI_Change_SIM_Mode()
{
    uart_write_bytes(UART_NUM_0, "AT+CNMP=2\r\n", 11);
}
void SIM7600EI_Modem::SIM7600EI_getGPRS_status()
{
    uart_write_bytes(UART_NUM_0, "AT+CGREG?\r\n", 11);
}

void SIM7600EI_Modem::SIM7600EI_Activate_PDP_Context()
{
    uart_write_bytes(UART_NUM_0, "AT+CGACT=1\r\n", 11);
}
void SIM7600EI_Modem::SIM7600EI_Activate_GPS()
{
    uart_write_bytes(UART_NUM_0, "AT+CGPS=1\r\n", 11);
}
void SIM7600EI_Modem::SIM7600EI_getGPS_INFO()
{
    uart_write_bytes(UART_NUM_0, "AT+CGPSINFO\r\n", 15);
}
void SIM7600EI_Modem::SIM7600EI_START_MQTT()
{
    uart_write_bytes(UART_NUM_0 , "AT+CMQTTSTART\r\n" , 15);
}
void SIM7600EI_Modem::SIM7600EI_ACCQ_MQTT_CLIENT()
{
   // uart_write_bytes(UART_NUM_0 , "AT+CMQTTACCQ=1,"client test1"\r\n" , 30);
}

//void SIM7600EI_Modem::SIM

/*********************************************************** END ****************************************************************************************************************************************************************/

/*********************************************************** PARSING RESPONSE DATA ******************************************************************************************************************************************/

int SIM7600EI_Modem::parser_data(char data1[], const char *cmd_type, Module_SIM7600EI_data *DATA, pass_data data2[15], pass_data data3[15], char data4[20], char data5[15], char data6[15], char data7[15])
{
    char *copsPtr = strstr(data1, cmd_type); // searching word
    if (copsPtr == nullptr)
    {
        printf("\nWord not found in response\n");
        return 0; // Return false or an error code
    }

    // Move the pointer after the word and skip leading whitespaces
    copsPtr += 5; // Length of word
    while (*copsPtr == ' ')
    {
        copsPtr++;
    }

    // Copy the Response data to respective variables
    strncpy(data2[0].data, copsPtr, 15);
    data2[0].data[14] = '\0'; // Ensure null-termination
    data2[0].size = strlen(data2[0].data);

    // Tokenize the remaining string based on comma separation
    char *token = strtok(copsPtr, ",");
    if (token == nullptr)
    {
        printf("\nResponse not found \n");
        return 0; // Return false
    }

    // Move to the next token
    token = strtok(nullptr, ",");
    if (token == nullptr)
    {
        printf("\nResponse not found\n");
        return 0; // Return false
    }

    strncpy(data3[0].data, token, 15);
    data3[0].data[14] = '\0';
    data3[0].size = strlen(data3[0].data);

    // Copy additional tokens if provided
    if (data4 != nullptr)
    {
        token = strtok(nullptr, ",");
        if (token != nullptr)
        {
            strncpy(data4, token, 20);
            data4[19] = '\0'; // Ensure null-termination
        }
    }

    if (data5 != nullptr)
    {
        token = strtok(nullptr, ",");
        if (token != nullptr)
        {
            strncpy(data5, token, 15);
            data5[14] = '\0'; // Ensure null-termination
        }
    }

    if (data6 != nullptr)
    {
        token = strtok(nullptr, ",");
        if (token != nullptr)
        {
            strncpy(data6, token, 15);
            data6[14] = '\0'; // Ensure null-termination
        }
    }

    if (data7 != nullptr)
    {
        token = strtok(nullptr, ",");
        if (token != nullptr)
        {
            strncpy(data7, token, 15);
            data7[14] = '\0';
        }
    }

    if (strcmp(cmd_type, "CREG:") == 0) // Compare strings using strcmp
    {
        DATA->SIM_REG_Data1 = static_cast<uint8_t>(atoi(data2[0].data)); // Convert char to uint8_t
        DATA->SIM_REG_Data2 = static_cast<uint8_t>(atoi(data3[0].data));
        printf("Registration data1: %d\n", DATA->SIM_REG_Data1);
        printf("Registration data2: %d\n", DATA->SIM_REG_Data2);
    }
    else if (strcmp(cmd_type , "COPS:") == 0)
    {
        DATA->SIM_operator_mode = static_cast<uint8_t>(atoi(data2[0].data)); 
        strcpy(DATA->SIM_operator_Name, data4);
        DATA->SIM_Access_technology_selected = static_cast<uint8_t>(data5[0] - '0');
        printf("Operator Mode: %d\n", DATA->SIM_REG_Data1);
        printf("SIM Operator Name: %s\n", DATA->SIM_operator_Name);
        printf("SIM Access technology selected: %d\n", DATA->SIM_Access_technology_selected);
    }
    else if (strcmp(cmd_type , "+CSQ:") == 0)
    {
        DATA->SIM_Signal_Data1 = static_cast<uint8_t>(atoi(data2[0].data)); 
        DATA->SIM_Signal_Data2 = static_cast<uint8_t>(atoi(data3[0].data));
        printf("SIM Signal data1 : %d\n", DATA->SIM_Signal_Data1);
        printf("SIM Signal data2 : %d\n", DATA->SIM_Signal_Data2);        

    }
        return 1; // Return true or a success code
}

/*********************************************************** END *********************************************************************************************************************************************************************************************************************************************************/

/************************************************************* PARSING LOCATION DATA **********************************************************************************************************************************************************************************************************************************************/
int SIM7600EI_Modem::parser_location_data(char dataResponse[], const char *GPSINFO_CMD, GNSS_data *DATA , char GPS_data1[20], char GPS_data2[20], char GPS_data3[20], char GPS_data4[20], char GPS_data5[20], char GPS_data6[20], char GPS_data7[20], char GPS_data8[20], char GPS_data9[20], char GPS_data10[20])
{
    char *gpsInfoPtr = strstr(dataResponse, GPSINFO_CMD); // searching word
    if (gpsInfoPtr == nullptr)
    {
        printf("\nGPS info not found in response\n");
        return 0; // Return false or an error code
    }

    // Move the pointer after the word and skip leading whitespaces
    gpsInfoPtr += strlen(GPSINFO_CMD); // Length of word
    while (*gpsInfoPtr == ' ')
    {
        gpsInfoPtr++;
    }

    // Tokenize the remaining string based on comma separation
    char *token = strtok(gpsInfoPtr, ",");
    if (token == nullptr)
    {
        printf("\nGPS info not found\n");
        return 0; // Return false
    }

    // Copy the GPS data to respective variables
    strncpy(GPS_data1, token, 20);
    GPS_data1[9] = '\0'; // Ensure null-termination

    // Move to the next token
    token = strtok(nullptr, ",");
    if (token == nullptr)
    {
        printf("\nGPS info not found\n");
        return 0; // Return false
    }

    strncpy(GPS_data2, token, 20);
    GPS_data2[9] = '\0'; // Ensure null-termination

    token = strtok(nullptr, ",");
    if (token == nullptr)
    {
        printf("\nGPS info not found\n");
        return 0; // Return false
    }
    strncpy(GPS_data3, token , 20);
    GPS_data3[9] = '\0';

    token = strtok(nullptr, ",");
    if (token == nullptr)
    {
        printf("\nGPS info not found\n");
        return 0; // Return false
    }
    strncpy(GPS_data4, token , 20);

    GPS_data4[9] = '\0';
    token = strtok(nullptr, ",");
    if (token == nullptr)
    {
        printf("\nGPS info not found\n");
        return 0; // Return false
    }

    strncpy(GPS_data5, token, 20);
    GPS_data5[9] = '\0';

    token = strtok(nullptr, ",");
    if (token == nullptr)
    {
        printf("\nGPS info not found\n");
        return 0; // Return false
    }

    strncpy(GPS_data6, token , 20);
    GPS_data6[9] = '\0';

    token = strtok(nullptr, ",");
    if (token == nullptr)
    {
        printf("\nGPS info not found\n");
        return 0; // Return false
    }

    strncpy(GPS_data7, token , 20);
    GPS_data7[9] = '\0';

    token = strtok(nullptr, ",");
    if (token == nullptr)
    {
        printf("\nGPS info not found\n");
        return 0; // Return false
    }

    strncpy(GPS_data8, token , 20);
    GPS_data8[9] = '\0';

    token = strtok(nullptr, ",");
    if (token == nullptr)
    {
        printf("\nGPS info not found\n");
        return 0; // Return false
    }

    strncpy(GPS_data9, token , 20);
    GPS_data9[9] = '\0';

    token = strtok(nullptr, ",");
    if (token == nullptr)
    {
        printf("\nGPS info not found\n");
        return 0; // Return false
    }

    strncpy(GPS_data10, token , 20);
    GPS_data10[9] = '\0';

    DATA->latitude = atof(GPS_data1);
    DATA->longitude = atof(GPS_data3);
    printf("Latitude : %lf\n",DATA->latitude);
    printf("Longitude : %lf\n",DATA->longitude);

    return 1; // Return true or a success code
}
/************************************************************** END *************************************************************************************************************************************************************************************************************************************************************************/

/******************************************************** Receiving AT COMMAND RESPONSE *****************************************************************************************************************************************************************************************************************************************/

bool SIM7600EI_Modem::parser(int parse_type, uint32_t timeout_ms) // Function to read AT command response
{
    Module_SIM7600EI_data moduleDATA;
    GNSS_data moduleGPS_DATA;
    char response[BUF_SIZE];
    pass_data resp_data1[15];
    pass_data resp_data2[15];
    char resp_data3[15];
    char resp_data4[15];
    //pass_data data_to_check1;
    //pass_data data_to_check2;
    //pass_data data_to_check3;
    //pass_data data_to_check4;
    static int par = -1;

    memset(response, 0, sizeof(response));

    TickType_t start_time = xTaskGetTickCount(); // Get the start time of the loop

    while ((xTaskGetTickCount() - start_time) < pdMS_TO_TICKS(timeout_ms))
    {
        memset(response, 0, sizeof(response));
        // Read UART data into the response buffer
        int bytesRead = uart_read_bytes(UART_NUM_0, response, BUF_SIZE - 1, pdMS_TO_TICKS(500));

        if (bytesRead > 0)
        {
            response[bytesRead] = '\0'; // Add null-terminator to the received data

            printf("\nReceived response: %s\n", response);

            // Check the response type using switch case

            switch (parse_type)
            {

            case OK_TYPE:
                // Perform actions for OK response
                if (strstr(response, OK) != NULL)
                {
                    printf("Response type: OK\n");
                    break;
                }
                else if (strstr(response, ERROR_type2) != NULL)
                {
                    printf("Response type: ERROR\n");
                    return false;
                    break;
                }
                else
                {
                    printf("Module fail to Start\n");
                    return false;
                    break;
                }

            case CPIN_TYPE:
                if (strstr(response, SIM_READY) != NULL)
                {
                    printf("Response type: OK\n");
                    break;
                }
                else if (strstr(response, ERROR_type2) != NULL)
                {
                    printf("Response type: ERROR\n");
                    return false;
                    break;
                }
                else if (strstr(response, SIM_NOT_READY) != NULL)
                {
                    printf("Response type: ERROR\n");
                    return false;
                    break;
                }
                else
                {
                    printf("SIM is not present in module\n");
                    return false;
                    break;
                }

            case CREG_TYPE:
                if (strstr(response, CREG) != NULL)
                {
                    printf("Response type: OK\n");
                    par = SIM7600EI_Modem_.parser_data(response, "CREG:", &moduleDATA, resp_data1, resp_data2);
                    break;
                }
                else if (strstr(response, SIM_Waiting_reg) != NULL)
                {
                    printf("Module waiting to register with network\n");
                    Module_data.sim_net_reg_sts = NETWORK_WAITING_TO_Reg;
                    par = SIM7600EI_Modem_.parser_data(response, "CREG:", &moduleDATA, resp_data1, resp_data2);
                    vTaskDelay(20000 / portTICK_PERIOD_MS);
                    return false;
                    break;
                }
                else if (strstr(response, SIM_reg_denied) != NULL)
                {
                    printf("Module register denied\n");
                    Module_data.sim_net_reg_sts = NETWORK_REG_DENIED;
                    par = SIM7600EI_Modem_.parser_data(response, "CREG:", &moduleDATA, resp_data1, resp_data2);

                    vTaskDelay(20000 / portTICK_PERIOD_MS);
                    return false;
                    break;
                }
                else if (strstr(response, SIM_waiting) != NULL)
                {
                    printf("Module searching new network register\n");
                    Module_data.sim_net_reg_sts = NETWORK_WAITING_TO_Reg;
                    par = SIM7600EI_Modem_.parser_data(response, "CREG:", &moduleDATA, resp_data1, resp_data2);
                    vTaskDelay(20000 / portTICK_PERIOD_MS);
                    return false;
                    break;
                }
                else if (strstr(response, ERROR_type1) != NULL)
                {
                    printf("Response type: ERROR\n");
                    return false;
                    break;
                }
                else if (strstr(response, ERROR_type2) != NULL)
                {
                    printf("Response type: ERROR\n");
                    return false;
                    break;
                }
                else
                {
                    printf("Module fail to register\n");
                    return false;
                    break;
                }

            case COPS_TYPE:
                memset(resp_data3, 0, sizeof(resp_data3));
                memset(resp_data4, 0, sizeof(resp_data4));

                if (strstr(response, COPS) != NULL)
                {
                    printf("Response type: OK\n");
                    par = SIM7600EI_Modem_.parser_data(response, "COPS:", &moduleDATA, resp_data1, resp_data2, resp_data3, resp_data4);
                    break;
                }
                else if (strstr(response, ERROR_type1) != NULL)
                {
                    printf("Response type: ERROR\n");
                    return false;
                    break;
                }
                else if (strstr(response, ERROR_type2) != NULL)
                {
                    printf("Response type: ERROR\n");
                    return false;
                    break;
                }
                else
                {
                    printf("Module fail to check operator\n");
                    return false;
                    break;
                }

            case CSQ_TYPE:
                if (strstr(response, CSQ) != NULL)
                {
                    printf("Response type: OK\n");
                    par = SIM7600EI_Modem_.parser_data(response, "+CSQ:", &moduleDATA, resp_data1, resp_data2);
                    break;
                }
                else if (strstr(response, ERROR_type2) != NULL)
                {
                    printf("Response type: ERROR\n");
                    return false;
                    break;
                }
                else if (strstr(response, ERROR_type1) != NULL)
                {
                    printf("Response type: OK\n");
                    break;
                }
                else
                {
                    printf("Module fail to check signal quality\n");

                    return false;
                    break;
                }

            case CNMP_TYPE:
                if (strstr(response, CNMP) != NULL)
                {
                    printf("Response type: OK\n");
                    break;
                }
                else if (strstr(response, ERROR_type1) != NULL)
                {
                    printf("Response type: ERROR\n");
                    return false;
                    break;
                }
                else if (strstr(response, ERROR_type2) != NULL)
                {
                    printf("Response type: ERROR\n");
                    return false;
                    break;
                }
                else
                {
                    printf("Module fail to check oprator\n");
                    return false;
                    break;
                }

            case CGPS_TYPE:
                if (strstr(response, OK) != NULL)
                {
                    printf("Response type: OK\n");
                    break;
                }
                else if (strstr(response, ERROR_type1) != NULL)
                {
                    printf("Response type: ERROR\n");
                    return false;
                    break;
                }
                else if (strstr(response, ERROR_type2) != NULL)
                {
                    printf("Response type: ERROR\n");
                    return false;
                    break;
                }
                else
                {
                    printf("Module GPS Activation fail\n");
                    return false;
                    break;
                }

            case CGPSINFO_TYPE:
                static char GPS_data1[20];
                static char GPS_data2[20];
                static char GPS_data3[20];
                static char GPS_data4[20];
                static char GPS_data5[20];
                static char GPS_data6[20];
                static char GPS_data7[20];
                static char GPS_data8[20];
                static char GPS_data9[20];
                static char GPS_data20[20];
                memset(GPS_data1, 0, 20);
                memset(GPS_data2, 0, 20);
                memset(GPS_data3, 0, 20);
                memset(GPS_data4, 0, 20);
                memset(GPS_data5, 0, 20);
                memset(GPS_data6, 0, 20);
                memset(GPS_data7, 0, 20);
                memset(GPS_data8, 0, 20);
                memset(GPS_data9, 0, 20);
                memset(GPS_data20, 0, 20);

                if (strstr(response, CGPSINFO) != NULL)
                {
                    printf("Response type: OK\n");
                    par = SIM7600EI_Modem_.parser_location_data(response, "CGPSINFO:", &moduleGPS_DATA, GPS_data1, GPS_data2, GPS_data3, GPS_data4, GPS_data5, GPS_data6, GPS_data7, GPS_data8, GPS_data9, GPS_data20);
                    break;
                }
                else if (strstr(response, ERROR_type1) != NULL)
                {
                    printf("Response type: ERROR\n");
                    return false;
                    break;
                }
                else if (strstr(response, ERROR_type2) != NULL)
                {
                    printf("Response type: ERROR\n");
                    return false;
                    break;
                }
                else
                {
                    printf("Module GPS Activation fail\n");
                    return false;
                    break;
                }

            case CFUN_TYPE:
                if (strstr(response, CFUN) != NULL)
                {
                    printf("Response type: OK\n");

                }
                else if (strstr(response, ERROR_type1) != NULL)
                {
                    printf("Response type: ERROR\n");
                    return false;
                    break;
                }
                else if (strstr(response, ERROR_type2) != NULL)
                {
                    printf("Response type: ERROR\n");
                    return false;
                    break;
                }
                else
                {
                    printf("Module fail to check oprator\n");
                    return false;
                    break;
                }

            case CGREG_TYPE:
                if (strstr(response, CGREG) != NULL)
                {
                    printf("Response type: OK\n");
                    par = SIM7600EI_Modem_.parser_data(response, "GREG:", &moduleDATA, resp_data1, resp_data2);
                    break;
                }
                else if (strstr(response, SIM_GPRS_Waiting_reg) != NULL)
                {
                    printf("Module GPRS waiting to register with network\n");
                    Module_data.GPRS_Status = GPRS_Waiting_to_connect;
                    par = SIM7600EI_Modem_.parser_data(response, "GREG:", &moduleDATA, resp_data1, resp_data2);
                    vTaskDelay(20000 / portTICK_PERIOD_MS);
                    return false;
                    break;
                }
                else if (strstr(response, SIM_GPRS_reg_denied) != NULL)
                {
                    printf("Module GPRS register denied\n");
                    Module_data.GPRS_Status = GPRS_registration_denied;
                    par = SIM7600EI_Modem_.parser_data(response, "GREG:", &moduleDATA, resp_data1, resp_data2);

                    vTaskDelay(20000 / portTICK_PERIOD_MS);
                    return false;
                    break;
                }
                else if (strstr(response, SIM_waiting) != NULL)
                {
                    printf("Module searching new network register\n");
                    Module_data.sim_net_reg_sts = NETWORK_WAITING_TO_Reg;
                    par = SIM7600EI_Modem_.parser_data(response, "GREG:", &moduleDATA, resp_data1, resp_data2);
                    vTaskDelay(20000 / portTICK_PERIOD_MS);
                    return false;
                    break;
                }
                else if (strstr(response, ERROR_type1) != NULL)
                {
                    printf("Response type: ERROR\n");
                    return false;
                    break;
                }
                else if (strstr(response, ERROR_type2) != NULL)
                {
                    printf("Response type: ERROR\n");
                    return false;
                    break;
                }
                else
                {
                    printf("Module fail to register\n");
                    return false;
                    break;
                }

            case CGACT_TYPE:
                if (strstr(response, OK) != NULL)
                {
                    printf("Response type: OK\n");
                    break;
                }
                else if (strstr(response, ERROR_type1) != NULL)
                {
                    printf("Response type: ERROR\n");
                    return false;
                    break;
                }
                else if (strstr(response, ERROR_type2) != NULL)
                {
                    printf("Response type: ERROR\n");
                    return false;
                    break;
                }
                else
                {
                    printf("Module GPS Activation fail\n");
                    return false;
                    break;
                }

            default:

                printf("Unknown response type\n");
                return false;
            }

            return true;
        }
    }

    if ((xTaskGetTickCount() - start_time) >= pdMS_TO_TICKS(timeout_ms))
    {

        printf("\n No response received within %d milliseconds\n", timeout_ms);
        return false;
    }

    return true;
}

/***************************************************************** END *********************************************************************************************************************************************************************/

/******************************************************************* SIM7600EI Module INITIALIZATION ************************************************************************************************************************************************************************/
void module_init()
{
    vTaskDelay(18000 / portTICK_PERIOD_MS);

    SIM7600EI_Modem_.SIM7600EI_module_init();
    if (SIM7600EI_Modem_.parser(OK_TYPE, 5000))
    {
        Module_data.Module_state = SIM7600EI_STATE_START_SUCCESS;
    }
    else
    {
        Module_data.Module_state = SIM7600EI_STATE_START_FAILED;
    }

    if (Module_data.Module_state == SIM7600EI_STATE_START_SUCCESS)
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        SIM7600EI_Modem_.getSIM_Status();
        if (SIM7600EI_Modem_.parser(CPIN_TYPE, 5000))
        {
            Module_data.sim_sts = sim_ready;
        }
        else
        {
            Module_data.sim_sts = sim_not_ready;
        }
    }

    if (Module_data.sim_sts == sim_ready)
    {

        vTaskDelay(2000 / portTICK_PERIOD_MS);
        SIM7600EI_Modem_.SIM7600EI_SIM_REG_STATUS();
        if (SIM7600EI_Modem_.parser(CREG_TYPE, 5000))
        {
            Module_data.sim_net_reg_sts = NETWORK_REGISTERED;
            SIM_registration_done_flag = true;
        }
        else
        {
            Module_data.sim_net_reg_sts = NETWORK_NOT_REGISTERED;
        }

        if (!SIM_registration_done_flag)
        {
            vTaskDelay(10000 / portTICK_PERIOD_MS);
            SIM7600EI_Modem_.SIM7600EI_SIM_REG_STATUS();
            if (SIM7600EI_Modem_.parser(CREG_TYPE, 5000))
            {
                Module_data.sim_net_reg_sts = NETWORK_REGISTERED;
                SIM_registration_done_flag = true;
            }
            else
            {
                Module_data.sim_net_reg_sts = NETWORK_NOT_REGISTERED;
            }
        }
    }

    if (Module_data.sim_net_reg_sts == NETWORK_REGISTERED)
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        SIM7600EI_Modem_.SIM7600EI_getGPRS_status();
        if(SIM7600EI_Modem_.parser(CGREG_TYPE , 5000))
        {
            Module_data.GPRS_Status = GPRS_ACTTVATED;
        }
        else
        {
            Module_data.GPRS_Status = GPRS_DEACTTVATED;
        } 

        vTaskDelay(2000 / portTICK_PERIOD_MS);
        SIM7600EI_Modem_.SIM7600EI_get_signal_quality();
        if (SIM7600EI_Modem_.parser(CSQ_TYPE, 5000))
        {
            Network_mode.oper_sel_mode = Oprator_Info_get;
        }
        else
        {
            Network_mode.oper_sel_mode = Oprator_Info_fail;
        }

        vTaskDelay(2000 / portTICK_PERIOD_MS);
        SIM7600EI_Modem_.SIM7600EI_SIM_Mode(); // Command to check SIM opration mode...To change mode send one more cmd
        if (SIM7600EI_Modem_.parser(CNMP_TYPE, 5000))
        {
            Network_mode.oper_net_type = AUTOMATIC_MODE;
        }
        else
        {
            Network_mode.oper_net_type = GSM_ONLY;
        }

        vTaskDelay(5000 / portTICK_PERIOD_MS);
        SIM7600EI_Modem_.SIM7600EI_get_operator();
        if (SIM7600EI_Modem_.parser(COPS_TYPE, 5000))
        {
            Network_mode.oper_sel_mode = Automatic_Oper_Sel;
        }
        else
        {
            Network_mode.oper_sel_mode = Mannual_Oper_Sel;
        }
    }

    if (Module_data.Module_state == SIM7600EI_STATE_START_SUCCESS)
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        SIM7600EI_Modem_.SIM7600EI_Activate_GPS();
        if (SIM7600EI_Modem_.parser(CGPS_TYPE, 5000))
        {
            Module_data.Module_GPS = GPS_ACTTVATED;
        }
        else
        {
            Module_data.Module_GPS = GPS_DEACTTVATED;
        }
    }
    if (Module_data.Module_GPS == GPS_ACTTVATED)
    {
        vTaskDelay(120000 / portTICK_PERIOD_MS);
        SIM7600EI_Modem_.SIM7600EI_getGPS_INFO();
        if (SIM7600EI_Modem_.parser(CGPSINFO_TYPE, 5000))
        {
            Module_data.Module_GPS = GPS_INFO_GET;
        }
        else
        {
            Module_data.Module_GPS = GPS_INFO_Fail;
        }
    }
    if ((Module_data.Module_state == SIM7600EI_STATE_START_SUCCESS) && (Module_data.GPRS_Status == GPRS_ACTTVATED))
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        SIM7600EI_Modem_.SIM7600EI_Module_functionality();
        if (SIM7600EI_Modem_.parser(CFUN_TYPE, 5000))
        {
            Module_data.Module_functionality = Full_functionality_online_mode;
        }
        else
        {
            Module_data.Module_functionality = Minimum_functionalit;
        }

        vTaskDelay(2000 / portTICK_PERIOD_MS);
        SIM7600EI_Modem_.SIM7600EI_Activate_PDP_Context();
        if (SIM7600EI_Modem_.parser(CGACT_TYPE, 5000))
        {
            Module_data.PDP_Status = PDP_ACTTVATED;
        }
        else
        {
            Module_data.PDP_Status = PDP_DEACTTVATED;
        }
        
    }

}

/******************************************************************* END ********************************************************************************************************************************/

/************************************************************** MQTT INITIALIZATION *************************************************************************************************************************************/

void module_MQTT_init()
{
    if((Module_data.GPRS_Status == GPRS_ACTTVATED) && (Module_data.PDP_Status == PDP_ACTTVATED))
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);

    }
}

/******************************************************************* END *********************************************************************************************************************************************/
