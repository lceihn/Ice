#include "ice.h"

#if ICE_FREERTOS

TaskHandle_t hTASK1_handle = NULL;
#if ICE_UART
TaskHandle_t hUART_handle = NULL;
#endif //ICE_UART
#if ICE_SPI
TaskHandle_t hSPI_handle = NULL;
#endif //ICE_SPI
#if ICE_ADC
TaskHandle_t hADC_handle = NULL;
#endif //ICE_ADC

static void app_task_create();
static void task1(void *pvParameters);      //看门狗
static void temp_task(void *pvParameters);  //温度监测
static void pll_task(void *pvParameters);  //监测PLL状态

#if ICE_ADC
static void adc_task(void *pvParameters);   //电压电流监测, 监测外部输入检波
#endif //ICE_ADC
#if ICE_UART
static void uart_task(void *pvParameters);
#endif //ICE_UART
#if ICE_SPI
static void spi_task(void *pvParameters);   //spi 从机
#endif //ICE_SPI
//********************************************************************//

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
    xTaskCreate( temp_task,   	            //task function
                 "task2",     	//task
                 1024,       //task stack size unit:word
                 NULL,      //task parameter
                 6,            //task priority
                 &hTEMP_handle );      //task handle
    xTaskCreate( pll_task,   	            //task function
                 "task4",     	//task
                 1024,       //task stack size unit:word
                 NULL,      //task parameter
                 18,            //task priority
                 &hPLL_handle );      //task handle

#if ICE_ADC
    xTaskCreate(adc_task,                  //task function
                "adcTask",         //task
                512,            //task stack size unit:word
                NULL,           //task parameter
                19,                //task priority
                &hADC_handle);             //task handle
#endif //ICE_ADC

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
                1024,            //task stack size unit:word
                NULL,           //task parameter
                21,                //task priority
                &hSPI_handle);             //task handle
#endif //ICE_SPI

}

/**
* @brief task1 priority:5, stack:512*4(2048)
* @param pvParameters
*/
void task1(void *pvParameters)
{
    for (;;)
    {
#if ICE_IWDG && ICE_FREERTOS
        ice_iwdg_feed(); //iwdg init
#endif
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
#if ICE_IWDG && ICE_FREERTOS
    ice_iwdg_init(); //iwdg init
#endif
    vTaskStartScheduler();  //start scheduler
}
#endif //ICE_FREERTOS