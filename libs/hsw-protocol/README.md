# Generic Protocol Stacks

This library offers a C interface for reusable protocol stacks similar to the [ISO/OSI Model](https://en.wikipedia.org/wiki/OSI_model).

It offers generic `ifx_protocol_t` structs that can be populated by concrete implementations. These `ifx_protocol_t` structs can be layered to achieve independence between them and have reusable components (e.g. I2C driver, Global Platform T=1', Global Platform SCP03, etc.).

## Example

```c
#include "infineon/ifx-protocol.h"

// Protocol can be populated by any concrete implementation
ifx_protocol_t protocol;

// The rest of the code can be used without any protocol specifics

// Activate communication channel to secure element
uint8_t *atr = NULL;
size_t atr_len = 0u;
ifx_protocol_activate(&protocol, &atr, &atr_len);
if (atr != NULL) free(atr);

// Exchange data with the secure element
uint8_t data[] = {0x00u, 0xa4u, 0x04u, 0x00u};
uint8_t *response = NULL;
size_t response_len = 0u;
ifx_protocol_transceive(&protocol, data, sizeof(data), &response, &response_len);
if (response != NULL) free(response);

// Perform cleanup of full protocol stack
ifx_protocol_destroy(&protocol);
```
