// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file include/ifx-record-handler-generic.h
 * \brief Generic Record encoding/decoding utility.
 */
#ifndef IFX_RECORD_HANDLER_GENERIC_H
#define IFX_RECORD_HANDLER_GENERIC_H

#include <stddef.h>
#include <stdint.h>
#include "infineon/ifx-ndef-record.h"
#include "infineon/ifx-record-external.h"
#include "infineon/ifx-utils.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Macro definitions */
/* Definitions of function identifiers */

/**
 * \brief Identifier for generic record encode ID
 */
#define IFX_RECORD_HANDLER_GEN_ENCODE     UINT8_C(0x01)

/**
 * \brief Identifier for generic record decode ID
 */
#define IFX_RECORD_HANDLER_GEN_DECODE     UINT8_C(0x02)

/**
 * \brief Identifier for generic set type ID
 */
#define IFX_RECORD_GEN_SET_TYPE_ID        UINT8_C(0x03)

/**
 * \brief Identifier for generic get type ID
 */
#define IFX_RECORD_GEN_GET_TYPE_ID        UINT8_C(0x04)

/**
 * \brief Identifier for generic record release memory
 */
#define IFX_RECORD_GENERIC_RELEASE_MEMORY UINT8_C(0x05)

/**
 * \brief Return maximum length for the type field
 */
#define IFX_RECORD_MAX_TYPE_LEN           UINT8_C(0xFF)

/**
 * \brief Encodes the generic record data into the payload.
 * \param[in] record_details     Pointer to the generic record data that was
 *                               updated in record handle.
 * \param[out] payload           Pointer to the payload byte array of generic
 * record.
 * \param[out] payload_length    Pointer to the payload length of generic
 * record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If encoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t record_handler_generic_encode(const void *record_details,
                                           uint8_t **payload,
                                           uint32_t *payload_length);

/**
 * \brief Decodes the NDEF record to the generic record details.
 * \param[in] payload           Pointer to the payload byte array of generic
 * record.
 * \param[in] payload_length    Pointer to the payload length of generic
 * record.
 * \param[out] record_details     Pointer to the generic record data that needs
 *                               to be updated from record handle.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If encoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t record_handler_generic_decode(const uint8_t *payload,
                                           uint32_t payload_length,
                                           void *record_details);

/**
 * \brief Sets the record type to the record handle.
 * \param[out] handle            Pointer to the record handle.
 * \param[in] type               Pointer to the record type.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If record type is set to handle successfully
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t record_handler_generic_set_type(ifx_record_handle_t *handle,
                                             const ifx_blob_t *type);

/**
 * \brief Gets the record type to the record handle.
 * \param[in] handle            Pointer to the record handle.
 * \param[out] type             Pointer to the record type.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If record type is read from handle successfully
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t record_handler_generic_get_type(const ifx_record_handle_t *handle,
                                             ifx_blob_t *type);

/**
 * @brief Release all the allocated memory for the created generic record data
 *
 * @param[in] record_data    data of the generic record
 * \retval IFX_SUCCESS If memory release operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 */
ifx_status_t record_handler_generic_deinit(void *record_data);

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* IFX_RECORD_HANDLER_GENERIC_H */
