// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file ndef-record/record-types/ifx-record-handler-handover-select.c
 * \brief Handover select record of encoding/decoding utility.
 * \details For more details refer to technical specification document for
 * Connection Handover(Version 1.3) 2014-01-16
 */
#include "ifx-record-handler-handover-select.h"
#include "infineon/ifx-ndef-lib.h"
#include "infineon/ifx-record-handler.h"

#define BYTE_LENGTH_OF_VERSION_INFO_FIELD UINT32_C(1)

/* Public functions */

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
                                      uint32_t *payload_length)
{
    ifx_status_t status = IFX_SUCCESS;
    ifx_record_hs_t *hs_record = (ifx_record_hs_t *) record_details;
    uint32_t index = UINT32_C(0);
    uint32_t count = UINT32_C(0);

    if ((NULL == hs_record) || (NULL == payload) || (NULL == payload_length))
    {
        return IFX_ERROR(IFX_RECORD_HANDLER_HS, IFX_RECORD_HANDLER_HS_ENCODE,
                         IFX_ILLEGAL_ARGUMENT);
    }

    ifx_blob_t local_record_bytes;
    IFX_MEMSET(&local_record_bytes, 0, sizeof(local_record_bytes));
    status = hs_record->local_record_encode(hs_record->local_record_list[count],
                                            hs_record->count_of_local_records,
                                            &local_record_bytes);

    uint8_t *payload_data = (uint8_t *) malloc(
        local_record_bytes.length + BYTE_LENGTH_OF_VERSION_INFO_FIELD);
    if (NULL == payload_data)
    {
        IFX_FREE(local_record_bytes.buffer);
        local_record_bytes.buffer = NULL;
        return IFX_ERROR(IFX_RECORD_HANDLER_HS, IFX_RECORD_HANDLER_HS_ENCODE,
                         IFX_OUT_OF_MEMORY);
    }
    payload_data[index++] = (hs_record->minor_version & 0x0F) |
                            ((hs_record->major_version << 4) & 0xF0);

    IFX_MEMCPY(&payload_data[index], local_record_bytes.buffer,
               local_record_bytes.length);

    index += local_record_bytes.length;

    *payload_length = index;
    *payload = (uint8_t *) malloc(*payload_length);
    if (NULL != *payload)
    {
        IFX_MEMCPY(*payload, payload_data, *payload_length);
    }
    else
    {
        status = IFX_ERROR(IFX_RECORD_HANDLER_HS, IFX_RECORD_HANDLER_HS_ENCODE,
                           IFX_OUT_OF_MEMORY);
    }
    IFX_FREE(payload_data);
    payload_data = NULL;
    IFX_FREE(local_record_bytes.buffer);
    local_record_bytes.buffer = NULL;
    return status;
}

/**
 * \brief Decodes the record payload bytes to the handover select record
 * details.
 * \param[in]   payload             Pointer to the payload byte array of
 * handover select record.
 * \param[in]   payload_length      Pointer to the payload length of handover
 * select record.
 * \param[out]  record_details      Pointer to the handover select record data
 *                                  where the decoded payload will be copied.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If decoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t record_handler_hs_decode(const uint8_t *payload,
                                      uint32_t payload_length,
                                      void *record_details)
{
    ifx_status_t status = IFX_SUCCESS;
    ifx_record_hs_t *hs_record = (ifx_record_hs_t *) record_details;
    uint32_t count = UINT32_C(0);
    uint8_t *temp_local_record_payload_buffer = NULL;

    if ((NULL == hs_record) || (NULL == payload))
    {
        return IFX_ERROR(IFX_RECORD_HANDLER_HS, IFX_RECORD_HANDLER_HS_DECODE,
                         IFX_ILLEGAL_ARGUMENT);
    }
    uint32_t index = UINT32_C(0);
    hs_record->minor_version = (uint8_t) (payload[index] & 0x0F);
    hs_record->major_version = (uint8_t) ((payload[index++] & 0xF0) >> 4);

    ifx_blob_t local_record_payload;
    IFX_MEMSET(&local_record_payload, 0, sizeof(local_record_payload));
    local_record_payload.length = payload_length - (index);
    temp_local_record_payload_buffer =
        (uint8_t *) malloc(local_record_payload.length);
    if (NULL != temp_local_record_payload_buffer)
    {
        local_record_payload.buffer = temp_local_record_payload_buffer;
        IFX_MEMCPY(local_record_payload.buffer, &payload[index],
                   local_record_payload.length);

        hs_record->local_record_list =
            (ifx_record_handle_t **) malloc(sizeof(ifx_record_handle_t));

        if (NULL != hs_record->local_record_list)
        {
            hs_record->local_record_list[count] =
                (ifx_record_handle_t *) malloc(sizeof(ifx_record_handle_t));
            if (NULL != hs_record->local_record_list[count])
            {
                status = hs_record->local_record_decode(
                    &local_record_payload, &count,
                    hs_record->local_record_list[count]);
            }
            else
            {
                IFX_FREE(hs_record->local_record_list);
                hs_record->local_record_list = NULL;
                status =
                    IFX_ERROR(IFX_RECORD_HANDLER_HS,
                              IFX_RECORD_HANDLER_HS_DECODE, IFX_OUT_OF_MEMORY);
            }
        }
        else
        {
            status = IFX_ERROR(IFX_RECORD_HANDLER_HS,
                               IFX_RECORD_HANDLER_HS_DECODE, IFX_OUT_OF_MEMORY);
        }
        IFX_FREE(temp_local_record_payload_buffer);
        temp_local_record_payload_buffer = NULL;
    }
    else
    {
        status = IFX_ERROR(IFX_RECORD_HANDLER_HS, IFX_RECORD_HANDLER_HS_DECODE,
                           IFX_OUT_OF_MEMORY);
    }

    hs_record->count_of_local_records = count;

    return status;
}
