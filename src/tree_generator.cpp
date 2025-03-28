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