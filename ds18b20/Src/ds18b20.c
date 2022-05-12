#include "ice.h"

#ifdef ICE_DS18B20

//////////////////////////////////////////////////////////////////////////////////	 
static void DstaticS18B20_Start(void);        //��ʼ�¶�ת��
static void DS18B20_Write_Byte(uint8_t dat);//д��һ���ֽ�
static uint8_t DS18B20_Read_Byte(void);        //����һ���ֽ�
static uint8_t DS18B20_Read_Bit(void);        //����һ��λ
static uint8_t DS18B20_Check(void);            //����Ƿ����DS18B20
static void DS18B20_Rst(void);            //��λDS18B20
static void DS18B20_IO_IN(void);

static void DS18B20_IO_OUT(void);
//////////////////////////////////////////////////////////////////////////////////

static void DS18B20_IO_OUT(void)
{
#ifdef ICE_GD32F30X
    gpio_init(DS18B20_DQ_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, DS18B20_DQ_GPIO_PIN);
#endif
}

static void DS18B20_IO_IN(void)
{
#ifdef ICE_GD32F30X
    gpio_init(DS18B20_DQ_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_10MHZ, DS18B20_DQ_GPIO_PIN);
#endif
}

//��λDS18B20
static void DS18B20_Rst(void)
{
    DS18B20_IO_OUT();    //SET PG11 OUTPUT
    DS18B20_DQ_OUT(0);    //����DQ
    delay_us(750);        //����750us
    DS18B20_DQ_OUT(1);    //DQ=1
    delay_us(15);        //15US
}

//�ȴ�DS18B20�Ļ�Ӧ
//����1:δ��⵽DS18B20�Ĵ���
//����0:����
static uint8_t DS18B20_Check(void)
{
    uint8_t retry = 0;
    DS18B20_IO_IN();    //SET PG11 INPUT
    while (DS18B20_DQ_IN && retry < 200)
    {
        retry++;
        delay_us(1);
    }
    if (retry >= 200)return 1;
    else retry = 0;
    while (!DS18B20_DQ_IN && retry < 240)
    {
        retry++;
        delay_us(1);
    }
    if (retry >= 240)return 1;
    return 0;
}

//��DS18B20��ȡһ��λ
//����ֵ��1/0
static uint8_t DS18B20_Read_Bit(void)
{
    uint8_t data;
    DS18B20_IO_OUT();    //SET PG11 OUTPUT
    DS18B20_DQ_OUT(0);
    delay_us(2);
    DS18B20_DQ_OUT(1);
    DS18B20_IO_IN();    //SET PG11 INPUT
    delay_us(12);
    if (DS18B20_DQ_IN)data = 1;
    else data = 0;
    delay_us(50);
    return data;
}

//��DS18B20��ȡһ���ֽ�
//����ֵ������������
static uint8_t DS18B20_Read_Byte(void)
{
    uint8_t i, j, dat;
    dat = 0;
    for (i = 1; i <= 8; i++)
    {
        j = DS18B20_Read_Bit();
        dat = (j << 7) | (dat >> 1);
    }
    return dat;
}

//дһ���ֽڵ�DS18B20
//dat��Ҫд����ֽ�
static void DS18B20_Write_Byte(uint8_t dat)
{
    uint8_t j;
    uint8_t testb;
    DS18B20_IO_OUT();    //SET PG11 OUTPUT;
    for (j = 1; j <= 8; j++)
    {
        testb = dat & 0x01;
        dat = dat >> 1;
        if (testb)
        {
            DS18B20_DQ_OUT(0);    // Write 1
            delay_us(2);
            DS18B20_DQ_OUT(1);
            delay_us(60);
        }
        else
        {
            DS18B20_DQ_OUT(0);    // Write 0
            delay_us(60);
            DS18B20_DQ_OUT(1);
            delay_us(2);
        }
    }
}

//��ʼ�¶�ת��
static void DS18B20_Start(void)
{
    DS18B20_Rst();
    DS18B20_Check();
    DS18B20_Write_Byte(0xcc);    // skip rom
    DS18B20_Write_Byte(0x44);    // convert
}

/**
 * @brief  ��ʼ��DS18B20��IO�� DQ ͬʱ���DS�Ĵ���
 * @return 1:������, 0:����
 */
uint8_t DS18B20_Init(void)
{

    DS18B20_DQ_SCK_ENABLE();    //ʹ��GPIO��ʱ��
    /*Configure GPIO pins : PAPin PAPin */
    gpio_init(DS18B20_DQ_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, DS18B20_DQ_GPIO_PIN);
    DS18B20_DQ_OUT(1);

    DS18B20_Rst();
    return DS18B20_Check();
}

/**
 * @brief  ��ds18b20�õ��¶�ֵ, ���ȣ�0.1C
 * @return �¶�ֵ ��-550~1250��
 */
int16_t DS18B20_Get_Temp(void)
{
    uint8_t temp;
    uint8_t TL, TH;
    int16_t tem;
    DS18B20_Start();            // ds1820 start convert
    DS18B20_Rst();
    DS18B20_Check();
    DS18B20_Write_Byte(0xcc);    // skip rom
    DS18B20_Write_Byte(0xbe);    // convert
    TL = DS18B20_Read_Byte();    // LSB
    TH = DS18B20_Read_Byte();    // MSB

    if (TH > 7)
    {
        TH = ~TH;
        TL = ~TL;
        temp = 0;                    //�¶�Ϊ��
    }
    else temp = 1;                //�¶�Ϊ��
    tem = TH;                    //��ø߰�λ
    tem <<= 8;
    tem += TL;                    //��õװ�λ
    tem = (float) tem * 0.625;        //ת��
    if (temp)return tem;        //�����¶�ֵ
    else return -tem;
}

#endif














