// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/ifx-record-bp.h
 * \brief Model for the brand protection record.
 */
#ifndef IFX_RECORD_BP_H
#define IFX_RECORD_BP_H

#include "infineon/ifx-ndef-bp-lib.h"
#include "infineon/ifx-ndef-record.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Macro definitions */
/* Definitions of function identifiers */

/**
 * \brief Identifier for model set brand protection record ID.
 */
#define IFX_RECORD_BP_SET            UINT8_C(0x01)

/**
 * \brief Identifier for model get brand protection record ID.
 */
#define IFX_RECORD_BP_GET            UINT8_C(0x02)

/**
 * \brief Identifier for model new brand protection record ID.
 */
#define IFX_RECORD_BP_NEW            UINT8_C(0x03)

/**
 * \brief Identifier for model brand protection record release memory
 */
#define IFX_RECORD_BP_RELEASE_MEMORY UINT8_C(0x04)

/**
 * \brief Type length for brand protection.
 */
#define IFX_RECORD_BP_TYPE_LEN       UINT8_C(0x20)

/**
 * \brief Type of brand protection record: "infineon.com:nfc-bridge-tag.x509".
 */
#define IFX_RECORD_BP_TYPE                                                     \
    {0x69, 0x6E, 0x66, 0x69, 0x6E, 0x65, 0x6F, 0x6E, 0x2E, 0x63, 0x6F,         \
     0x6D, 0x3A, 0x6E, 0x66, 0x63, 0x2D, 0x62, 0x72, 0x69, 0x64, 0x67,         \
     0x65, 0x2D, 0x74, 0x61, 0x67, 0x2E, 0x78, 0x35, 0x30, 0x39};

/**
 * \brief Error ID for undefined certificate handler error.
 */
#define IFX_BP_CERT_HANDLERS_NOT_DEFINED UINT8_C(0x01)

/**
 * \brief Function prototype declaration for certificate encoding function.
 */
typedef ifx_status_t (*ifx_record_bp_cert_encoder_t)(const void *,
                                                     ifx_blob_t **);

/**
 * \brief Function prototype declaration for certificate decoding function.
 */
typedef ifx_status_t (*ifx_record_bp_cert_decoder_t)(const ifx_blob_t *,
                                                     void *);

/**
 * \brief Creates a new brand protection record and the respective handle for
 * the record. This handle can be used for encoding and decoding operations.
 *
 * \param[out] handle   Handle of the created brand protection record
 * \return ifx_status_t
 * \retval IFX_SUCCESS If new record creation is successful
 * \retval IFX_ILLEGAL_ARGUMENT In case of NULL init_handler
 * \retval IFX_OUT_OF_MEMORY If memory allocation fails
 */
ifx_status_t ifx_record_bp_new(ifx_record_handle_t *handle);

/**
 * \brief Registers the brand protection record with the NDEF library. Only the
 * registered records are encoded and decoded by the NDEF library.
 *
 * \return ifx_status_t
 * \retval IFX_SUCCESS If init_handler is registered properly to the record
 * init_handler list
 * \retval IFX_RECORD_INFO_ALREADY_REGISTERED If the same record type is already
 * registered
 * \retval IFX_ILLEGAL_ARGUMENT In case of NULL init_handler
 * \retval IFX_OUT_OF_MEMORY If memory allocation fails
 */
ifx_status_t ifx_record_bp_register(void);

/**
 * \brief Sets the certificate encoder and decoder callback functions for
 * parsing certificates.
 *
 * \param[out] handle       Pointer to the record handle
 * \param[in] encoder       Callback function to encode certificate
 * \param[in] decoder       Callback function to decode certificate
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the certificate parser set operation is successful
 * \retval NDEF_ERROR_INVALID_RECORD If the record handle is not a brand
 * protection type
 * \retval IFX_ILLEGAL_ARGUMENT If input parameters are NULL or invalid
 */
ifx_status_t ifx_record_bp_set_certificate_handlers(
    ifx_record_handle_t *handle, ifx_record_bp_cert_encoder_t encoder,
    ifx_record_bp_cert_decoder_t decoder);

/**
 * \brief Sets certificate in the brand protection record for a given record
 * handle.
 *
 * \param[out] handle       Pointer to record handle
 * \param[in] certificate   Pointer to certificate
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the certificate set operation is successful
 * \retval NDEF_ERROR_INVALID_RECORD If the record handle is not a brand
 * protection type
 * \retval IFX_ILLEGAL_ARGUMENT If input parameters are NULL or invalid
 */
ifx_status_t ifx_record_bp_set_certificate(ifx_record_handle_t *handle,
                                           const void *certificate);

/**
 * \brief Gets the certificate from the brand protection record handle.
 *
 * \param[in] handle       Pointer to the record handle
 * \param[out] certificate   Pointer to the certificate
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the certificate get operation is successful
 * \retval NDEF_ERROR_INVALID_RECORD If the record handle is not a brand
 * protection type
 * \retval IFX_ILLEGAL_ARGUMENT If input parameters are NULL or invalid
 */
ifx_status_t ifx_record_bp_get_certificate(const ifx_record_handle_t *handle,
                                           void *certificate);

/**
 * \brief Sets the payload in brand protection record handle.
 *
 * \param[in,out] handle   Pointer to the record handle
 * \param[in] payload      Pointer to the payload
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If input parameters are NULL or invalid
 * \retval IFX_OUT_OF_MEMORY If memory allocation fails
 */
ifx_status_t ifx_record_bp_set_payload(ifx_record_handle_t *handle,
                                       const ifx_blob_t *payload);

/**
 * \brief Gets the payload from the brand protection record handle.
 *
 * \param[in] handle       Pointer to the record handle
 * \param[out] payload     Pointer to the payload
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If input parameters are NULL or invalid
 * \retval IFX_OUT_OF_MEMORY If memory allocation fails
 */
ifx_status_t ifx_record_bp_get_payload(const ifx_record_handle_t *handle,
                                       ifx_blob_t *payload);

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* IFX_RECORD_BP_H */
