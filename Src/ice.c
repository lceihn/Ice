#include "ice.h"

/* global variable  */
#if ICE_UART
IceUart ice_uart;
#endif
#if ICE_SPI
IceSpi ice_spi;
#endif
#if ICE_ADC
uint16_t ice_adc[ICE_ADCx_MAX_NUMS];
#endif //ICE_ADC


void ice_init()
{
    __set_PRIMASK(1); //��ֹȫ���ж�
#ifdef ICE_GD32F30X
    ice_gd32_init(); //gd32 system init
#endif
#if defined(ICE_GD32F30X) || defined(ICE_STM32)
    bsp_InitDWT();
#endif
#ifdef ICE_RP2040
//    vreg_set_voltage(VREG_VOLTAGE_1_30);//300MHz��Ҫ��ѹ�����270MHz����Ҫ�����
    set_sys_clock_khz(PLL_SYS_KHZ, true);   //�������ֻ�ܸ�ϵͳ��Ƶ, ����ʱ��Ƶ�ʲ�����ű�
    // ����ʱ��Ƶ�ʺ�ϵͳ��Ƶһ��
    clock_configure(clk_peri,
                    0,
                    CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
                    ICE_SYS_CLK,
                    ICE_SYS_CLK);
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
#if ICE_SPI
    ice_spi_init();
#endif
#if ICE_ADC
    ice_adc_init();  //adc init
#endif
#if ICE_PWM
    ice_pwm_init();  //pwm init
#endif
#if ICE_PIO
    ice_pio_init();
#endif
#ifdef ICE_DS18B20
    ice_ds18b20_init(); //ds18b20 init
#endif
#ifdef ICE_EASYFLASH
    ice_easyflash_init(); //easyflash init
#endif
#if ICE_IWDG && !ICE_FREERTOS
    ice_iwdg_init(); //iwdg init
#endif
#if ICE_FREERTOS
    ice_freertos_init();
#endif
}
