# Global Platform T=1' Library
> C library for Global Platform T=1' communication

This library implements the Global Platform T=1' protocol as a reusable layer for generic `Protocol` stacks.

As the protocol only works over [I2C](https://en.wikipedia.org/wiki/I%C2%B2C) or [SPI](https://en.wikipedia.org/wiki/Serial_Peripheral_Interface) a concrete driver implementation of the Infineon `hsw-i2c` or `hsw-spi` and `hsw-timer` interface is required when consuming this library.

## Enable I2C/SPI interface
- If not using `cmake` to build the code, following macros need to be defined for compilation.
  Define the `IFX_T1PRIME_USE_I2C` and `IFX_T1PRIME_INTERFACE_I2C` macro with a *true* value (e.g. `1`) when compiling your code (this enables the build for the `I2C` variant of the T=1' protocol).

  ```c
  #define IFX_T1PRIME_USE_I2C 1
  #define IFX_T1PRIME_INTERFACE_I2C 1
  ```

- If using `cmake` to build the code.
  ```
  // Set IFX_T1PRIME_USE_I2C from CmakeLists.txt file.
  set(IFX_T1PRIME_USE_I2C ON)
  ```
  or, during `cmake` build, we can use
  ```
  cmake -DIFX_T1PRIME_USE_I2C=0N ..
  ```

  > Note: The `SPI` interface is enabled by setting `IFX_T1PRIME_USE_I2C=OFF`, which will not define `IFX_T1PRIME_INTERFACE_I2C`. By default `IFX_T1PRIME_USE_I2C=ON` is enabled from CmakeLists.txt file.

## Platform specific interface implementations
- `hsw-i2c` or `hsw-spi`:

  `hsw-i2c` or `hsw-spi` interface is used to abstract the I2C/SPI-specific communication functionality from the target platform. This interface is built on the `hsw-protocol` APIs. To implement concrete implementation of I2C/SPI driver, (at least) the following functions need to be provided. These functions shall mainly wrap the target MCU's I2C/SPI functions that can be used in hsw-protocol library.
  * A function to initialize the `ifx_protocol_t` object with the concrete implementation (e.g. `ifx_i2c_psoc6_initialize()`).This function sets up all members of the provided `ifx_protocol_t` struct and makes it usable in a generic protocol stack. This function shall do the platform-specific I2C/SPI driver initialization.

  * A function to transmit data from the host MCU to the secure element via `I2C/SPI` (e.g. `ifx_i2c_psoc6_transmit()`).

  * A function to receive data from secure element to host MCU via `I2C/SPI` (e.g. `ifx_i2c_psoc6_receive()`)

  For the I2C/SPI-specific functionality the getters and setters also need to be implemented. These shall be used to update the I2C/SPI driver's parameters.(e.g. for I2C -  frequency, slave address, or guard time). These getters and setters are defined in the `infineon/ifx-i2c.h` or  `infineon/ifx-spi.h` interface, which has to be included in the platform-specific implementation.

  For the SPI specific functionality the following getters and setters need to be implemented:
  * `ifx_spi_get_clock_frequency(ifx_protocol_t *self, uint32_t *frequency_hz_buffer)`
  * `ifx_spi_set_clock_frequency(ifx_protocol_t *self, uint32_t frequency_hz)`
  * `ifx_spi_get_clock_polarity(ifx_protocol_t *self, bool *cpol_buffer)`
  * `ifx_spi_set_clock_polarity(ifx_protocol_t *self, bool cpol)`
  * `ifx_spi_get_clock_phase(ifx_protocol_t *self, bool *cpha_buffer)`
  * `ifx_spi_set_clock_phase(ifx_protocol_t *self, bool cpha)`
  * `ifx_spi_get_buffer_size(ifx_protocol_t *self, size_t *buffer_size_buffer)`
  * `ifx_spi_set_buffer_size(ifx_protocol_t *self, size_t buffer_size)`
  * `ifx_spi_get_guard_time(ifx_protocol_t *self, uint32_t *guard_time_us_buffer)`
  * `ifx_spi_set_guard_time(ifx_protocol_t *self, uint32_t guard_time_us)`

  For the I2C specific functionality the following getters and setters need to be implemented:
  * `ifx_i2c_get_clock_frequency(ifx_protocol_t *self, uint32_t *frequency_buffer)`
  * `ifx_i2c_set_clock_frequency(ifx_protocol_t *self, uint32_t frequency)`
  * `ifx_i2c_get_slave_address(ifx_protocol_t *self, uint16_t *address_buffer)`
  * `ifx_i2c_set_slave_address(ifx_protocol_t *self, uint16_t address)`
  * `ifx_i2c_get_guard_time(ifx_protocol_t *self, uint32_t *guard_time_us_buffer)`
  * `ifx_i2c_set_guard_time(ifx_protocol_t *self, uint32_t guard_time_us)`

- `hsw-timer`:

  The `hsw-timer` interface is used to abstract timer calls from the target platform. Objects of the `ifx_timer_t` struct are used throughout all libraries to measure timeout values, waiting times, etc. For a concrete implementation of the timer interface, (at least) the following function(s) need to be provided. These functions shall mainly wrap the target MCU's timer functions which are defined in the hsw-timer interface.

  * A function to initialize the `ifx_timer_t` object with the concrete implementation (e.g. `ifx_timer_psoc6_initialize()`). This function sets up all members of the provided `ifx_timer_t` struct. This function shall do the platform-specific timer driver initialization.

  * `void *_start`: Pointer to start of the timer.

  * `uint64_t _duration`: Duration of the timer.

  For the timer-specific functionality, the following functions need to be implemented. These functions are defined in the `infineon/ifx-timer.h` interface, which has to be included in the platform-specific implementation.

  * `ifx_timer_set(ifx_timer_t *timer, uint64_t us)`: Sets a new timer for the amount of microseconds provided in `us` and stores all required information in its `timer` parameter.

  * `ifx_timer_has_elapsed(const ifx_timer_t *timer)`: Checks if the given `timer` has elapsed. Developers can use the members set by `ifx_timer_set()` to check if the timer has already elapsed.

  * `ifx_timer_join(const ifx_timer_t *timer)`: Waits for the given `timer` to elapse. Once again developers can use the data they previously set in `ifx_timer_set()` to calculate timer end times, etc.

  * `ifx_timer_destroy(ifx_timer_t *timer)`: If `ifx_timer_set()` allocated any dynamic data this function can be used to perform the necessary cleanup.

- `hsw-logger`:

  `hsw-logger` interface offers generic `ifx_logger_t` structs that can be populated by concrete implementations. These `ifx_logger_t` structs are self-contained and can therefore be nested, joined, etc. The actual logging function uses `printf` syntax so it should feel familiar to `C` developers. These loggers can be configured at runtime to a specific level to only trace the desired data (Debug, Info, Warning, Error). Concrete platform implementations are required to use the host MCU's preferred logging methods (e.g. stdout, file-logger). To provide concrete implementation of logger interface, (at least) the following function(s)need to be provided. These functions shall mainly wrap the target MCU's logging function which are used in the hsw-logger interface.

  * A function to initialize the `ifx_logger_t` object with the concrete implementation (e.g. `ifx_logger_psoc6_initialize()`). This function sets up all members of the provided `ifx_logger_t` struct. This function shall do the platform-specific logger  initialization.

  * `ifx_logger_log_callback_t _log`: Logging function for concrete implementation. This _log function shall be set to platform-specific implementation of the logging function, (e.g. printf, UART writing, etc., ).
  * `ifx_logger_set_level_callback_t _set_level`: Set log level. By default `ifx_logger_set_level()` will simply update `ifx_logger_t._level`. If no custom setter is required use `NULL`.
  * `ifx_logger_destroy_callback_t _destructor`: `ifx_logger_destroy()` will call `free()` for ifx_logger_t._data . If any further cleanup is necessary implement it in this function. Otherwise use `NULL`.
  * `ifx_log_level _level`: Set by `ifx_logger_set_level()`. This shall be initialized to 0.
  * `void *_data`: Logger properties/state that can be used internally. This shall be set to `NULL`, if there is no logger properties to be used.

  The actual logging functions are available in the `infineon/ifx-logger.h` interface.

  ```c
  #include "infineon/ifx-logger.h"
  ```

## Usage
Initialize the t1-prime protocol and call the protocol APIs for further protocol communications.

```c
/* Polling Mode */
#include "infineon/ifx-t1prime.h"
#include "infineon/ifx-protocol.h"

// Provide Protocol instance of SPI/I2C driver
ifx_protocol_t driver;

/* Initialize protocol driver layer here with I2C/SPI implementation.
Note: Does not work without initialized driver layer for I2C/SPI. */

// code placeholder

ifx_protocol_t protocol;

// Prerequisite: I2C/SPI and Timer concrete implementation to be integrated.
ifx_t1prime_initialize(&protocol, &driver);

uint8_t data[] = {0x00u, 0xa4u, 0x04u, 0x00u};
uint8_t *response = NULL;
size_t response_len = 0u;
ifx_protocol_activate(&protocol, &response, &response_len);
ifx_protocol_transceive(&protocol, data, sizeof(data), &response, &response_len);
ifx_protocol_destroy(&protocol);
```

### IRQ / Polling mode

The implementation supports both the T=1' interrupt as well as the polling mode to detect when the secure element is ready to send data. By default polling mode is used.

To enable interrupt mode use `ifx_t1prime_set_irq_handler()` to set a custom interrupt handler. This interrupt handler shall wait until the GPIO interrupt has triggered or the given amount of microseconds has elapsed.
It is up to the concrete implementation to check which GPIO pin to use and how to check if the time has elapsed.
This enables platform independent code with interrupt handling being an optional plug-in solution.

```c
/* IRQ Mode */
#include "infineon/ifx-error.h"
#include "infineon/ifx-protocol.h"
#include "infineon/ifx-t1prime.h"

/**
 * \brief Pseudo code implementation of interrupt handler
 */
ifx_status_t irq(ifx_protocol_t *self, uint32_t timeout)
{
  uint32_t end_time = now() + timeout;
  while (now() < end_time)
  {
    if (gpio_read() == GPIO_HIGH)
    {
      return IFX_T1PRIME_IRQ_TRIGGERED;
    }
  }
  return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_IRQ, IFX_T1PRIME_IRQ_NOT_TRIGGERED);
}

// Provide Protocol instance of SPI/I2C driver
ifx_protocol_t driver;

/* Initialize protocol driver layer here with I2C/SPI implementation.
Note: Does not work without initialized driver layer for I2C/SPI. */

// code placeholder

ifx_protocol_t protocol;

// Prerequisite: I2C/SPI and Timer concrete implementation to be integrated.
ifx_t1prime_initialize(&protocol, &driver);
ifx_t1prime_set_irq_handler(&protocol, &irq);

// The rest can be reused as any other protocol stack
uint8_t data[] = {0x00u, 0xa4u, 0x04u, 0x00u};
uint8_t *response = NULL;
size_t response_len = 0u;
ifx_protocol_activate(&protocol, &response, &response_len);
ifx_protocol_transceive(&protocol, data, sizeof(data), &response, &response_len);
ifx_protocol_destroy(&protocol);
```
### GP T=1' POR
The GP T=1' host library implements the proprietary Power On Reset(POR) with S-block(POR-Request) with PCB 1101 1000b (0xd8). This POR performs cold reset and does not replay a response. The Host Device shall wait for a duration of Power Wake-Up Time (PWT) according to the GP T=1' specification before initiating any communication with the Secure Element. Calling ifx_t1prime_s_por() will automatically wait for PWT after transmitting the S(POR) block. The function shall be called as shown below.

```c
/* POR usage */
#include "infineon/ifx-protocol.h"
#include "infineon/ifx-t1prime.h"

// Initialize protocol stack
ifx_protocol_t driver;

/* Initialize protocol driver layer here with I2C/SPI implementation.
Note: Does not work without initialized driver layer for I2C/SPI. */

// Code placeholder

ifx_protocol_t protocol;

// Prerequisite: I2C/SPI and Timer concrete implementation to be integrated.
ifx_t1prime_initialize(&protocol, &driver);

// The rest can be reused as any other protocol stack
ifx_protocol_activate(&protocol, &response, &response_len);
ifx_t1prime_s_por(&protocol);
ifx_protocol_destroy(&protocol);
```

## Components
* **t1-prime**
This component includes setting physical layer parameters for Global Platform T=1' protocol and function pointer implementations for hsw_protocol library. This components is responsible for framing and transceiving the APDUs as per GP T=1' protocol. The pre-requisite is that, this protocol library requires concrete implementation of physical layer protocol(I2C /SPI).

## Directory Structure
The library directory is structured according to the Pitchfork Layout.

```
hsw-t1prime
|-- .cmake/                 # Includes sources for dependency management
|-- LICENSES/               # Includes list of licenses used for the library
|-- data/                   # Includes Doxygen, cppcheck configuration files
|-- docs/                   # Includes documentation source files, images and the generated API reference
|-- include/                # Public Headers(.h) of the library
|-- src/                    # Sources(.c) and Private headers(.h) of the library
|-- .clang-format           # clang-format configuration file
|-- .gitignore              # Library specific gitignore file
|-- CMakeLists.txt          # Cmake build configurations for the library
`-- README.md               # Overview of the hsw-t1prime library
```

## Dependencies

* **hsw-crc**
  This dependent library is a generic library used for CRC calculation. This library is used for CRC calculation of GlobalPlatform T=1' protocol stack.

* **hsw-error**
  This dependent library is used for creating and parsing error information.

* **hsw-i2c**
  This interface is used for the I2C protocol implementation of the Global Platform T=1' protocol stack.
  It contains mock implementations of source code.

* **hsw-spi**
  This interface is used for the SPI protocol implementation of the Global Platform T=1' protocol stack.
  It contains mock implementations of source code.

* **hsw-logger**
  This dependent library is a generic abstraction interface used for logging information/errors. It offers generic interface that can be populated by concrete implementations, such as file logger or console logger.

* **hsw-protocol**
  This dependent library provides functionalities such as initialize, transceive and terminate to communicate with the device. It can be implemented for any communication protocol such as I2C, UART, etc.

* **hsw-timer**
  This dependent library provides the functionalities of `Timer` related APIs used in Global Platform T=1' protocol stack. It contains mock implementations of source code.

## References

* Global Platform Technology APDU Transport over SPI / I2C, Version 1.0
