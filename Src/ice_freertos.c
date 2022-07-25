#include "ice.h"

#if ICE_FREERTOS

TaskHandle_t hTASK1_handle = NULL;
#if ICE_UART
TaskHandle_t hUART_handle = NULL;
#endif //ICE_UART
#if ICE_SPI
TaskHandle_t hspi_handle = NULL;
#endif //ICE_SPI
#if ICE_ADC
TaskHandle_t hADC_handle = NULL;
#endif //ICE_ADC

static void app_task_create();
static void task1(void *pvParameters);
#if ICE_UART
static void uart_task(void *pvParameters);
#endif //ICE_UART

#if ICE_SPI
static void spi_task(void *pvParameters);
#endif //ICE_SPI

#if ICE_ADC
static void adc_task(void *pvParameters);
#endif //ICE_ADC

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
                 &hTASK1_handle );      //task handle
#if ICE_UART
    xTaskCreate(uart_task,                  //task function
                "uartTask",         //task
                512,            //task stack size unit:word
                NULL,           //task parameter
                20,                //task priority
                &hUART_handle);             //task handle
#endif //ICE_UART

#if ICE_SPI
    xTaskCreate(spi_task,                  //task function
                "spiTask",         //task
                512,            //task stack size unit:word
                NULL,           //task parameter
                21,                //task priority
                &hspi_handle);             //task handle
#endif //ICE_SPI

#if ICE_ADC
    xTaskCreate(adc_task,                  //task function
                "adcTask",         //task
                512,            //task stack size unit:word
                NULL,           //task parameter
                19,                //task priority
                &hADC_handle);             //task handle
#endif //ICE_ADC
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

#if ICE_SPI
/**
* @brief uart dma rx task, priority:20, stack:512*4(2048)
* @param pvParameters
*/
void spi_task(void *pvParameters)
{
    for (;;)
    {
        ice_spi_task(&ice_spi);
    }
}
#endif //ICE_SPI

#if ICE_ADC
/**
* @brief adc dma task, priority:19, stack:512*4(2048)
* @param pvParameters
*/
void adc_task(void *pvParameters)
{
    for (;;)
    {
        ice_adc_task();
        vTaskDelay(1000);
    }
}
#endif //ICE_ADC

/**
* @brief FreeRTOS init
*/
void ice_freertos_init()
{
    app_task_create();      //create task
    vTaskStartScheduler();  //start scheduler
}
#endif //ICE_FREERTOS