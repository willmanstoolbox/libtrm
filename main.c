#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LIBTRM_IMPLEMENTATION
#include "libtrm.h"

int main() {
    printf("======================================================================================================\n");
    printf(" Thank you for using libtrm. This tutorial will help you understand how to use it in your own project.\n");
    printf("======================================================================================================\n\n");

    printf("Step 1: Initializing the data container.\n");
    printf("We create a trm_memory_t struct to hold our data. We don't even need to set it to 0 anymore because the library does it for us.\n\n");
    trm_memory_t ram;

    printf("Step 2: Calling the libtrm API.\n");
    trm_result_t result = trm_get_memory(&ram);

    if (result != TRM_OK) {
        if (result == TRM_ERR_IO) {
            printf("ERROR: libtrm failed to read the memory files. This will happen if you are not on Linux. (I guess this could also be a permission issue but you will know better than me here)\n");
        } else if (result == TRM_ERR_PARSE) {
            printf("ERROR: Files found, but the kernel isn't reporting the data we need for some reason.\n");
        } else {
            printf("ERROR: An unknown error occurred. Code: %d\n", result);
        }
        return 1;
    }

    printf("We're done! Let's look at the baseline memory footprint of this app:\n");
    printf("------------------------------------------------------------\n");
    printf("  -> RSS: %zu kB  This counts shared standard libraries multiple times.\n", ram.rss_kb);
    printf("  -> PSS: %zu kB  This is your true, proportional footprint. Look how small!\n", ram.pss_kb);
    printf("  -> USS: %zu kB  RAM that only you are using.)\n", ram.uss_kb);
    printf("  -> Shared: %zu kB  RAM you are saving by sharing libraries with the OS. So yeah Linux was lying all along.)\n");
    printf("  -> PSS in Bytes: %zu bytes\n", ram.pss_bytes);
    printf("     (This can be done without the API by just multiplying by 1024, but I decided to add it so you can log it easier or do math with it.)\n");

    printf("Step 3: Demonstrating Lazy Allocation.\n");
    printf("malloc() will allocate some ram here\n\n");

    size_t massive_size = 50 * 1024 * 1024;
    char* big_array = (char*)malloc(massive_size);

    if (big_array == NULL) {
        printf("Failed to allocate memory!\n");
        return 1;
    }

    trm_get_memory(&ram);
    printf("--- After malloc() ---\n");
    printf("  -> USS is now: %zu kB\n", ram.uss_kb);
    printf("Notice how it barely changed! Linux lied and gave us an IOU instead of real RAM.\n\n");

    printf("Step 4: Forcing the physical allocation.\n");
    printf("Now we use memset() to write actual data into all 50 Megabytes...\n\n");

    memset(big_array, 7, massive_size);

    trm_get_memory(&ram);
    printf("--- After memset() ---\n");
    printf("  -> USS is now: %zu kB\n", ram.uss_kb);
    printf("  -> PSS is now: %zu kB\n", ram.pss_kb);
    printf("The PSS and USS jumped by exactly ~51,200 kB (50MB). libtrm caught the real allocation.\n\n");

    printf("Step 5: Why USS matters.\n");
    printf("If you killed this process right now, the OS would regain exactly %zu bytes.\n", ram.uss_bytes);
    printf("Not the RSS, and not just the PSS—but the USS. That's the real cost of your code.\n\n");

    free(big_array);

    printf("=============================================================================\n");
    printf(" Tutorial Complete! Thank you for checking out the project.\n");
    printf(" Feel free to ask any questions at ticuette@gmail.com.\n");
    printf(" You can support my work or check out other tools at willmanstoolbox.com\n");
    printf("=============================================================================\n");

    return 0;
}