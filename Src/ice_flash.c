#include "ice.h"

#ifdef ICE_RP2040

/**
* @brief ʹ��rp2040 �Դ���flash����, ������lenΪ0�����
* @param addr д���ַ
* @param buf  ����ָ��
* @param len  ����
*/
void rp2040_flash_program(uint32_t addr, const uint8_t *buf, uint16_t len)
{
    static uint8_t temp[FLASH_PAGE_SIZE]; //ҳ������
    asm("cpsid i" : : : "memory");

    //��ַ��ҳƫ��
    uint8_t page_offset = addr % FLASH_PAGE_SIZE;
    //ҳ�׵�ַ(д)
    uint32_t page_fwaddr = addr - page_offset;
    //ҳ�׵�ַ(��)
    uint32_t page_fraddr = page_fwaddr + XIP_BASE;
    //��д��ҳ������
    uint8_t page_nums = len / FLASH_PAGE_SIZE;
    //��д�����ҳ����ֽ�����
    uint8_t byte_nums = len % FLASH_PAGE_SIZE;

    //д����������1ҳ
    for (int i = 0; i < page_nums; ++i)
    {
        //������ǰҳƫ��ǰ������
        for (int j = 0; j < page_offset; ++j)
            temp[j] = *(uint8_t *)page_fraddr++;
        //�滻��д������
        for (int j = page_offset; j < FLASH_PAGE_SIZE; ++j)
            temp[j] = *buf++;
        //ҳд��
        flash_range_program(page_fwaddr, temp, FLASH_PAGE_SIZE);
        //�޸�ҳ��ַ
        page_fwaddr += FLASH_PAGE_SIZE;
        //д��һҳ��ƫ��Ϊ0
        page_offset = 0;
    }

    //�����һҳ
    int num = 0;
    //������ǰҳƫ��ǰ������
    for ( ; num < page_offset; ++num)
        temp[num] = *(uint8_t *)page_fraddr++;
    //�滻ʣ��Ҫд����ֽ�����
    for ( ; num < page_offset + byte_nums; ++num)
        temp[num] = *buf++;
    //�޸����һҳ�Ķ���ַ
    page_fraddr += byte_nums;
    //�������һҳʣ���ֽڵ�����
    for ( ; num < FLASH_PAGE_SIZE; ++num)
        temp[num] = *(uint8_t *)page_fraddr++;
    //ҳд��
    flash_range_program(page_fwaddr, temp, FLASH_PAGE_SIZE);
    asm("cpsie i" : : : "memory");
}

#endif