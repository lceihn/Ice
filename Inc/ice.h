/**
* @file     ice.h    
* @brief
* @note     
* @author   lizhongyu
* @date     2022/5/9
* @version  V1.0
* @par Copyright(c):  806277022@qq.com
* @par History:  2022/5/9 第一个版本V1.0
*/

#ifndef _ICE_H
#define _ICE_H

/************ Include ************/
#include "ice_cfg.h"
#include "ice_def.h"


/************ Macro **************/



/************ Definition *********/



/************ Declare ************/
#if ICE_UART
extern IceUart ice_uart;
extern uint16_t curt_th;
extern uint16_t vr_th;
#endif


/************ Function ***********/
#ifdef ICE_GD32F30X
uint32_t get_systick_us(void);
uint32_t get_systick_ms(void);
void delay_ms(uint32_t ms);
void delay_us(uint32_t us);
void systick_deinit(void);
void systick_handler(void);
void ice_gd32_init();

/* function declarations */
/* this function handles NMI exception */
void NMI_Handler(void);
/* this function handles HardFault exception */
void HardFault_Handler(void);
/* this function handles MemManage exception */
void MemManage_Handler(void);
/* this function handles BusFault exception */
void BusFault_Handler(void);
/* this function handles UsageFault exception */
void UsageFault_Handler(void);
/* this function handles SVC exception */
void SVC_Handler(void);
/* this function handles DebugMon exception */
void DebugMon_Handler(void);
/* this function handles PendSV exception */
void PendSV_Handler(void);
/* this function handles SysTick exception */
void SysTick_Handler(void);

#if ICE_UART
void USART0_IRQHandler(void);
#endif

#if ICE_EXTI
void EXTI10_15_IRQHandler(void);
#endif

#endif

#ifdef ICE_RP2040
void rp2040_flash_program(uint32_t addr, const uint8_t *buf, uint16_t len);
#endif

#if ICE_IWDG
void ice_iwdg_init();
void ice_iwdg_feed();
#endif

#if ICE_GPIO
void ice_gpio_init();
#endif

#if ICE_EXTI
void ice_exit_init();
void ice_exti_it_callback();
#endif

#if ICE_UART
void ice_uart_init(IceUart *ice);           //ice uart init
void ice_uart_it_callback(IceUart *ice);    //ice uart interrupt callback, add to stm32f1xx_it.c void USARTx_IRQHandler(void)
void ice_uart_task(IceUart *ice);		 //process uart receive task
#endif

#if ICE_SPI
void ice_spi_init();
void ice_spi_write(uint8_t *buf, uint16_t len);
#endif

#if ICE_ADC
void ice_adc_init();
uint16_t ice_adc_get_value(uint8_t ch);
uint16_t ice_adc_get_volt(uint8_t ch);
float ice_adc_get_inTemp();
#endif

#if ICE_PWM
void ice_pwm_init();
void ice_pwm_set_cycle(uint8_t cyc);
void ice_pwm_set_value(uint16_t val);
#endif

#ifdef ICE_DS18B20
void ice_ds18b20_init();
#ifdef ICE_GD32F30X
__INLINE int16_t ice_ds18b20_getTemp();
#endif
#endif

#ifdef ICE_EASYFLASH
void ice_easyflash_init();
#endif

void ice_init();


#endif //_ICE_H
