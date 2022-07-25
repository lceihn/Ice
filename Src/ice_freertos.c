#include "ice.h"

#if ICE_FREERTOS

TaskHandle_t htask1 = NULL;

static void app_task_create();
static void task1(void *pvParameters);

/**
* @brief create all task
*/
void app_task_create()
{
    xTaskCreate( task1,   	            //task function
                 "task1",     	//task
                 512,       //task stack size unit:word
                 NULL,      //task parameter
                 5,            //task priority
                 &htask1 );             //task handle
}

/**
* @brief
* @param pvParameters
*/
void task1(void *pvParameters)
{
    for (;;)
    {
        vTaskDelay(1);
    }
}

/**
* @brief FreeRTOS init
*/
void ice_freertos_init()
{
    app_task_create();      //create task
    vTaskStartScheduler();  //start scheduler
}
#endif //ICE_FREERTOS