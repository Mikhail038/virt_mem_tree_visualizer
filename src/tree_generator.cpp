#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

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
    
    // printf("[%s]\n", buf);

    fclose(mmap_file);
}