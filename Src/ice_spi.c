#include "ice.h"

#if ICE_SPI

static void spi_config();
static void spi_rx_dma_config();

void spi_config()
{
#if (ICE_SPI_MODE == ICE_SPI_SLAVE)
    //spi�ӻ�
    rcu_periph_clock_enable(ICE_SPIx_RCU2);
    rcu_periph_clock_enable(ICE_SPIx_RCU1);
    rcu_periph_clock_enable(RCU_AF);

    /* ICE_SPIx GPIO config: NSS/PB12, SCK/PB13, MISO/PB14, MOSI/PB15 */
    gpio_init(ICE_SPIx_Port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, ICE_SPIx_CS_Pin | ICE_SPIx_SCK_Pin | ICE_SPIx_MOSi_Pin);
    gpio_init(ICE_SPIx_Port, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, ICE_SPIx_MISO_Pin);

    spi_parameter_struct  spi_init_struct;
    /* ICE_SPIx parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX; //spi ͨ��ģʽ: ȫ˫��
    spi_init_struct.device_mode          = SPI_SLAVE;                //spi ����ģʽ: �ӻ�
    spi_init_struct.frame_size           = SPI_FRAMESIZE_16BIT;      //spi ���ݿ��: 16bit
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;   //spi ������ʽ: CPOL=0,CPHA=0 (ʱ�Ӽ��Ե�, ��һ��ʱ�ӱ��ز���)
    spi_init_struct.nss                  = SPI_NSS_HARD;             //spi Ӳ��Ƭѡ: ʧ��
    spi_init_struct.prescale             = SPI_PSC_2;                //spi    ʱ��: APB1_CLK / 2 = 30MHz
    spi_init_struct.endian               = SPI_ENDIAN_MSB;           //spi  MSB��ǰ
    spi_init(ICE_SPIx, &spi_init_struct);                   //spi ��ʼ�����ò���

    spi_dma_enable(ICE_SPIx, SPI_DMA_RECEIVE);
    spi_enable(ICE_SPIx);
    spi_write(ICE_SPIx, 0);
#else

#endif
}

void spi_rx_dma_config()
{
#if (ICE_SPI_MODE == ICE_SPI_SLAVE)
    //spi�ӻ� ʹ��DMA���ճ�ʼ������, �жϴ���
    rcu_periph_clock_enable(ICE_SPIx_DMAx_RCU);  //DMA ʱ��ʹ��

    dma_parameter_struct dma_init_struct;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_16BIT;      //DMA �洢���ݿ��  16bit
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;  //DMA �������ݿ��  16bit
    dma_init_struct.number       = ICE_SPIx_DMAx_SIZE;          //DMA size: 2
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE; //DMA �����ַ����: ��
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;  //DMA �ڴ��ַ����: ��
    /* ICE_SPIx receive dma config:DMA0, DMA_CH3  */
    dma_deinit(ICE_SPIx_DMAx, ICE_SPIx_DMAx_Rx_CH);
    dma_init_struct.number       = ICE_SPIx_DMAx_SIZE;          //DMA size: 2
    dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(ICE_SPIx);   //DMA �����ַ
    dma_init_struct.memory_addr  = (uint32_t)ice_spi.rx;        //DMA �ڴ��ַ
    dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;    //DMA ����: ����->�ڴ�
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;     //DMA ���ȼ�
    dma_init(ICE_SPIx_DMAx, ICE_SPIx_DMAx_Rx_CH, &dma_init_struct); //DMA ��ʼ�����ò���
    /* configure DMA mode */
    dma_circulation_enable(ICE_SPIx_DMAx, ICE_SPIx_DMAx_Rx_CH);    //DMA ѭ��ģʽ:ʹ�� , ����Ҫʹ��ѭ��ģʽ ������������ DMA��SPI������
    dma_memory_to_memory_disable(ICE_SPIx_DMAx, ICE_SPIx_DMAx_Rx_CH);  //DMA ����:�ڴ�->�ڴ�:ʧ��

    dma_channel_enable(ICE_SPIx_DMAx, ICE_SPIx_DMAx_Rx_CH);        //DMA ͨ��ʹ��, ���������ò���֮��, �ֲ���˵ͨ��ʹ�ܺ������ò�������Ч��
    dma_interrupt_enable(ICE_SPIx_DMAx, ICE_SPIx_DMAx_Rx_CH, DMA_INT_HTF); //����������жϱ�־
    dma_interrupt_enable(ICE_SPIx_DMAx, ICE_SPIx_DMAx_Rx_CH, DMA_INT_FTF); //����ȫ�������жϱ�־
#endif
}

void ice_spi_init()
{
#ifdef ICE_GD32F30X

#if (ICE_SPI_MODE == ICE_SPI_SLAVE)
    spi_rx_dma_config();
#endif
    spi_config();

#if (ICE_SPI_MODE == ICE_SPI_SLAVE)
    nvic_irq_enable(ICE_SPIx_DMAx_Rx_IRQn, 14, 0); //DMA����ͨ���ж�ʹ��
#endif
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

void ice_spi_task(IceSpi *ice)
{
#if (ICE_SPI_MODE == ICE_SPI_SLAVE)

#if ICE_FREERTOS

    /*
        ����ulTaskNotifyTake��һ������˵����
        1. �˲�������ΪpdFALSE������vTaskMsgPro��TCB(������ƿ�)�еı���ulNotifiedValue��һ
        2. �˲�������ΪpdTRUE������vTaskMsgPro��TCB(������ƿ�)�еı���ulNotifiedValue����
    */
    uint32_t notice = ulTaskNotifyTake(pdTRUE, portMAX_DELAY); //��ȡ��������ź���
    if (notice == 1) //received data
    {
        uint8_t reg = ice_spi.rx[0] & 0x001F;

        if (!(ice_spi.rx[0] & 0x8000)) { //д
            if (reg == 0x00) {
                __NOP();
            }
        }

//        dma_channel_disable(ICE_SPIx_DMAx, ICE_SPIx_DMAx_Rx_CH); //dma disable
        dma_memory_address_config(ICE_SPIx_DMAx, ICE_SPIx_DMAx_Rx_CH, (uint32_t)(ice->rx)); //dma mem address
        dma_transfer_number_config(ICE_SPIx_DMAx, ICE_SPIx_DMAx_Rx_CH, ICE_SPIx_DMAx_SIZE); //dma size
        spi_i2s_data_receive(ICE_SPIx);
        dma_channel_enable(ICE_SPIx_DMAx, ICE_SPIx_DMAx_Rx_CH);
//        ice->rx_flag = 0;
    }

#else
    if (ice->rx_flag) //received data
    {
        uint8_t reg = ice_spi.rx[0] & 0x001F;

        if (!(ice_spi.rx[0] & 0x8000)) //д
        {

        }
        dma_channel_disable(ICE_SPIx_DMAx, ICE_SPIx_DMAx_Rx_CH);
        dma_memory_address_config(ICE_SPIx_DMAx, ICE_SPIx_DMAx_Rx_CH, (uint32_t)(ice->rx));
        dma_transfer_number_config(ICE_SPIx_DMAx, ICE_SPIx_DMAx_Rx_CH, ICE_SPIx_DMAx_SIZE);
        spi_i2s_data_receive(ICE_SPIx);
        dma_channel_enable(ICE_SPIx_DMAx, ICE_SPIx_DMAx_Rx_CH);
        ice->rx_flag = 0;
    }
#endif //ICE_FREERTOS

#endif //ICE_SPI_MODE

}


#endif