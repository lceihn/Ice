/*
 * This file is part of the EasyFlash Library.
 *
 * Copyright (c) 2015-2019, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2015-01-16
 */

#include <easyflash.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "ice.h"


/* 配置默认的环境变量 */
//电流 驻波 阈值设置(adc 值)
static uint16_t env_curt_th = 4095;
static uint16_t env_vr_th = 4095;

static const ef_env default_env_set[] = {
//        {   key  , value, value_len }，
    {"user", "ice", 0},                  //用户名
    {"password", "ice", 0},              //密码
    {"curt_th", &env_curt_th, sizeof(env_curt_th)},     //电流 adc 阈值
    {"vr_th", &env_vr_th, sizeof(env_vr_th)},           //驻波 adc 阈值
    {"email", "806277022@qq.com", 0},    //邮箱

};

#if ICE_EF_DEBUG
static char log_buf[128];
#endif

#ifdef ICE_GD32F30X
static void clear_flag();
#endif

/**
 * Flash port for hardware initialize.
 *
 * @param default_env default ENV set for user
 * @param default_env_size default ENV size
 *
 * @return result
 */
EfErrCode ef_port_init(ef_env const **default_env, size_t *default_env_size)
{
    EfErrCode result = EF_NO_ERR;

    *default_env = default_env_set;
    *default_env_size = sizeof(default_env_set) / sizeof(default_env_set[0]);

    return result;
}

/**
 * Read data from flash.
 * @note This operation's units is word.
 *
 * @param addr flash address
 * @param buf buffer to store read data
 * @param size read bytes size
 *
 * @return result
 */
EfErrCode ef_port_read(uint32_t addr, uint32_t *buf, size_t size)
{
    EfErrCode result = EF_NO_ERR;

    /* You can add your code under here. */
#ifdef ICE_RP2040
    addr += XIP_BASE;
#endif
//#ifdef ICE_GD32F30X
    uint8_t *buf_8 = (uint8_t *) buf;
    size_t i;

    /*copy from flash to ram */
    for (i = 0; i < size; i++, addr++, buf_8++)
        *buf_8 = *(uint8_t *) addr;

//#endif
    return result;
}

/**
 * Erase data on flash.
 * @note This operation is irreversible.
 * @note This operation's units is different which on many chips.
 *
 * @param addr flash address
 * @param size erase bytes size
 *
 * @return result
 */
EfErrCode ef_port_erase(uint32_t addr, size_t size)
{
    EfErrCode result = EF_NO_ERR;

    /* make sure the start address is a multiple of EF_ERASE_MIN_SIZE */
    EF_ASSERT(addr % EF_ERASE_MIN_SIZE == 0);

    /* You can add your code under here. */
#ifdef ICE_GD32F30X
    fmc_state_enum sta;
    size_t erase_pages, i;

    /* calculate pages */
    erase_pages = size / EF_ERASE_MIN_SIZE;
    if (size % EF_ERASE_MIN_SIZE != 0)
    {
        erase_pages++;
    }

    /* start erase */
    fmc_unlock();
    clear_flag();

    for (i = 0; i < erase_pages; i++)
    {
        sta = fmc_page_erase(addr + (EF_ERASE_MIN_SIZE * i));
        clear_flag();
        if (sta > 1)
        {
            result = EF_ERASE_ERR;
            break;
        }
    }

    fmc_lock();
#endif
#ifdef ICE_RP2040
    asm("cpsid i" : : : "memory");
    size_t erase_pages;
    /* calculate pages */
    erase_pages = size / EF_ERASE_MIN_SIZE;
    if (size % EF_ERASE_MIN_SIZE != 0)
        erase_pages++;
    for (int i = 0; i < erase_pages; ++i)
        flash_range_erase(addr + (EF_ERASE_MIN_SIZE * i), EF_ERASE_MIN_SIZE);
    asm("cpsie i" : : : "memory");
#endif

    return result;
}

/**
 * Write data to flash.
 * @note This operation's units is word.
 * @note This operation must after erase. @see flash_erase.
 *
 * @param addr flash address
 * @param buf the write data buffer
 * @param size write bytes size
 *
 * @return result
 */
EfErrCode ef_port_write(uint32_t addr, const uint32_t *buf, size_t size)
{
    EfErrCode result = EF_NO_ERR;

    /* You can add your code under here. */
#ifdef ICE_GD32F30X
    size_t i;
    uint32_t read_data;

    fmc_unlock();
    clear_flag();

    for (i = 0; i < size; i += 4, buf++, addr += 4)
    {
        /* write data */
        fmc_word_program(addr, *buf);
        read_data = *(uint32_t *) addr;
        /* check data */
        if (read_data != *buf)
        {
            result = EF_WRITE_ERR;
            break;
        }
    }
    fmc_lock();
#endif
#ifdef ICE_RP2040
    uint8_t *buf_8 = (uint8_t *) buf;
    rp2040_flash_program(addr, buf_8, size);
#endif

    return result;
}

/**
 * lock the ENV ram cache
 */
void ef_port_env_lock(void)
{
    /* You can add your code under here. */
#ifdef ICE_GD32F30X
    __disable_irq();
#endif
#ifdef ICE_RP2040
    asm("cpsid i" : : : "memory");
#endif
}

/**
 * unlock the ENV ram cache
 */
void ef_port_env_unlock(void)
{
    /* You can add your code under here. */
#ifdef ICE_GD32F30X
    __enable_irq();
#endif
#ifdef ICE_RP2040
    asm("cpsie i" : : : "memory");
#endif
}


/**
 * This function is print flash debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 *
 */
void ef_log_debug(const char *file, const long line, const char *format, ...)
{

#ifdef PRINT_DEBUG

    va_list args;

    /* args point to the first variable parameter */
//    va_start(args, format);
//
//    /* You can add your code under here. */
//    ef_print("[Flash](%s:%ld) ", file, line);
//    /* must use vprintf to print */
//    vsprintf(log_buf, format, args);
//    ef_print("%s", log_buf);
//    printf("\r");
//
//    va_end(args);

#endif

}

/**
 * This function is print flash routine info.
 *
 * @param format output format
 * @param ... args
 */
void ef_log_info(const char *format, ...)
{
    va_list args;

    /* args point to the first variable parameter */
//    va_start(args, format);
//
//    /* You can add your code under here. */
//    ef_print("[Flash]");
//    /* must use vprintf to print */
//    vsprintf(log_buf, format, args);
//    ef_print("%s", log_buf);
//    printf("\r");
//
//    va_end(args);
}

/**
 * This function is print flash non-package info.
 *
 * @param format output format
 * @param ... args
 */
void ef_print(const char *format, ...)
{
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);

    /* You can add your code under here. */
    /* must use vprintf to print */
#if ICE_EF_DEBUG
    vsprintf(log_buf, format, args);
    printf("%s", log_buf);
#endif
    va_end(args);
}
#ifdef ICE_GD32F30X
static void clear_flag()
{
    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_BANK0_END);
    fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
    fmc_flag_clear(FMC_FLAG_BANK0_PGERR);
}
#endif
