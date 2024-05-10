// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/ifx-record-mime.h
 * \brief Model interface to create the MIME record types and set/get record
 * fields.
 */
#ifndef IFX_RECORD_MIME_H
#define IFX_RECORD_MIME_H

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
 * \brief Identifier for model new MIME record
 */
#define IFX_RECORD_MIME_NEW UINT8_C(0x01)

/**
 * \brief Identifier for model set MIME record ID
 */
#define IFX_RECORD_MIME_SET UINT8_C(0x02)

/**
 * \brief Identifier for model get MIME record ID
 */
#define IFX_RECORD_MIME_GET UINT8_C(0x03)

/**
 * \brief Creates a new Multipurpose Internet Mail Extensions (MIME) record and
 * handle of the created record. This handle holds the values of the record
 * needed for encode and decode operations.
 * \param[out] handle       Handle of the created MIME record.
 * \param[in] type 		    Pointer to the type.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the record creation operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t ifx_record_mime_new(ifx_record_handle_t *handle,
                                 const ifx_blob_t *type);

/**
 * \brief Sets the payload in the MIME record for the given record handle.
 * \param[out] handle       Pointer to the record handle.
 * \param[in] payload       Pointer to the payload.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t ifx_record_mime_set_payload(ifx_record_handle_t *handle,
                                         const ifx_blob_t *payload);

/**
 * \brief Gets the payload from the MIME record for the given record handle.
 * \param[out] handle       Pointer to the record handle.
 * \param[in] payload       Pointer to the payload.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t ifx_record_mime_get_payload(const ifx_record_handle_t *handle,
                                         ifx_blob_t *payload);

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* IFX_RECORD_MIME_H */
