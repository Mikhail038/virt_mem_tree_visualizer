#ifndef PARCER_H
#define PARCER_H

#include <stdint.h>

#define VPN_9_MASK      0x1ff
#define VPN_MASK_SIZE   9
#define VPN_START_SHIFT_SIZE 12

void parce_vpn(uint64_t virtual_page_number, uint8_t blocks_amnt);

#endif