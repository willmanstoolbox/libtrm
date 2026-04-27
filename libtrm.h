#ifndef LIBTRM_H
#define LIBTRM_H

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#ifndef LIBTRM_DEF
#define LIBTRM_DEF extern
#endif

typedef enum {
    TRM_OK = 0,
    TRM_ERR_NULL_PTR = -1,
    TRM_ERR_IO = -2,
    TRM_ERR_PARSE = -3,
    TRM_ERR_PARTIAL_DATA = -4
} trm_result_t;

typedef struct {
    size_t pss_kb; size_t pss_bytes;
    size_t rss_kb; size_t rss_bytes;
    size_t uss_kb; size_t uss_bytes;
    size_t shared_saved_kb; size_t shared_saved_bytes;
} trm_memory_t;

LIBTRM_DEF trm_result_t trm_get_memory(trm_memory_t* mem);

#ifdef LIBTRM_IMPLEMENTATION

static inline size_t trm_kb_to_bytes(size_t kb) {
    if (kb > SIZE_MAX / 1024) return SIZE_MAX;
    return kb * 1024;
}

static int trm_parse_kb(const char* p, size_t* out) {
    while (*p == ' ' || *p == '\t') p++;

    if (*p < '0' || *p > '9') return 0;

    size_t val = 0;
    while (*p >= '0' && *p <= '9') {
        size_t digit = (size_t)(*p - '0');

        if (val > (SIZE_MAX - digit) / 10) return 0;

        val = (val * 10) + digit;
        p++;
    }

    while (*p == ' ' || *p == '\t') p++;

    if (p[0] != 'k' || p[1] != 'B') return 0;

    *out = val;
    return 1;
}

LIBTRM_DEF trm_result_t trm_get_memory(trm_memory_t* mem) {
    if (mem == NULL) return TRM_ERR_NULL_PTR;
    memset(mem, 0, sizeof(trm_memory_t));

    FILE *file = fopen("/proc/self/smaps_rollup", "r");

    if (file == NULL) {
        file = fopen("/proc/self/smaps", "r");
        if (file == NULL) return TRM_ERR_IO;
    }

    char line[256];
    size_t priv_clean_kb = 0, priv_dirty_kb = 0;
    size_t parsed_val = 0;
    unsigned int parse_flags = 0;

    while (fgets(line, sizeof(line), file)) {

        size_t len = strlen(line);
        int is_truncated = (len > 0 && line[len - 1] != '\n' && !feof(file));

        if (strncmp(line, "Rss:", 4) == 0) {
            if (trm_parse_kb(line + 4, &parsed_val)) {
                mem->rss_kb += parsed_val;
                parse_flags |= 1;
            }
        }
        else if (strncmp(line, "Pss:", 4) == 0) {
            if (trm_parse_kb(line + 4, &parsed_val)) {
                mem->pss_kb += parsed_val;
                parse_flags |= 2;
            }
        }
        else if (strncmp(line, "Private_Clean:", 14) == 0) {
            if (trm_parse_kb(line + 14, &parsed_val)) {
                priv_clean_kb += parsed_val;
                parse_flags |= 4;
            }
        }
        else if (strncmp(line, "Private_Dirty:", 14) == 0) {
            if (trm_parse_kb(line + 14, &parsed_val)) {
                priv_dirty_kb += parsed_val;
                parse_flags |= 8;
            }
        }

        if (is_truncated) {
            int ch;
            while ((ch = fgetc(file)) != '\n' && ch != EOF);
        }
    }

    fclose(file);

    int has_rss = (parse_flags & 1);
    int has_pss = (parse_flags & 2);
    int has_private = (parse_flags & 4) && (parse_flags & 8);

    if (!has_rss || !has_pss) {
        return TRM_ERR_PARSE;
    }

    mem->uss_kb = priv_clean_kb + priv_dirty_kb;
    if (mem->rss_kb > mem->pss_kb) mem->shared_saved_kb = mem->rss_kb - mem->pss_kb;

    mem->pss_bytes = trm_kb_to_bytes(mem->pss_kb);
    mem->rss_bytes = trm_kb_to_bytes(mem->rss_kb);
    mem->uss_bytes = trm_kb_to_bytes(mem->uss_kb);
    mem->shared_saved_bytes = trm_kb_to_bytes(mem->shared_saved_kb);

    if (!has_private) {
        return TRM_ERR_PARTIAL_DATA;
    }

    return TRM_OK;
}

#endif
#endif