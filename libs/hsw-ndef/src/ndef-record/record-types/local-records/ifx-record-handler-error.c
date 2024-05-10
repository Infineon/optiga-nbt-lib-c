// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file ndef-record/record-types/local-records/ifx-record-handler-error.c
 * \brief Error record of encoding/decoding utility.
 * This record is used only by handover select record and not intended to use
 * for any other records.
 * \details Error Record is included in a Handover Select
 * Record to indicate that the handover selector failed to successfully process
 * the most recently received handover request message. This record shall be
 * used only in Handover Select record. For more details refer to technical
 * specification document for Connection Handover(CH 1.4)
 */
#include "ifx-record-handler-error.h"
#include "infineon/ifx-ndef-lib.h"

/* Public functions */

/**
 * \brief Encodes the error record data details to the payload.
 * \param[in] record_details    Pointer to the error record details that was
 *                              updated in the record handle.
 * \param[out] payload          Pointer to the payload byte array of error
 * record.
 * \param[out] payload_length   Pointer to the payload length of error record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If encoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t record_handler_error_encode(const void *record_details,
                                         uint8_t **payload,
                                         uint32_t *payload_length)
{
    ifx_status_t status = IFX_SUCCESS;
    uint32_t index = UINT32_C(0);
    ifx_record_error_t *error_rec = (ifx_record_error_t *) record_details;

    if ((NULL == error_rec) || (NULL == payload) || (NULL == payload_length))
    {
        return IFX_ERROR(IFX_RECORD_HANDLER_ERROR,
                         IFX_RECORD_HANDLER_ERROR_ENCODE, IFX_ILLEGAL_ARGUMENT);
    }

    uint32_t record_details_size = sizeof(error_rec->error_reason);

    if (NULL != error_rec->error->buffer)
    {
        record_details_size += error_rec->error->length;
    }

    uint8_t *payload_data = (uint8_t *) malloc(record_details_size);
    if (NULL == payload_data)
    {
        status = IFX_ERROR(IFX_RECORD_HANDLER_ERROR,
                           IFX_RECORD_HANDLER_ERROR_ENCODE, IFX_OUT_OF_MEMORY);
    }
    else
    {
        payload_data[index++] = error_rec->error_reason;
        if (NULL != error_rec->error->buffer)
        {
            IFX_MEMCPY(&payload_data[index], error_rec->error->buffer,
                       error_rec->error->length);
            index += error_rec->error->length;
        }

        *payload_length = index;
        *payload = (uint8_t *) malloc(*payload_length);
        if (NULL != *payload)
        {
            IFX_MEMCPY(*payload, payload_data, *payload_length);
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_HANDLER_ERROR,
                          IFX_RECORD_HANDLER_ERROR_ENCODE, IFX_OUT_OF_MEMORY);
        }
        IFX_FREE(payload_data);
        payload_data = NULL;
    }

    return status;
}

/**
 * \brief Decodes the NDEF record payload to the error record details.
 * \param[in] payload           Pointer to the payload byte array of error
 * record.
 * \param[in] payload_length    Pointer to the payload length of error record.
 * \param[out] record_details     Pointer to the error record details that needs
 *                               to be updated from the record handle.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If decoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t record_handler_error_decode(const uint8_t *payload,
                                         uint32_t payload_length,
                                         void *record_details)
{
    ifx_status_t status = IFX_SUCCESS;
    uint32_t index = UINT32_C(0);
    ifx_record_error_t *error_rec = (ifx_record_error_t *) record_details;

    if ((NULL == error_rec) || (NULL == payload))
    {
        return IFX_ERROR(IFX_RECORD_HANDLER_ERROR,
                         IFX_RECORD_HANDLER_ERROR_DECODE, IFX_ILLEGAL_ARGUMENT);
    }

    error_rec->error_reason = payload[index++];
    error_rec->error = (ifx_blob_t *) malloc(sizeof(ifx_blob_t));

    if (NULL == error_rec->error)
    {
        status = IFX_ERROR(IFX_RECORD_HANDLER_ERROR,
                           IFX_RECORD_HANDLER_ERROR_DECODE, IFX_OUT_OF_MEMORY);
    }
    else
    {
        error_rec->error->length = payload_length - index;
        if (!(error_rec->error->length))
        {
            error_rec->error->buffer = NULL;
        }
        else
        {
            error_rec->error->buffer =
                (uint8_t *) malloc(error_rec->error->length);
            if (NULL == error_rec->error->buffer)
            {
                IFX_FREE(error_rec->error);
                error_rec->error = NULL;
                status = IFX_ERROR(IFX_RECORD_HANDLER_ERROR,
                                   IFX_RECORD_HANDLER_ERROR_DECODE,
                                   IFX_OUT_OF_MEMORY);
            }
            else
            {
                IFX_MEMCPY(error_rec->error->buffer, &payload[index],
                           error_rec->error->length);
            }
        }
    }

    return status;
}
