#ifndef __DS18B20_H
#define __DS18B20_H

/************************** DS18B20 ����Ҫ�޸�********************************/
#ifdef ICE_GD32F30X
#define	DS18B20_DQ_OUT(x)   gpio_bit_write(DS18B20_DQ_GPIO_PORT, DS18B20_DQ_GPIO_PIN, x)
#define	DS18B20_DQ_IN       gpio_input_bit_get(DS18B20_DQ_GPIO_PORT, DS18B20_DQ_GPIO_PIN)
#endif

/************************** DS18B20 ����ʵ�� *****************************/
uint8_t DS18B20_Init(void);			//��ʼ��DS18B20
int16_t DS18B20_Get_Temp(void);     //��ȡ�¶�

#endif















