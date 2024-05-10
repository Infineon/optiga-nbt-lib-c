// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/ifx-tlv.h
 * \brief provides the function implementation for TLV(tag length value) DGI
 * encoder and decoder.
 */
#ifndef IFX_TLV_H
#define IFX_TLV_H

#include "ifx-utils-lib.h"
#include "ifx-utils.h"
#include "infineon/ifx-error.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Function identifier */

/**
 * \brief Function identifier of simple TLV encoder DGI for 1 byte & 3 byte
 * TLV length field size
 */
#define IFX_TLV_DGI_ENCODER               UINT8_C(0x01)

/**
 * \brief Function identifier of simple TLV decoder DGI for 1 byte & 3 byte
 * TLV length field size
 */
#define IFX_TLV_DGI_DECODER               UINT8_C(0x02)

/**
 * \brief Its identifier byte, followed by 2 bytes length in encoded bytes
 */
#define IFX_TLV_DGI_2B_LEN_IDENTIFIER     UINT8_C(0xFF)

/**
 * \brief Size of TLV identifier for 2B length size
 */
#define IFX_TLV_DGI_LEN_IDENTIFIER_SIZE   UINT8_C(0x01)

/**
 * \brief Minimum size of TLV length field (1 or 3 bytes)
 */
#define IFX_TLV_DGI_MIN_LEN_SIZE          UINT8_C(0x01)

/**
 * \brief Size of TLV tag field
 */
#define IFX_TLV_DGI_TAG_SIZE              UINT8_C(0x02)

/**
 * \brief Size of TLV length field of 1 byte
 */
#define IFX_TLV_DGI_LEN_SIZE_1B           UINT8_C(0x01)

/**
 * \brief Size of TLV length field of 2 bytes
 */
#define IFX_TLV_DGI_LEN_SIZE_2B           UINT8_C(0x02)

/**
 * \brief Size of TLV length field, when TLV length value is greater than 254
 * bytes
 */
#define IFX_TLV_DGI_LEN_WITH_ID_SIZE      UINT8_C(0x03)

/**
 * \brief Return error status if encoded byte array is invalid
 */
#define IFX_TLV_DGI_ENCODED_BYTES_INVALID UINT8_C(0xF9)

/**
 * \brief Data storage for TLV(tag, length & value) fields
 * TLV value field can be NULL.
 */
typedef struct
{
    /** Tag of 2 bytes */
    uint16_t tag;

    /** Pointer for store length of data stream */
    uint32_t length;

    /** Pointer to byte array which contains the data stream */
    uint8_t *value;
} ifx_tlv_t;

/**
 * \brief Encodes the array of TLV type data into byte array.
 * This method encodes in DGI TLV format, which is similar to a simple TLV
 * format but with 2 bytes TAG.
 * TAG (2 bytes)
 * LENGTH (1 or 3 bytes): If value >=255, 0xFF followed by 2 bytes length.
 * Otherwise 1 byte.
 * VALUE (0 or more bytes)
 *
 * \param[in] tlv_data              pointer to array of TLV type data
 * \param[in] tlv_data_count         number of TLV type data
 * \param[out] encoded_bytes        pointer to ifx_blob_t type encoded byte
 * array
 * \return ifx_status_t             IFX_SUCCESS, if successful otherwise error
 * information in case of error error status IFX_ILLEGAL_ARGUMENT, if parameter
 * is NULL or illegal error status IFX_OUT_OF_MEMORY, if memory allocation is
 * invalid
 */
ifx_status_t ifx_tlv_dgi_encode(const ifx_tlv_t *tlv_data,
                                uint32_t tlv_data_count,
                                ifx_blob_t *encoded_bytes);

/**
 * \brief Decodes the byte array into array of TLV type data.
 * This method decodes DGI TLV formatted data, which is similar to a simple TLV
 * format but with 2 bytes TAG.
 * TAG (2 bytes)
 * LENGTH (1 or 3 bytes): If value>=255, 0xFF followed by 2 bytes length.
 * Otherwise 1 byte.
 * VALUE (0 or more bytes)
 *
 * \param[in] encoded_bytes         pointer to ifx_blob_t type encoded byte
 * array
 * \param[out] tlv_data_count        pointer to number of TLV type data
 * \param[out] tlv_data             pointer to array of TLV type data, memory
 * should be allocated before passing with this function
 * \return ifx_status_t             IFX_SUCCESS, if successful otherwise error
 * information in case of error error status IFX_ILLEGAL_ARGUMENT, if parameter
 * is NULL or illegal error status IFX_OUT_OF_MEMORY, if memory allocation is
 * invalid error status IFX_TLV_DGI_ENCODED_BYTES_INVALID, if encoded bytes
 * contain invalid TLV structure
 */
ifx_status_t ifx_tlv_dgi_decode(const ifx_blob_t *encoded_bytes,
                                uint32_t *tlv_data_count, ifx_tlv_t *tlv_data);
/**
 * \note Externally allocate enough memory for TLV type(output parameter) array
 * to decode encoded bytes before using ifx_tlv_dgi_decode API.
 */

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* IFX_TLV_H */
