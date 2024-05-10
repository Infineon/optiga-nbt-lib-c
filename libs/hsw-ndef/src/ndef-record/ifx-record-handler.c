// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file ndef_record/ifx-record-handler.c
 * \brief NDEF record encoding/decoding utility.
 */
#include "infineon/ifx-ndef-lib.h"
#include "infineon/ifx-record-handler.h"
#include "infineon/ifx-utils.h"

/* Static functions */

/**
 * \brief
 * Calculates the size of the NDEF record.
 * \param[in] record    Pointer to the NDEF record.
 * \return uint32_t     Size of the NDEF record.
 */
static uint32_t calculate_size_of_ndef_record(const ifx_ndef_record_t *record)
{
    uint32_t size = UINT32_C(0);
    if (NULL != record)
    {
        size = IFX_NDEF_HEADER_FIELD_LEN;
        size += record->type_length + IFX_NDEF_TYPE_FIELD_LEN;

        if (IFX_NDEF_ID_LEN_FIELD_NONE != record->id_length)
        {
            size += record->id_length + IFX_NDEF_ID_FIELD_LEN;
        }

        if (IFX_NDEF_SR_PAYLOAD_LEN_FIELD_MAX_LEN >= record->payload_length)
        {
            size += record->payload_length + IFX_NDEF_SR_PAYLOAD_LEN_FIELD_LEN;
        }
        else
        {
            size += record->payload_length + IFX_NDEF_PAYLOAD_LEN_FIELD_LEN;
        }
    }

    return size;
}

/**
 * \brief Encodes the record handle to the NDEF records.
 * \param[in] handle            Pointer to the record handle.
 * \param[out] record           Pointer to the NDEF record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If encoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
static ifx_status_t encode_handle_to_record(const ifx_record_handle_t *handle,
                                            ifx_ndef_record_t *record)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == record) || (handle == NULL))
    {
        status = IFX_ERROR(IFX_RECORD_HANDLER, IFX_RECORD_HANDLER_ENCODE,
                           IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        record->tnf = handle->tnf;
        if (IFX_NDEF_ID_LEN_FIELD_NONE == handle->id.length)
        {
            record->id = NULL;
            record->id_length = IFX_NDEF_ID_LEN_FIELD_NONE;
        }
        else
        {
            record->id = (uint8_t *) malloc(handle->id.length);
            if (NULL == record->id)
            {
                status =
                    IFX_ERROR(IFX_RECORD_HANDLER, IFX_RECORD_HANDLER_ENCODE,
                              IFX_OUT_OF_MEMORY);
            }
            else
            {
                IFX_MEMCPY(record->id, handle->id.buffer, handle->id.length);
                record->id_length = handle->id.length;
            }
        }

        if (IFX_SUCCESS == status)
        {
            record->type = (uint8_t *) malloc(handle->type.length);
            if (NULL == record->type)
            {
                status =
                    IFX_ERROR(IFX_RECORD_HANDLER, IFX_RECORD_HANDLER_ENCODE,
                              IFX_OUT_OF_MEMORY);
            }
            else
            {
                IFX_MEMCPY(record->type, handle->type.buffer,
                           handle->type.length);
                record->type_length = handle->type.length;
            }
        }

        if (IFX_SUCCESS == status)
        {
            status = handle->encode_record((void *) handle->record_data,
                                           &(record->payload),
                                           &(record->payload_length));
        }
    }
    return status;
}

/**
 * \brief Sets the short record (SR) flag, ID length (IL) flag, and chunk flag
 * (CF) in the header flag field.
 * \param[in] record                Pointer to the NDEF record.
 * \param[out] header_flag_field    Pointer to the header flag field.
 * \retval IFX_SUCCESS If decoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 */
static ifx_status_t set_ndef_flags(const ifx_ndef_record_t *record,
                                   uint8_t *header_flag_field)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == record) || (header_flag_field == NULL))
    {
        status = IFX_ERROR(IFX_RECORD_HANDLER, IFX_RECORD_HANDLER_ENCODE,
                           IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        if (IFX_NDEF_ID_LEN_FIELD_NONE != record->id_length)
        {
            *header_flag_field |= IFX_RECORD_HEADER_MASK_ID_FLAG;
        }

        if (IFX_NDEF_SR_PAYLOAD_LEN_FIELD_MAX_LEN >= record->payload_length)
        {
            *header_flag_field |= IFX_RECORD_HEADER_MASK_SR_FLAG;
        }
    }

    return status;
}

/**
 * \brief Decodes the NDEF record to the record handle.
 * \param[in] record            Pointer to the NDEF record.
 * \param[out] handle           Pointer to the record handle.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If decoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
static ifx_status_t decode_record_to_handle(ifx_ndef_record_t *record,
                                            ifx_record_handle_t *handle)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == record))
    {
        status = IFX_ERROR(IFX_RECORD_HANDLER, IFX_RECORD_HANDLER_DECODE,
                           IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        status = ifx_ndef_record_retrieve_handle(record->tnf, record->type,
                                                 record->type_length, handle);
        if (IFX_SUCCESS == status)
        {
            IFX_FREE(handle->id.buffer); // Deallocating as allocation is done
                                         // below with required ID value
            handle->id.buffer = NULL;
            if (IFX_NDEF_ID_LEN_FIELD_NONE == record->id_length)
            {
                IFX_MEMSET(&handle->id, 0, sizeof(handle->id));
            }
            else
            {
                handle->id.length = record->id_length;
                handle->id.buffer = (uint8_t *) malloc(handle->id.length);

                if (NULL == handle->id.buffer)
                {
                    status =
                        IFX_ERROR(IFX_RECORD_HANDLER, IFX_RECORD_HANDLER_DECODE,
                                  IFX_OUT_OF_MEMORY);
                }
                else
                {
                    IFX_MEMCPY(handle->id.buffer, record->id,
                               handle->id.length);
                }
            }
        }

        if (IFX_SUCCESS == status)
        {
            IFX_FREE(handle->type.buffer); // Deallocating as allocation is done
                                           // below with required Type value
            handle->type.buffer = NULL;
            handle->type.length = record->type_length;
            handle->type.buffer = (uint8_t *) malloc(handle->type.length);

            if (NULL == handle->type.buffer)
            {
                status =
                    IFX_ERROR(IFX_RECORD_HANDLER, IFX_RECORD_HANDLER_DECODE,
                              IFX_OUT_OF_MEMORY);
            }
            else
            {
                IFX_MEMCPY(handle->type.buffer, record->type,
                           handle->type.length);
            }
        }

        if (IFX_SUCCESS == status)
        {
            status =
                handle->decode_record(record->payload, record->payload_length,
                                      (void *) handle->record_data);
        }
    }
    return status;
}

/**
 * \brief Encodes the NDEF record into the record bytes
 * \param[in] record        Pointer to the encoded NDEF record.
 * \param[out] bytes        Pointer to the URI record data that was updated in
 *                          the record handle.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If encoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 */
static ifx_status_t encode_record_to_bytes(const ifx_ndef_record_t *record,
                                           ifx_blob_t *bytes)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == bytes) || (NULL == record))
    {
        status = IFX_ERROR(IFX_RECORD_HANDLER, IFX_RECORD_HANDLER_ENCODE,
                           IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        uint32_t record_size = calculate_size_of_ndef_record(record);
        bytes->buffer = (uint8_t *) malloc(record_size);
        if (NULL == bytes->buffer)
        {
            status = IFX_ERROR(IFX_RECORD_HANDLER, IFX_RECORD_HANDLER_ENCODE,
                               IFX_OUT_OF_MEMORY);
        }
        else
        {
            uint32_t index = 0;
            bytes->buffer[index++] = record->tnf;
            bytes->buffer[index++] = record->type_length;

            if (record->payload_length < 0xFF)
            {
                bytes->buffer[index++] = record->payload_length;
            }
            else
            {
                IFX_UPDATE_U32((uint8_t *) (&(bytes->buffer[index])),
                               record->payload_length);
                index += IFX_NDEF_PAYLOAD_LEN_FIELD_LEN;
            }

            if (IFX_NDEF_ID_LEN_FIELD_NONE != record->id_length)
            {
                bytes->buffer[index++] = record->id_length;
            }

            IFX_MEMCPY(&bytes->buffer[index], record->type,
                       record->type_length);
            index += record->type_length;

            if (IFX_NDEF_ID_LEN_FIELD_NONE != record->id_length)
            {
                IFX_MEMCPY(&bytes->buffer[index], record->id,
                           record->id_length);
                index += record->id_length;
            }

            IFX_MEMCPY(&bytes->buffer[index], record->payload,
                       record->payload_length);
            index += record->payload_length;

            bytes->length = index;
        }
    }

    return status;
}

/**
 * \brief Decodes the bytes array to get the NDEF record.
 * \param[in] bytes             Pointer to the bytes data.
 * \param[out] record           Pointer to the decoded NDEF record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If decoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
static ifx_status_t decode_bytes_to_record(ifx_blob_t *bytes,
                                           ifx_ndef_record_t *record)
{
    ifx_status_t status = IFX_SUCCESS;
    uint32_t index = UINT32_C(0);
    if ((NULL == bytes) || (NULL == record))
    {
        status = IFX_ERROR(IFX_RECORD_HANDLER, IFX_RECORD_HANDLER_DECODE,
                           IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        uint8_t header_tnf_flag_data = bytes->buffer[index++];
        record->tnf = header_tnf_flag_data & IFX_RECORD_TNF_MASK;
        record->type_length = bytes->buffer[index++];

        if (header_tnf_flag_data & IFX_RECORD_HEADER_MASK_SR_FLAG)
        {
            record->payload_length = bytes->buffer[index++];
        }
        else
        {
            IFX_READ_U32((uint8_t *) (&bytes->buffer[index]),
                         record->payload_length);
            index += IFX_NDEF_PAYLOAD_LEN_FIELD_LEN;
        }

        if (header_tnf_flag_data & IFX_RECORD_HEADER_MASK_ID_FLAG)
        {
            record->id_length = bytes->buffer[index++];
        }
        else
        {
            record->id_length = IFX_NDEF_ID_LEN_FIELD_NONE;
        }

        record->type = (uint8_t *) malloc(record->type_length);
        if (NULL == record->type)
        {
            return IFX_ERROR(IFX_RECORD_HANDLER, IFX_RECORD_HANDLER_DECODE,
                             IFX_OUT_OF_MEMORY);
        }

        IFX_MEMCPY(record->type, &bytes->buffer[index], record->type_length);
        index += record->type_length;

        if (IFX_NDEF_ID_LEN_FIELD_NONE != record->id_length)
        {
            record->id = (uint8_t *) malloc(record->id_length);
            if (NULL == record->id)
            {
                return IFX_ERROR(IFX_RECORD_HANDLER, IFX_RECORD_HANDLER_DECODE,
                                 IFX_OUT_OF_MEMORY);
            }
            IFX_MEMCPY(record->id, &bytes->buffer[index], record->id_length);
            index += record->id_length;
        }

        record->payload = (uint8_t *) malloc(record->payload_length);
        if (NULL == record->payload)
        {
            return IFX_ERROR(IFX_RECORD_HANDLER, IFX_RECORD_HANDLER_DECODE,
                             IFX_OUT_OF_MEMORY);
        }
        IFX_MEMCPY(record->payload, &bytes->buffer[index],
                   record->payload_length);
        index += record->payload_length;
        bytes->length -= index;
    }

    return status;
}

/* Public functions */

/**
 * \brief Encodes the record bytes for the specific record type handle given as
 * input.
 * \param[in] handle            Pointer to the handle of specific record type.
 * \param[out] record_bytes     Pointer to the encoded record bytes.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If encoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 */
ifx_status_t record_handler_encode(const ifx_record_handle_t *handle,
                                   ifx_blob_t *record_bytes)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == record_bytes))
    {
        status = IFX_ERROR(IFX_RECORD_HANDLER, IFX_RECORD_HANDLER_ENCODE,
                           IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        ifx_ndef_record_t record;
        IFX_MEMSET(&record, 0, sizeof(record));
        uint32_t index_of_header_flag_field = record_bytes->length;
        status = encode_handle_to_record(handle, &record);
        if (IFX_SUCCESS == status)
        {
            status = encode_record_to_bytes(&record, record_bytes);
        }

        if (IFX_SUCCESS == status)
        {
            status = set_ndef_flags(
                &record, &record_bytes->buffer[index_of_header_flag_field]);
        }
        IFX_FREE(record.id);
        IFX_FREE(record.type);
        IFX_FREE(record.payload);
    }

    return status;
}

/**
 * \brief Decodes the record handles information from the given record bytes as
 * input.
 * \param[in] record_bytes      Pointer to the record bytes whose details to be
 * decoded to the record handle.
 * \param[out] handle           Pointer to the record handle where record
 * details are updated.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If decoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 */
ifx_status_t record_handler_decode(ifx_blob_t *record_bytes,
                                   ifx_record_handle_t *handle)
{
    ifx_status_t status = IFX_SUCCESS;

    if ((NULL == handle) || (NULL == record_bytes))
    {
        status = IFX_ERROR(IFX_RECORD_HANDLER, IFX_RECORD_HANDLER_DECODE,
                           IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        ifx_ndef_record_t record;
        IFX_MEMSET(&record, 0, sizeof(record));
        status = decode_bytes_to_record(record_bytes, &record);
        if (IFX_SUCCESS == status)
        {
            status = decode_record_to_handle(&record, handle);
        }
        IFX_FREE(record.id);
        IFX_FREE(record.type);
        IFX_FREE(record.payload);
    }

    return status;
}
