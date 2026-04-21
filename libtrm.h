#ifndef LIBTRM_H
#define LIBTRM_H

#include <stddef.h>

typedef struct {
    size_t pss_kb;
    size_t pss_bytes;
    size_t rss_kb;
    size_t rss_bytes;
    size_t shared_kb;
    size_t shared_bytes;
} trm_memory_t;

int trm_get_memory(trm_memory_t* mem);

#endif

#ifdef LIBTRM_IMPLEMENTATION

#include <stdio.h>

int trm_get_memory(trm_memory_t* mem) {
    if (mem == NULL) return -1;

    int is_rollup = 1;
    FILE *file = fopen("/proc/self/smaps_rollup", "r");

    if (file == NULL) {
        file = fopen("/proc/self/smaps", "r");
        is_rollup = 0;

        if (file == NULL) {
            return -1;
        }
    }

    char line[256];
    mem->pss_kb = 0;
    mem->rss_kb = 0;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'R' && line[1] == 's' && line[2] == 's' && line[3] == ':') {
            char *p = line + 4;
            while (*p == ' ' || *p == '\t') p++;

            size_t val = 0;
            while (*p >= '0' && *p <= '9') {
                val = (val * 10) + (*p - '0');
                p++;
            }
            mem->rss_kb += val;
        }
        else if (line[0] == 'P' && line[1] == 's' && line[2] == 's' && line[3] == ':') {
            char *p = line + 4;
            while (*p == ' ' || *p == '\t') p++;

            size_t val = 0;
            while (*p >= '0' && *p <= '9') {
                val = (val * 10) + (*p - '0');
                p++;
            }
            mem->pss_kb += val;

            if (is_rollup) {
                break;
            }
        }
    }

    mem->pss_bytes = mem->pss_kb * 1024;
    mem->rss_bytes = mem->rss_kb * 1024;

    if (mem->rss_kb > mem->pss_kb) {
        mem->shared_kb = mem->rss_kb - mem->pss_kb;
    } else {
        mem->shared_kb = 0;
    }
    mem->shared_bytes = mem->shared_kb * 1024;

    fclose(file);
    return 0;
}

#endif