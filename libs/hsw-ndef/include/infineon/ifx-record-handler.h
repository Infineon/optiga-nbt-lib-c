// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/ifx-record-handler.h
 * \brief NDEF record encoding/decoding utility.
 */
#ifndef IFX_RECORD_HANDLER_H
#define IFX_RECORD_HANDLER_H

#include <stddef.h>
#include <stdint.h>
#include "infineon/ifx-error.h"
#include "infineon/ifx-ndef-record.h"
#include "infineon/ifx-utils.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Macro definitions */
/* Definitions of function identifiers */

/**
 * \brief Identifier for NDEF record encode ID
 */
#define IFX_RECORD_HANDLER_ENCODE             UINT8_C(0x01)

/**
 * \brief Identifier for NDEF record decode ID
 */
#define IFX_RECORD_HANDLER_DECODE             UINT8_C(0x02)

/**
 * \brief Length of header field in bytes
 */
#define IFX_NDEF_HEADER_FIELD_LEN             UINT32_C(0x01)

/**
 * \brief Length of type length field in bytes
 */
#define IFX_NDEF_TYPE_FIELD_LEN               UINT32_C(0x01)

/**
 * \brief Length of id length field in bytes
 */
#define IFX_NDEF_ID_FIELD_LEN                 UINT32_C(0x01)

/**
 * \brief Length of short record payload length field in bytes
 */
#define IFX_NDEF_SR_PAYLOAD_LEN_FIELD_LEN     UINT32_C(0x01)

/**
 * \brief Length of payload length field in bytes
 */
#define IFX_NDEF_PAYLOAD_LEN_FIELD_LEN        UINT32_C(0x04)

/**
 * \brief Maximum payload length value of short record
 */
#define IFX_NDEF_SR_PAYLOAD_LEN_FIELD_MAX_LEN UINT32_C(0xFF)

/**
 * \brief Invalid id length
 */
#define IFX_NDEF_ID_LEN_FIELD_NONE            UINT32_C(0x00)

/**
 * \brief Length of empty ndef message
 */
#define IFX_NDEF_EMPTY_MESSAGE_LEN            UINT32_C(0x03)

/* Public functions */

/**
 * \brief Encodes the record bytes for the specific record type handle given as
 * input.
 * \param[in] handle            Pointer to the handle of specific record type.
 * \param[out] record_bytes     Pointer to the encoded record bytes.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If encoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 */
ifx_status_t record_handler_encode(const ifx_record_handle_t *handle,
                                   ifx_blob_t *record_bytes);

/**
 * \brief Decodes the record handles information from the given record bytes as
 * input.
 * \param[in] record_bytes      Pointer to the record bytes whose details to be
 * decoded to the record handle.
 * \param[out] handle           Pointer to the record handle where record
 * details are updated.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If decoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 */
ifx_status_t record_handler_decode(ifx_blob_t *record_bytes,
                                   ifx_record_handle_t *handle);

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* IFX_RECORD_HANDLER_H */
