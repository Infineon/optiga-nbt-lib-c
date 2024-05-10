// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file include/ifx-record-handler-uri.h
 * \brief URI Record encoding/decoding utility.
 * \details For more details refer to technical specification document for URI
 * Record Type Definition(NFCForum-TS-RTD_URI_1.0)
 */
#ifndef IFX_RECORD_HANDLER_URI_H
#define IFX_RECORD_HANDLER_URI_H

#include <stddef.h>
#include <stdint.h>
#include "infineon/ifx-record-handler.h"
#include "infineon/ifx-utils.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Macro definitions */
/* Definitions of function identifiers */

/**
 * \brief Identifier for URI record encode ID
 */
#define IFX_RECORD_HANDLER_URI_ENCODE         UINT8_C(0x01)

/**
 * \brief Identifier for URI record decode ID
 */
#define IFX_RECORD_HANDLER_URI_DECODE         UINT8_C(0x02)

/**
 * \brief Offset of the URI value field in the URI record payload
 */
#define IFX_RECORD_URI_VALUE_OFFSET           UINT8_C(0x01)

/**
 * \brief Offset of the URI identifier code field in the URI record payload
 */
#define IFX_RECORD_URI_IDENTIFIER_CODE_OFFSET UINT8_C(0x00)

/**
 * \brief Length of the URI identifier field in an URI record payload
 */
#define IFX_RECORD_URI_IDENTIFIER_SIZE        UINT8_C(0x01)

/**
 * \brief Size of the length field of the URI blob data type
 */
#define IFX_RECORD_URI_BLOB_LEN_SIZE          UINT8_C(0x04)

/* Structure definitions */

/**
 * \brief Structure defines URI record details such as identifier code and URI
 * data
 */
typedef struct
{
    uint8_t identifier_code; /**< URI record identifier code */
    ifx_blob_t *uri;         /**< URI data and its length */
} ifx_record_uri_t;

/* public functions */

/**
 * \brief Encodes the URI record data into the payload.
 * \param[in] record_details     Pointer to the URI record data that was updated
 *                               in record handle.
 * \param[out] payload           Pointer to the payload byte array of URI
 * record.
 * \param[out] payload_length    Pointer to the payload length of URI record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If encoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t record_handler_uri_encode(const void *record_details,
                                       uint8_t **payload,
                                       uint32_t *payload_length);

/**
 * \brief Decodes the NDEF record to the URI record details.
 * \param[in] payload               Pointer to the payload byte array of URI
 * record.
 * \param[in] payload_length        Pointer to the payload length of URI
 * record.
 * \param[out] record_details       Pointer to the URI record type fields of
 * handler.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If decoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t record_handler_uri_decode(const uint8_t *payload,
                                       uint32_t payload_length,
                                       void *record_details);

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* IFX_RECORD_HANDLER_URI_H */
