#include "ice.h"

#if ICE_PWM

#ifdef ICE_GD32F30X
inline static void set_pwm_value(uint16_t pwm_value) {
    timer_channel_output_pulse_value_config(ICE_PWM_TIMER, ICE_PWM_CH, pwm_value);
}
#endif

#ifdef ICE_RP2040
__force_inline static void set_pwm_value(uint16_t pwm_value) {
    pwm_set_gpio_level(ICE_PWM_Pin, pwm_value);
}
#endif

/**
* @brief pwm init
*/
void ice_pwm_init()
{
#ifdef ICE_GD32F30X
    rcu_periph_clock_enable(ICE_PWM_PCLK);
    rcu_periph_clock_enable(RCU_AF);

    /* remap */
//    gpio_pin_remap_config(GPIO_ICE_TIMER_FULL_REMAP, ENABLE);

    /*Configure PB10 (ICE_TIMER CH1) as alternate function*/
    gpio_init(ICE_PWM_Port, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, ICE_PWM_Pin); //PB0 -> ICE_TIMER_CH2

    /* -----------------------------------------------------------------------
    TIMER CLK = SystemCoreClock = 120MHz
    PWM period = 120000 / 1000 = 12kHz
    ----------------------------------------------------------------------- */
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(ICE_PWM_TCLK);

    timer_deinit(ICE_PWM_TIMER);

    /* ICE_TIMER configuration */
    timer_initpara.prescaler = 0;
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period = (uint16_t) (ICE_PWM_PERIOD - 1);
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(ICE_PWM_TIMER, &timer_initpara);

    /* CHx configuration in PWM mode */
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(ICE_PWM_TIMER, ICE_PWM_CH, &timer_ocintpara);

    /* CH2 configuration in PWM mode0 */
    timer_channel_output_pulse_value_config(ICE_PWM_TIMER, ICE_PWM_CH, ICE_PWM_PERIOD / 2);
    timer_channel_output_mode_config(ICE_PWM_TIMER, ICE_PWM_CH, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(ICE_PWM_TIMER, ICE_PWM_CH, TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(ICE_PWM_TIMER);
    /* auto-reload preload enable */
    timer_enable(ICE_PWM_TIMER);
#endif
#ifdef ICE_RP2040
    //GPIO ����ΪPWM���
    gpio_set_function(ICE_PWM_Pin, GPIO_FUNC_PWM);
    //��ȡ��Ӧ��PWMͨ��
    uint slice_num = pwm_gpio_to_slice_num(ICE_PWM_Pin);
    //Ĭ�ϲ���Ƶ
//    pwm_set_clkdiv_int_frac(slice_num, 0, 0);
    //���ö�Ӧ����ʱ�ӷ�Ƶ��, �������ü�������Ϊ100MHz, 10ns, 125/1.25=100MHz
//    pwm_set_clkdiv_int_frac(slice_num, 1, 4);
    //���ñȽ�ֵ֮ǰΪ�ߵ�ƽ���, Ĭ�Ͼ��Ǹߵ�ƽ���, ����Ҫ����
//    pwm_set_output_polarity(slice_num, false, false);
    //����PWM����Ϊ125����������, 125 * 10 = 1250 ns = 800kHz
    pwm_set_wrap(slice_num, ICE_PWM_PERIOD);
    //����ռ�ձ�Ϊ50%
    pwm_set_gpio_level(ICE_PWM_Pin, ICE_PWM_PERIOD / 2);
    //����PWM�жϺ�PWMÿ�����ڽ����Ļص�����
//    pwm_clear_irq(slice_num);
//    pwm_set_irq_enabled(slice_num, true);
//    irq_set_exclusive_handler(PWM_IRQ_WRAP, PWM_PeriodFinishedCallback);
//    irq_set_enabled(PWM_IRQ_WRAP, true);
    //PWMʹ��
    pwm_set_enabled(slice_num, true);
#endif
}

/**
* @brief ���� PWMռ�ձ�
* @param cyc Ҫ���õ�ռ�ձ� 0 ~ 100 %
*/
void ice_pwm_set_cycle(uint8_t cyc)
{
    if (cyc > 100)
        cyc = 100;

    set_pwm_value(cyc / 100.0 * ICE_PWM_PERIOD);
}

/**
* @brief ����PWM�ıȽ�ֵ
* @param val PWM �Ƚ�ֵ
*/
void ice_pwm_set_value(uint16_t val)
{
    if (val > ICE_PWM_PERIOD)
        val = ICE_PWM_PERIOD;

    set_pwm_value(val);
}

#endif