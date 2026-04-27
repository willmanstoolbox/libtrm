#include <stdio.h>
#include <string.h>

#define LIBTRM_IMPLEMENTATION
#include "../libtrm.h"

void test_header(const char* name) {
    printf("\n[TEST]: %s\n", name);
    printf("------------------------------------------\n");
}

int main() {
    printf("==========================================\n");
    printf(" libtrm Robustness & Error Handling Suite\n");
    printf("==========================================\n");

    test_header("NULL Pointer Check");
    trm_result_t res = trm_get_memory(NULL);

    if (res == TRM_ERR_NULL_PTR) {
        printf("PASSED: Function returned TRM_ERR_NULL_PTR as expected.\n");
    } else {
        printf("FAILED: Function did not catch NULL pointer (Code: %d)\n", res);
    }

    test_header("Internal Memory Wipe");
    trm_memory_t ram;

    memset(&ram, 0xFF, sizeof(trm_memory_t));
    printf("Status: Struct filled with garbage (0xFF).\n");

    res = trm_get_memory(&ram);

    if (res == TRM_OK || res == TRM_ERR_PARTIAL_DATA) {
        if (ram.pss_kb != (size_t)-1) {
            printf("PASSED: Internal memset() successfully wiped the garbage data.\n");
            printf("Post-call PSS: %zu kB\n", ram.pss_kb);
        } else {
            printf("FAILED: Struct still contains garbage values.\n");
        }
    } else {
        if (ram.pss_kb == 0) {
            printf("PASSED: Call failed, but struct was safely zeroed out.\n");
        } else {
            printf("FAILED: Call failed and left garbage in the struct.\n");
        }
    }

    test_header("Linux Environment Validation");
    res = trm_get_memory(&ram);
    if (res != TRM_ERR_IO) {
        printf("PASSED: /proc access is valid on this system.\n");
    } else {
        printf("INFO: /proc is inaccessible. This is expected if NOT on Linux.\n");
    }

    printf("\n==========================================\n");
    printf(" All Robustness Checks Complete.\n");
    printf("==========================================\n");

    printf("=============================================================================\n");
    printf(" Error showcase complete! Thank you for checking out the project.\n");
    printf(" Feel free to ask any questions at ticuette@gmail.com.\n");
    printf(" You can support my work or check out other tools at willmanstoolbox.com\n");
    printf("=============================================================================\n");

    return 0;
}