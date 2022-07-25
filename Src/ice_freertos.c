#include "ice.h"

#if ICE_FREERTOS

TaskHandle_t htask1_handle = NULL;
#if ICE_UART
TaskHandle_t huart_handle = NULL;
#endif //ICE_UART

static void app_task_create();
static void task1(void *pvParameters);
#if ICE_UART
static void uart_task(void *pvParameters);
#endif //ICE_UART

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
                 &htask1_handle );      //task handle
#if ICE_UART
    xTaskCreate(uart_task,                  //task function
                "uartTask",         //task
                512,            //task stack size unit:word
                NULL,           //task parameter
                20,                //task priority
                &huart_handle);             //task handle
#endif //ICE_UART
}

/**
* @brief
* @param pvParameters
*/
void task1(void *pvParameters)
{
    for (;;)
    {
        LED0(0);
        vTaskDelay(50);
        LED0(1);
        vTaskDelay(1000);
    }
}

#if ICE_UART
/**
* @brief uart dma rx task, priority:20, stack:512*4(2048)
* @param pvParameters
*/
void uart_task(void *pvParameters)
{
    for (;;)
    {
        ice_uart_task(&ice_uart);
    }
}
#endif //ICE_UART

/**
* @brief FreeRTOS init
*/
void ice_freertos_init()
{
    app_task_create();      //create task
    vTaskStartScheduler();  //start scheduler
}
#endif //ICE_FREERTOS