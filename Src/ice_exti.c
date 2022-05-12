#include "ice.h"


#if ICE_EXTI

/**
* @brief 外部中断初始化
*/
void ice_exit_init()
{
#ifdef ICE_GD32F30X
    /* enable the clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);

    /* configure button pin as input */ //power en
    gpio_init(PWR_EN_Port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, PWR_EN_Pin);

    /* connect EXTI line to GPIO pin */
    gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOB, GPIO_PIN_SOURCE_12);

    /* configure EXTI line */
    exti_init(EXTI_12, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
    exti_interrupt_flag_clear(EXTI_12);
    nvic_irq_enable(EXTI10_15_IRQn, 2, 0);
#endif

}

/**
* @brief 外部中断回调函数
*/
void ice_exti_it_callback()
{
#ifdef ICE_GD32F30X
    if (PWR_EN_IN)
        VD_SW(1);
#endif
}

#endif