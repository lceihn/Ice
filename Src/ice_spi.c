#include "ice.h"

#if ICE_SPI

void ice_spi_init()
{
#ifdef ICE_GD32F30X
    //spi从机 使用DMA接收初始化配置, 中断处理
    rcu_periph_clock_enable(RCU_DMA0);  //DMA 时钟使能

    dma_parameter_struct dma_init_struct;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_16BIT;      //DMA 存储数据宽度  16bit
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;  //DMA 外设数据宽度  16bit
    dma_init_struct.number       = 2;                           //DMA size: 2
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE; //DMA 外设地址递增: 关
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;  //DMA 内存地址递增: 开
    /* SPI1 receive dma config:DMA0, DMA_CH3  */
    dma_deinit(DMA0, DMA_CH3);
    dma_init_struct.number       = 2;                           //DMA size: 2
    dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(SPI1);   //DMA 外设地址
    dma_init_struct.memory_addr  = (uint32_t)ice_spi.rx;        //DMA 内存地址
    dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;    //DMA 方向: 外设->内存
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;     //DMA 优先级
    dma_init(DMA0, DMA_CH3, &dma_init_struct); //DMA 初始化配置参数
    /* configure DMA mode */
    dma_circulation_enable(DMA0, DMA_CH3);    //DMA 循环模式:使能 , 必须要使能循环模式 才能正常传输 DMA到SPI的数据
    dma_memory_to_memory_disable(DMA0, DMA_CH3);  //DMA 方向:内存->内存:失能

    dma_channel_enable(DMA0, DMA_CH3);        //DMA 通道使能, 必须在配置参数之后, 手册上说通道使能后再配置参数就无效了
    dma_interrupt_enable(DMA0, DMA_CH3, DMA_INT_HTF); //传输半数据中断标志
    dma_interrupt_enable(DMA0, DMA_CH3, DMA_INT_FTF); //传输全部数据中断标志

    //spi从机
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_SPI1);
    rcu_periph_clock_enable(RCU_AF);

    /* SPI1 GPIO config: NSS/PB12, SCK/PB13, MISO/PB14, MOSI/PB15 */
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_15);
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);

    spi_parameter_struct  spi_init_struct;
    /* SPI1 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX; //spi 通信模式: 全双工
    spi_init_struct.device_mode          = SPI_SLAVE;                //spi 主从模式: 从机
    spi_init_struct.frame_size           = SPI_FRAMESIZE_16BIT;      //spi 数据宽度: 16bit
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;   //spi 采样方式: CPOL=0,CPHA=0 (时钟极性低, 第一个时钟边沿采样)
    spi_init_struct.nss                  = SPI_NSS_HARD;             //spi 硬件片选: 失能
    spi_init_struct.prescale             = SPI_PSC_2;                //spi    时钟: APB1_CLK / 2 = 30MHz
    spi_init_struct.endian               = SPI_ENDIAN_MSB;           //spi  MSB在前
    spi_init(SPI1, &spi_init_struct);                       //spi 初始化配置参数
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


#endif