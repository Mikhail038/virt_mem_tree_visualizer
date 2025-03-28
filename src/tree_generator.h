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

void put_mmap_to_buffer(pid_t pid, char** buf, uint64_t* buffer_size);


#endif