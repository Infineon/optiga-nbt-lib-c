# APDU Protocol Library
> Protocol Stack Extension for APDU Exchange

This library is built on top of the generic `hsw-protocol` library and offers an extended interface for exchanging [APDU](https://en.wikipedia.org/wiki/Smart_card_application_protocol_data_unit)s.

It can be used on top of any existing `ifx_protocol_t` stack to better fit the needs of interacting with secure elements.

## Example

```c
#include "infineon/ifx-apdu.h"
#include "infineon/ifx-apdu-protocol.h"
#include "infineon/ifx-protocol.h"

// Protocol can be populated by any concrete implementation
ifx_protocol_t protocol;

// Build and exchange APDUs
ifx_apdu_t apdu = {
  .cla = 0x00u,
  .ins = 0xA4u,
  .p1 = 0x04u,
  .p2 = 0x00u,
  .lc = 0u,
  .data = NULL,
  .le = 0u
};
ifx_apdu_response_t response;
ifx_apdu_protocol_transceive(&protocol, &apdu, &response);

// Verify response
if (response.sw != 0x9000u)
{
  // Handle error
}
```
