/**
 * @file ice_uart.c
 * @brief stm32 uart standred communication
 * @author lceihn 
 * @version 1.0
 * @date 2021-12-10
 * 
 * @copyright Copyright (c) 2021  806277022@qq.com
 * 
 * @par log:2021-12-10: version 1.0
 *
 */

/* Include */
#include "ice.h"

/* static function */
#if ICE_UART

static void uart_init(IceUart *ice);
static void uart_dma_init(IceUart *ice);
static void uart_receive_dma(IceUart *ice);
static void uart_transmit(uint32_t huart, uint8_t *pData, uint16_t Size);

static void cmd_0x01_handle(IceUart *ice);
static void cmd_0x02_handle(IceUart *ice);
static void cmd_0x03_handle(IceUart *ice);


static uint8_t isChecked(IceUart *ice);
static void clear(IceUart *ice);

#if ICE_UART_DEBUG

#ifdef __GNUC__

#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)

PUTCHAR_PROTOTYPE
{
#ifdef ICE_GD32F30X
    usart_data_transmit(ice_uart.huart, ch);
    while (RESET == usart_flag_get(ice_uart.huart, USART_FLAG_TBE));
#endif
    return ch;
}
#endif

#endif

/**
 * @brief  return check ok ? 校验数据
 * @param  ice              
 * @return 
 */
static uint8_t isChecked(IceUart *ice)
{
    uint8_t ok = 1;
    ice->error = Ack_OK;

    if (ice->rx_len != ICE_UART_SIZE) //接收数据大小:     //todo 接收校验
    {
        ice->error = Packtet_Size_Err;
    }
    else if (ice->rx[HEAD] != ICE_UART_HEAD) //校验帧头帧尾
    {
        ice->error = Head_Or_End_Err;
    }
    else if (ice->rx[CHECK] != (uint8_t)(ice->rx[HEAD] ^ ice->rx[CMD] ^ ice->rx[ADDR] ^ ice->rx[DB1] ^ ice->rx[DB2])) //校验方式: 异或校验， CHECK 位前
    {
        ice->error = Check_Err;
    }

    if (ice->error)
        ok = 0;

    return ok;
}

/**
 * @brief 将接收缓冲区内容清0, 清空接收标志位
 * @param ice_buf
 */
static void clear(IceUart *ice)
{
    for (uint8_t i = 0; i < ice->rx_len; ++i)
        ice->rx[i] = 0;

    ice->rx_flag = 0;
}

/**
* @brief send data
* @param ice
*/
static void sendData(IceUart *ice)
{
    //todo 发送函数

    uart_transmit(ice->huart, ice->tx, ICE_UART_SIZE);
}


/**
* @brief uart init
*/
static void uart_init(IceUart *ice)
{
#ifdef ICE_GD32F30X
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);

    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);

    /* connect port to USARTx_Tx */ //PA9
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

    /* connect port to USARTx_Rx */ //PA10
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

    /* USART configure */
    usart_deinit(ice->huart);
    usart_baudrate_set(ice->huart, 115200U);
    usart_receive_config(ice->huart, USART_RECEIVE_ENABLE);
    usart_transmit_config(ice->huart, USART_TRANSMIT_ENABLE);
    usart_enable(ice->huart);

    usart_interrupt_enable(ice->huart, USART_INT_IDLE); //开启空闲中断
#endif

}

/**
* @brief gd32 uart's dma init
* @param ice
*/
static void uart_dma_init(IceUart *ice)
{
#ifdef ICE_GD32F30X
/*****************  DMA_RX_CONFIG   ****************/
    dma_parameter_struct dma_init_struct;
    /* enable the DMA clock */
    rcu_periph_clock_enable(RCU_DMA0);

    /* configure the USART RX DMA channel */
    dma_deinit(DMA0, DMA_CH4);
    //dma_struct_para_init(&dma_init_struct);
    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr = (uint32_t)ice->rx;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = ICE_DMA_SIZE;
    dma_init_struct.periph_addr = ((uint32_t)&USART_DATA(ice->huart));
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_LOW;
    dma_init(DMA0, DMA_CH4, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_enable(DMA0, DMA_CH4);
    /*****************  END  DMA_RX_CONFIG  ****************/
    dma_channel_enable(DMA0, DMA_CH4);
    usart_dma_receive_config(ice->huart, USART_DENR_ENABLE);  //USART配置DMA接收
#endif
}

/**
* @brief 通过指定串口发送数据(阻塞式)
* @param huart 对应串口
* @param pData 数据指针
* @param Size 数据大小
*/
static void uart_transmit(uint32_t huart, uint8_t *pData, uint16_t Size)
{
#ifdef ICE_GD32F30X
    for (int i = 0; i < Size; ++i)
    {
        usart_data_transmit(huart, pData[i]);
        while (RESET == usart_flag_get(huart, USART_FLAG_TBE));
    }
#endif
}

/**
* @brief
* @param ice
*/
static void uart_receive_dma(IceUart *ice)
{
#ifdef ICE_GD32F30X
    dma_memory_address_config(DMA0, DMA_CH4, (uint32_t)ice->rx);
    dma_transfer_number_config(DMA0, DMA_CH4, ICE_DMA_SIZE);
    dma_channel_enable(DMA0, DMA_CH4);
#endif
}

/**
 *@brief 中断回调函数, 需要添加到gd32fxxx_it.c中对应的UARTX_Handler处理函数中
 */
void ice_uart_it_callback(IceUart *ice)
{
#ifdef ICE_GD32F30X
    if(RESET != usart_interrupt_flag_get(ice->huart, USART_INT_FLAG_IDLE)) //检测到空闲线
    {
        usart_interrupt_flag_clear(ice->huart, USART_INT_FLAG_IDLE); //清除空闲中断标志
        usart_data_receive(ice->huart); //清除接收完成标志
        dma_channel_disable(DMA0, DMA_CH4);  //CHEN位为0时才能配置DMA
        ice->rx_len = ICE_DMA_SIZE - dma_transfer_number_get(DMA0, DMA_CH4);
        ice->rx_flag = 1;
    }
#endif
}

/**
 * @brief uart init, start uart dma receive
 */
void ice_uart_init(IceUart *ice)
{
#ifdef ICE_GD32F30X
    ice->huart = USART0;

    uart_init(ice); //gd32 uart init
    uart_dma_init(ice); //gd32 uart's dma init

    //串口中断使能
    nvic_irq_enable(USART0_IRQn, 3, 3);
#endif
}

/**
* @brief uart received data handle
 */
void ice_uart_task(IceUart *ice)
{
    if (ice->rx_flag) //received data
    {
        ice->error = Ack_OK;
        uint8_t ok = isChecked(ice); //校验数据

        if (ok) //校验通过
        {
            switch (ice->rx[CMD])
            {
                case 0x01:
                    cmd_0x01_handle(ice);
                    break;
                case 0x02:
                    cmd_0x02_handle(ice);
                    break;
                case 0x03:
                    cmd_0x03_handle(ice);
                    break;
                default:
                    ice->error = CMD_Err; //不存在对应的指令
                    break;
            }
        }
        clear(ice);
        uart_receive_dma(ice); //开启新的dma接收
    }
}

/**
* @brief
* @param ice
*/
void cmd_0x01_handle(IceUart *ice)
{

}

/**
* @brief
* @param ice
*/
void cmd_0x02_handle(IceUart *ice)
{

}

/**
* @brief
* @param ice
*/
void cmd_0x03_handle(IceUart *ice)
{

}

#endif
