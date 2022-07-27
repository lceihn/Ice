#include "ice.h"

#if ICE_SPI

static void spi_config();
static void spi_rx_dma_config();

void spi_config()
{
#if (ICE_SPI_MODE == ICE_SPI_SLAVE)
    //spi从机
    rcu_periph_clock_enable(ICE_SPIx_RCU2);
    rcu_periph_clock_enable(ICE_SPIx_RCU1);
    rcu_periph_clock_enable(RCU_AF);

    /* ICE_SPIx GPIO config: NSS/PB12, SCK/PB13, MISO/PB14, MOSI/PB15 */
    gpio_init(ICE_SPIx_Port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, ICE_SPIx_CS_Pin | ICE_SPIx_SCK_Pin | ICE_SPIx_MOSi_Pin);
    gpio_init(ICE_SPIx_Port, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, ICE_SPIx_MISO_Pin);

    spi_parameter_struct  spi_init_struct;
    /* ICE_SPIx parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX; //spi 通信模式: 全双工
    spi_init_struct.device_mode          = SPI_SLAVE;                //spi 主从模式: 从机
    spi_init_struct.frame_size           = SPI_FRAMESIZE_16BIT;      //spi 数据宽度: 16bit
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;   //spi 采样方式: CPOL=0,CPHA=0 (时钟极性低, 第一个时钟边沿采样)
    spi_init_struct.nss                  = SPI_NSS_HARD;             //spi 硬件片选: 失能
    spi_init_struct.prescale             = SPI_PSC_2;                //spi    时钟: APB1_CLK / 2 = 30MHz
    spi_init_struct.endian               = SPI_ENDIAN_MSB;           //spi  MSB在前
    spi_init(ICE_SPIx, &spi_init_struct);                   //spi 初始化配置参数

    spi_dma_enable(ICE_SPIx, SPI_DMA_RECEIVE);
    spi_enable(ICE_SPIx);
    spi_write(ICE_SPIx, 0);
#else

#endif
}

void spi_rx_dma_config()
{
#if (ICE_SPI_MODE == ICE_SPI_SLAVE)
    //spi从机 使用DMA接收初始化配置, 中断处理
    rcu_periph_clock_enable(ICE_SPIx_DMAx_RCU);  //DMA 时钟使能

    dma_parameter_struct dma_init_struct;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_16BIT;      //DMA 存储数据宽度  16bit
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;  //DMA 外设数据宽度  16bit
    dma_init_struct.number       = ICE_SPIx_DMAx_SIZE;          //DMA size: 2
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE; //DMA 外设地址递增: 关
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;  //DMA 内存地址递增: 开
    /* ICE_SPIx receive dma config:DMA0, DMA_CH3  */
    dma_deinit(ICE_SPIx_DMAx, ICE_SPIx_DMAx_Rx_CH);
    dma_init_struct.number       = ICE_SPIx_DMAx_SIZE;          //DMA size: 2
    dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(ICE_SPIx);   //DMA 外设地址
    dma_init_struct.memory_addr  = (uint32_t)ice_spi.rx;        //DMA 内存地址
    dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;    //DMA 方向: 外设->内存
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;     //DMA 优先级
    dma_init(ICE_SPIx_DMAx, ICE_SPIx_DMAx_Rx_CH, &dma_init_struct); //DMA 初始化配置参数
    /* configure DMA mode */
    dma_circulation_enable(ICE_SPIx_DMAx, ICE_SPIx_DMAx_Rx_CH);    //DMA 循环模式:使能 , 必须要使能循环模式 才能正常传输 DMA到SPI的数据
    dma_memory_to_memory_disable(ICE_SPIx_DMAx, ICE_SPIx_DMAx_Rx_CH);  //DMA 方向:内存->内存:失能

    dma_channel_enable(ICE_SPIx_DMAx, ICE_SPIx_DMAx_Rx_CH);        //DMA 通道使能, 必须在配置参数之后, 手册上说通道使能后再配置参数就无效了
    dma_interrupt_enable(ICE_SPIx_DMAx, ICE_SPIx_DMAx_Rx_CH, DMA_INT_HTF); //传输半数据中断标志
    dma_interrupt_enable(ICE_SPIx_DMAx, ICE_SPIx_DMAx_Rx_CH, DMA_INT_FTF); //传输全部数据中断标志
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
    nvic_irq_enable(ICE_SPIx_DMAx_Rx_IRQn, 14, 0); //DMA接收通道中断使能
#endif
#endif

#ifdef ICE_RP2040
    spi_init(ICE_SPI_ID, ICE_SPI_CLK);
    spi_set_format(ICE_SPI_ID,8,ICE_SPI_CPOL,ICE_SPI_CPHA,ICE_SPI_MSB_FIRST);
    gpio_set_function(ICE_SPI_MISO_Pin, GPIO_FUNC_SPI);
    gpio_set_function(ICE_SPI_SCK_Pin, GPIO_FUNC_SPI);
    gpio_set_function(ICE_SPI_MOSI_Pin, GPIO_FUNC_SPI);
#if ICE_SPI_HARD_CS
    gpio_set_function(ICE_SPI_CS_Pin, GPIO_FUNC_SPI); //! 使用硬件CS有BUG, 发送多字节时 当CPHA为0时硬件CS不是连续的!!, 当CPHA为1时硬件CS是连续的!!
#else
    //软件CS控制, 使用GPIO
    gpio_init(ICE_SPI_CS_Pin);
    gpio_set_dir(ICE_SPI_CS_Pin, GPIO_OUT);
    gpio_put(ICE_SPI_CS_Pin, 1);
#endif
#endif
}

/**
* @brief 硬件spi发送len字节数据
* @param buf 数据指针
* @param len 发送长度
*/
void ice_spi_write(uint8_t *buf, uint16_t len)
{
#ifdef ICE_RP2040
#if ICE_SPI_HARD_CS
    spi_write_blocking(ICE_SPI_ID, buf, len);
#else
    //软件CS控制
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
        函数ulTaskNotifyTake第一个参数说明：
        1. 此参数设置为pdFALSE，任务vTaskMsgPro的TCB(任务控制块)中的变量ulNotifiedValue减一
        2. 此参数设置为pdTRUE，任务vTaskMsgPro的TCB(任务控制块)中的变量ulNotifiedValue清零
    */
    uint32_t notice = ulTaskNotifyTake(pdTRUE, portMAX_DELAY); //获取任务计数信号量
    if (notice == 1) //received data
    {
        uint8_t reg = ice_spi.rx[0] & 0x001F;

        if (!(ice_spi.rx[0] & 0x8000)) { //写
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

        if (!(ice_spi.rx[0] & 0x8000)) //写
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