# libtrm

A lightweight, zero-dependency, single-header C library for Linux that reads detailed process memory metrics.

## Motivation
Standard memory profiling tools often rely on RSS (Resident Set Size). While useful as a baseline, RSS counts shared libraries multiple times across different processes. If five programs use the same 10MB standard library, RSS attributes that full 10MB to *each* program.

`libtrm` parses `/proc/self/smaps_rollup` and falls back to `/proc/self/smaps` on older kernels to give you a more nuanced and actionable view of your memory footprint:

* **USS (Unique Set Size):** Memory strictly private to your process. This is the amount of physical RAM the OS would actually reclaim if your process terminated.
* **PSS (Proportional Set Size):** Your USS plus a proportional share of loaded shared libraries.
* **RSS (Resident Set Size):** The standard resident memory metric, provided for comparison.

## Installation
`libtrm` uses the standard STB-style single-header design. Drop `libtrm.h` into your project.

In **exactly one** C file, define the implementation macro before including the header to compile the library logic:

```c
#define LIBTRM_IMPLEMENTATION
#include "libtrm.h"
```

In any other file where you need to call the API, just include the header normally:

```c
#include "libtrm.h"
```

## Quick Start
```c
#include <stdio.h>

#define LIBTRM_IMPLEMENTATION
#include "libtrm.h"

int main() {
    trm_memory_t mem;
    trm_result_t result = trm_get_memory(&mem);

    if (result == TRM_OK) {
        printf("Private RAM (USS): %zu KB\n", mem.uss_kb);
        printf("Proportional RAM (PSS): %zu KB\n", mem.pss_kb);
    } else {
        printf("Failed to read memory metrics (Code: %d)\n", result);
    }

    return 0;
}
```

## API Reference

### `trm_memory_t`
The data container populated by the library.
* `uss_kb` / `uss_bytes`: RAM uniquely bound to your process.
* `pss_kb` / `pss_bytes`: Your proportional footprint, accounting for shared library division.
* `rss_kb` / `rss_bytes`: The standard baseline memory metric.
* `shared_saved_kb` / `shared_saved_bytes`: A derived heuristic representing the physical RAM your process is saving because the OS is sharing library pages with other programs.

### `trm_result_t trm_get_memory(trm_memory_t* mem)`
The core function. Defensively zeroes out the provided struct before attempting to read data.

**Returns:**
* `TRM_OK (0)`: Success. All core metrics were successfully parsed.
* `TRM_ERR_NULL_PTR (-1)`: Null pointer provided.
* `TRM_ERR_IO (-2)`: Failed to open `/proc` files. This is expected on non-Linux systems or in highly restricted environments.
* `TRM_ERR_PARSE (-3)`: Files opened, but the parser failed to extract baseline metrics.
* `TRM_ERR_PARTIAL_DATA (-4)`: The kernel provided basic metrics but omitted the private page data required to calculate USS.

## Performance & Caveats
* **Context Switching:** Reading from the `/proc` virtual filesystem requires a kernel context switch and text parsing. **Do not call this function in a tight loop**. Use it periodically or triggered by debug events.
* **Kernel Dependence:** Memory reporting in Linux is inherently complex. These metrics are kernel-reported aggregates, and their availability can be influenced by containerization boundaries or kernel versions.

## Support & Contact
If you find any bugs, have any ideas to improve this, or just want to chat about C in general, feel free to email me.

**Email:** ticuette@gmail.com  
**Check out other tools at:** [willmanstoolbox.com](https://willmanstoolbox.com)
