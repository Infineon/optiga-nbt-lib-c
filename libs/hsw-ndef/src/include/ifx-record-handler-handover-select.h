// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file include/ifx-record-handler-handover-select.h
 * \brief Handover Select Record encoding/decoding utility.
 * \details For more details refer to technical specification document for
 * Connection Handover(Version 1.3) 2014-01-16
 */
#ifndef IFX_RECORD_HANDLER_HANDOVER_SELECT_H
#define IFX_RECORD_HANDLER_HANDOVER_SELECT_H

#include <stddef.h>
#include <stdint.h>
#include "infineon/ifx-ndef-record.h"
#include "infineon/ifx-utils.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Macro definitions */
/* Definitions of function identifiers */

/**
 * \brief Identifier for handover select record encode ID
 */
#define IFX_RECORD_HANDLER_HS_ENCODE UINT8_C(0x01)

/**
 *  \brief Identifier for handover select record decode ID
 */
#define IFX_RECORD_HANDLER_HS_DECODE UINT8_C(0x02)

/**
 * \brief Function prototype declaration for local record encode function
 */
typedef ifx_status_t (*ifx_hs_local_record_encoder_t)(
    const ifx_record_handle_t *record_handles, uint32_t number_of_records,
    ifx_blob_t *ndef_message);

/**
 * \brief Function prototype declaration for local record decode function
 */
typedef ifx_status_t (*ifx_hs_local_record_decoder_t)(
    const ifx_blob_t *ndef_message, uint32_t *number_of_records,
    ifx_record_handle_t *record_handles);

/* Structure definitions */

/**
 * \brief Structure defines handover select record details such as major, minor
 * versions and local record list for encoding & decoding.
 */
typedef struct
{
    uint8_t major_version; /**< Major version */

    uint8_t minor_version; /**< Minor version */

    ifx_record_handle_t *
        *local_record_list; /**< Pointer to local records list which contains
                               array of local record handles*/

    uint32_t count_of_local_records; /**< Count of local records*/

    ifx_hs_local_record_encoder_t
        local_record_encode; /**< Function mapper for local record encoding*/

    ifx_hs_local_record_decoder_t
        local_record_decode; /**< Function mapper for local record decoding*/
} ifx_record_hs_t;

/* public functions */

/**
 * \brief Encodes the handover select record data to the payload bytes.
 * \param[in]   record_details      Pointer to the handover select record data
 *                                  that was updated in the record handle.
 * \param[out]  payload             Pointer to the payload byte array of
 * handover select record.
 * \param[out]  payload_length      Pointer to the payload length of handover
 * select record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If encoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t record_handler_hs_encode(const void *record_details,
                                      uint8_t **payload,
                                      uint32_t *payload_length);

/**
 * \brief Decodes the record payload bytes to the handover select record
 * details.
 * \param[in]   payload             Pointer to the payload byte array of
 * handover select record.
 * \param[in]   payload_length      Pointer to the payload length of handover
 * select record.
 * \param[out]  record_details      Pointer to the handover select record data
 *                                  where the decoded payload will be
 * \return ifx_status_t
 * \retval IFX_SUCCESS If decoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t record_handler_hs_decode(const uint8_t *payload,
                                      uint32_t payload_length,
                                      void *record_details);

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* IFX_RECORD_HANDLER_HANDOVER_SELECT_H */
