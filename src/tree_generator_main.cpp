#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "tree_generator.h"
#include <iostream>

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "argument needed: <PID x32>\n");
        return 1;
    }

    uint32_t pid = 0;
    sscanf(argv[1], "%d", &pid);

    char* buf = 0;
    uint64_t buffer_size = 0;
    put_mmap_to_buffer(pid, &buf, &buffer_size);

    std::vector<vm_page_parced> pages_parced = parce_mmap_buffer(buf);
    
    tree<uint16_t> vm_tree = make_tree(pages_parced);

    printf("!!!\n");
    // abort();
    tree<uint16_t>::sibling_iterator it = vm_tree.begin();
    while (it != vm_tree.end())
        std::cout << "[" << (*it++) << "]" << std::endl;

    // tree<uint16_t>::iterator sib2 =  vm_tree.begin();
    // tree<uint16_t>::iterator end2 =  vm_tree.end();
    // while(sib2!=end2)
    // { 
    //     for (uint16_t i=0; i < vm_tree.depth(sib2)-2; ++i)
    //     {
    //         std::cout << " ";
    //     }

    //     std::cout << (*sib2) << std::endl;
    //     ++sib2;
    // }
}

