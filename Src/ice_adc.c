#include "ice.h"

#if ICE_ADC

static void adc_dma_init();
static void adc_init();

void ice_adc_task()
{
#ifdef ICE_GD32F30X
    adc_software_trigger_enable(ICE_ADCx, ADC_REGULAR_CHANNEL); //����һ��adcת��
#endif

#if ICE_FREERTOS
    /*
        ����ulTaskNotifyTake��һ������˵����
        1. �˲�������ΪpdFALSE������vTaskMsgPro��TCB(������ƿ�)�еı���ulNotifiedValue��һ
        2. �˲�������ΪpdTRUE������vTaskMsgPro��TCB(������ƿ�)�еı���ulNotifiedValue����
    */
    uint32_t notice = ulTaskNotifyTake(pdTRUE, portMAX_DELAY); //��ȡ��������ź���
    if (notice == 1) //conversion completed
    {
        __IO uint16_t volt = ice_adc_get_volt(ADC_VOLT_RANK);
        __IO uint16_t curt = ice_adc_get_volt(ADC_CURT_RANK);
    }
#endif //ICE_FREERTOS
}

void ice_adc_it_call_back()
{
#ifdef ICE_GD32F30X
    if (dma_flag_get(ICE_ADCx_DMAx, ICE_ADCx_DMAx_CH, DMA_FLAG_FTF)) { //����ͨ��x�������
        dma_flag_clear(ICE_ADCx_DMAx, ICE_ADCx_DMAx_CH, DMA_FLAG_FTF); //�����־λ

#if ICE_FREERTOS
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        /* ��������֪ͨ */
        vTaskNotifyGiveFromISR(hADC_handle, &xHigherPriorityTaskWoken);
        /* ���xHigherPriorityTaskWoken = pdTRUE����ô�˳��жϺ��е���ǰ������ȼ�����ִ�� */
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
#endif

    }
#endif //ICE_GD32F30X

}

void adc_dma_init()
{
#ifdef ICE_GD32F30X
    rcu_periph_clock_enable(ICE_ADCx_DMAx_RCU);
    /* ADC_DMA_channel configuration */
    dma_parameter_struct dma_data_parameter;

    /* ADC DMA_channel configuration */
    dma_deinit(ICE_ADCx_DMAx, ICE_ADCx_DMAx_CH);

    /* initialize DMA single data mode */
    dma_data_parameter.periph_addr  = (uint32_t)(&ADC_RDATA(ICE_ADCx));
    dma_data_parameter.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_data_parameter.memory_addr  = (uint32_t)(&ice_adc);
    dma_data_parameter.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_data_parameter.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_data_parameter.number       = ICE_ADCx_CH_NUMS;
    dma_data_parameter.priority     = DMA_PRIORITY_LOW;
    dma_init(ICE_ADCx_DMAx, ICE_ADCx_DMAx_CH, &dma_data_parameter);

    dma_circulation_enable(ICE_ADCx_DMAx, ICE_ADCx_DMAx_CH);
//    dma_circulation_disable(ICE_ADCx_DMAx, ICE_ADCx_DMAx_CH); //!�ر�ѭ��ģʽ

    /* enable DMA channel */
    dma_channel_enable(ICE_ADCx_DMAx, ICE_ADCx_DMAx_CH);
//    dma_channel_disable(ICE_ADCx_DMAx, ICE_ADCx_DMAx_CH); //! ��ʼ��ʱ�ر�dmaͨ��
    dma_interrupt_enable(ICE_ADCx_DMAx, ICE_ADCx_DMAx_CH, DMA_INT_FTF); //����ȫ�������жϱ�־
#endif //ICE_GD32F30X
}

void adc_init()
{
#ifdef ICE_GD32F30X
    /* enable GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    /* enable ADC clock */
    rcu_periph_clock_enable(ICE_ADCx_RCU);

    /* config ADC clock */
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV6);

    /* ADC mode config */
    adc_mode_config(ADC_MODE_FREE);
    /* ADC continuous function enable */
    adc_special_function_config(ICE_ADCx, ADC_CONTINUOUS_MODE, DISABLE); //! ��ֹ����ģʽ
    /* ADC scan function enable */
    adc_special_function_config(ICE_ADCx, ADC_SCAN_MODE, ENABLE);
    /* ADC data alignment config */
    adc_data_alignment_config(ICE_ADCx, ADC_DATAALIGN_RIGHT);

    /* ADC channel length config */
    adc_channel_length_config(ICE_ADCx, ADC_REGULAR_CHANNEL, ICE_ADCx_CH_NUMS);

    /* ADC regular channel config */
    gpio_init(ADC_VOLT_Port, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, ADC_VOLT_Pin);                   //������adc����ͨ���������
    gpio_init(ADC_CURT_Port, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, ADC_CURT_Pin);
    adc_regular_channel_config(ICE_ADCx, ADC_VOLT_RANK, ADC_VOLT_CH, ADC_SAMPLETIME_55POINT5);   //������adc����ͨ���������
    adc_regular_channel_config(ICE_ADCx, ADC_CURT_RANK, ADC_CURT_CH, ADC_SAMPLETIME_55POINT5);

    /* ADC trigger config */
    adc_external_trigger_source_config(ICE_ADCx, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);
    adc_external_trigger_config(ICE_ADCx, ADC_REGULAR_CHANNEL, ENABLE);

    /* ADC DMA function enable */
    adc_dma_mode_enable(ICE_ADCx);
    /* enable ADC interface */
    adc_enable(ICE_ADCx);
    delay_ms(1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ICE_ADCx);

    /* ADC software trigger enable */
//    adc_software_trigger_enable(ICE_ADCx, ADC_REGULAR_CHANNEL);

//    /* config the GPIO as analog mode */
//    gpio_init(ADC_VOLT_Port, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, ADC_VOLT_Pin);
//    gpio_init(ADC_CURT_Port, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, ADC_CURT_Pin);
//    /* ADC mode config */
//    adc_mode_config(ADC_MODE_FREE);
//    /* ADC data alignment config */
//    adc_data_alignment_config(ICE_ADCx, ADC_DATAALIGN_RIGHT);
//    /* ADC channel length config */
//    adc_channel_length_config(ICE_ADCx, ADC_REGULAR_CHANNEL, 1U);
//
//    /* ADC trigger config */
//    adc_external_trigger_source_config(ICE_ADCx, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);
//    /* ADC external trigger config */
//    adc_external_trigger_config(ICE_ADCx, ADC_REGULAR_CHANNEL, ENABLE);
//
//    /* enable ADC interface */
//    adc_enable(ICE_ADCx);
//    delay_ms(1U);
//    /* ADC calibration and reset calibration */
//    adc_calibration_enable(ICE_ADCx);
#endif //ICE_GD32F30X

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
#endif //ICE_RP2040
}

void ice_adc_init()
{
    adc_dma_init();
    adc_init();
#ifdef ICE_GD32F30X
    nvic_irq_enable(ICE_ADCx_DMAx_IRQn, 15, 0);
#endif //ICE_GD32F30X
}

/**
* @brief ��ȡADC�� ch ����adcֵ
* @param rank adc ͨ�����
* @return ����adcֵ
*/
uint16_t ice_adc_get_value(uint8_t rank)
{
    uint16_t adc_value;
#ifdef ICE_GD32F30X
    adc_value = ice_adc[rank];
#endif
#ifdef ICE_RP2040
    adc_value = adc_read();
#endif
    return adc_value;
}

/**
* @brief ��ȡADC�� ch ���ĵ�ѹֵ
* @param rank adc ͨ�����
* @return ���ص�ѹֵ, unit: mV
*/
uint16_t ice_adc_get_volt(uint8_t rank)
{
    uint16_t adc_value;
#ifdef ICE_GD32F30X
    adc_value = ice_adc[rank];
#endif
    return (ICE_ADC_VOLT_MV_MAX * adc_value / ICE_ADC_VALUE_MAX);
}

/**
* @brief ��ȡ�ڲ��¶ȴ�������ֵ
* @return �¶�
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