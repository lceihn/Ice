/**
* @file     ice_def.h    
* @brief    
* @note     
* @author   lizhongyu
* @date     2022/5/9
* @version  V1.0
* @par Copyright(c):  806277022@qq.com
* @par History:  2022/5/9 第一个版本V1.0
*/

#ifndef _ICE_DEF_H
#define _ICE_DEF_H


/************ Definition *********/
//--------------------------------------------------------------------------------------------------------------------//
#if ICE_UART /* 串口 defines */
/* error definition */
typedef enum
{
    Ack_OK = 0,                     //communication normal
    Packtet_Size_Err,               //packet size error
    Head_Or_End_Err,                //head or end error
    Check_Err,                      //check error
    CMD_Err,                        //command is not exist error
}IceUartError;

/* ice uart structure */
typedef struct
{
#ifdef ICE_STM32
    UART_HandleTypeDef *huart;      //pointer to stm32 huart x
#endif
#ifdef ICE_GD32F30X
    uint32_t huart;                 //gd32 usartx
#endif
    uint8_t rx[ICE_UART_DMA_SIZE];  //ice uart receive data buffer(maxlength:256)
    uint8_t rx_flag;                //ice uart rx flag, 0:wait for receive, 1:received packet data
    uint8_t rx_len;                 //ice uart actually receive packet length
    uint8_t tx[ICE_UART_DMA_SIZE];  //ice uart tx data buffer(maxlength:256)
    uint8_t err;                    //ice uart error
}IceUart;
#endif
#if ICE_SPI /* spi defines */
#define ICE_SPI_BUF_SIZE (2)
typedef struct {
    uint16_t tx[ICE_SPI_BUF_SIZE];  // 发送数据
    uint16_t rx[ICE_SPI_BUF_SIZE];  // 接收数据
    uint8_t rx_flag;                // 接收标志
}IceSpi;
#endif
//--------------------------------------------------------------------------------------------------------------------//
#define ICE_ASSERT(EXPR)                                                      \
if (!(EXPR))                                                                  \
{                                                                             \
    while (1);                                                                \
}


#endif //_ICE_DEF_H
