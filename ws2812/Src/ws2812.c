#include "ice.h"

#ifdef ICE_WS2812

/**
* @brief
* @param rgb ÑÕÉ«µÄRGBÖµ
*/
void ws2812_set_color(uint32_t rgb)
{
    uint32_t grb = ((rgb & 0xFF0000) >> 8u) | ((rgb & 0x00FF00) << 8u) | (rgb & 0x0000FF);
    pio_sm_put_blocking(ICE_PIO0, ICE_SM0, grb << 8u);
}

#endif