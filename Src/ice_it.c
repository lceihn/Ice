/*!
    \file    gd32f30x_it.c
    \brief   interrupt service routines

    \version 2017-02-10, V1.0.0, firmware for GD32F30x
    \version 2018-10-10, V1.1.0, firmware for GD32F30x
    \version 2018-12-25, V2.0.0, firmware for GD32F30x
    \version 2020-09-30, V2.1.0, firmware for GD32F30x 
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "ice.h"

#ifdef ICE_GD32F30X
/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while (1){
    }
}
#if !ICE_FREERTOS
/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
}

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
}
#endif

/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
    systick_handler();
#if ICE_FREERTOS
#if (INCLUDE_xTaskGetSchedulerState == 1 )
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
#endif /* INCLUDE_xTaskGetSchedulerState */
        xPortSysTickHandler();
#if (INCLUDE_xTaskGetSchedulerState == 1 )
    }
#endif /* INCLUDE_xTaskGetSchedulerState */
#endif //ICE_FREERTOS
}

#if ICE_UART
void USART0_IRQHandler(void)
{
    ice_uart_it_callback(&ice_uart);
}

void USART1_IRQHandler(void)
{
    ice_uart_it_callback(&ice_uart);
}

void USART2_IRQHandler(void)
{
    ice_uart_it_callback(&ice_uart);
}
#endif

#if ICE_EXTI
void EXTI10_15_IRQHandler(void)
{
    if (RESET != exti_interrupt_flag_get(EXTI_12)) {
        ice_exti_it_callback();
        exti_interrupt_flag_clear(EXTI_12);
    }
}
#endif

#if ICE_SPI
void DMA0_Channel3_IRQHandler(void)
{
#if (ICE_SPI_MODE == ICE_SPI_SLAVE)
    if ((DMA_INTF(ICE_SPIx_DMAx) & DMA_FLAG_ADD(DMA_INT_HTF, ICE_SPIx_DMAx_Rx_CH))) //接收通道x半传输完成
    {
        DMA_INTC(ICE_SPIx_DMAx) |= DMA_FLAG_ADD(DMA_INT_HTF, ICE_SPIx_DMAx_Rx_CH); //清除中断标志位

        if (ice_spi.rx[0] & 0x8000) //读
        {
            SPI_CTL0(ICE_SPIx) &= (uint32_t)(~SPI_CTL0_SPIEN);  //要改变SPI配置参数 就必须先失能SPI 修改后再使能
            SPI_CTL0(ICE_SPIx) |= 0x00000001;
            SPI_CTL0(ICE_SPIx) |= (uint32_t)SPI_CTL0_SPIEN;
        }
    }
    else if ((DMA_INTF(ICE_SPIx_DMAx) & DMA_FLAG_ADD(DMA_INT_FTF, ICE_SPIx_DMAx_Rx_CH)))//接收通道x传输完成
    {
        DMA_INTC(ICE_SPIx_DMAx) |= DMA_FLAG_ADD(DMA_INT_FTF, ICE_SPIx_DMAx_Rx_CH); //清除中断标志位

        DMA_CHCTL(ICE_SPIx_DMAx, ICE_SPIx_DMAx_Rx_CH) &= ~DMA_CHXCTL_CHEN; //dma disable
        SPI_CTL0(ICE_SPIx) &= (uint32_t)(~SPI_CTL0_SPIEN);  //要改变SPI配置参数 就必须先失能SPI 修改后再使能
        SPI_CTL0(ICE_SPIx) &= 0xFFFFFFFE;
        SPI_CTL0(ICE_SPIx) |= (uint32_t)SPI_CTL0_SPIEN;
        spi_write(ICE_SPIx, 0);
        ice_spi.rx_flag = 1;
    }
#endif
}
#endif

#endif





