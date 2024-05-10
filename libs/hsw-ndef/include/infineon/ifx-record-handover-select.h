// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/ifx-record-handover-select.h
 * \brief Model interface to create the handover select record types and set/get
 * record fields.
 */
#ifndef IFX_RECORD_HANDOVER_SELECT_H
#define IFX_RECORD_HANDOVER_SELECT_H

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
 * \brief Identifier for model set handover select record ID
 */
#define IFX_RECORD_HS_SET            UINT8_C(0x01)

/**
 * \brief Identifier for model get handover select record ID
 */
#define IFX_RECORD_HS_GET            UINT8_C(0x02)

/**
 * \brief Identifier for model handover select record release memory
 */
#define IFX_RECORD_HS_RELEASE_MEMORY UINT8_C(0x03)
/**
 * \brief Type information for URI record 'Hs'
 */
#define IFX_RECORD_HS_TYPE                                                     \
    {                                                                          \
        0x48, 0x73                                                             \
    }

/* Structure definitions */

/**
 * \brief This Structure holds the list of alternate carrier record handles and
 * a single error record, which is recognised as handover select's local records
 */
typedef struct
{
    /** List of alternative carrier records*/
    ifx_record_handle_t **alt_carrier_rec_list;

    /** Single error record */
    ifx_record_handle_t *error_record;
} ifx_local_record_handles_t;

/* public functions */

/**
 * \brief Creates a new handover select record and handle of the created record.
 * This handle holds the values of the record needed for encode and decode
 * operations.
 * \param[out] handle    Handle of the created select record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the record creation operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t ifx_record_hs_new(ifx_record_handle_t *handle);

/**
 * \brief Sets the major version in the handover select record details for the
 * given record handle.
 * \param[out] handle        Pointer to the record handle obtained while
 *                           creating the record.
 * \param[in] major_version  Major version field of the handover select record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_hs_set_major_ver(ifx_record_handle_t *handle,
                                         uint8_t major_version);

/**
 * \brief Sets the minor version in handover select record details for the given
 * record handle.
 * \param[out] handle        Pointer to the record handle obtained while
 *                           creating the record.
 * \param[in] minor_version  Minor version field of the handover select record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_hs_set_minor_ver(ifx_record_handle_t *handle,
                                         uint8_t minor_version);

/**
 * \brief Sets the list of local records (alternative carrier and error records)
 * to the handover select record for the given record handle.
 * \param[out] handle                   Pointer to the record handle obtained
 *                                      while creating the record.
 * \param[in] local_record_list         Pointer to the list of local records
 *                                      such as alternative carrier, error
 * records.
 * \param[in] count_of_local_records    Number of the local records.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t ifx_record_hs_set_local_records(
    ifx_record_handle_t *handle,
    const ifx_local_record_handles_t *local_record_list,
    uint32_t count_of_local_records);

/**
 * \brief Gets the major version of the handover select record from the given
 * record handle.
 * \param[in] handle            Pointer to the record handle obtained while
 *                              creating the record.
 * \param[out] major_version    Major version field of the handover select
 * record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_hs_get_major_ver(const ifx_record_handle_t *handle,
                                         uint8_t *major_version);

/**
 * \brief Gets the minor version of handover select record from the given record
 * handle.
 * \param[in] handle            Pointer to the record handle obtained while
 *                              creating the record.
 * \param[out] minor_version    Minor version field of the handover select
 * record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_hs_get_minor_ver(const ifx_record_handle_t *handle,
                                         uint8_t *minor_version);

/**
 * \brief Gets the list of local records from the given record handle.
 * \param[in] handle             Pointer to the record handle obtained while
 *                               creating the record.
 * \param[out] local_record_list Pointer to the list of local records such as
 *                               alternative carrier, error records.
 * \param[out] count_of_local_records   Number of the local records.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t ifx_record_hs_get_local_records(
    const ifx_record_handle_t *handle,
    ifx_local_record_handles_t *local_record_list,
    uint32_t *count_of_local_records);

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* IFX_RECORD_HANDOVER_SELECT_H */
