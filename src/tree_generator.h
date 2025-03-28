#ifndef PARCER_H
#define PARCER_H

#include <stdlib.h>
#include <stdint.h>

struct vm_page
{
    uint64_t vpn = 0;
    uint32_t pfn = 0;
};

uint64_t count_symbols_in_file (FILE* stream);

#endif