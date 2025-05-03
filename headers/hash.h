#ifndef HASH_INCLUDED
#define HASH_INCLUDED

#include <stdint.h>

uint32_t MurMur32Hash(const void * data, size_t len);

uint32_t crc32Hash(const void * data, size_t len);

#endif
