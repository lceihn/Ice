#include "ice.h"

#ifdef ICE_DS18B20

///**
//* @brief ��ds18b20�õ��¶�ֵ, ���ȣ�0.1C
//* @return �¶�ֵ ��-550~1250���� �з���
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

    //flag = 1 ��ʾ ��ʼ��ʧ��, flag = 0 ��ʾ��ʼ���ɹ�
    ICE_ASSERT(!flag)

    //��ʼ����֮������������
    for (int i = 0; i < 5; ++i)
    {
        DS18B20_Get_Temp();
//        delay_ms(1);
    }
}

#endif