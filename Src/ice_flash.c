#include "ice.h"

#ifdef ICE_RP2040

/**
* @brief 使用rp2040 自带的flash方法, 不考虑len为0的情况
* @param addr 写入地址
* @param buf  数据指针
* @param len  长度
*/
void rp2040_flash_program(uint32_t addr, const uint8_t *buf, uint16_t len)
{
    static uint8_t temp[FLASH_PAGE_SIZE]; //页缓冲区
    asm("cpsid i" : : : "memory");

    //地址的页偏移
    uint8_t page_offset = addr % FLASH_PAGE_SIZE;
    //页首地址(写)
    uint32_t page_fwaddr = addr - page_offset;
    //页首地址(读)
    uint32_t page_fraddr = page_fwaddr + XIP_BASE;
    //待写入页的数量
    uint8_t page_nums = len / FLASH_PAGE_SIZE;
    //待写入的余页后的字节数量
    uint8_t byte_nums = len % FLASH_PAGE_SIZE;

    //写入数量大于1页
    for (int i = 0; i < page_nums; ++i)
    {
        //读出当前页偏移前的内容
        for (int j = 0; j < page_offset; ++j)
            temp[j] = *(uint8_t *)page_fraddr++;
        //替换待写入内容
        for (int j = page_offset; j < FLASH_PAGE_SIZE; ++j)
            temp[j] = *buf++;
        //页写入
        flash_range_program(page_fwaddr, temp, FLASH_PAGE_SIZE);
        //修改页地址
        page_fwaddr += FLASH_PAGE_SIZE;
        //写完一页后偏移为0
        page_offset = 0;
    }

    //读最后一页
    int num = 0;
    //读出当前页偏移前的内容
    for ( ; num < page_offset; ++num)
        temp[num] = *(uint8_t *)page_fraddr++;
    //替换剩余要写入的字节内容
    for ( ; num < page_offset + byte_nums; ++num)
        temp[num] = *buf++;
    //修改最后一页的读地址
    page_fraddr += byte_nums;
    //读出最后一页剩余字节的内容
    for ( ; num < FLASH_PAGE_SIZE; ++num)
        temp[num] = *(uint8_t *)page_fraddr++;
    //页写入
    flash_range_program(page_fwaddr, temp, FLASH_PAGE_SIZE);
    asm("cpsie i" : : : "memory");
}

#endif