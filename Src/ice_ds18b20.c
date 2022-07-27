#include "ice.h"

#ifdef ICE_DS18B20

///**
//* @brief 从ds18b20得到温度值, 精度：0.1C
//* @return 温度值 （-550~1250）， 有符号
//*/
//int16_t ice_ds18b20_getTemp() {
//    return DS18B20_Get_Temp();
//}


void ice_ds18b20_init()
{
#ifdef ICE_GD32F30X
    gpio_init(DS18B20_DQ_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, DS18B20_DQ_GPIO_PIN);
    DS18B20_DQ(1);
#endif
    uint8_t cnt = 0;
    uint8_t flag = 0;

    do {
        flag = DS18B20_Init();
        cnt++;
        if (cnt >= 2)
            break;
    } while (flag);

    //flag = 1 表示 初始化失败, flag = 0 表示初始化成功
    ICE_ASSERT(!flag)

    //初始化完之后多读几次数据
    for (int i = 0; i < 5; ++i)
    {
        DS18B20_Get_Temp();
//        delay_ms(1);
    }
}

#endif