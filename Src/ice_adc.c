#include "ice.h"

#if ICE_ADC

void ice_adc_init()
{
#ifdef ICE_GD32F30X
    /* enable GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC0);
    /* config ADC clock */
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV6);

    /* config the GPIO as analog mode */
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, ADC_CURT_Pin); //电流adc
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, ADC_VR_Pin); //反向检波电压adc

    /* ADC mode config */
    adc_mode_config(ADC_MODE_FREE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    /* ADC channel length config */
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 1U);

    /* ADC trigger config */
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);
    /* ADC external trigger config */
    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);

    /* enable ADC interface */
    adc_enable(ADC0);
    delay_ms(1U);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);
#endif

#ifdef ICE_RP2040
    // ADC is in an unknown state. We should start by resetting it
    reset_block(RESETS_RESET_ADC_BITS);
    unreset_block_wait(RESETS_RESET_ADC_BITS);

    // Now turn it back on. Staging of clock etc is handled internally
    adc_hw->cs = ADC_CS_EN_BITS;

    // Internal staging completes in a few cycles, but poll to be sure
    while (!(adc_hw->cs & ADC_CS_READY_BITS)) {
        tight_loop_contents();
    }

    // Make sure GPIO is high-impedance, no pullups etc
//    adc_gpio_init(ADC_VOLT_Pin);
//    // Select ADC input 3 (GPIO29)
//    adc_select_input(ADC_VOLT_CH);

#if ICE_ADC_TEMP_SENSOR
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);
#endif
#endif

}

/**
* @brief 获取ADC第 ch 个的adc值
* @param ch adc 通道序号
* @return 返回adc值
*/
uint16_t ice_adc_get_value(uint8_t ch)
{
    uint16_t adc_value;
#ifdef ICE_GD32F30X
    /* ADC regular channel config */
    adc_regular_channel_config(ADC0, 0U, ch, ADC_SAMPLETIME_7POINT5);
    /* ADC software trigger enable */
    adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);

    /* wait the end of conversion flag */
    while(!adc_flag_get(ADC0, ADC_FLAG_EOC));
    /* clear the end of conversion flag */
    adc_flag_clear(ADC0, ADC_FLAG_EOC);
    /* return regular channel sample value */
    adc_value = adc_regular_data_read(ADC0);
#endif
#ifdef ICE_RP2040
    adc_value = adc_read();
#endif
    return adc_value;
}

/**
* @brief 获取ADC第 ch 个的电压值
* @param ch adc 通道序号
* @return 返回电压值, unit: mV
*/
uint16_t ice_adc_get_volt(uint8_t ch)
{
    uint16_t adc_value = ice_adc_get_value(ch);
    return (ICE_ADC_VOLT_MV_MAX * adc_value / ICE_ADC_VALUE_MAX);
}

/**
* @brief 获取内部温度传感器的值
* @return 温度
*/
float ice_adc_get_inTemp()
{
    float temp = 0;
#ifdef ICE_RP2040
    //T = 27 - (ADC_voltage (V) - 0.706)/0.001721
    temp = 27 - (ice_adc_get_volt(0) / 1000.0 - 0.706) / 0.001721;
#endif
    return temp;
}

#endif