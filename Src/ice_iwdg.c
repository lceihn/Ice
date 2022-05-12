#include "ice.h"

#if ICE_IWDG

void ice_iwdg_init()
{
#ifdef ICE_GD32F30X
/* confiure FWDGT counter clock: 40KHz(IRC40K) / 64 = 0.625 KHz */
    fwdgt_config(ICE_IWDG_TIMEOUT / 1.6,FWDGT_PSC_DIV64);

    /* After 4 seconds to generate a reset */
    fwdgt_enable();
#endif
#ifdef ICE_RP2040
    // Enable the watchdog, requiring the watchdog to be updated every 100ms or the chip will reboot
    // second arg is pause on debug which means the watchdog will pause when stepping through code
    watchdog_enable(ICE_IWDG_TIMEOUT, 1);
#endif
}

void ice_iwdg_feed()
{
#ifdef ICE_GD32F30X
    fwdgt_counter_reload();
#endif
#ifdef ICE_RP2040
    watchdog_update();
#endif
}

#endif