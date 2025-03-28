#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include <vector>

#include "tree_generator.h"

#define EXEC_LINE_LENGTH 100

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "argument needed: <PID x32>\n");
        return 1;
    }

    uint32_t pid = 0;
    sscanf(argv[1], "%d", &pid);

    char exec_line[EXEC_LINE_LENGTH] = {};
    char mmap_filename[] = "/tmp/memory_map_%d";

    sprintf(exec_line, "sudo pagemap2 %d > %s", pid, mmap_filename); //fill exec_line
    
    /* run pagemap2 to get memory map */
    system(exec_line);

    FILE* mmap_file = fopen(mmap_filename, "r");
    
    uint32_t buffer_size = 1 + count_symbols_in_file(mmap_file);
    char* buf = (char*) calloc(buffer_size, sizeof (*buf));
    assert(buf != NULL);

    /* read file to buffer */
    fread(buf, sizeof(char), buffer_size, mmap_file);
    
    // printf("[%s]\n", buf);

    std::vector<vm_page> pages{};
    

    fclose(mmap_file);
}