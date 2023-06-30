#include "SIM7600EI_lib.h"


extern "C"
{
    void app_main(void)
        {
            uart_init();

            //Delay for module to ON all operations 
            vTaskDelay(2000 / portTICK_PERIOD_MS);

            module_init();

           // module_MQTT_init();
             

        }
}
