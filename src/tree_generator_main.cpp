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

    // print_tree(vm_tree);

    const char* json_filename = "tree.json";
    tree_to_json(vm_tree, json_filename);
}

