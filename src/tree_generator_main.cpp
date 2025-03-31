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

    const char* json_filename = "tree2.json";
    tree_to_json(vm_tree, json_filename);

    // tree<uint16_t> my_tree;

    // tree<uint16_t>::iterator iA = my_tree.insert(my_tree.end(), 0x11);
    // tree<uint16_t>::iterator iB = my_tree.append_child(iA, 0x22);
    // tree<uint16_t>::iterator iB1 = my_tree.append_child(iA, 0x33);
    // tree<uint16_t>::iterator iC = my_tree.append_child(iB, 0x44); 
    // tree<uint16_t>::iterator iC1 = my_tree.append_child(iB, 0x45); 
    // tree<uint16_t>::iterator iC2 = my_tree.append_child(iB, 0x46); 
    // my_tree.append_child(iC1, 0xf6); 
    // my_tree.append_child(iC1, 0xf7); 
    // my_tree.append_child(iC1, 0xf8); 

    // my_tree.append_child(iC2, 0xf8); 


    // tree<uint16_t>::iterator iD = my_tree.append_child(iC1, 0xf3); 
    // my_tree.append_child(iD, 0xc1); 
    // my_tree.append_child(iD, 0xc2); 
    // my_tree.append_child(iD, 0xc3); 
    // my_tree.append_child(iD, 0xc4); 

    // tree_to_json(my_tree, json_filename);

}

