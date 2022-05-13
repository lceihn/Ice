#include "ice.h"

#if ICE_SPI

void ice_spi_init()
{
#ifdef ICE_GD32F30X

#endif
#ifdef ICE_RP2040
    spi_init(ICE_SPI_ID, ICE_SPI_CLK);
    spi_set_format(ICE_SPI_ID,8,ICE_SPI_CPOL,ICE_SPI_CPHA,ICE_SPI_MSB_FIRST);
    gpio_set_function(ICE_SPI_MISO_Pin, GPIO_FUNC_SPI);
    gpio_set_function(ICE_SPI_SCK_Pin, GPIO_FUNC_SPI);
    gpio_set_function(ICE_SPI_MOSI_Pin, GPIO_FUNC_SPI);
#if ICE_SPI_HARD_CS
    gpio_set_function(ICE_SPI_CS_Pin, GPIO_FUNC_SPI); //! ʹ��Ӳ��CS��BUG, ���Ͷ��ֽ�ʱ ��CPHAΪ0ʱӲ��CS����������!!, ��CPHAΪ1ʱӲ��CS��������!!
#else
    //���CS����, ʹ��GPIO
    gpio_init(ICE_SPI_CS_Pin);
    gpio_set_dir(ICE_SPI_CS_Pin, GPIO_OUT);
    gpio_put(ICE_SPI_CS_Pin, 1);
#endif

#endif
}

/**
* @brief Ӳ��spi����len�ֽ�����
* @param buf ����ָ��
* @param len ���ͳ���
*/
__force_inline void ice_spi_write(uint8_t *buf, uint16_t len)
{
#if ICE_SPI_HARD_CS
#ifdef ICE_RP2040
    spi_write_blocking(ICE_SPI_ID, buf, len);
#endif
#else
    //���CS����
    ICE_SPI_CS(0);
#ifdef ICE_RP2040
    spi_write_blocking(ICE_SPI_ID, buf, len);
#endif
    ICE_SPI_CS(1);
#endif
}


#endif