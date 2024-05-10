# Utilities

This library offers general utilities used by the different Infineon host software libraries.This library offers general utilities used by the different Infineon host software libraries.

## Example
- Example code for concatenating two `ifx_blob_t` type data structures.
  ```c
  #include "infineon/ifx-error.h"
  #include "infineon/ifx-utils.h"
  ifx_blob_t append_data, input_data;

  uint8_t append_bytes[] = {0x05, 0x04, 0x03, 0x02, 0x01};
  uint8_t input_bytes[] = {0x01, 0x02, 0x03, 0x04, 0x05};

  input_data.buffer = input_bytes;
  input_data.length = sizeof(input_bytes);

  input_data.buffer = append_bytes;
  input_data.length = sizeof(append_bytes);

  // Function call to concatenation of two ifx_blob_t type data structure.
  ifx_status_t status =  ifx_utils_concat(&append_bytes, &input_bytes);
  if(status == IFX_SUCCESS)
  {
    // ifx_blob_t type input data append_bytes to be appended in the end of ifx_blob_t type input_bytes.
    // input_bytes.length = sizeof(input_bytes) + sizeof(append_bytes)
    // input_bytes.buffer[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x05, 0x04, 0x03, 0x02, 0x01}
  }
  ```

- Example code for TLV encoding and decoding.

  ```c
  #include "infineon/ifx-error.h"
  #include "infineon/ifx-tlv.h"
  #include "infineon/ifx-utils.h"

  ifx_status_t status;
  uint8_t tlv_value_0[] = {0x01, 0x02, 0x03, 0x04, 0x05};
  uint8_t tlv_value_1[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
  uint32_t tlv_count = 2;

  ifx_tlv_t encode_tlv[2], decode_tlv[2];
  encode_tlv[0].tag = 0x1234;
  encode_tlv[0].length = sizeof(tlv_value_0);
  encode_tlv[0].value = tlv_value_0;

  encode_tlv[1].tag = 0x1234;
  encode_tlv[1].length = sizeof(tlv_value_1);
  encode_tlv[1].value = tlv_value_1;

  ifx_blob_t encoded_bytes;
  status = ifx_tlv_dgi_encode(&encode_tlv[0], tlv_count, &encoded_bytes);
  if(status == IFX_SUCCESS)
  {
    status = ifx_tlv_dgi_decode(&encoded_bytes, &tlv_count, decode_tlv);
    if(status == IFX_SUCCESS)
    {
      // Encoded TLV bytes decoded successfully.
      // encode_tlv and decode_tlv to be same
    }
  }
  ```
