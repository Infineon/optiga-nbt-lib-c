// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/ifx-record-alt-carrier.h
 * \brief Model interface to create alternative carrier record types and get/set
 * record fields.
 * \details Alternative Carrier Record is used within global
 * handover NDEF records to describe a single alternative carrier.
 */
#ifndef IFX_RECORD_ALT_CARRIER_H
#define IFX_RECORD_ALT_CARRIER_H

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
 * \brief Identifier for model set alternate carrier record ID
 */
#define IFX_RECORD_AC_SET            UINT8_C(0x01)

/**
 * \brief Identifier for model get alternate carrier record ID
 */
#define IFX_RECORD_AC_GET            UINT8_C(0x02)

/**
 * \brief Identifier for model alternative carrier record release memory
 */
#define IFX_RECORD_AC_RELEASE_MEMORY UINT8_C(0x03)

/**
 * \brief Type information for alternative carrier record 'ac'
 */
#define IFX_RECORD_AC_TYPE                                                     \
    {                                                                          \
        0x61, 0x63                                                             \
    }

/* enum definitions */

/**
 * \brief Enumeration defines the different carrier power states of alternative
 * carrier record
 */
typedef enum
{
    IFX_CPS_INACTIVE = 0x00,
    IFX_CPS_ACTIVE,
    IFX_CPS_ACTIVATING,
    IFX_CPS_UNKNOWN
} ifx_record_ac_cps;

/* struct definition */

/**
 * \brief Structure holds byte array of data and its length
 */
typedef struct
{
    /** \brief Array length in bytes */
    uint8_t data_length;

    /** \brief Array of data */
    uint8_t *data;
} ifx_record_data_ref_t;

/**
 * \brief Defines record details respective to alternative carrier record
 */
typedef struct
{
    /** Carrier power state of alternative carrier */
    ifx_record_ac_cps cps;

    /** Identifier for carrier technology */
    ifx_record_data_ref_t *carrier_data_ref;

    /** Count of auxiliary data references */
    uint8_t auxiliary_data_ref_count;

    /** Array of auxiliary data references that provides additional about
     * alternative carrier */
    ifx_record_data_ref_t **auxiliary_data_ref;
} ifx_record_ac_t;

/* public functions */

/**
 * \brief Creates an alternative carrier record and handle of the created
 * record. This handle holds the values of the record needed for encode and
 * decode operations.
 * \param[out] handle    Handle of the created alternative carrier record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If record creation operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation fails
 */
ifx_status_t ifx_record_ac_new(ifx_record_handle_t *handle);

/**
 * \brief Sets a carrier power state (CPS) in the alternative carrier record
 * details for the given record handle.
 * \param[out] handle           Pointer to the record handle obtained while
 * creating the record.
 * \param[in] cps               Carrier power state of the alternative carrier
 * record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_ac_set_cps(ifx_record_handle_t *handle,
                                   ifx_record_ac_cps cps);

/**
 * \brief Sets a carrier data reference in alternative carrier record details
 * for the given record handle.
 * \param[out] handle           Pointer to the record handle obtained while
 * creating the record.
 * \param[in] carrier_data_ref  Pointer to the carrier data reference.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation fails
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_ac_set_carrier_ref(
    ifx_record_handle_t *handle, const ifx_record_data_ref_t *carrier_data_ref);

/**
 * \brief Sets the list of auxiliary data references to alternative carrier
 * record for the given record handle.
 * \param[out] handle                    Pointer to the record handle obtained
 * while creating the record.
 * \param[in] auxiliary_data_ref         Pointer to the list of auxiliary data
 * references.
 * \param[in] auxiliary_data_ref_count   Count of auxiliary data references.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation fails
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_ac_set_aux_ref(
    ifx_record_handle_t *handle,
    const ifx_record_data_ref_t *auxiliary_data_ref,
    uint8_t auxiliary_data_ref_count);

/**
 * \brief Gets a carrier power state (CPS) of alternative carrier record details
 * from the given record handle.
 * \param[in] handle           Pointer to the record handle obtained while
 * creating the record.
 * \param[out] cps             Pointer to the carrier power state of alternative
 * carrier record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID, if record type is invalid
 */
ifx_status_t ifx_record_ac_get_cps(const ifx_record_handle_t *handle,
                                   ifx_record_ac_cps *cps);

/**
 * \brief Gets a carrier data reference of alternative carrier record details
 * from the given record handle.
 * \param[in] handle                Pointer to the record handle obtained while
 * creating the record.
 * \param[out] carrier_data_ref     Pointer to the carrier data reference.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation fails
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_ac_get_carrier_ref(
    const ifx_record_handle_t *handle, ifx_record_data_ref_t *carrier_data_ref);

/**
 * \brief Gets the list of auxiliary data references of alternative carrier
 * record from the given record handle.
 * \param[in] handle                        Pointer to the record handle
 * obtained while creating the record.
 * \param[out] auxiliary_data_ref           Pointer to the list of auxiliary
 * data references.
 * \param[out] auxiliary_data_ref_count     Pointer to the count of auxiliary
 * data references.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation fails
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_ac_get_aux_ref(
    const ifx_record_handle_t *handle,
    ifx_record_data_ref_t *auxiliary_data_ref,
    uint8_t *auxiliary_data_ref_count);

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* IFX_RECORD_ALT_CARRIER_H */
