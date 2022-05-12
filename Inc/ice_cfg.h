/**
* @file     ice_cfg.h
* @brief
* @note
* @author   lizhongyu
* @date     2022/5/9
* @version  V1.0
* @par Copyright(c):  806277022@qq.com
* @par History:  2022/5/9 第一个版本V1.0
*/

#ifndef _ICE_CFG_H
#define _ICE_CFG_H

/*** 配置芯片的的型号, 目前支持3种, STM32系列, GD32F30X系列, RP2040(树莓派Pico) ***/
//#define ICE_GD32F30X
//#define ICE_STM32
//#define ICE_RP2040
#if (defined (ICE_GD32F30X) + defined (ICE_STM32) + defined (ICE_RP2040) > 1)
#error "重复定义芯片"
#elif (defined (ICE_GD32F30X) + defined (ICE_STM32) + defined (ICE_RP2040) == 0)
#error "未定义芯片"
#endif


/************ 使能外设 ************/
#define ICE_IWDG    0  //看门狗
#define ICE_GPIO    0  //GPIO
#define ICE_EXTI    0  //外部中断
#define ICE_UART    0  //串口
#define ICE_PWM     0  //PWM
#define ICE_SPI     0  //SPI
#define ICE_I2C     0  //I2C
#define ICE_ADC     0  //ADC

/************ 附加功能 ************/
#define ICE_UART_DEBUG      0   //启用 printf 重定向至串口
#define ICE_EF_DEBUG        0   //使能 easyflash 打印输出


#ifdef ICE_GD32F30X
#include "gd32f30x.h"
#include "gd32f30x_libopt.h"
//#include "gd32f30x_it.h"
#endif

#ifdef ICE_RP2040
#include "hardware/irq.h"
#if ICE_GPIO
    #include "hardware/gpio.h"
#endif
#if ICE_PWM
    #include "hardware/pwm.h"
#endif
#if ICE_ADC
    #include "hardware/adc.h"
    #include "hardware/resets.h"
#endif
#if ICE_IWDG
    #include "hardware/watchdog.h"
#endif
#endif

#ifdef ICE_DS18B20
#include "ds18b20.h"
#endif

#ifdef ICE_EASYFLASH
#include "easyflash.h"
#endif

#if ICE_UART_DEBUG
#include <stdio.h>
#endif

/*** 配置参数 ***/
//*************** 系统参数配置 ***************************************************************************************//
//-------------------------------------------------
#ifdef ICE_GD32F30X
#define ICE_SYS_CLK     (120 * 1000 * 1000)  //主频120MHz
#endif
//-------------------------------------------------
#ifdef ICE_STM32
#define ICE_SYS_CLK     (72 * 1000 * 1000)  //主频72MHz
#endif
//-------------------------------------------------
#ifdef ICE_RP2040
#define ICE_SYS_CLK     (125 * 1000 * 1000)  //主频125MHz
#endif

//************** 看门狗 配置 *****************************************************************************************//
#if ICE_IWDG
#define ICE_IWDG_TIMEOUT  (4000)    //看门狗超时时间 4000 ms
#endif

//************** GPIO 配置 *******************************************************************************************//
#if ICE_GPIO
//-------------------------------------------------
#ifdef ICE_GD32F30X
    #define VD_SW_Pin     GPIO_PIN_7   //漏压开关
    #define VD_SW_Port    GPIOA
    #define VD_SW(x)      gpio_bit_write(VD_SW_Port, VD_SW_Pin, x)
#endif
//-------------------------------------------------
#ifdef ICE_STM32
#endif
//-------------------------------------------------
#ifdef ICE_RP2040
    #define LED_PIN    (29)    //LED_PIN ---> GP29
    #define LED(x)     gpio_put(LED_PIN, x)
    #define LED_IN     gpio_get(LED_PIN)
#endif

#endif
//************** 外部中断 配置 ***************************************************************************************//
#if ICE_EXTI
//-------------------------------------------------
#ifdef ICE_GD32F30X
    #define PWR_EN_Pin      GPIO_PIN_12   //外部输入使能引脚
    #define PWR_EN_Port     GPIOB
    #define PWR_EN_IN       gpio_input_bit_get(PWR_EN_Port, PWR_EN_Pin)
#endif
//-------------------------------------------------
#ifdef ICE_STM32
#endif
//-------------------------------------------------
#ifdef ICE_RP2040
#endif
#endif
//************** 串口 配置 *******************************************************************************************//
#if ICE_UART
#define ICE_DMA_SIZE        (256)     //dma rx size DMA接收区缓冲大小
#define ICE_UART_SIZE       (6)       //packet size 一帧数据大小
#define ICE_UART_HEAD       (0x11)    //packet head 帧头
//#define ICE_UART_END        (0x21)    //packet end  帧尾
/* packet bytes definition */
#define HEAD     (0)		//head
#define CMD      (1)	    //command
#define ADDR     (2)	    //addr
#define DB1      (3)	    //data byte 1
#define DB2      (4)	    //data byte 2
#define CHECK    (5)        //check
#define END      CHECK      //end

#if (ICE_UART_SIZE != END + 1)
#error "ICE_UART_SIZE(数据包大小)定义有误"
#endif

#endif
//************** ADC 配置 ********************************************************************************************//
#if ICE_ADC /* adc defines */
#define ICE_ADC_TEMP_SENSOR     (0)  //内部温度传感器使能
//-------------------------------------------------
#ifdef ICE_GD32F30X
    #define ADC_BIT_NUM     (12U)  //GD32F30X 为 12bit adc
    #define ADC_VR_Pin      GPIO_PIN_2
    #define ADC_VR_CH       ADC_CHANNEL_2
    #define ADC_CURT_Pin    GPIO_PIN_4
    #define ADC_CURT_CH     ADC_CHANNEL_4
#endif
//-------------------------------------------------
#ifdef ICE_STM32
#endif
//-------------------------------------------------
#ifdef ICE_RP2040
#define ADC_BIT_NUM     (12U)  //RP2040 为 12bit adc
#define ADC_VOLT_Pin    (27)   //GP27 -> ADC1
#define ADC_VOLT_CH     (1)
#endif
//-------------------------------------------------
#ifndef ADC_BIT_NUM
#define ADC_BIT_NUM     (12U) //adc 位数
#endif
#define ICE_ADC_VALUE_MAX       (1 << ADC_BIT_NUM) // adc最大值
#define ICE_ADC_VOLT_MAX        (3.3f)      // adc最大值对应的实际电压3.3V
#define ICE_ADC_VOLT_MV_MAX     (3300.0f)   // adc最大值对应的实际电压3300mV
#endif
//************** PWM 配置 ********************************************************************************************//
#if ICE_PWM /* PWM defines */
//-------------------------------------------------
#ifdef ICE_GD32F30X
    #define ICE_PWM_PCLK    RCU_GPIOB   //PB0 -> TIMER2_CH2, 不需要重映射IO
    #define ICE_PWM_TCLK    RCU_TIMER2
    #define ICE_PWM_Port    GPIOB
    #define ICE_PWM_Pin     GPIO_PIN_0
    #define ICE_PWM_TIMER   TIMER2
    #define ICE_PWM_CH      TIMER_CH_2
#endif
//-------------------------------------------------
#ifdef ICE_STM32
#endif
//-------------------------------------------------
#ifdef ICE_RP2040
    #define ICE_PWM_Pin     (20)    //GP20->PWM
#endif
//-------------------------------------------------
#define ICE_PWM_FREQ    (12 * 1000)  // 配置 PWM 频率为: 12kHz, 默认占空比为50%,
                                     // 默认输出极性为高, 即计数器小于比较值输出为高电平
#define ICE_PWM_PERIOD  (ICE_SYS_CLK / ICE_PWM_FREQ)

#if (ICE_PWM_PERIOD > (UINT16_MAX + 1) || ICE_PWM_PERIOD == 0)
#error "PWM 频率设置出现错误, 可能溢出了"
#endif
#endif
//************** ds18b20 配置 ****************************************************************************************//
#ifdef ICE_DS18B20 /* ds18b20 defines */
//-------------------------------------------------
#ifdef ICE_GD32F30X
    #define      DS18B20_DQ_SCK_ENABLE()    rcu_periph_clock_enable(RCU_GPIOB)
    #define      DS18B20_DQ_GPIO_PORT       GPIOB
    #define      DS18B20_DQ_GPIO_PIN        GPIO_PIN_15
    #define      DS18B20_DQ(x)              gpio_bit_write(DS18B20_DQ_GPIO_PORT, DS18B20_DQ_GPIO_PIN, x)
#endif
//-------------------------------------------------
#ifdef ICE_STM32
#endif
//-------------------------------------------------
#ifdef ICE_RP2040
#endif
#endif
//************** easyflash 配置 **************************************************************************************//
#ifdef ICE_EASYFLASH /* easyflash defines */
//-------------------------------------------------
#ifdef ICE_GD32F30X
    #define ICE_FLASH_PAGE_SIZE     (2048)  //2KB, GD32F30X 页大小
    #define ICE_EF_WRITE_GRAN       (32)    //only support 1(nor flash)/ 8(stm32f4)/ 32(stm32f1)
    #define ICE_EF_START_ADDR       (FLASH_BASE + 200 * ICE_FLASH_PAGE_SIZE)  //easyflash 起始地址
    #define ICE_ENV_AREA_SIZE       (20 * ICE_FLASH_PAGE_SIZE)  //easyflash环境变量存储空间大小为 20 * 2 = 40KB
#endif
//-------------------------------------------------
#ifdef ICE_STM32
#endif
//-------------------------------------------------
#ifdef ICE_RP2040
#endif
//-------------------------------------------------
#define ICE_EF_ENV_VER_NUM      (0)  //添加新的环境变量后需要+1
#endif


#endif //_ICE_CFG_H
