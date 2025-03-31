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
    pages_vector pages_parced;

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

tree<uint16_t> make_tree(pages_vector& pages)
{
    tree<uint16_t> result{};
    tree<uint16_t>::iterator top;

    top = result.set_head(0);//let s think first page can only be one //FIXME

    uint16_t start_fixed_vpn[VPN_BLOCKS_AMNT] = {0};
    find_unique(pages, start_fixed_vpn, 1, result, top); 

    return result;
}

void find_unique(pages_vector pages, uint16_t fixed_vpn[VPN_BLOCKS_AMNT], 
                                    uint8_t key_layer, tree<uint16_t> result, tree<uint16_t>::iterator father)
{
    std::vector<uint16_t> unique_vpn;
    std::vector<tree<uint16_t>::iterator> unique_fathers;

    // printf("father [%x]: ", father);
    for (uint64_t cnt = 0; cnt < pages.size(); cnt++)
    {
        if (std::find(unique_vpn.begin(), unique_vpn.end(), pages[cnt].vpn[key_layer]) == unique_vpn.end())
        {
            /* put unique page number in array */
            unique_vpn.push_back(pages[cnt].vpn[key_layer]);

            /* put unique page number in tree and itter too array for proper work on next layer */
            unique_fathers.push_back(result.append_child(father, pages[cnt].vpn[key_layer]));
        }
    }

    if (key_layer < VPN_BLOCKS_AMNT - 1)
    {
        for (uint64_t cnt = 0; cnt < unique_vpn.size(); cnt++)
        {
            uint16_t new_fixed_vpn[VPN_BLOCKS_AMNT];
            for (uint64_t copy_cnt = 0; copy_cnt < key_layer; copy_cnt++)
            {
                new_fixed_vpn[copy_cnt] = fixed_vpn[copy_cnt];
            }
            new_fixed_vpn[key_layer] = unique_vpn[cnt];

            find_unique(pages, new_fixed_vpn, key_layer + 1, result, unique_fathers[cnt]);
        }        
    }
    else
    {
        return;
    }

}

void print_tree(const tree<uint16_t>& tr)
{
    tree<uint16_t>::pre_order_iterator it = tr.begin();
    tree<uint16_t>::pre_order_iterator end = tr.end();

    if(!tr.is_valid(it)) return;

    uint32_t rootdepth = tr.depth(it);

    std::cout << "-----" << std::endl;
    while(it != end) 
    {
        for(uint32_t cnt = 0; cnt < tr.depth(it) - rootdepth; ++cnt)
            std::cout << "  ";
        std::cout << (*it) << std::endl << std::flush;
        ++it;
    }
    std::cout << "-----" << std::endl;
}

void tree_to_json(const tree<uint16_t>& tr, const char* json_filename)
{
    tree<uint16_t>::pre_order_iterator it = tr.begin();

    if(!tr.is_valid(it)) return;


    FILE* file = fopen(json_filename, "w");
    // fprintf(file, "[\n");

    self_to_json(file, tr, it);

    // fprintf(file, "]");
    fclose(file);
}

void self_to_json(FILE* file, const tree<uint16_t>& tr, tree<uint16_t>::pre_order_iterator it)
{
    auto old_it = it;
    uint8_t old_depth = tr.depth(it) - tr.depth(tr.begin());

    depth_based_space(file, old_depth);
    fprintf(file, "{\"Item\": { \"Name\": \"%x\" }\n", (*old_it));

    it++;

    if (it == tr.end())
    {
        depth_based_space(file, old_depth);
        fprintf(file, "}\n");

        uint8_t bubble = old_depth - 1;
        while (bubble != 0)
        {
            depth_based_space(file, bubble);
            fprintf(file, "]\n");
    
            depth_based_space(file, bubble);
            fprintf(file, "}\n");
            bubble--;
        }

        fprintf(file, "]\n");
        fprintf(file, "}\n");

        return;
    };

    uint8_t depth = tr.depth(it) - tr.depth(tr.begin());

    if (depth > old_depth)
    {
        depth_based_space(file, old_depth);
        fprintf(file, "  ,\"Children\": [\n");

        
        self_to_json(file, tr, it);
    }

    if (depth == old_depth)
    {
        depth_based_space(file, old_depth);
        fprintf(file, "}, \n");

        self_to_json(file, tr, it);
    }

    if (depth < old_depth)
    {
        uint8_t cycles = old_depth - depth;

        for (uint8_t cnt = 0; cnt < cycles; cnt++)
        {
            depth_based_space(file, old_depth - cnt);
            fprintf(file, "}\n");   
            
            depth_based_space(file, old_depth - cnt - 1);
            fprintf(file, "]\n");    
        }
        



        // auto next_it = it;
        // next_it++;

        // if ((next_it != tr.end()) && ((tr.depth(next_it) - tr.depth(tr.begin())) < depth))
        // {
        //     depth_based_space(file, depth);
        //     fprintf(file, "}\n");
        // }
        // else 
        // {
            depth_based_space(file, depth);
            fprintf(file, "},\n");
        // }

        self_to_json(file, tr, it);
    }
    
}

void depth_based_space(FILE* file, uint8_t depth)
{
    for (uint8_t i = 0; i < depth; ++i)
    {
        fprintf(file, "   ");
    }
}
