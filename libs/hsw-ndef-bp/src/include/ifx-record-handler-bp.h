// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file include/ifx-record-handler-bp.h
 * \brief Brand protection record encoding/decoding utility.
 */
#ifndef IFX_RECORD_HANDLER_BP_H
#define IFX_RECORD_HANDLER_BP_H

#include "infineon/ifx-ndef-bp-lib.h"
#include "infineon/ifx-record-bp.h"
#include "infineon/ifx-record-handler.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Macro definitions */
/* Definitions of function identifiers */

/**
 * \brief Identifier for brand protection record encode ID.
 */
#define IFX_RECORD_HANDLER_BP_ENCODE UINT8_C(0x01)

/**
 * \brief Identifier for brand protection record decode ID.
 */
#define IFX_RECORD_HANDLER_BP_DECODE UINT8_C(0x02)

/**
 * \brief Defines a structure for the brand protection record details with
 * payload, certificate encoder and decoder.
 */
typedef struct
{
    /** Pointer to payload of the brand protection record*/
    ifx_blob_t *payload;

    /** Callback function for the certificate encoding function */
    ifx_record_bp_cert_encoder_t encoder;

    /** Callback function for the certificate decoding function */
    ifx_record_bp_cert_decoder_t decoder;
} ifx_record_bp_t;

/**
 * \brief Encodes the brand protection record data into payload bytes.
 *
 * \param[in]   record_details      Pointer to brand protection record data (
 * \ref ifx_record_bp_t )
 * \param[out]  payload             Pointer to payload byte array of the brand
 * protection record
 * \param[out]  payload_length      Pointer to payload length of the brand
 * protection record
 * \return ifx_status_t
 * \retval IFX_SUCCESS If encode operation is successful
 * \retval IFX_OUT_OF_MEMORY If memory allocation fails
 * \retval IFX_ILLEGAL_ARGUMENT If input parameters are NULL or invalid
 */
ifx_status_t record_handler_bp_encode(const void *record_details,
                                      uint8_t **payload,
                                      uint32_t *payload_length);

/**
 * \brief Decodes the record payload bytes to brand protection record details.
 *
 * \param[in]   payload             Pointer to payload byte array of the brand
 * protection record
 * \param[in]   payload_length      Pointer to payload length of the brand
 * \param[out]  record_details      Pointer to brand protection record data (
 * \ref ifx_record_bp_t ) where the decoded payload will be copied
 * protection record
 * \return ifx_status_t
 * \retval IFX_SUCCESS If decode operation is successful
 * \retval IFX_OUT_OF_MEMORY If memory allocation fails
 * \retval IFX_ILLEGAL_ARGUMENT If input parameters are NULL or invalid
 */
ifx_status_t record_handler_bp_decode(const uint8_t *payload,
                                      uint32_t payload_length,
                                      void *record_details);

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* IFX_RECORD_HANDLER_BP_H */
