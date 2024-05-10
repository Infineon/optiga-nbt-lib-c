// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/records/ifx-record-handler-alt-carrier.h
 * \brief Alternative carrier record encoding/decoding utility.
 * \details The alternative carrier record is used within global handover NDEF
 * records to describe a single alternative carrier. For more details refer to
 * technical specification document NFC data exchange format
 * (NFCForum-TS-NDEF_1.0)
 */
#ifndef IFX_RECORD_HANDLER_ALT_CARRIER_H
#define IFX_RECORD_HANDLER_ALT_CARRIER_H

#include <stddef.h>
#include <stdint.h>
#include "infineon/ifx-error.h"
#include "infineon/ifx-record-alt-carrier.h"
#include "infineon/ifx-utils.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Macro definitions */
/* Definitions of function identifiers */

/**
 * \brief Identifier for ALT carrier record encode ID
 */
#define IFX_RECORD_HANDLER_AC_ENCODE UINT8_C(0x01)

/**
 * \brief Identifier for ALT carrier record decode ID
 */
#define IFX_RECORD_HANDLER_AC_DECODE UINT8_C(0x02)

/* Public functions */
/**
 * \brief Encodes the alternative carrier record data to record the payload
 * bytes.
 * \param[in]   record_details      Pointer to the alternative carrier record
 *                                  data that was updated in record handle.
 * \param[out]  payload             Pointer to the payload byte array of
 * alternative carrier record.
 * \param[out]  payload_length      Pointer to the payload length of
 * alternative carrier record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If encoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t record_handler_ac_encode(const void *record_details,
                                      uint8_t **payload,
                                      uint32_t *payload_length);

/**
 * \brief Decodes the record payload bytes to the error record details.
 * \param[in]   payload             Pointer to the payload byte array of the
 *                                  alternative carrier record.
 * \param[in]   payload_length      Payload length of the alternative carrier
 * record.
 * \param[out]  record_details      Pointer to the alternative carrier record
 *                                  data where the decoded payload will be
 * copied.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If decoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 * \retval IFX_UNSPECIFIED_ERROR If unspecified error came
 */
ifx_status_t record_handler_ac_decode(const uint8_t *payload,
                                      uint32_t payload_length,
                                      void *record_details);

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* IFX_RECORD_HANDLER_ALT_CARRIER_H */
