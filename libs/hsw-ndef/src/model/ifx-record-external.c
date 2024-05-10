// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file model/ifx-record-external.c
 * \brief Model interface to create the external record types and get/set record
 * fields. \details For more details refer to technical specification document
 * for NFC Record Type Definition(NFCForum-TS-RTD_1.0)
 */
#include "ifx-record-handler-generic.h"
#include "infineon/ifx-ndef-errors.h"
#include "infineon/ifx-ndef-lib.h"
#include "infineon/ifx-record-external.h"
#include "infineon/ifx-record-handler.h"

/**
 * \brief Creates a new external record and handle of the created record.
 * This handle holds the values of the record needed for encode and decode
 * operations.
 * \param[out] handle    Handle of the created external record.
 * \param[in] type 		 Pointer to the type.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the record creation operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t ifx_record_ext_new(ifx_record_handle_t *handle,
                                const ifx_blob_t *type)
{
    ifx_status_t status = IFX_SUCCESS;

    if ((NULL != handle) && (NULL != type))
    {
        ifx_record_generic_t *external_record =
            (ifx_record_generic_t *) malloc(sizeof(ifx_record_generic_t));
        if (NULL != external_record)
        {
            handle->tnf = IFX_RECORD_TNF_TYPE_EXT;
            handle->type.length = 0x00;
            handle->type.buffer = NULL;
            status = record_handler_generic_set_type(handle, type);
            handle->id.buffer = NULL;
            handle->id.length = IFX_NDEF_ID_LEN_FIELD_NONE;
            handle->encode_record = record_handler_generic_encode;
            handle->decode_record = record_handler_generic_decode;
            handle->deinit_record = record_handler_generic_deinit;
            handle->record_data = (void *) external_record;
        }
        else
        {
            status = IFX_ERROR(IFX_RECORD_EXTERNAL, IFX_RECORD_EXT_NEW,
                               IFX_OUT_OF_MEMORY);
        }
    }
    else
    {
        status = IFX_ERROR(IFX_RECORD_EXTERNAL, IFX_RECORD_EXT_NEW,
                           IFX_ILLEGAL_ARGUMENT);
    }

    return status;
}

/**
 * \brief Sets the payload in the external record for the given record handle.
 * \param[out] handle       Pointer to the record handle.
 * \param[in] payload       Pointer to the payload.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t ifx_record_ext_set_payload(ifx_record_handle_t *handle,
                                        const ifx_blob_t *payload)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL != handle) && (NULL != payload))
    {
        ((ifx_record_generic_t *) handle->record_data)->payload =
            (ifx_blob_t *) malloc(sizeof(ifx_blob_t));
        if (NULL != ((ifx_record_generic_t *) handle->record_data)->payload)
        {
            ((ifx_record_generic_t *) handle->record_data)->payload->length =
                payload->length;
            ((ifx_record_generic_t *) handle->record_data)->payload->buffer =
                (uint8_t *) malloc(payload->length);
            if (NULL !=
                ((ifx_record_generic_t *) handle->record_data)->payload->buffer)
            {
                IFX_MEMCPY(((ifx_record_generic_t *) handle->record_data)
                               ->payload->buffer,
                           payload->buffer, payload->length);
            }
            else
            {
                IFX_FREE(
                    ((ifx_record_generic_t *) handle->record_data)->payload);
                ((ifx_record_generic_t *) handle->record_data)->payload = NULL;
                status = IFX_ERROR(IFX_RECORD_EXTERNAL, IFX_RECORD_EXT_SET,
                                   IFX_OUT_OF_MEMORY);
            }
        }
        else
        {
            status = IFX_ERROR(IFX_RECORD_EXTERNAL, IFX_RECORD_EXT_SET,
                               IFX_OUT_OF_MEMORY);
        }
    }
    else
    {
        status = IFX_ERROR(IFX_RECORD_EXTERNAL, IFX_RECORD_EXT_SET,
                           IFX_ILLEGAL_ARGUMENT);
    }

    return status;
}

/**
 * \brief Gets the payload from the external record for the given record handle.
 * \param[out] handle       Pointer to the record handle.
 * \param[in] payload       Pointer to the payload.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t ifx_record_ext_get_payload(const ifx_record_handle_t *handle,
                                        ifx_blob_t *payload)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL != payload) && (NULL != handle))
    {
        ifx_record_generic_t *payload_data =
            (ifx_record_generic_t *) handle->record_data;

        payload->length = payload_data->payload->length;
        payload->buffer = (uint8_t *) malloc(payload->length);
        if (NULL != payload->buffer)
        {
            IFX_MEMCPY(payload->buffer, payload_data->payload->buffer,
                       payload_data->payload->length);
        }
        else
        {
            status = IFX_ERROR(IFX_RECORD_EXTERNAL, IFX_RECORD_EXT_GET,
                               IFX_OUT_OF_MEMORY);
        }
    }
    else
    {
        status = IFX_ERROR(IFX_RECORD_EXTERNAL, IFX_RECORD_EXT_GET,
                           IFX_ILLEGAL_ARGUMENT);
    }

    return status;
}
