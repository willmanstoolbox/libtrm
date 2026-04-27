#ifndef LIBTRM_H
#define LIBTRM_H

#include <stddef.h>
#include <stdio.h>
#include <string.h>

typedef enum {
    TRM_OK = 0,
    TRM_ERR_NULL_PTR = -1,
    TRM_ERR_IO = -2,
    TRM_ERR_PARSE = -3
} trm_result_t;

typedef struct {
    size_t pss_kb; size_t pss_bytes;
    size_t rss_kb; size_t rss_bytes;
    size_t uss_kb; size_t uss_bytes;
    size_t shared_kb; size_t shared_bytes;
} trm_memory_t;

static inline trm_result_t trm_get_memory(trm_memory_t* mem) {
    if (mem == NULL) return TRM_ERR_NULL_PTR;
    memset(mem, 0, sizeof(trm_memory_t));

    FILE *file = fopen("/proc/self/smaps_rollup", "r");
    int is_rollup = 1;

    if (file == NULL) {
        file = fopen("/proc/self/smaps", "r");
        is_rollup = 0;
        if (file == NULL) return TRM_ERR_IO;
    }

    char line[256];
    size_t priv_clean_kb = 0, priv_dirty_kb = 0;
    int found_any = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "Rss:", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ' || *p == '\t') p++;
            size_t val = 0;
            while (*p >= '0' && *p <= '9') { val = (val * 10) + (*p - '0'); p++; }
            mem->rss_kb += val;
            found_any = 1;
        }
        else if (strncmp(line, "Pss:", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ' || *p == '\t') p++;
            size_t val = 0;
            while (*p >= '0' && *p <= '9') { val = (val * 10) + (*p - '0'); p++; }
            mem->pss_kb += val;
            if (is_rollup) break;
        }
        else if (strncmp(line, "Private_Clean:", 14) == 0) {
            char *p = line + 14;
            while (*p == ' ' || *p == '\t') p++;
            size_t val = 0;
            while (*p >= '0' && *p <= '9') { val = (val * 10) + (*p - '0'); p++; }
            priv_clean_kb += val;
        }
        else if (strncmp(line, "Private_Dirty:", 14) == 0) {
            char *p = line + 14;
            while (*p == ' ' || *p == '\t') p++;
            size_t val = 0;
            while (*p >= '0' && *p <= '9') { val = (val * 10) + (*p - '0'); p++; }
            priv_dirty_kb += val;
        }
    }

    if (!found_any) { fclose(file); return TRM_ERR_PARSE; }

    mem->uss_kb = priv_clean_kb + priv_dirty_kb;
    mem->pss_bytes = mem->pss_kb * 1024;
    mem->rss_bytes = mem->rss_kb * 1024;
    mem->uss_bytes = mem->uss_kb * 1024;

    if (mem->rss_kb > mem->pss_kb) mem->shared_kb = mem->rss_kb - mem->pss_kb;
    mem->shared_bytes = mem->shared_kb * 1024;

    fclose(file);
    return TRM_OK;
}

#endif