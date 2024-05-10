# CRC Implementations
This library offers reusable Cyclic Redundancy Check (CRC) implementations.

## Example

```c
#include "infineon/ifx-crc.h"

// Generate some data
uint8_t data[] = {0x01u, 0x02u, 0x03u, 0x04u};
size_t data_len = sizeof(data);

// Calculate different CRC values
uint16_t x25 = ifx_crc16_ccitt_x25(data, data_len);
uint16_t mcrf4xx = ifx_crc16_mcrf4xx(data, data_len);
```
