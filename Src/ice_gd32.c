//
// Created by 80627 on 2022/4/18.
//

#include "ice.h"

#ifdef ICE_GD32F30X

static void systick_config(void);
static void nvic_config(void);

/**
* @brief gd32 时钟节拍设置, 用于延时
*/
void systick_config(void)
{
    /* setup systick timer for 1000Hz interrupts */
    if (SysTick_Config(SystemCoreClock / 1000U)){
        /* capture error */
        while (1){
        }
    }
    /* configure the systick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x00U);
}

/**
 * @brief gd32 中断组及优先级设置
 */
void nvic_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);

    nvic_irq_enable(EXTI5_9_IRQn, 2, 0);
    nvic_irq_enable(USART0_IRQn, 3, 3);
    //nvic_irq_enable(TIMER2_IRQn, 3, 0);
}

/**
* @brief 系统初始化
*/
void ice_gd32_init()
{
    SystemInit(); //sys clk 120MHz
#if ICE_IWDG
    /* enable IRC40K */
    rcu_osci_on(RCU_IRC40K);
    /* wait till IRC40K is ready */
    while(SUCCESS != rcu_osci_stab_wait(RCU_IRC40K)){
    }
#endif
    systick_config(); //1ms tick
    nvic_config(); //nvic priority
}

#endif