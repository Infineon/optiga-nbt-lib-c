// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file ndef-record/record-types/ifx-record-handler-uri.c
 * \brief URI record encoding/decoding utility.
 * \details For more details refer to technical specification document for URI
 * Record Type Definition(NFCForum-TS-RTD_URI_1.0)
 */
#include "ifx-record-handler-uri.h"
#include "infineon/ifx-ndef-lib.h"
#include "infineon/ifx-record-handler.h"

/* Public functions */

/**
 * \brief Encodes the URI record data into the payload.
 * \param[in] record_details     Pointer to the URI record data that was updated
 *                               in record handle.
 * \param[out] payload           Pointer to the payload byte array of URI
 * record.
 * \param[out] payload_length    Pointer to the payload length of URI record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If encoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t record_handler_uri_encode(const void *record_details,
                                       uint8_t **payload,
                                       uint32_t *payload_length)
{
    ifx_status_t status = IFX_SUCCESS;
    ifx_record_uri_t *uri_rec = (ifx_record_uri_t *) record_details;

    if ((NULL == uri_rec) || (NULL == uri_rec->uri) || (NULL == payload))
    {
        status = IFX_ERROR(IFX_RECORD_HANDLER_URI,
                           IFX_RECORD_HANDLER_URI_ENCODE, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        /* storing uri record payload data */
        *payload = (uint8_t *) malloc(IFX_RECORD_URI_IDENTIFIER_SIZE +
                                      uri_rec->uri->length);
        if (NULL != *payload)
        {
            IFX_MEMCPY(*payload, &uri_rec->identifier_code,
                       IFX_RECORD_URI_IDENTIFIER_SIZE);
            IFX_MEMCPY(*payload + IFX_RECORD_URI_IDENTIFIER_SIZE,
                       uri_rec->uri->buffer, uri_rec->uri->length);
            *payload_length =
                IFX_RECORD_URI_IDENTIFIER_SIZE + uri_rec->uri->length;
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_HANDLER_URI, IFX_RECORD_HANDLER_URI_ENCODE,
                          IFX_OUT_OF_MEMORY);
        }
    }

    return status;
}

/**
 * \brief Decodes the NDEF record to the URI record details.
 * \param[in] payload               Pointer to the payload byte array of URI
 * record.
 * \param[in] payload_length        Pointer to the payload length of URI
 * record.
 * \param[out] record_details       Pointer to the URI record type fields of
 * handler.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If decoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t record_handler_uri_decode(const uint8_t *payload,
                                       uint32_t payload_length,
                                       void *record_details)
{
    ifx_status_t status = IFX_SUCCESS;
    ifx_record_uri_t *uri_rec = (ifx_record_uri_t *) record_details;

    if ((NULL == record_details) || (NULL == payload))
    {
        status = IFX_ERROR(IFX_RECORD_HANDLER_URI,
                           IFX_RECORD_HANDLER_URI_DECODE, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        /* Dynamic memory block allocation is required to store the URI data. */
        uri_rec->uri = (ifx_blob_t *) malloc(sizeof(ifx_blob_t));

        if (uri_rec->uri == NULL)
        {
            return IFX_ERROR(IFX_RECORD_HANDLER_URI,
                             IFX_RECORD_HANDLER_URI_DECODE, IFX_OUT_OF_MEMORY);
        }

        IFX_MEMSET(uri_rec->uri, 0, sizeof(ifx_blob_t));
        /* Copy the prefix data from the payload to the record details. */
        uri_rec->identifier_code =
            payload[IFX_RECORD_URI_IDENTIFIER_CODE_OFFSET];

        /* Allocate the dynamic memory blocks to store the URI record data. */
        uri_rec->uri->buffer =
            (uint8_t *) malloc(payload_length - IFX_RECORD_URI_IDENTIFIER_SIZE);
        if (uri_rec->uri->buffer == NULL)
        {
            return IFX_ERROR(IFX_RECORD_HANDLER_URI,
                             IFX_RECORD_HANDLER_URI_DECODE, IFX_OUT_OF_MEMORY);
        }

        IFX_MEMSET(uri_rec->uri->buffer, 0,
                   payload_length - IFX_RECORD_URI_IDENTIFIER_SIZE);

        /* Copy the URI data from the payload to the record details. */
        IFX_MEMCPY(uri_rec->uri->buffer, &payload[IFX_RECORD_URI_VALUE_OFFSET],
                   (payload_length - IFX_RECORD_URI_IDENTIFIER_SIZE));
        uri_rec->uri->length = payload_length - IFX_RECORD_URI_IDENTIFIER_SIZE;
    }

    return status;
}