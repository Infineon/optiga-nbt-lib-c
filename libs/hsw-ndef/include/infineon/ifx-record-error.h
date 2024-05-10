// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/ifx-record-error.h
 * \brief Model interface to create error record types and get/set record
 * fields. \details This record is used only by the handover select and not
 * intended to use for any other records.
 */
#ifndef IFX_RECORD_ERROR_H
#define IFX_RECORD_ERROR_H

#include <stddef.h>
#include <stdint.h>
#include "infineon/ifx-error.h"
#include "infineon/ifx-utils.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Macro definitions */
/* Definitions of function identifiers */

/**
 * \brief Identifier for model set error record ID
 */
#define IFX_RECORD_ERROR_SET            UINT8_C(0x01)

/**
 * \brief Identifier for model get error record ID
 */
#define IFX_RECORD_ERROR_GET            UINT8_C(0x02)

/**
 * \brief Identifier for model error record release memory
 */
#define IFX_RECORD_ERROR_RELEASE_MEMORY UINT8_C(0x03)

/**
 * \brief Type information for error record 'err'
 */
#define IFX_RECORD_ERROR_TYPE                                                  \
    {                                                                          \
        0x65, 0x72, 0x72                                                       \
    }

/* Structure definitions */
/**
 * \brief Structure defines error record details such as error reason and data.
 */
typedef struct
{
    uint8_t error_reason; /**< Error reason that caused handover selector to
                             return the error record */

    ifx_blob_t
        *error; /**< Error data that provides information on the condition
               handover selector entered into erroneous state */
} ifx_record_error_t;

/* public functions */

/**
 * \brief Creates an error record and handle of the created record.
 * This handle holds the values of the record needed for encode and decode
 * operations.
 * \param[out] handle    Handle of the created error record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the record creation operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation fails
 */
ifx_status_t ifx_record_error_new(ifx_record_handle_t *handle);

/**
 * \brief Sets the error reason in the error record details for the given record
 * handle.
 * \param[out] handle      Pointer to the record handle obtained while creating
 *                         the record.
 * \param[in] error_reason Error reason that defines the failure cause of the
 *                         handover selector.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_error_set_reason(ifx_record_handle_t *handle,
                                         uint8_t error_reason);

/**
 * \brief Sets the error data in the error record details for the given record
 * handle.
 * \param[out] handle     Pointer to the record handle obtained while
 *                        creating the record.
 * \param[in] error  Pointer to error data.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_error_set_error_data(ifx_record_handle_t *handle,
                                             const ifx_blob_t *error);

/**
 * \brief Gets an error reason of an error record from the given record handle.
 * \param[in] handle        Pointer to the record handle obtained while
 *                          creating the record.
 * \param[out] error_reason Pointer to the error reason that defines
 *                          failure cause of the handover selector.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_error_get_reason(const ifx_record_handle_t *handle,
                                         uint8_t *error_reason);

/**
 * \brief Gets an error data of an error record from the given record handle.
 * \param[in] handle            Pointer to the record handle obtained while
 * creating the record.
 * \param[out] error       Pointer to error data.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t ifx_record_error_get_error_data(const ifx_record_handle_t *handle,
                                             ifx_blob_t *error);

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* IFX_RECORD_ERROR_H */
