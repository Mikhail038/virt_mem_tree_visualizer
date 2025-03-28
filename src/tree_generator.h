#ifndef TREE_GENERATOR_H
#define TREE_GENERATOR_H

#include <stdlib.h>
#include <stdint.h>

#include <vector>

#include "parcer.h"

#define VPN_BLOCKS_AMNT 5

struct vm_page
{
    uint64_t vpn;
    uint32_t pfn;
};

struct vm_page_parced
{
    uint16_t vpn[VPN_BLOCKS_AMNT];
    uint32_t pfn;
};

struct vpn_node
{
    uint16_t vpn;

    vpn_node* parent;
    std::vector<vpn_node*> children;

    void vpn_node::push(vpn_node* node, vpn_node* parent);
};

uint64_t count_symbols_in_file (FILE* stream);

void put_mmap_to_buffer(pid_t pid, char** buf, uint64_t* buffer_size);

std::vector<vm_page_parced> parce_mmap_buffer(char* buf, uint64_t buffer_size);

uint64_t parce_buff_on_pages(std::vector<vm_page>& pages, char* buf, uint64_t buffer_size);

vpn_node* make_tree(std::vector<vm_page>& pages);

#endif