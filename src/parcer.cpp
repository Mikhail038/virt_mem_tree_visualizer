#include <stdio.h>
#include <stdlib.h>
#include "parcer.h"

void parce_vpn(uint64_t virtual_page_number, uint8_t blocks_amnt)
{
    printf("virt %.lX\n", virtual_page_number);

    virtual_page_number >>= VPN_START_SHIFT_SIZE;
    for (uint8_t cnt = 0; cnt < blocks_amnt; cnt++)
    {
        uint8_t shift = cnt * VPN_MASK_SIZE;
        uint64_t vpn = (virtual_page_number & (VPN_9_MASK << shift)) >> shift;
        printf("VPN[%.1X] %.1lX\n", cnt, vpn);
    }
}