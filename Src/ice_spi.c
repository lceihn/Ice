#include "ice.h"

#if ICE_SPI

void ice_spi_init()
{
#ifdef ICE_GD32F30X
    //spi�ӻ� ʹ��DMA���ճ�ʼ������, �жϴ���
    rcu_periph_clock_enable(RCU_DMA0);  //DMA ʱ��ʹ��

    dma_parameter_struct dma_init_struct;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_16BIT;      //DMA �洢���ݿ��  16bit
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;  //DMA �������ݿ��  16bit
    dma_init_struct.number       = 2;                           //DMA size: 2
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE; //DMA �����ַ����: ��
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;  //DMA �ڴ��ַ����: ��
    /* SPI1 receive dma config:DMA0, DMA_CH3  */
    dma_deinit(DMA0, DMA_CH3);
    dma_init_struct.number       = 2;                           //DMA size: 2
    dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(SPI1);   //DMA �����ַ
    dma_init_struct.memory_addr  = (uint32_t)ice_spi.rx;        //DMA �ڴ��ַ
    dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;    //DMA ����: ����->�ڴ�
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;     //DMA ���ȼ�
    dma_init(DMA0, DMA_CH3, &dma_init_struct); //DMA ��ʼ�����ò���
    /* configure DMA mode */
    dma_circulation_enable(DMA0, DMA_CH3);    //DMA ѭ��ģʽ:ʹ�� , ����Ҫʹ��ѭ��ģʽ ������������ DMA��SPI������
    dma_memory_to_memory_disable(DMA0, DMA_CH3);  //DMA ����:�ڴ�->�ڴ�:ʧ��

    dma_channel_enable(DMA0, DMA_CH3);        //DMA ͨ��ʹ��, ���������ò���֮��, �ֲ���˵ͨ��ʹ�ܺ������ò�������Ч��
    dma_interrupt_enable(DMA0, DMA_CH3, DMA_INT_HTF); //����������жϱ�־
    dma_interrupt_enable(DMA0, DMA_CH3, DMA_INT_FTF); //����ȫ�������жϱ�־

    //spi�ӻ�
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_SPI1);
    rcu_periph_clock_enable(RCU_AF);

    /* SPI1 GPIO config: NSS/PB12, SCK/PB13, MISO/PB14, MOSI/PB15 */
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_15);
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);

    spi_parameter_struct  spi_init_struct;
    /* SPI1 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX; //spi ͨ��ģʽ: ȫ˫��
    spi_init_struct.device_mode          = SPI_SLAVE;                //spi ����ģʽ: �ӻ�
    spi_init_struct.frame_size           = SPI_FRAMESIZE_16BIT;      //spi ���ݿ��: 16bit
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;   //spi ������ʽ: CPOL=0,CPHA=0 (ʱ�Ӽ��Ե�, ��һ��ʱ�ӱ��ز���)
    spi_init_struct.nss                  = SPI_NSS_HARD;             //spi Ӳ��Ƭѡ: ʧ��
    spi_init_struct.prescale             = SPI_PSC_2;                //spi    ʱ��: APB1_CLK / 2 = 30MHz
    spi_init_struct.endian               = SPI_ENDIAN_MSB;           //spi  MSB��ǰ
    spi_init(SPI1, &spi_init_struct);                       //spi ��ʼ�����ò���
//    spi_i2s_interrupt_enable(SPI1, SPI_I2S_INT_TBE);
//    spi_i2s_interrupt_enable(SPI1, SPI_I2S_INT_RBNE);
//    spi_dma_enable(SPI1, SPI_DMA_TRANSMIT);
    spi_dma_enable(SPI1, SPI_DMA_RECEIVE);
    spi_enable(SPI1);
    spi1_write(0);
//    timer_parameter_struct timer_initpara;
//
//    rcu_periph_clock_enable(RCU_TIMER2);
//
//    timer_deinit(TIMER2);
//
//    /* TIMER2 configuration */
//    timer_initpara.prescaler         = 120 - 1; //CLK = 1MHz   T = 1us
//    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
//    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
//    timer_initpara.period            = 50 - 1; //50us time out
//    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
//    timer_initpara.repetitioncounter = 0;
//    timer_init(TIMER2,&timer_initpara);
//
//    /* auto-reload preload enable */
//    timer_auto_reload_shadow_enable(TIMER2);
//
//    /* TIMER2 counter enable */
//    timer_interrupt_enable(TIMER2, TIMER_INT_UP);
//    //timer_enable(TIMER2);
#endif
#ifdef ICE_RP2040
    spi_init(ICE_SPI_ID, ICE_SPI_CLK);
    spi_set_format(ICE_SPI_ID,8,ICE_SPI_CPOL,ICE_SPI_CPHA,ICE_SPI_MSB_FIRST);
    gpio_set_function(ICE_SPI_MISO_Pin, GPIO_FUNC_SPI);
    gpio_set_function(ICE_SPI_SCK_Pin, GPIO_FUNC_SPI);
    gpio_set_function(ICE_SPI_MOSI_Pin, GPIO_FUNC_SPI);
#if ICE_SPI_HARD_CS
    gpio_set_function(ICE_SPI_CS_Pin, GPIO_FUNC_SPI); //! ʹ��Ӳ��CS��BUG, ���Ͷ��ֽ�ʱ ��CPHAΪ0ʱӲ��CS����������!!, ��CPHAΪ1ʱӲ��CS��������!!
#else
    //���CS����, ʹ��GPIO
    gpio_init(ICE_SPI_CS_Pin);
    gpio_set_dir(ICE_SPI_CS_Pin, GPIO_OUT);
    gpio_put(ICE_SPI_CS_Pin, 1);
#endif

#endif
}

/**
* @brief Ӳ��spi����len�ֽ�����
* @param buf ����ָ��
* @param len ���ͳ���
*/
void ice_spi_write(uint8_t *buf, uint16_t len)
{
#ifdef ICE_RP2040
#if ICE_SPI_HARD_CS
    spi_write_blocking(ICE_SPI_ID, buf, len);
#else
    //���CS����
    ICE_SPI_CS(0);
    spi_write_blocking(ICE_SPI_ID, buf, len);
    ICE_SPI_CS(1);
#endif
#endif
}


#endif