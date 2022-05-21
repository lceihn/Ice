#include "ice.h"

#if ICE_PIO

void ice_pio_init()
{
    //使用 pio0 的 状态机0
    PIO pio = ICE_PIO0;
    uint sm = ICE_SM0;
#ifdef ICE_WS2812
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_Pin);
#endif
}

//bitloop:
//out x, 1       side 0 [T3 - 1] ; Side-set still takes place when instruction stalls
//        jmp !x do_zero side 1 [T1 - 1] ; Branch on the bit we shifted out. Positive pulse
//do_one:
//jmp  bitloop   side 1 [T2 - 1] ; Continue driving high, for a long pulse
//do_zero:
//nop            side 0 [T2 - 1] ; Or drive low, for a short pulse

#endif