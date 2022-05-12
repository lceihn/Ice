//
// Created by 80627 on 2022/1/6.
//

#include "ice.h"

#if ICE_GPIO

/**
* @brief gpio init
*/
void ice_gpio_init()
{
#ifdef ICE_GD32F30X
    /* enable gpio clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
//    rcu_periph_clock_enable(RCU_AF);

    /* SWD remap */
    //如果使用到了 PA15/PB3/PB4 则需要重映射, 并且要先使能 RCU_AF, 再重映射
//    gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);

    /* configure gpio port */
    gpio_init(VD_SW_Port, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, VD_SW_Pin);
    VD_SW(0);
#endif

#ifdef ICE_RP2040
    /* LED PIN: GPIO_OUT_PP */
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    LED(1);

#endif
}

#endif
