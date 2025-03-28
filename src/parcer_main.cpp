#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "parcer.h"

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "argument needed: ./*.bin <VPN x64>\n");

        return 1;
    }

    uint64_t virtual_page_number = 0;
    sscanf(argv[1], "%lX", &virtual_page_number);

    parce_and_print_vpn(virtual_page_number, VPN_BLOCKS_AMNT);
}