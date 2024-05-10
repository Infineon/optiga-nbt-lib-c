// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/ifx-ndef-message.h
 * \brief NDEF message encoding/decoding utility.
 * \details For more details refer to technical specification document NFC Data
 * Exchange Format(NFCForum-TS-NDEF_1.0)
 */
#ifndef IFX_NDEF_MESSAGE_H
#define IFX_NDEF_MESSAGE_H

#include <stddef.h>
#include <stdint.h>
#include "infineon/ifx-utils.h"
#include "infineon/ifx-ndef-record.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Macro definitions */
/* Definitions of function identifiers */

/**
 * \brief Identifier for NDEF message encode ID
 */
#define IFX_NDEF_MESSAGE_ENCODE UINT8_C(0x01)

/**
 * \brief Identifier for NDEF message decode ID
 */
#define IFX_NDEF_MESSAGE_DECODE UINT8_C(0x02)

/**
 * \brief Empty ndef record data
 */
#define IFX_NDEF_MESSAGE_EMPTY  UINT8_C(0xD0)

/* public functions */

/**
 * \brief Encodes the array of the NDEF record handles into the NDEF message.
 * \param[in] record_handles Pointer to the array of NDEF record handles.
 * \param[in] number_of_records Number of NDEF records
 * \param[out] ndef_message Pointer to the object that stores the encoded NDEF
 * message.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If encoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t ifx_ndef_message_encode(const ifx_record_handle_t *record_handles,
                                     uint32_t number_of_records,
                                     ifx_blob_t *ndef_message);

/**
 * \brief Decodes the NDEF message buffer to the NDEF records array.
 * \param[in] ndef_message Pointer to the NDEF message
 * \param[out] number_of_records Pointer to the total number of records.
 * \param[out] record_handles Pointer to the array of decoded NDEF record
 * handles.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If decoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 */
ifx_status_t ifx_ndef_message_decode(const ifx_blob_t *ndef_message,
                                     uint32_t *number_of_records,
                                     ifx_record_handle_t *record_handles);

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* IFX_NDEF_MESSAGE_H */
