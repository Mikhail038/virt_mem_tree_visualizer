#ifndef PARCER_H
#define PARCER_H

#include <stdint.h>

#define VPN_9_MASK      0x1ff
#define VPN_MASK_SIZE   9
#define VPN_START_SHIFT_SIZE 12
#define VPN_BLOCKS_AMNT 5

void parce_and_print_vpn(uint64_t virtual_page_number, uint8_t blocks_amnt);
void parce_vpn(uint16_t vpn_parced[VPN_BLOCKS_AMNT], uint64_t virtual_page_number, uint8_t blocks_amnt);

#endif