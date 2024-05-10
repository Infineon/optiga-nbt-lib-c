# APDU Library
This library offers utilities to encode and decode [APDU](https://en.wikipedia.org/wiki/Smart_card_application_protocol_data_unit) objects as well as their responses.

In smart card applications a host typically sends binary `APDU` data to a secure element and reads back a response. This binary data may be somewhat difficult to create and understand depending on the length of the data, etc. This library can be used to work with `APDU`s in a more generic and structured way.

## Example

```c
#include "infineon/ifx-apdu.h"
#include "infineon/ifx-protocol.h"

// Encode APDU so that it can be send to secure element
ifx_apdu_t apdu = {
  .cla = 0x01u,
  .ins = 0x02u,
  .p1 = 0x03u,
  .p2 = 0x04u,
  .lc = 0u,
  .data = NULL,
  .le = 0u
};
uint8_t *encoded;
size_t encoded_len;
ifx_apdu_encode(&apdu, &encoded, &encoded_len);

// Perform some interaction with a secure element (not of importance here)
uint8_t *response;
size_t response_len;
ifx_protocol_t protocol;

// Initialize the `ifx_protocol_t` object with the concrete implementation of driver.(Eg. I2C, SPI, etc ..)
// Code placeholder

ifx_protocol_activate(&protocol, NULL, NULL);
ifx_protocol_transceive(&protocol, encoded, encoded_len, &response, &response_len);

// Decode APDU response and check status
ifx_apdu_response_t decoded;
ifx_apdu_response_decode(&decoded, response, response_len);

if (decoded.sw != 0x9000u)
{
  // Error indicator set
}
```
