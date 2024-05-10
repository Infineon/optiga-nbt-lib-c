# C Interface for Joinable Timers

This is a header-only `C` interface for `ifx_timer_t` objects. No actual implementation is provided rather developers can use the header to have the same code running on different platforms by linking to concrete implementations.

## Example

```c
#include "infineon/ifx-timer.h"

// Set a timer for 5 ms (5000us)
ifx_timer_t timer;
ifx_timer_set(&timer, 5 * 1000);

// Check if timer has elapsed
if (!ifx_timer_has_elapsed(&timer))
{
    // Wait for timer to finish
    ifx_timer_join(&timer);
}

// Clear timer and free resources
ifx_timer_destroy(&timer);
```

## Mock Implementation for unit testing

For developers' convenience a mock implementation of the library `Infineon::ifx-timer-mock` is provided that can be consumed when using this project as a submodule (e.g. via [CPM](https://github.com/cpm-cmake/CPM.cmake)). This version performs basic parameter validation but besides this is a full NOOP implementation only suitable for unit testing.

It is only meant as a replacement for testing libraries relying on the interface functionality without the dependency to an actual implementation. Therefore it will not be installed when installing this project via cmake.
