// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file ifx-tlv.c
 * \brief Provides the function implementation for TLV(tag, length and value)
 * encoder and decoder.
 */
#include "infineon/ifx-tlv.h"

#include <stdlib.h>

/**
 * \brief Encodes the array of TLV type data into byte array.
 * This method encodes in DGI TLV format, which is similar to a simple TLV
 * format but with 2 bytes TAG.
 * TAG (2 bytes)
 * LENGTH (1 or 3 bytes): If value >=255, 0xFF followed by 2 bytes length.
 * Otherwise 1 byte.
 * VALUE (0 or more bytes)
 *
 * \param[in] tlv_data              pointer to array of TLV type data
 * \param[in] tlv_data_count        number of TLV type data
 * \param[out] encoded_bytes        pointer to ifx_blob_t type encoded byte
 * array
 * \return ifx_status_t             IFX_SUCCESS, if successful otherwise error
 * information in case of error error status IFX_ILLEGAL_ARGUMENT, if parameter
 * is NULL or illegal error status IFX_OUT_OF_MEMORY, if memory allocation is
 * invalid
 */
ifx_status_t ifx_tlv_dgi_encode(const ifx_tlv_t *tlv_data,
                                uint32_t tlv_data_count,
                                ifx_blob_t *encoded_bytes)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(tlv_data) ||
        IFX_VALIDATE_NULL_PTR_MEMORY(encoded_bytes) || (tlv_data_count == 0))
    {
        return IFX_ERROR(IFX_TLV, IFX_TLV_DGI_ENCODER, IFX_ILLEGAL_ARGUMENT);
    }
#endif

    uint32_t tlv_count;
    uint32_t encoded_buffer_offset;
    uint32_t total_required_encoded_bytes;

    /* get total bytes required for encoding */
    total_required_encoded_bytes = 0x00;
    for (tlv_count = 0x00; tlv_count < tlv_data_count; tlv_count++)
    {
        total_required_encoded_bytes +=
            (IFX_TLV_DGI_TAG_SIZE + tlv_data[tlv_count].length);

        if (tlv_data[tlv_count].length >= UINT8_MAX)
        {
            total_required_encoded_bytes += IFX_TLV_DGI_LEN_WITH_ID_SIZE;
        }
        else
        {
            total_required_encoded_bytes += IFX_TLV_DGI_LEN_SIZE_1B;
        }
    }

    /* allocating memory to store the encoded bytes */
    encoded_bytes->buffer = (uint8_t *) malloc(total_required_encoded_bytes);
    if (IFX_VALIDATE_NULL_PTR_MEMORY(encoded_bytes->buffer))
    {
        return IFX_ERROR(IFX_TLV, IFX_TLV_DGI_ENCODER, IFX_OUT_OF_MEMORY);
    }

    /* copy the TLV fields into encoded bytes */
    encoded_buffer_offset = 0x00;
    uint32_t index;
    for (index = 0x00; index < tlv_data_count; index++)
    {
        /* add TLV tag value to byte array  */
        IFX_UPDATE_U16(&encoded_bytes->buffer[encoded_buffer_offset],
                       tlv_data[index].tag);
        encoded_buffer_offset += IFX_TLV_DGI_TAG_SIZE;

        /* check whether TLV value size is equal to or greater than 255 bytes */
        if (tlv_data[index].length >= UINT8_MAX)
        {
            /* add the identifier byte 0xff to byte array to identify 2B length
             * field of TLV */
            encoded_bytes->buffer[encoded_buffer_offset] =
                IFX_TLV_DGI_2B_LEN_IDENTIFIER;
            encoded_buffer_offset += IFX_TLV_DGI_LEN_IDENTIFIER_SIZE;

            /* add TLV length(2B) value to byte array */
            IFX_UPDATE_U16(&encoded_bytes->buffer[encoded_buffer_offset],
                           tlv_data[index].length);
            encoded_buffer_offset += IFX_TLV_DGI_LEN_SIZE_2B;
        }
        else
        {
            /* add TLV length(1B) value to byte array */
            encoded_bytes->buffer[encoded_buffer_offset] =
                tlv_data[index].length;
            encoded_buffer_offset += IFX_TLV_DGI_LEN_SIZE_1B;
        }

        /* if TLV length is non zero, then add value fields, otherwise not
         * adding value bytes */
        if (tlv_data[index].length)
        {
            /* add TLV value to byte array */
            IFX_MEMCPY(&encoded_bytes->buffer[encoded_buffer_offset],
                       tlv_data[index].value, tlv_data[index].length);
            encoded_buffer_offset += tlv_data[index].length;
        }
    }

    /* set the length of the total encoded bytes */
    encoded_bytes->length = total_required_encoded_bytes;

    return IFX_SUCCESS;
}

/**
 * \brief Decodes the byte array into array of TLV type data.
 * This method decodes DGI TLV formatted data, which is similar to a simple TLV
 * format but with 2 bytes TAG.
 * TAG (2 bytes)
 * LENGTH (1 or 3 bytes): If value>=255, 0xFF followed by 2 bytes length.
 * Otherwise 1 byte.
 * VALUE (0 or more bytes)
 *
 * \param[in] encoded_bytes         pointer to ifx_blob_t type encoded byte
 * array
 * \param[out] tlv_data_count        pointer to number of TLV type data
 * \param[out] tlv_data             pointer to array of TLV type data, memory
 * should be allocated before passing with this function
 * \return ifx_status_t             IFX_SUCCESS, if successful otherwise error
 * information in case of error error status IFX_ILLEGAL_ARGUMENT, if parameter
 * is NULL or illegal error status IFX_OUT_OF_MEMORY, if memory allocation is
 * invalid error status IFX_TLV_DGI_ENCODED_BYTES_INVALID, if encoded bytes
 * contain invalid TLV structure
 */
ifx_status_t ifx_tlv_dgi_decode(const ifx_blob_t *encoded_bytes,
                                uint32_t *tlv_data_count, ifx_tlv_t *tlv_data)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_BLOB(encoded_bytes) ||
        IFX_VALIDATE_NULL_PTR_MEMORY(tlv_data) ||
        IFX_VALIDATE_NULL_PTR_MEMORY(tlv_data_count))
    {
        return IFX_ERROR(IFX_TLV, IFX_TLV_DGI_DECODER, IFX_ILLEGAL_ARGUMENT);
    }
#endif

    uint32_t tlv_counter = 0x00;
    uint32_t decoded_buffer_offset;

    /* loop through the encoded byte array & decode the fields */
    decoded_buffer_offset = 0x00;
    while (decoded_buffer_offset < (encoded_bytes->length))
    {
        /* copy the 2 bytes tag, if it is available */
        if ((decoded_buffer_offset + IFX_TLV_DGI_TAG_SIZE) <=
            encoded_bytes->length)
        {
            IFX_READ_U16(&encoded_bytes->buffer[decoded_buffer_offset],
                         tlv_data[tlv_counter].tag);
            decoded_buffer_offset += IFX_TLV_DGI_TAG_SIZE;
        }
        else
        {
            return IFX_ERROR(IFX_TLV, IFX_TLV_DGI_DECODER,
                             IFX_TLV_DGI_ENCODED_BYTES_INVALID);
        }

        /* check if length offset is present */
        if ((decoded_buffer_offset + IFX_TLV_DGI_MIN_LEN_SIZE) >
            encoded_bytes->length)
        {
            return IFX_ERROR(IFX_TLV, IFX_TLV_DGI_DECODER,
                             IFX_TLV_DGI_ENCODED_BYTES_INVALID);
        }

        /* check whether the length identifier is present */
        if (encoded_bytes->buffer[decoded_buffer_offset] ==
            IFX_TLV_DGI_2B_LEN_IDENTIFIER)
        {
            /* length is of 2 bytes excluding the identifier byte */
            decoded_buffer_offset += IFX_TLV_DGI_LEN_IDENTIFIER_SIZE;

            /* copy the 2 bytes length, if it is available */
            if ((decoded_buffer_offset + IFX_TLV_DGI_LEN_SIZE_2B) <=
                encoded_bytes->length)
            {
                IFX_READ_U16(&encoded_bytes->buffer[decoded_buffer_offset],
                             tlv_data[tlv_counter].length);
                decoded_buffer_offset += IFX_TLV_DGI_LEN_SIZE_2B;
            }
            else
            {
                return IFX_ERROR(IFX_TLV, IFX_TLV_DGI_DECODER,
                                 IFX_TLV_DGI_ENCODED_BYTES_INVALID);
            }
        }
        else
        {
            tlv_data[tlv_counter].length =
                encoded_bytes->buffer[decoded_buffer_offset];
            decoded_buffer_offset += IFX_TLV_DGI_LEN_SIZE_1B;
        }

        /* copy the value field if available */
        if (tlv_data[tlv_counter].length)
        {
            /* allocate the memory for TLV value field */
            tlv_data[tlv_counter].value =
                (uint8_t *) malloc(tlv_data[tlv_counter].length);
            if (IFX_VALIDATE_NULL_PTR_MEMORY(tlv_data[tlv_counter].value))
            {
                return IFX_ERROR(IFX_TLV, IFX_TLV_DGI_DECODER,
                                 IFX_OUT_OF_MEMORY);
            }

            /* copy the value bytes, if available */
            if ((decoded_buffer_offset + tlv_data[tlv_counter].length) <=
                encoded_bytes->length)
            {
                IFX_MEMCPY(tlv_data[tlv_counter].value,
                           &encoded_bytes->buffer[decoded_buffer_offset],
                           tlv_data[tlv_counter].length);
                decoded_buffer_offset += tlv_data[tlv_counter].length;
            }
            else
            {
                return IFX_ERROR(IFX_TLV, IFX_TLV_DGI_DECODER,
                                 IFX_TLV_DGI_ENCODED_BYTES_INVALID);
            }
        }
        else
        {
            /* if length value is zero, then TLV value field is pointing to NULL
             * memory */
            tlv_data[tlv_counter].value = NULL;
        }

        /* increment the TLV data type counter */
        tlv_counter++;
    }

    /* update the total number of TLV available */
    *tlv_data_count = tlv_counter;

    return IFX_SUCCESS;
}
