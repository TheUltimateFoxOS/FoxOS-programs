#pragma once

#include <stdint.h>

#define FEXEC_MAGIC 0xf0c505ab

typedef struct {
    uint32_t magic;
    uint8_t version;
    uint8_t reserved[11];

    uint64_t entry;
    uint64_t size;
} fexec_header_t;