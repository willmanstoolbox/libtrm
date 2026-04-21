# libtrm 

A lightweight, zero-dependency, single-header C library for Linux that exposes your application's true memory footprint by reading the Proportional Set Size (PSS).

## What this solves:
If you use standard tools (like `top` or standard OS calls) to check how much RAM your C application is using, you are usually given the **RSS (Resident Set Size)**. 

**RSS is a lie.** It counts shared libraries multiple times. If you have 5 programs running, they all share the same standard libraries in physical RAM, but RSS charges *your* program for the full size of that shared code. This makes monitoring your program's RAM usage inaccurate.
**PSS (Proportional Set Size)** is the honest metric. If a shared library is 10MB and 5 apps are using it, PSS only credits your app for 2MB. `libtrm` is a lightning-fast custom parser that reads `/proc/self/smaps_rollup` (with a safe fallback to `smaps` for older kernels) to give you the absolute truth about your memory footprint.

## Installation

1. Drop `libtrm.h` into your project folder.
2. In **exactly one** C file, define the implementation macro *before* including the header:

#define LIBTRM_IMPLEMENTATION

#include "libtrm.h"

That's it. You got libtrm ready to go. Now you can use the API to read the true (or fake) RAM usage of your program at any particular time, anywhere in your code. You can also check how much memory shared libraries are saving you!

## API Reference

### `trm_memory_t`
The data container populated by the library.
* `pss_kb` / `pss_bytes` - Your true, proportional footprint.
* `rss_kb` / `rss_bytes` - The standard (bloated) memory metric.
* `shared_kb` / `shared_bytes` - The RAM you are saving by sharing with the OS (`rss - pss`).

### `int trm_get_memory(trm_memory_t* mem)`
The core function. Pass a pointer to your initialized struct. 
* Returns `0` on success.
* Returns `-1` on failure (this usually means you are not on Linux or you don't have read permissions, but if you find any edge cases feel free to email me or open an issue).

## See It In Action
To see exactly how to use the library and watch it catch Linux in a "lazy allocation" lie, compile and run the **`main.c`** file included in this repository. It serves as a fully interactive tutorial and stress test, do whatever you want with it.

## Support & Contact
If you find any bugs, have any ideas to improve this or just want to chat about C in general feel free to email me.

**Email:** ticuette@gmail.com

**More Tools:** You can support my work or check out other tools at [willmanstoolbox.com](https://willmanstoolbox.com)
