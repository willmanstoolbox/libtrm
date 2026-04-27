# libtrm 

A lightweight, zero-dependency, single-header C library for Linux that exposes your application's true memory footprint.

## What this solves
If you use standard tools to check how much RAM your C application is using, you are usually given the RSS (Resident Set Size). 

**RSS is a lie.** It counts shared libraries multiple times. If you have 5 programs running, they all share the same standard libraries in physical RAM, but RSS charges your program for the full size of that shared code. This makes monitoring your program's RAM usage inaccurate.

`libtrm` gives you the absolute truth by parsing three critical metrics:

* **PSS (Proportional Set Size):** Your fair share. If a 10MB library is shared by 5 apps, you are only charged 2MB.
* **USS (Unique Set Size):** This is the RAM that only you are using. If you kill your process, this is exactly how much memory the OS gets back.
* **RSS (Resident Set Size):** The standard metric, kept for comparison.

## Installation
Just drop `libtrm.h` into your project. Because it uses `static inline` linkage, you can just include it anywhere:

```c
#include "libtrm.h"
```

If you prefer the STB-style where the logic only lives in one file, define the implementation macro in exactly one C file:

```c
#define LIBTRM_IMPLEMENTATION
#include "libtrm.h"
```

That's it. You've got `libtrm` ready to go. Now you can use the API to read the true (or fake) RAM usage of your program at any time. You can also check how much memory shared libraries are saving you!

## API Reference

### `trm_memory_t`
The data container populated by the library.
* `uss_kb` / `uss_bytes` — RAM unique to your process.
* `pss_kb` / `pss_bytes` — Your true, proportional footprint.
* `rss_kb` / `rss_bytes` — The standard (bloated) memory metric.
* `shared_kb` / `shared_bytes` — The RAM you are saving by sharing with the OS.

### `trm_result_t trm_get_memory(trm_memory_t* mem)`
The core function. It defensively zeroes out your struct at the start, so you don't read garbage data if the call fails.

**Returns:**
* `TRM_OK (0)` — Success.
* `TRM_ERR_NULL_PTR (-1)` — You passed a NULL pointer.
* `TRM_ERR_IO (-2)` — Failed to read /proc files (Expected if you are not on Linux).
* `TRM_ERR_PARSE (-3)` — Files found, but the kernel isn't reporting data.

## See It In Action
* **main.c**: An interactive tutorial. It allocates 50MB and uses `libtrm` to show you how the kernel "lies" about allocation until you actually touch the memory.
* **errors_test.c**: A robustness suite. It proves the library handles NULL pointers and dirty stack memory without crashing.

## Support & Contact
If you find any bugs, have any ideas to improve this, or just want to chat about C in general, feel free to email me.

**Email:** ticuette@gmail.com  
**More Tools:** Check out other tools at [willmanstoolbox.com](https://willmanstoolbox.com)