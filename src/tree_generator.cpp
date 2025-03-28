#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <iostream>

#include <vector>

#include "tree.hh"

#include "tree_generator.h"
#include "parcer.h"

uint64_t count_symbols_in_file (FILE* stream)
{
    assert(stream != NULL);

    fseek(stream, 0, SEEK_END);

    uint64_t size = ftell(stream);

    fseek(stream, 0, SEEK_SET);
    return size;
}

#define EXEC_LINE_LENGTH 100

void put_mmap_to_buffer(pid_t pid, char** buf, uint64_t* buffer_size)
{
    char exec_line[EXEC_LINE_LENGTH] = {};
    char mmap_filename[] = "/tmp/memory_map_%d";

    sprintf(exec_line, "sudo pagemap2 %d > %s", pid, mmap_filename); //fill exec_line
    
    /* run pagemap2 to get memory map */
    system(exec_line);

    FILE* mmap_file = fopen(mmap_filename, "r");
    
    *buffer_size = 1 + count_symbols_in_file(mmap_file);
    *buf = (char*) calloc(*buffer_size, sizeof (*buf));
    assert(buf != NULL);

    /* read file to buffer */
    fread(*buf, sizeof(char), *buffer_size, mmap_file);
    // printf("[%s]\n", *buf);

    fclose(mmap_file);
}

using pages_vector = std::vector<vm_page_parced>;

pages_vector parce_mmap_buffer(char* buf)
{
    std::vector<vm_page_parced> pages_parced;

    std::vector<vm_page> pages{};
    uint64_t amnt_pages = parce_buff_on_pages(pages, buf);

    for (uint64_t cnt = 0; cnt < amnt_pages; cnt++)
    {
        vm_page_parced page_parced;
        parce_vpn(page_parced.vpn, pages[cnt].vpn, VPN_BLOCKS_AMNT);

        pages_parced.push_back(page_parced);
        // printf("%x %x %x %x %x\n", pages_parced[cnt].vpn[0], pages_parced[cnt].vpn[1], pages_parced[cnt].vpn[2], pages_parced[cnt].vpn[3], pages_parced[cnt].vpn[4]);
    }

    return pages_parced;
}

uint64_t parce_buff_on_pages(std::vector<vm_page>& pages, char* buf)
{
    /* count amount of lines */
    char sym = '\0';
    uint64_t amnt_lines = 0;
    uint64_t cnt = 0;
    while ((sym = buf[cnt++]) != '\0')
    {
        if (sym != '\n')
        {
            amnt_lines++;
            while ((sym = buf[cnt++]) != '\n');
        }
    }
    
    /* parce char* to char*[] of lines */
    char* line_ptrs[amnt_lines];
    uint64_t first_char = 0, char_cnt = 0, line_cnt = 0;
    while (line_cnt < amnt_lines)
    {
        char_cnt = 0;

        while (buf[first_char + char_cnt] != '\n')
        {
            char_cnt++;
        }

        if (char_cnt != 0)
        {
            line_ptrs[line_cnt] = (char*) &(buf[first_char]);
            line_ptrs[line_cnt][char_cnt] = '\0';
            // printf("[%lu/%lu] (%s)\n", line_cnt, amnt_lines - 1, line_ptrs[line_cnt]);
            line_cnt++;
        }
        first_char += char_cnt + 1;
    }

    for (uint64_t cnt = 1 /* skip first line */ ; cnt < amnt_lines; cnt++)
    {
        vm_page page;

        sscanf(line_ptrs[cnt], "%lx : pfn %x", &(page.vpn), &(page.pfn));

        pages.push_back(page);

        // printf("%llx %llx\n", page.vpn, page.pfn);
    }
    // printf("[%1x]\n", pages[0].vpn);

    return amnt_lines - 1;
}

void print_pages(const pages_vector& pages) {
    std::cout << "-- Size: " << pages.size() << "\n";
    for (auto& page : pages) {
        page.print();
    }
}


// pages is modified inside
tree<uint16_t> make_tree(pages_vector pages)
{
    tree<uint16_t> result{};

    auto compare_by_first = [](const vm_page_parced& lhs, const vm_page_parced& rhs) {
        return lhs.vpn[0] == rhs.vpn[0];
    };

    pages_vector first_layer_pages = pages;
    auto last = std::unique(first_layer_pages.begin(), first_layer_pages.end(), compare_by_first);
    first_layer_pages.erase(last, first_layer_pages.end());

    print_pages(first_layer_pages);

    for (auto& first_page : first_layer_pages)
    {
        pages_vector second_layer_pages; // for the node first_page
        std::copy(pages.begin(), pages.end(), [&](const vm_page_parced& page) {
                                                return page.vpn[0] == first_page.vpn[0];});
    }


    return {};
}

