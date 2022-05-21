/**
* @file     ice_cfg.h
* @brief
* @note
* @author   lizhongyu
* @date     2022/5/9
* @version  V1.0
* @par Copyright(c):  806277022@qq.com
* @par History:  2022/5/9 ��һ���汾V1.0
*/

#ifndef _ICE_CFG_H
#define _ICE_CFG_H

/*** ����оƬ�ĵ��ͺ�, Ŀǰ֧��3��, STM32ϵ��, GD32F30Xϵ��, RP2040(��ݮ��Pico) ***/
//#define ICE_GD32F30X
//#define ICE_STM32
#define ICE_RP2040
#if (defined (ICE_GD32F30X) + defined (ICE_STM32) + defined (ICE_RP2040) > 1)
#error "�ظ�����оƬ"
#elif (defined (ICE_GD32F30X) + defined (ICE_STM32) + defined (ICE_RP2040) == 0)
#error "δ����оƬ"
#endif


/************ ʹ������ ************/
#define ICE_IWDG    0  //���Ź�
#define ICE_GPIO    1  //GPIO
#define ICE_EXTI    0  //�ⲿ�ж�
#define ICE_UART    0  //����
#define ICE_PWM     0  //PWM
#define ICE_SPI     1  //SPI
#define ICE_I2C     0  //I2C
#define ICE_ADC     0  //ADC
#define ICE_PIO     1  //PIO

/************ ���ӹ��� ************/
#define ICE_UART_DEBUG      0   //���� printf �ض���������
#define ICE_EF_DEBUG        0   //ʹ�� easyflash ��ӡ���


#ifdef ICE_GD32F30X
#include "gd32f30x.h"
#include "gd32f30x_libopt.h"
//#include "gd32f30x_it.h"
#endif

#ifdef ICE_RP2040
#include "hardware/irq.h"
#include "hardware/clocks.h"
#include <pico/stdlib.h>
#if ICE_GPIO
#include "hardware/gpio.h"
#endif
#if ICE_SPI
#include "hardware/spi.h"
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
#if ICE_PIO
#include "hardware/pio.h"
#endif

#ifdef ICE_DS18B20
#include "ds18b20.h"
#endif

#ifdef ICE_EASYFLASH
#include "easyflash.h"
#endif

#ifdef ICE_WS2812
#include "ws2812.pio.h"
#endif

#if ICE_UART_DEBUG
#include <stdio.h>
#endif

/*** ���ò��� ***/
//*************** ϵͳ�������� ***************************************************************************************//
//-------------------------------------------------
#ifdef ICE_GD32F30X
#define ICE_SYS_CLK     (120 * 1000 * 1000)  //��Ƶ120MHz
#endif
//-------------------------------------------------
#ifdef ICE_STM32
#define ICE_SYS_CLK     (72 * 1000 * 1000)  //��Ƶ72MHz
#endif
//-------------------------------------------------
#ifdef ICE_RP2040
#define ICE_SYS_CLK     (125 * 1000 * 1000)  //��Ƶ125MHz
#define PLL_SYS_KHZ     (ICE_SYS_CLK / 1000)
#endif

//************** ���Ź� ���� *****************************************************************************************//
#if ICE_IWDG
#define ICE_IWDG_TIMEOUT  (4000)    //���Ź���ʱʱ�� 4000 ms
#endif

//************** GPIO ���� *******************************************************************************************//
#if ICE_GPIO
//-------------------------------------------------
#ifdef ICE_GD32F30X
    #define VD_SW_Pin     GPIO_PIN_7   //©ѹ����
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
//************** �ⲿ�ж� ���� *****************************************************************************************//
#if ICE_EXTI
//-------------------------------------------------
#ifdef ICE_GD32F30X
    #define PWR_EN_Pin      GPIO_PIN_12   //�ⲿ����ʹ������
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
//************** ���� ���� ********************************************************************************************//
#if ICE_UART
#define ICE_DMA_SIZE        (256)     //dma rx size DMA�����������С
#define ICE_UART_SIZE       (6)       //packet size һ֡���ݴ�С
#define ICE_UART_HEAD       (0x11)    //packet head ֡ͷ
//#define ICE_UART_END        (0x21)    //packet end  ֡β
/* packet bytes definition */
#define HEAD     (0)		//head
#define CMD      (1)	    //command
#define ADDR     (2)	    //addr
#define DB1      (3)	    //data byte 1
#define DB2      (4)	    //data byte 2
#define CHECK    (5)        //check
#define END      CHECK      //end

#if (ICE_UART_SIZE != END + 1)
#error "ICE_UART_SIZE(���ݰ���С)��������"
#endif

#endif
//************** SPI ���� ********************************************************************************************//
#ifdef ICE_SPI
//-------------------------------------------------
#ifdef ICE_GD32F30X
#endif
#ifdef ICE_STM32
#endif
#ifdef ICE_RP2040
#define ICE_SPI_ID          spi0
#define ICE_SPI_CLK         (2 * 1000 * 1000) //2MHz //spi ʱ��Ƶ��
#define ICE_SPI_MISO_Pin    (0)  //MISO Pin ---> GP0
#define ICE_SPI_SCK_Pin     (2)  //SCK Pin ---> GP2
#define ICE_SPI_MOSI_Pin    (3)  //MOSI Pin ---> GP3
#define ICE_SPI_CS_Pin      (1)  //CS Pin ---> GP1
#define ICE_SPI_CS(x)       gpio_put(ICE_SPI_CS_Pin, x)
#define ICE_SPI_CPOL        (0)  //ʱ�Ӽ���, 0:��, 1:��
#define ICE_SPI_CPHA        (0)  //ʱ����λ, 0:1���ز���, 1:2���ز���
#define ICE_SPI_MSB_FIRST   (1)  //MSB��ǰ
#define ICE_SPI_HARD_CS     (0)  //Ӳ��CS, 0:ʧ��, 1:ʹ��
#endif
//-------------------------------------------------
//-------------------------------------------------
#endif
//-------------------------------------------------
//************** ADC ���� ********************************************************************************************//
#if ICE_ADC /* adc defines */
#define ICE_ADC_TEMP_SENSOR     (0)  //�ڲ��¶ȴ�����ʹ��
//-------------------------------------------------
#ifdef ICE_GD32F30X
    #define ADC_BIT_NUM     (12U)  //GD32F30X Ϊ 12bit adc
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
#define ADC_BIT_NUM     (12U)  //RP2040 Ϊ 12bit adc
#define ADC_VOLT_Pin    (27)   //GP27 -> ADC1
#define ADC_VOLT_CH     (1)
#endif
//-------------------------------------------------
#ifndef ADC_BIT_NUM
#define ADC_BIT_NUM     (12U) //adc λ��
#endif
#define ICE_ADC_VALUE_MAX       (1 << ADC_BIT_NUM) // adc���ֵ
#define ICE_ADC_VOLT_MAX        (3.3f)      // adc���ֵ��Ӧ��ʵ�ʵ�ѹ3.3V
#define ICE_ADC_VOLT_MV_MAX     (3300.0f)   // adc���ֵ��Ӧ��ʵ�ʵ�ѹ3300mV
#endif
//************** PWM ���� ********************************************************************************************//
#if ICE_PWM /* PWM defines */
//-------------------------------------------------
#ifdef ICE_GD32F30X
    #define ICE_PWM_PCLK    RCU_GPIOB   //PB0 -> TIMER2_CH2, ����Ҫ��ӳ��IO
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
#define ICE_PWM_FREQ    (12 * 1000)  // ���� PWM Ƶ��Ϊ: 12kHz, Ĭ��ռ�ձ�Ϊ50%,
                                     // Ĭ���������Ϊ��, ��������С�ڱȽ�ֵ���Ϊ�ߵ�ƽ
#define ICE_PWM_PERIOD  (ICE_SYS_CLK / ICE_PWM_FREQ)

#if (ICE_PWM_PERIOD > (UINT16_MAX + 1) || ICE_PWM_PERIOD == 0)
#error "PWM Ƶ�����ó��ִ���, ���������"
#endif
#endif
//************** PIO ���� ********************************************************************************************//
#if ICE_PIO
#ifdef ICE_RP2040
#define ICE_PIO0    pio0    //PIO 0
#define ICE_SM0     0       //״̬��0
#endif
#endif
//************** ds18b20 ���� ****************************************************************************************//
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
//************** easyflash ���� **************************************************************************************//
#ifdef ICE_EASYFLASH /* easyflash defines */
//-------------------------------------------------
#ifdef ICE_GD32F30X
    #define ICE_FLASH_PAGE_SIZE     (2048)  //2KB, GD32F30X ҳ��С
    #define ICE_EF_WRITE_GRAN       (32)    //only support 1(nor flash)/ 8(stm32f4)/ 32(stm32f1)
    #define ICE_EF_START_ADDR       (FLASH_BASE + 200 * ICE_FLASH_PAGE_SIZE)  //easyflash ��ʼ��ַ
    #define ICE_ENV_AREA_SIZE       (20 * ICE_FLASH_PAGE_SIZE)  //easyflash���������洢�ռ��СΪ 20 * 2 = 40KB
#endif
//-------------------------------------------------
#ifdef ICE_STM32
#endif
//-------------------------------------------------
#ifdef ICE_RP2040
#endif
//-------------------------------------------------
#define ICE_EF_ENV_VER_NUM      (0)  //����µĻ�����������Ҫ+1
#endif
//************** WS2812 ���� *****************************************************************************************//
#ifdef ICE_WS2812
#define RED     (0xFF0000) //��
#define ORANGE  (0xFF7F00) //��
#define YELLOW  (0xFFFF00) //��
#define GREEN   (0x00FF00) //��
#define INDIGO  (0x007FFF) //��
#define BLUE    (0x0000FF) //��
#define PURPLE  (0x8B00FF) //��

#ifdef ICE_RP2040
#define WS2812_Pin    (28)  //ws2812 pin
#endif
#endif

#endif //_ICE_CFG_H
