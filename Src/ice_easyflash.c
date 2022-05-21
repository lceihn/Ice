#include "ice.h"

#ifdef ICE_EASYFLASH

/**
* @brief easyflash init
*/
void ice_easyflash_init()
{
    EfErrCode err = EF_NO_ERR;
    err = easyflash_init();
    ICE_ASSERT(!err)
}

#endif
