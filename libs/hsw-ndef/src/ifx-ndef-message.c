// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file ifx-ndef-message.c
 * \brief NDEF message encoding/decoding utility.
 * \details For more details refer to technical specification document NFC Data
 * Exchange Format(NFCForum-TS-NDEF_1.0)
 */
#include "infineon/ifx-ndef-lib.h"
#include "infineon/ifx-ndef-message.h"
#include "infineon/ifx-record-handler.h"

/* Macro defintions */

/**
 * \brief Index of the first NDEF record is decoded in the NDEF message.
 */
#define FIRST_NDEF_RECORD      UINT32_C(0)

/**
 * \brief Mask value to extract the message begin (MB) bit in the header.
 */
#define MASK_MB_FLAG_IN_HEADER UINT8_C(0x80)

/**
 * \brief Mask value to extract the message end (ME) bit in the header.
 */
#define MASK_ME_FLAG_IN_HEADER UINT8_C(0x40)

/**
 * \brief Increment factor for the record number
 */
#define RECORD_NUMBER_FACTOR   UINT32_C(0x01)

/* Static functions */

/**
 * \brief Sets the NDEF header flags of record in the NDEF message.
 *
 * \param[in] record_number     Index of the current NDEF record.
 * \param[in] total_records     Total number of the NDEF records parsed based
 *                               on the message end (ME) flag.
 * \param[out] header_flag_field Pointer to the NDEF record data.
 * \return void
 */
static void set_ndef_header_flags(const uint32_t record_number,
                                  const uint32_t total_records,
                                  uint8_t *header_flag_field)
{
    if (NULL != header_flag_field)
    {
        if (FIRST_NDEF_RECORD == record_number)
        {
            *header_flag_field |= MASK_MB_FLAG_IN_HEADER;
        }
        if ((total_records - 0x01) == record_number)
        {
            *header_flag_field |= MASK_ME_FLAG_IN_HEADER;
        }
    }
}

/**
 * \brief Decodes the header to get the record number.
 * \param[in] header_flag_field    Header flag field of the NDEF record.
 * \param[out] record_number       Pointer to the current NDEF record number.
 * \return void
 */
static void get_ndef_record_number(const uint8_t header_flag_field,
                                   uint32_t *record_number)
{
    if (header_flag_field & MASK_MB_FLAG_IN_HEADER)
    {
        *record_number = FIRST_NDEF_RECORD;
    }
    else
    {
        *record_number += RECORD_NUMBER_FACTOR;
    }
}

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
                                     const uint32_t number_of_records,
                                     ifx_blob_t *ndef_message)
{
    ifx_status_t status = IFX_SUCCESS;
    const uint8_t empty_record_data[IFX_NDEF_EMPTY_MESSAGE_LEN] = {
        IFX_NDEF_MESSAGE_EMPTY};

    if ((NULL == record_handles) || (NULL == ndef_message))
    {
        status = IFX_ERROR(IFX_NDEF_MESSAGE, IFX_NDEF_MESSAGE_ENCODE,
                           IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        if (NULL == IFX_MEMSET(ndef_message, 0, sizeof(ifx_blob_t)))
        {
            status = IFX_ERROR(IFX_NDEF_MESSAGE, IFX_NDEF_MESSAGE_ENCODE,
                               IFX_OUT_OF_MEMORY);
        }
        else
        {
            if (0 == number_of_records)
            {
                ndef_message->length = IFX_NDEF_EMPTY_MESSAGE_LEN;
                ndef_message->buffer = (uint8_t *) malloc(ndef_message->length);
                if (NULL == ndef_message->buffer)
                {
                    return IFX_ERROR(IFX_NDEF_MESSAGE, IFX_NDEF_MESSAGE_ENCODE,
                                     IFX_OUT_OF_MEMORY);
                }
                IFX_MEMCPY(ndef_message->buffer, empty_record_data,
                           ndef_message->length);
            }
            else
            {
                ndef_message->length = UINT32_C(0);
                uint32_t index_of_header_flag_field = UINT32_C(0);
                uint32_t record_index = UINT32_C(0);
                uint32_t offset = UINT32_C(0);
                for (; record_index < number_of_records; record_index++)
                {
                    ifx_blob_t encoded_record;
                    IFX_MEMSET(&encoded_record, 0x00, sizeof(ifx_blob_t));
                    status = record_handler_encode(
                        &record_handles[record_index], &encoded_record);
                    if (IFX_SUCCESS == status)
                    {

                        uint8_t *buffer_temp = (uint8_t *) realloc(
                            ndef_message->buffer,
                            ndef_message->length + encoded_record.length);
                        if (NULL == buffer_temp)
                        {
                            IFX_FREE(ndef_message->buffer);
                            ndef_message->buffer = NULL;
                            return IFX_ERROR(IFX_NDEF_MESSAGE,
                                             IFX_NDEF_MESSAGE_ENCODE,
                                             IFX_OUT_OF_MEMORY);
                        }

                        ndef_message->buffer = buffer_temp;
                        IFX_MEMCPY(&ndef_message->buffer[offset],
                                   encoded_record.buffer,
                                   encoded_record.length);
                        ndef_message->length += encoded_record.length;
                        offset += encoded_record.length;
                        IFX_FREE(encoded_record.buffer);
                        set_ndef_header_flags(
                            record_index, number_of_records,
                            &ndef_message->buffer[index_of_header_flag_field]);
                        index_of_header_flag_field = ndef_message->length;
                    }
                }
                IFX_UNUSED_VARIABLE(record_index);
            }
        }
    }

    return status;
}

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
                                     ifx_record_handle_t *record_handles)
{
    ifx_status_t status = IFX_SUCCESS;
    const uint8_t empty_message_data[IFX_NDEF_EMPTY_MESSAGE_LEN] = {
        IFX_NDEF_MESSAGE_EMPTY};
    if ((NULL == ndef_message) || (NULL == record_handles) ||
        (NULL == number_of_records))
    {
        return (IFX_ERROR(IFX_NDEF_MESSAGE, IFX_NDEF_MESSAGE_DECODE,
                          IFX_ILLEGAL_ARGUMENT));
    }

    if (!IFX_MEMCMP((ndef_message->buffer), empty_message_data,
                    IFX_NDEF_EMPTY_MESSAGE_LEN))
    {
        record_handles = NULL;
        *number_of_records = 0;
    }
    else
    {
        uint32_t record_number = UINT32_C(0);
        ifx_blob_t ndef_message_copy;
        IFX_MEMSET(&ndef_message_copy, 0, sizeof(ifx_blob_t));
        ndef_message_copy.buffer = (uint8_t *) malloc(ndef_message->length);
        if (NULL == ndef_message_copy.buffer)
        {
            return (IFX_ERROR(IFX_NDEF_MESSAGE, IFX_NDEF_MESSAGE_DECODE,
                              IFX_OUT_OF_MEMORY));
        }
        IFX_MEMCPY(ndef_message_copy.buffer, ndef_message->buffer,
                   ndef_message->length);
        ndef_message_copy.length = ndef_message->length;
        while (ndef_message_copy.length)
        {
            uint32_t length_before_decode = ndef_message_copy.length;
            get_ndef_record_number(*(ndef_message_copy.buffer), &record_number);
            status = record_handler_decode(&ndef_message_copy,
                                           &record_handles[record_number]);
            ndef_message_copy.buffer +=
                length_before_decode - ndef_message_copy.length;
        }
        ndef_message_copy.buffer -= ndef_message->length;
        IFX_FREE(ndef_message_copy.buffer);
        ndef_message_copy.buffer = NULL;

        *number_of_records = ++record_number;
    }

    return status;
}