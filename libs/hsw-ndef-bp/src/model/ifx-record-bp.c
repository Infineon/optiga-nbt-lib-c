// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file model/ifx-record-bp.c
 * \brief Model interface to create brand protection record types and set/get
 * record fields.
 */
#include <stdio.h>
#include <stdlib.h>
#include "ifx-record-handler-bp.h"
#include "infineon/ifx-ndef-errors.h"
#include "infineon/ifx-ndef-record.h"
#include "infineon/ifx-record-bp.h"
#include "infineon/ifx-record-handler.h"

/**
 * \brief Release all the allocated memory for the created brandprotection
 * record data
 *
 * \param[in] record_data    data of the error record
 * \retval IFX_SUCCESS If memory release operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 */
static ifx_status_t record_bp_deinit(void *record_data)
{
    if (NULL == record_data)
    {
        return IFX_ERROR(IFX_RECORD_BP, IFX_RECORD_BP_RELEASE_MEMORY,
                         IFX_ILLEGAL_ARGUMENT);
    }

    ifx_record_bp_t *bp_record = (ifx_record_bp_t *) (record_data);
    if (NULL != bp_record->payload->buffer)
    {
        IFX_FREE(bp_record->payload->buffer);
        bp_record->payload->buffer = NULL;
    }
    if (NULL != bp_record->payload)
    {
        IFX_FREE(bp_record->payload);
        bp_record->payload = NULL;
    }
    return IFX_SUCCESS;
}

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
ifx_status_t ifx_record_bp_new(ifx_record_handle_t *handle)
{
    if (NULL == handle)
    {
        return IFX_ERROR(IFX_RECORD_BP, IFX_RECORD_BP_NEW,
                         IFX_ILLEGAL_ARGUMENT);
    }

    ifx_record_bp_t *brandprotection_rec =
        (ifx_record_bp_t *) malloc(sizeof(ifx_record_bp_t));
    if (NULL == brandprotection_rec)
    {
        return IFX_ERROR(IFX_RECORD_BP, IFX_RECORD_BP_NEW, IFX_OUT_OF_MEMORY);
    }

    handle->tnf = IFX_RECORD_TNF_TYPE_EXT;
    handle->type.length = IFX_RECORD_BP_TYPE_LEN;
    handle->type.buffer = (uint8_t *) malloc(IFX_RECORD_BP_TYPE_LEN);
    if (NULL == handle->type.buffer)
    {
        IFX_FREE(brandprotection_rec);
        brandprotection_rec = NULL;
        return IFX_ERROR(IFX_RECORD_BP, IFX_RECORD_BP_NEW, IFX_OUT_OF_MEMORY);
    }

    const uint8_t type[] = IFX_RECORD_BP_TYPE;
    IFX_MEMCPY(handle->type.buffer, type, IFX_RECORD_BP_TYPE_LEN);

    handle->id.buffer = NULL;
    handle->id.length = IFX_NDEF_ID_LEN_FIELD_NONE;
    handle->encode_record = record_handler_bp_encode;
    handle->decode_record = record_handler_bp_decode;
    handle->deinit_record = record_bp_deinit;
    brandprotection_rec->encoder = NULL;
    brandprotection_rec->decoder = NULL;
    handle->record_data = (void *) brandprotection_rec;

    return IFX_SUCCESS;
}

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
ifx_status_t ifx_record_bp_register(void)
{
    ifx_status_t status = IFX_SUCCESS;
    const uint8_t type[] = IFX_RECORD_BP_TYPE;
    ifx_record_init_t bp_record_init_handler;
    IFX_MEMSET(&bp_record_init_handler, 0x00, sizeof(ifx_record_init_t));
    bp_record_init_handler.type = (uint8_t *) malloc(IFX_RECORD_BP_TYPE_LEN);
    if (NULL == bp_record_init_handler.type)
    {
        return IFX_ERROR(IFX_RECORD_BP, IFX_RECORD_BP_NEW, IFX_OUT_OF_MEMORY);
    }

    IFX_MEMCPY(bp_record_init_handler.type, type, IFX_RECORD_BP_TYPE_LEN);

    bp_record_init_handler.type_length = IFX_RECORD_BP_TYPE_LEN;
    bp_record_init_handler.get_handle = ifx_record_bp_new;

    status = ifx_ndef_record_register_handle(&bp_record_init_handler);
    IFX_FREE(bp_record_init_handler.type);
    bp_record_init_handler.type = NULL;

    return status;
}

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
    ifx_record_bp_cert_decoder_t decoder)
{
    if ((NULL == handle) || (NULL == encoder) || (NULL == decoder))
    {
        return IFX_ERROR(IFX_RECORD_BP, IFX_RECORD_BP_SET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    const uint8_t type[] = IFX_RECORD_BP_TYPE;
    if (IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
    {
        return IFX_ERROR(IFX_RECORD_BP, IFX_RECORD_BP_SET, IFX_RECORD_INVALID);
    }

    ((ifx_record_bp_t *) (handle->record_data))->encoder = encoder;
    ((ifx_record_bp_t *) (handle->record_data))->decoder = decoder;

    return IFX_SUCCESS;
}

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
                                           const void *certificate)
{
    ifx_status_t status = IFX_SUCCESS;

    if ((NULL == handle) || (NULL == certificate))
    {
        status =
            IFX_ERROR(IFX_RECORD_BP, IFX_RECORD_BP_SET, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        const uint8_t type[] = IFX_RECORD_BP_TYPE;
        if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
        {
            if (NULL != ((ifx_record_bp_t *) (handle->record_data))->encoder)
            {
                status =
                    ((ifx_record_bp_t *) (handle->record_data))
                        ->encoder(certificate,
                                  &(((ifx_record_bp_t *) (handle->record_data))
                                        ->payload));
            }
            else
            {
                status = IFX_ERROR(IFX_RECORD_BP, IFX_RECORD_BP_SET,
                                   IFX_BP_CERT_HANDLERS_NOT_DEFINED);
            }
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_BP, IFX_RECORD_BP_SET, IFX_RECORD_INVALID);
        }
    }

    return status;
}

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
                                           void *certificate)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == certificate))
    {
        status =
            IFX_ERROR(IFX_RECORD_BP, IFX_RECORD_BP_GET, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {

        const uint8_t type[] = IFX_RECORD_BP_TYPE;
        if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
        {
            if (NULL != ((ifx_record_bp_t *) (handle->record_data))->decoder)
            {
                status =
                    ((ifx_record_bp_t *) (handle->record_data))
                        ->decoder(((ifx_record_bp_t *) (handle->record_data))
                                      ->payload,
                                  certificate);
            }
            else
            {
                status = IFX_ERROR(IFX_RECORD_BP, IFX_RECORD_BP_GET,
                                   IFX_BP_CERT_HANDLERS_NOT_DEFINED);
            }
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_BP, IFX_RECORD_BP_GET, IFX_RECORD_INVALID);
        }
    }

    return status;
}

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
                                       const ifx_blob_t *payload)
{
    if (IFX_VALIDATE_NULL_PTR_BLOB(payload) || NULL == handle)
    {
        return IFX_ERROR(IFX_RECORD_BP, IFX_RECORD_BP_SET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    ((ifx_record_bp_t *) handle->record_data)->payload =
        (ifx_blob_t *) malloc(sizeof(ifx_blob_t));
    if (NULL == ((ifx_record_bp_t *) handle->record_data)->payload)
    {
        return IFX_ERROR(IFX_RECORD_BP, IFX_RECORD_BP_SET, IFX_OUT_OF_MEMORY);
    }

    ((ifx_record_bp_t *) handle->record_data)->payload->length =
        payload->length;
    ((ifx_record_bp_t *) handle->record_data)->payload->buffer =
        (uint8_t *) malloc(payload->length);
    if (NULL == ((ifx_record_bp_t *) handle->record_data)->payload->buffer)
    {
        IFX_FREE(((ifx_record_bp_t *) handle->record_data)->payload);
        ((ifx_record_bp_t *) handle->record_data)->payload = NULL;
        return IFX_ERROR(IFX_RECORD_BP, IFX_RECORD_BP_SET, IFX_OUT_OF_MEMORY);
    }

    IFX_MEMCPY(((ifx_record_bp_t *) handle->record_data)->payload->buffer,
               payload->buffer, payload->length);

    return IFX_SUCCESS;
}

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
                                       ifx_blob_t *payload)
{
    if (IFX_VALIDATE_NULL_PTR_MEMORY(handle) ||
        IFX_VALIDATE_NULL_PTR_MEMORY(payload))
    {
        return IFX_ERROR(IFX_RECORD_BP, IFX_RECORD_BP_GET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    ifx_record_bp_t *payload_data = (ifx_record_bp_t *) handle->record_data;

    payload->length = payload_data->payload->length;
    payload->buffer = (uint8_t *) malloc(payload->length);
    if (NULL == payload->buffer)
    {
        return IFX_ERROR(IFX_RECORD_BP, IFX_RECORD_BP_GET, IFX_OUT_OF_MEMORY);
    }

    IFX_MEMCPY(payload->buffer, payload_data->payload->buffer,
               payload_data->payload->length);

    return IFX_SUCCESS;
}
