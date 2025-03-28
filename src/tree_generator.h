#ifndef TREE_GENERATOR_H
#define TREE_GENERATOR_H

#include <stdlib.h>
#include <stdint.h>

#include <vector>

#include "parcer.h"

#define VPN_BLOCKS_AMNT 5

struct vm_page
{
    uint64_t vpn = 0;
    uint32_t pfn = 0;
};

struct vm_page_parced
{
    uint16_t vpn[VPN_BLOCKS_AMNT];
    uint32_t pfn;
};


uint64_t count_symbols_in_file (FILE* stream);

void put_mmap_to_buffer(pid_t pid, char** buf, uint64_t* buffer_size);

void parce_mmap_buffer(std::vector<vm_page_parced>& pages_parced, char* buf, uint64_t buffer_size);

uint64_t parce_buff_on_pages(std::vector<vm_page>& pages, char* buf, uint64_t buffer_size);

#endif