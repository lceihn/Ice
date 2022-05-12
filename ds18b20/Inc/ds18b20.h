#ifndef __DS18B20_H
#define __DS18B20_H

/************************** DS18B20 不需要修改********************************/
#ifdef ICE_GD32F30X
#define	DS18B20_DQ_OUT(x)   gpio_bit_write(DS18B20_DQ_GPIO_PORT, DS18B20_DQ_GPIO_PIN, x)
#define	DS18B20_DQ_IN       gpio_input_bit_get(DS18B20_DQ_GPIO_PORT, DS18B20_DQ_GPIO_PIN)
#endif

/************************** DS18B20 函数实现 *****************************/
uint8_t DS18B20_Init(void);			//初始化DS18B20
int16_t DS18B20_Get_Temp(void);     //获取温度

#endif















