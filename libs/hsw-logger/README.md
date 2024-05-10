# Generic Logging Interface

This library offers a `C` interface for logging data.

It offers generic `ifx_logger_t` struct that can be populated by concrete implementations. These `ifx_logger_t` struct are self-contained and can therefore be nested, joined, etc.

The actual `ifx_logger_log()` function uses `printf` syntax so it should feel familiar to `C` developers.


## Example

```c
#include "infineon/ifx-logger.h"

// Log source information reusable across e.g. library
#define IFX_LOG_TAG "example"

// Logger can be populated by any concrete implementation
ifx_logger_t logger;

// No more implementation specific functionality
ifx_logger_set_level(&logger, IFX_LOG_INFO);
ifx_logger_log(&logger, IFX_LOG_TAG, IFX_LOG_WARN, "Something happened");
ifx_logger_log(&logger, IFX_LOG_TAG, IFX_LOG_INFO, "The answer to life, the universe, and everything is: %d", 42);
ifx_logger_destroy(&logger);
```

## Default Logger

Wherever possible it is recommended that libraries keep track of the used logger instance themselves (e.g. via `struct` member). This enables configurable log configurations / levels across library layers. Where keeping track of this logger is not possible a default logger can be set using `ifx_logger_set_default()`. This default logger is then available via `ifx_logger_default` that can be passed to any log function.

```c
#include <stdio.h>
#include "infineon/ifx-logger.h"
#include "infineon/ifx-logger-file.h"

ifx_logger_log(ifx_logger_default, "test", IFX_LOG_INFO, "This should NOT be logged");

// Update default logger
ifx_logger_t logger;
ifx_logger_file_initialize(&logger, stdout);
ifx_logger_set_level(&logger, IFX_LOG_INFO);
ifx_logger_set_default(&logger);

ifx_logger_log(ifx_logger_default, "test", IFX_LOG_INFO, "This SHOULD be logged");
```
