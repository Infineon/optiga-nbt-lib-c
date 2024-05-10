// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/records/ifx-record-handler-error.h
 * \brief Error Record encoding/decoding utility.
 * This record is used only by handover select record and not intended to use
 * for any other records.
 * \details Error Record is included in a Handover Select
 * Record to indicate that the handover selector failed to successfully process
 * the most recently received handover request message. This record shall be
 * used only in Handover Select record. For more details refer to technical
 * specification document for Connection Handover(CH 1.4)
 */
#ifndef IFX_RECORD_HANDLER_ERROR_H
#define IFX_RECORD_HANDLER_ERROR_H

#include <stddef.h>
#include <stdint.h>
#include "infineon/ifx-error.h"
#include "infineon/ifx-ndef-record.h"
#include "infineon/ifx-record-error.h"
#include "infineon/ifx-utils.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Macro definitions */
/* Definitions of function identifiers */

/**
 * \brief Identifier for error record encode ID
 */
#define IFX_RECORD_HANDLER_ERROR_ENCODE UINT8_C(0x01)

/**
 * \brief Identifier for error record decode ID
 */
#define IFX_RECORD_HANDLER_ERROR_DECODE UINT8_C(0x02)

/* public functions */
/**
 * \brief Encodes the error record data details to the payload.
 * \param[in] record_details    Pointer to the error record details that was
 *                              updated in the record handle.
 * \param[out] payload          Pointer to the payload byte array of error
 * record.
 * \param[out] payload_length   Pointer to the payload length of error record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If encoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t record_handler_error_encode(const void *record_details,
                                         uint8_t **payload,
                                         uint32_t *payload_length);

/**
 * \brief Decodes the NDEF record payload to the error record details.
 * \param[in] payload           Pointer to the payload byte array of error
 * record.
 * \param[in] payload_length    Pointer to the payload length of error record.
 * \param[out] record_details     Pointer to the error record details that needs
 *                               to be updated from the record handle.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If decoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t record_handler_error_decode(const uint8_t *payload,
                                         uint32_t payload_length,
                                         void *record_details);

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* IFX_RECORD_HANDLER_ERROR_H */
