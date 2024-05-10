// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file ndef-record/record-types/ifx-record-handler-generic.c
 * \brief Generic record encoding/decoding utility.
 */
#include "ifx-record-handler-generic.h"
#include "infineon/ifx-ndef-lib.h"
#include "infineon/ifx-ndef-record.h"

/* Public functions */

/**
 * \brief Encodes the generic record data into the payload.
 * \param[in] record_details     Pointer to the generic record data that was
 *                               updated in record handle.
 * \param[out] payload           Pointer to the payload byte array of generic
 * record.
 * \param[out] payload_length    Pointer to the payload length of generic
 * record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If encoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t record_handler_generic_encode(const void *record_details,
                                           uint8_t **payload,
                                           uint32_t *payload_length)
{
    ifx_status_t status = IFX_SUCCESS;
    ifx_record_generic_t *generic_rec = (ifx_record_generic_t *) record_details;

    if ((NULL == generic_rec->payload) || (NULL == payload))
    {
        status = IFX_ERROR(IFX_RECORD_HANDLER_GENERIC,
                           IFX_RECORD_HANDLER_GEN_ENCODE, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        /* storing generic record payload data */
        *payload = (uint8_t *) malloc(generic_rec->payload->length);
        if (NULL != *payload)
        {
            IFX_MEMCPY(*payload, generic_rec->payload->buffer,
                       generic_rec->payload->length);
            *payload_length = generic_rec->payload->length;
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_HANDLER_GENERIC,
                          IFX_RECORD_HANDLER_GEN_ENCODE, IFX_OUT_OF_MEMORY);
        }
    }

    return status;
}

/**
 * \brief Decodes the NDEF record to the generic record details.
 * \param[in] payload           Pointer to the payload byte array of generic
 * record.
 * \param[in] payload_length    Pointer to the payload length of generic
 * record.
 * \param[out] record_details     Pointer to the generic record data that needs
 *                               to be updated from record handle.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If encoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t record_handler_generic_decode(const uint8_t *payload,
                                           uint32_t payload_length,
                                           void *record_details)
{
    ifx_status_t status = IFX_SUCCESS;
    ifx_record_generic_t *generic_rec = (ifx_record_generic_t *) record_details;
    if ((NULL == generic_rec) || (NULL == payload))
    {
        status = IFX_ERROR(IFX_RECORD_HANDLER_GENERIC,
                           IFX_RECORD_HANDLER_GEN_DECODE, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        /* Storing payload data to generic record. */
        generic_rec->payload = (ifx_blob_t *) malloc(sizeof(ifx_blob_t));
        if (NULL != generic_rec->payload)
        {
            generic_rec->payload->buffer = (uint8_t *) malloc(payload_length);
            if (NULL != generic_rec->payload->buffer)
            {
                IFX_MEMCPY(generic_rec->payload->buffer, payload,
                           payload_length);
                generic_rec->payload->length = payload_length;
            }
            else
            {
                status =
                    IFX_ERROR(IFX_RECORD_HANDLER_GENERIC,
                              IFX_RECORD_HANDLER_GEN_DECODE, IFX_OUT_OF_MEMORY);
            }
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_HANDLER_GENERIC,
                          IFX_RECORD_HANDLER_GEN_DECODE, IFX_OUT_OF_MEMORY);
        }
    }

    return status;
}

/**
 * \brief Sets the record type to the record handle.
 * \param[out] handle            Pointer to the record handle.
 * \param[in] type               Pointer to the record type.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If record type is set to handle successfully
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t record_handler_generic_set_type(ifx_record_handle_t *handle,
                                             const ifx_blob_t *type)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == type))
    {
        status = IFX_ERROR(IFX_RECORD_HANDLER_GENERIC,
                           IFX_RECORD_GEN_SET_TYPE_ID, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        if (IFX_RECORD_MAX_TYPE_LEN >= type->length)
        {
            if (handle->type.length)
            {
                IFX_FREE(handle->type.buffer);
                handle->type.buffer = NULL;
                handle->type.length = 0x00;
            }

            handle->type.buffer = (uint8_t *) malloc(type->length);
            if (NULL != handle->type.buffer)
            {
                IFX_MEMCPY(handle->type.buffer, type->buffer, type->length);
                handle->type.length = type->length;
            }
            else
            {
                status =
                    IFX_ERROR(IFX_RECORD_HANDLER_GENERIC,
                              IFX_RECORD_GEN_SET_TYPE_ID, IFX_OUT_OF_MEMORY);
            }
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_HANDLER_GENERIC,
                          IFX_RECORD_GEN_SET_TYPE_ID, IFX_ILLEGAL_ARGUMENT);
        }
    }

    return status;
}

/**
 * \brief Gets the record type to the record handle.
 * \param[in] handle            Pointer to the record handle.
 * \param[out] type             Pointer to the record type.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If record type is read from handle successfully
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t record_handler_generic_get_type(const ifx_record_handle_t *handle,
                                             ifx_blob_t *type)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == type))
    {
        status = IFX_ERROR(IFX_RECORD_HANDLER_GENERIC,
                           IFX_RECORD_GEN_GET_TYPE_ID, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        type->buffer = (uint8_t *) malloc(handle->type.length);
        if (NULL != type->buffer)
        {
            IFX_MEMCPY(type->buffer, handle->type.buffer, handle->type.length);
            type->length = handle->type.length;
        }
        else
        {
            status = IFX_ERROR(IFX_RECORD_HANDLER_GENERIC,
                               IFX_RECORD_GEN_GET_TYPE_ID, IFX_OUT_OF_MEMORY);
        }
    }

    return status;
}

/**
 * @brief Release all the allocated memory for the created generic record data
 *
 * @param[in] record_data    data of the generic record
 * \retval IFX_SUCCESS If memory release operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 */
ifx_status_t record_handler_generic_deinit(void *record_data)
{
    if (NULL == record_data)
    {
        return IFX_ERROR(IFX_RECORD_HANDLER_GENERIC,
                         IFX_RECORD_GENERIC_RELEASE_MEMORY,
                         IFX_ILLEGAL_ARGUMENT);
    }
    ifx_record_generic_t *generic_record =
        (ifx_record_generic_t *) (record_data);

    if (generic_record->payload->buffer != NULL)
    {
        IFX_FREE(generic_record->payload->buffer);
        generic_record->payload->buffer = NULL;
    }
    if (generic_record->payload != NULL)
    {
        IFX_FREE(generic_record->payload);
        generic_record->payload = NULL;
    }
    return IFX_SUCCESS;
}
