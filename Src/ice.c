#include "ice.h"

/* global variable  */
#if ICE_UART
IceUart ice_uart;
//电流 驻波 阈值设置 (adc 值)
uint16_t curt_th = 4095;
uint16_t vr_th = 4095;
#endif


void ice_init()
{
#ifdef ICE_GD32F30X
    ice_gd32_init(); //gd32 system init
#endif
#if ICE_GPIO
    ice_gpio_init(); //gpio init
#endif
#if ICE_EXTI
    ice_exit_init(); //exit init
#endif
#if ICE_UART
    ice_uart_init(&ice_uart); //uart init
#endif
#if ICE_ADC
    ice_adc_init();  //adc init
#endif
#if ICE_PWM
    ice_pwm_init();  //pwm init
#endif
#ifdef ICE_DS18B20
    ice_ds18b20_init(); //ds18b20 init
#endif
#ifdef ICE_EASYFLASH
    ice_easyflash_init(); //easyflash init
#endif
#if ICE_IWDG
    ice_iwdg_init(); //iwdg init
#endif
}
