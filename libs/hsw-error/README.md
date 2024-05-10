# Infineon Error Library

This library offers utilities to create and decode C style `ifx_status_t` error codes.

In complex library chains it is often difficult to track exactly where errors come from.
All **Infineon** host software libraries therefore encode their errors according to the following schema:

| Bits    | Information                                                                                                                                     |
|---------|-------------------------------------------------------------------------------------------------------------------------------------------------|
| `31`    | Error indicator set to `1` in case of error                                                                                                     |
| `30-24` | Library identifier (e.g. `0x01` for `hsw-apdu` library)                                                                                         |
| `23-16` | Module identifier (e.g. `0x10` for `apdu-response` module)                                                                                      |
| `15-8`  | Function identifier in module (e.g. `0x03` for `ifx_apdu_response_decode` in `hsw-apdu` library)                                                |
| `7-0`   | Function-specific error reason (e.g. `0xFE` out of memory).<br>Values between 0xF0 and 0xFF are reserved for reusable error codes defined here. |

A return value of `0x811003FE` would therefore indicate:
  * Bit `31` is set -> **error**.
  * Bits `30-24` are set to `0x01` -> `hsw-apdu` library
  * Bits `23-16` are set to `0x10` -> `apdu-response` module
  * Bits `15-8` are set to `0x03` -> `ifx_apdu_response_decode` function
  * Bits `7-0` are set to `0xFE` -> **Out of memory**

## Example

```c
#include "infineon/ifx-error.h"
#include "infineon/ifx-apdu.h"

ifx_status_t error_code = IFX_ERROR(LIB_APDU, IFX_APDU_ENCODE, IFX_OUT_OF_MEMORY);
if (ifx_error_check(error_code))
{
  switch(ifx_error_get_module(error_code))
  {
  case LIB_APDU:
    switch (ifx_error_get_function(error_code))
    {
    case IFX_APDU_ENCODE:
      switch (ifx_error_get_reason(error_code))
      {
        case IFX_OUT_OF_MEMORY:
          printf("Out of memory during APDU encoding\n");
          break;
        default:
          printf("Unknown error during APDU encoding\n");
         break;
      }
      break;
    default:
      printf("Error in unknown APDU function\n");
      break;
    }
    break;
  default:
    printf("Error in unknown module\n");
    break;
  }
}
```

## Reusable Error Reasons

This library offers a set of reusable error reasons similar to standard exceptions in other languages.

The following table gives an overview of these values and a quick description for each.

| Value  | Name                    | Description                                                           |
|--------|-------------------------|-----------------------------------------------------------------------|
| `0xFF` | `IFX_UNSPECIFIED_ERROR` | Error without any further information                                 |
| `0xFE` | `IFX_OUT_OF_MEMORY`     | Could not allocate memory                                             |
| `0xFD` | `IFX_ILLEGAL_ARGUMENT`  | Illegal argument given to function call                               |
| `0xFC` | `IFX_TOO_LITTLE_DATA`   | Too little data available (e.g. in decoding functions)                |
| `0xFB` | `IFX_INVALID_STATE`     | Program (currently) in invalid state                                  |
| `0xFA` | `IFX_PROGRAMMING_ERROR` | Error that should have already been caught by the programmer occurred |
