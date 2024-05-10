// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file
 * ndef-record/record-types/carrier-configuration-records/ifx-record-handler-bluetooth.c
 * \brief Bluetooth record of encoding/decoding utility.
 * Bluetooth Out-of-Band record data can be exchanged in connection handover
 * request and/or select messages as alternative carrier information.
 */
#include "ifx-bluetooth-core-config.h"
#include "ifx-record-handler-bluetooth.h"
#include "infineon/ifx-ndef-lib.h"

/* Macro definitions */
#define BYTE_LENGTH_OF_DATALENGTH_FIELD UINT8_C(1)
#define BYTE_LENGTH_OF_OOB_DATA_LENGTH  UINT8_C(2)

/* Static functions */

/**
 * \brief Encodes the extended inquiry response (EIR) type data to the payload
 * bytes.
 * \param[in] eir_type  EIR type data that needs to be encoded into the payload
 * bytes.
 * \param[out] payload  Pointer to the payload.
 * \param[out] index    Pointer to the updated index of payload buffer.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If encoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 */
static ifx_status_t encode_eir_types_to_payload(ifx_record_eir_data_t eir_type,
                                                uint8_t **payload,
                                                uint32_t *index)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == payload) || (NULL == index))
    {
        status = IFX_ERROR(IFX_RECORD_HANDLER_BT, IFX_RECORD_HANDLER_BT_ENCODE,
                           IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        if (0 != eir_type.data_length)
        {
            uint8_t *buffer_temp = (uint8_t *) realloc(
                *payload, (*index) + eir_type.data_length +
                              BYTE_LENGTH_OF_DATALENGTH_FIELD);
            if (NULL == buffer_temp)
            {
                IFX_FREE(*payload);
                *payload = NULL;
                status =
                    IFX_ERROR(IFX_RECORD_HANDLER_BT,
                              IFX_RECORD_HANDLER_BT_ENCODE, IFX_OUT_OF_MEMORY);
            }
            else
            {
                *payload = buffer_temp;
                (*payload)[(*index)++] = eir_type.data_length;
                (*payload)[(*index)++] = eir_type.data_type;
                IFX_MEMCPY(&((*payload)[(*index)]), eir_type.data,
                           eir_type.data_length -
                               BYTE_LENGTH_OF_DATALENGTH_FIELD);
                *index +=
                    (eir_type.data_length - BYTE_LENGTH_OF_DATALENGTH_FIELD);
            }
        }
        else
        {
            status = IFX_SUCCESS;
        }
    }
    return status;
}

/**
 * \brief Decodes the payload bytes to the EIR type data.
 * \param[in] payload       Pointer to the payload.
 * \param[in] index         Pointer to the updated index of payload buffer.
 * \param[out] eir_type     Pointer to the EIR type data.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If decoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 */
static ifx_status_t decode_eir_types_from_payload(
    const uint8_t *payload, uint32_t *index, ifx_record_eir_data_t *eir_type)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == payload) || (NULL == index) || (NULL == eir_type))
    {
        status = IFX_ERROR(IFX_RECORD_HANDLER_BT, IFX_RECORD_HANDLER_BT_DECODE,
                           IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        uint8_t offset = 0;
        eir_type->data_length = payload[offset++];
        eir_type->data_type = payload[offset++];
        eir_type->data = (uint8_t *) malloc(eir_type->data_length -
                                            BYTE_LENGTH_OF_DATALENGTH_FIELD);
        if (NULL != eir_type->data)
        {
            IFX_MEMCPY(eir_type->data, &payload[offset],
                       eir_type->data_length - BYTE_LENGTH_OF_DATALENGTH_FIELD);
            *index += (eir_type->data_length + BYTE_LENGTH_OF_DATALENGTH_FIELD);
        }
        else
        {
            status = IFX_ERROR(IFX_RECORD_HANDLER_BT,
                               IFX_RECORD_HANDLER_BT_DECODE, IFX_OUT_OF_MEMORY);
        }
    }
    return status;
}

/* Public functions */

/**
 * \brief Encodes the Bluetooth carrier configuration record data details to the
 * payload.
 * \param[in] record_details    Pointer to the Bluetooth carrier configuration
 * record details that was updated in record handle.
 * \param[out] payload          Pointer to the payload byte array of Bluetooth
 * carrier configuration record.
 * \param[out] payload_length   Pointer to the payload length of Bluetooth
 * carrier configuration record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If encoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t record_handler_bt_encode(const void *record_details,
                                      uint8_t **payload,
                                      uint32_t *payload_length)
{
    ifx_status_t status = IFX_SUCCESS;
    uint32_t index = UINT32_C(0);
    const ifx_record_bt_t *bt_record = (ifx_record_bt_t *) record_details;

    if ((NULL == bt_record) || (NULL == payload) || (NULL == payload_length))
    {
        return IFX_ERROR(IFX_RECORD_HANDLER_BT, IFX_RECORD_HANDLER_BT_ENCODE,
                         IFX_ILLEGAL_ARGUMENT);
    }

    *payload = (uint8_t *) malloc(IFX_RECORD_BT_DEV_ADDR_LEN +
                                  BYTE_LENGTH_OF_OOB_DATA_LENGTH);
    if (NULL == *payload)
    {
        status = IFX_ERROR(IFX_RECORD_HANDLER_BT, IFX_RECORD_HANDLER_BT_ENCODE,
                           IFX_OUT_OF_MEMORY);
    }
    else
    {
        /* Since OOB data length is unknown at the start, reserve the memory
        holder and update the length after all the EIR fields are processed. */
        uint8_t index_of_oob_data_length = index;

        index += BYTE_LENGTH_OF_OOB_DATA_LENGTH;

        if ((NULL != (bt_record->device_addr)) && (IFX_SUCCESS == status))
        {
            IFX_MEMCPY(&((*payload)[index]), bt_record->device_addr,
                       IFX_RECORD_BT_DEV_ADDR_LEN);
            index += IFX_RECORD_BT_DEV_ADDR_LEN;
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_HANDLER_BT, IFX_RECORD_HANDLER_BT_ENCODE,
                          IFX_ILLEGAL_ARGUMENT);
        }

        if (IFX_SUCCESS == status)
        {
            status = encode_eir_types_to_payload(
                bt_record->optional_eir_types.device_class, payload, &index);
        }

        if (IFX_SUCCESS == status)
        {
            status = encode_eir_types_to_payload(
                bt_record->optional_eir_types.simple_pairing_hash_c, payload,
                &index);
        }

        if (IFX_SUCCESS == status)
        {
            status = encode_eir_types_to_payload(
                bt_record->optional_eir_types.simple_pairing_randomizer_r,
                payload, &index);
        }

        if (IFX_SUCCESS == status)
        {
            status = encode_eir_types_to_payload(
                bt_record->optional_eir_types.service_class_uuid, payload,
                &index);
        }

        if (IFX_SUCCESS == status)
        {
            status = encode_eir_types_to_payload(
                bt_record->optional_eir_types.local_name, payload, &index);
        }

        if ((NULL != bt_record->optional_eir_types.additional_eir_types) &&
            (IFX_SUCCESS == status))
        {
            if (0 !=
                bt_record->optional_eir_types.count_of_additional_eir_types)
            {
                uint32_t count = UINT32_C(0);
                while (
                    count <
                    bt_record->optional_eir_types.count_of_additional_eir_types)
                {
                    if (IFX_SUCCESS == status)
                    {
                        status = encode_eir_types_to_payload(
                            bt_record->optional_eir_types
                                .additional_eir_types[count++],
                            payload, &index);
                    }
                }
            }
        }

        if (IFX_SUCCESS == status)
        {
            /* Updates the OOB data length. */
            (*payload)[index_of_oob_data_length] = (index & 0x00FF);
            (*payload)[index_of_oob_data_length + 1] = (index & 0xFF00) >> 8;

            *payload_length = index;
        }
    }

    return status;
}

/**
 * \brief Decodes the NDEF record payload to the Bluetooth carrier configuration
 * record.
 * \param[in] payload           Pointer to the payload byte array of bluetooth
 *                               carrier configuration record.
 * \param[in] payload_length    Pointer to the payload length of bluetooth
 *                               carrier configuration record.
 * \param[out] record_details     Pointer to the Bluetooth carrier configuration
 *                               record that needs to be updated from record
 * handle.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If decoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 */
ifx_status_t record_handler_bt_decode(const uint8_t *payload,
                                      uint32_t payload_length,
                                      void *record_details)
{
    ifx_status_t status = IFX_SUCCESS;
    uint32_t index = UINT32_C(0);
    uint32_t count = UINT32_C(0);
    ifx_record_bt_t *bt_record = (ifx_record_bt_t *) record_details;

    if ((NULL == bt_record) || (NULL == payload))
    {
        return IFX_ERROR(IFX_RECORD_HANDLER_BT, IFX_RECORD_HANDLER_BT_DECODE,
                         IFX_ILLEGAL_ARGUMENT);
    }

    bt_record->oob_data_length =
        payload[index] | (uint16_t) (payload[index + 0x01] << 8);
    index += 0x02;

    IFX_MEMCPY(bt_record->device_addr, &payload[index],
               IFX_RECORD_BT_DEV_ADDR_LEN);
    index += IFX_RECORD_BT_DEV_ADDR_LEN;

    while ((index < payload_length) && (IFX_SUCCESS == status))
    {
        ifx_record_eir_data_t eir_data;
        status =
            decode_eir_types_from_payload(&payload[index], &index, &eir_data);
        if (IFX_SUCCESS == status)
        {
            switch (eir_data.data_type)
            {
            case IFX_BT_DEVICE_CLASS:
                IFX_MEMCPY(&bt_record->optional_eir_types.device_class,
                           &eir_data, sizeof(eir_data));
                break;

            case IFX_BT_SIMPLE_PAIRING_HASH_C_192:
            case IFX_BT_SIMPLE_PAIRING_HASH_C_256:
                IFX_MEMCPY(&bt_record->optional_eir_types.simple_pairing_hash_c,
                           &eir_data, sizeof(ifx_record_eir_data_t));
                break;

            case IFX_BT_SIMPLE_PAIRING_RANDOMIZER_R_192:
            case IFX_BT_SIMPLE_PAIRING_RANDOMIZER_R_256:
                IFX_MEMCPY(
                    &bt_record->optional_eir_types.simple_pairing_randomizer_r,
                    &eir_data, sizeof(ifx_record_eir_data_t));
                break;

            case IFX_BT_INCOMPLETE_SERVICE_CLASS_UUID_16_BIT:
            case IFX_BT_COMPLETE_SERVICE_CLASS_UUID_16_BIT:
            case IFX_BT_INCOMPLETE_SERVICE_CLASS_UUID_32_BIT:
            case IFX_BT_COMPLETE_SERVICE_CLASS_UUID_32_BIT:
            case IFX_BT_INCOMPLETE_SERVICE_CLASS_UUID_128_BIT:
            case IFX_BT_COMPLETE_SERVICE_CLASS_UUID_128_BIT:
                IFX_MEMCPY(&bt_record->optional_eir_types.service_class_uuid,
                           &eir_data, sizeof(ifx_record_eir_data_t));
                break;

            case IFX_BT_SHORTENED_LOCAL_NAME:
            case IFX_BT_COMPLETE_LOCAL_NAME:
                IFX_MEMCPY(&bt_record->optional_eir_types.local_name, &eir_data,
                           sizeof(ifx_record_eir_data_t));
                break;

            default:
                IFX_MEMCPY(&bt_record->optional_eir_types
                                .additional_eir_types[count++],
                           &eir_data, sizeof(ifx_record_eir_data_t));
                break;
            }
        }
    }

    if (IFX_SUCCESS == status)
    {
        bt_record->optional_eir_types.count_of_additional_eir_types = count;
    }

    return status;
}
