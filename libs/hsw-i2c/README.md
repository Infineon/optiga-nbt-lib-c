# C Interface for I2C Drivers

This is a header-only C interface for reusable [I2C](https://en.wikipedia.org/wiki/I%C2%B2C) drivers. No actual implementation is provided rather developers can use the header to have the same code running on different platforms by linking to concrete implementations.

The interface is designed to be used in combination with the Infineon `hsw-protocol` library. To be fully usable as an I2C driver concrete implementations must also provide functions to initialize the `ifx_protocol_t` structs.

## Example

```c
#include "infineon/ifx-i2c.h"
#include "infineon/ifx-protocol.h"

// Protocol initialized depending on actual stack
ifx_protocol_t protocol;

// Set protocol parameters for I2C communication
ifx_i2c_set_clock_frequency(&protocol, 400000);
ifx_i2c_set_slave_address(&protocol, 0x10);

// Call protocol functionality like any other stack
uint8_t data[] = {0x00, 0xA4, 0x04, 0x00};
uint8_t *response = NULL;
size_t response_len = 0x00;
ifx_protocol_transceive(&protocol, data, sizeof(data), &response, &response_len);
```


## Mock Implementation for Unit testing

For developers' convenience a mock implementation of the library `Infineon::ifx-i2c-mock` is provided that can be consumed when using this project as a submodule (e.g. via [CPM](https://github.com/cpm-cmake/CPM.cmake)). This version performs basic parameter validation but besides this is a full NOOP implementation only suitable for unit testing.

It is only meant as a replacement for testing libraries relying on the interface functionality without the dependency to an actual implementation. Therefore it will not be installed when installing this project via cmake.
