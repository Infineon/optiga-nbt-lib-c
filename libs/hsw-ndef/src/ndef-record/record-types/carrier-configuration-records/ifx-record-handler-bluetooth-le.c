// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file
 * ndef-record/record-types/carrier-configuration-records/ifx-record-handler-bluetooth-le.c
 * \brief Bluetooth record of encoding/decoding utility.
 * Bluetooth Out-of-Band record data can be exchanged in connection handover
 * request and/or select messages as alternative carrier information.
 */
#include "ifx-bluetooth-core-config.h"
#include "ifx-record-handler-bluetooth-le.h"
#include "infineon/ifx-ndef-lib.h"

/** Macro definitions */
#define BYTE_LENGTH_OF_DATALENGTH_FIELD UINT8_C(1)

/* Static functions */

/**
 * \brief Encodes an advertising and scan response data (AD) type to the payload
 * bytes of data.
 * \param[in] ad_type       Bluetooth low energy (BLE) configuration data field
 *                          is in the format of AD.
 * \param[out] payload      Pointer to the payload bytes of Bluetooth low
 * energy AD.
 * \param[out] index        Index that points to the end of payload data field
 * after encoding.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If encoding into the payload bytes is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 */
static ifx_status_t encode_ad_types_to_payload(ifx_record_ad_data_t ad_type,
                                               uint8_t **payload,
                                               uint32_t *index)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == payload) || (NULL == index))
    {
        status = IFX_ERROR(IFX_RECORD_HANDLER_BLE,
                           IFX_RECORD_HANDLER_BLE_ENCODE, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        if (0 != ad_type.data_length)
        {
            uint8_t *buffer_temp = (uint8_t *) realloc(
                *payload, (*index) + ad_type.data_length +
                              BYTE_LENGTH_OF_DATALENGTH_FIELD);
            if (NULL == buffer_temp)
            {
                IFX_FREE(*payload);
                *payload = NULL;
                status =
                    IFX_ERROR(IFX_RECORD_HANDLER_BLE,
                              IFX_RECORD_HANDLER_BLE_ENCODE, IFX_OUT_OF_MEMORY);
            }
            else
            {
                *payload = buffer_temp;
                (*payload)[(*index)++] = ad_type.data_length;
                (*payload)[(*index)++] = ad_type.data_type;
                IFX_MEMCPY(&((*payload)[(*index)]), ad_type.data,
                           ad_type.data_length -
                               BYTE_LENGTH_OF_DATALENGTH_FIELD);
                *index +=
                    (ad_type.data_length - BYTE_LENGTH_OF_DATALENGTH_FIELD);
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
 * \brief Decodes an advertising and scan response data (AD) type from the
 * payload bytes of data.
 * \param[in] payload       Pointer to the payload bytes of Bluetooth low
 * energy AD.
 * \param[in] index         Index that points to the end of payload data field
 * after decoding.
 * \param[out] ad_type      Bluetooth low energy configuration data field in the
 *                          format of AD.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If decoding into the AD type data is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 */
static ifx_status_t decode_ad_types_from_payload(const uint8_t *payload,
                                                 uint32_t *index,
                                                 ifx_record_ad_data_t *ad_type)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == payload) || (NULL == index) || (NULL == ad_type))
    {
        status = IFX_ERROR(IFX_RECORD_HANDLER_BLE,
                           IFX_RECORD_HANDLER_BLE_DECODE, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        uint8_t offset = UINT8_C(0);
        if (payload[offset] != UINT8_C(0))
        {
            ad_type->data_length = payload[offset++];
            ad_type->data_type = payload[offset++];
            ad_type->data = (uint8_t *) malloc(ad_type->data_length - 1);
            if (NULL != ad_type->data)
            {
                IFX_MEMCPY(ad_type->data, &payload[offset],
                           ad_type->data_length - 1);
                *index += (ad_type->data_length + 1);
            }
            else
            {
                status =
                    IFX_ERROR(IFX_RECORD_HANDLER_BLE,
                              IFX_RECORD_HANDLER_BLE_DECODE, IFX_OUT_OF_MEMORY);
            }
        }
    }
    return status;
}

/* Public functions */

/**
 * \brief Encodes the Bluetooth low energy carrier configuration record data
 * details to the payload.
 * \param[in] record_details    Pointer to the Bluetooth low energy carrier
 *                              configuration record that was updated
 *                              in record handle
 * \param[out] payload          Pointer to the payload byte array of bluetooth
 *                              low energy carrier configuration record
 * \param[out] payload_length   Pointer to the payload length of Bluetooth low
 *                              energy carrier configuration record
 * \return ifx_status_t
 * \retval IFX_SUCCESS If encoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 * \retval IFX_INVALID_STATE If object is in an invalid state
 */
ifx_status_t record_handler_ble_encode(const void *record_details,
                                       uint8_t **payload,
                                       uint32_t *payload_length)
{
    ifx_status_t status = IFX_SUCCESS;
    uint32_t index = UINT32_C(0);

    if ((NULL == payload) || (NULL == payload_length) ||
        (NULL == record_details))
    {
        return IFX_ERROR(IFX_RECORD_HANDLER_BLE, IFX_RECORD_HANDLER_BLE_ENCODE,
                         IFX_ILLEGAL_ARGUMENT);
    }

    const ifx_record_ble_t *btle_record = (ifx_record_ble_t *) record_details;

    *payload = (uint8_t *) malloc(IFX_BLE_DEV_ADDR_LEN);
    if (NULL == *payload)
    {
        status = IFX_ERROR(IFX_RECORD_HANDLER_BLE,
                           IFX_RECORD_HANDLER_BLE_ENCODE, IFX_OUT_OF_MEMORY);
    }
    else
    {
        if (0 != btle_record->device_addr.data_length)
        {
            status = encode_ad_types_to_payload(btle_record->device_addr,
                                                payload, &index);
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_HANDLER_BLE, IFX_RECORD_HANDLER_BLE_ENCODE,
                          IFX_INVALID_STATE);
        }

        if (IFX_SUCCESS == status)
        {

            if (0 != btle_record->role.data_length)
            {
                status = encode_ad_types_to_payload(btle_record->role, payload,
                                                    &index);
            }
            else
            {
                status =
                    IFX_ERROR(IFX_RECORD_HANDLER_BLE,
                              IFX_RECORD_HANDLER_BLE_ENCODE, IFX_INVALID_STATE);
            }
        }

        if (IFX_SUCCESS == status)
        {
            status = encode_ad_types_to_payload(
                btle_record->optional_ad_types.security_manager_tk_val, payload,
                &index);
        }

        if (IFX_SUCCESS == status)
        {
            status = encode_ad_types_to_payload(
                btle_record->optional_ad_types.secure_conn_confirmation_val,
                payload, &index);
        }

        if (IFX_SUCCESS == status)
        {
            status = encode_ad_types_to_payload(
                btle_record->optional_ad_types.secure_conn_random_val, payload,
                &index);
        }

        if (IFX_SUCCESS == status)
        {
            status = encode_ad_types_to_payload(
                btle_record->optional_ad_types.appearance, payload, &index);
        }

        if (IFX_SUCCESS == status)
        {
            status = encode_ad_types_to_payload(
                btle_record->optional_ad_types.flags, payload, &index);
        }

        if (IFX_SUCCESS == status)
        {
            status = encode_ad_types_to_payload(
                btle_record->optional_ad_types.local_name, payload, &index);
        }

        if (IFX_SUCCESS == status)
        {
            if (0 !=
                btle_record->optional_ad_types.count_of_additional_ad_types)
            {
                uint32_t count = UINT32_C(0);
                while (
                    count <
                    btle_record->optional_ad_types.count_of_additional_ad_types)
                {
                    if (IFX_SUCCESS == status)
                    {
                        status = encode_ad_types_to_payload(
                            btle_record->optional_ad_types
                                .additional_ad_types[count++],
                            payload, &index);
                    }
                }
            }
        }

        if (IFX_SUCCESS == status)
        {
            *payload_length = index;
        }
    }

    return status;
}

/**
 * \brief Decodes the NDEF record payload to Bluetooth low energy carrier
 * configuration record.
 * \param[in] payload           Pointer to the payload byte array of Bluetooth
 * low energy carrier configuration record
 * \param[in] payload_length    Pointer to the payload length of Bluetooth low
 * energy carrier configuration record
 * \param[out] record_details   Pointer to the Bluetooth low energy carrier
 * configuration record details that needs to be updated from record handle
 * \return ifx_status_t
 * \retval IFX_SUCCESS If decoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_INVALID_STATE If object is in an invalid state
 */
ifx_status_t record_handler_ble_decode(const uint8_t *payload,
                                       uint32_t payload_length,
                                       void *record_details)
{
    ifx_status_t status = IFX_SUCCESS;
    uint32_t index = UINT32_C(0);
    uint32_t count = UINT32_C(0);
    ifx_record_ble_t *btle_record = (ifx_record_ble_t *) record_details;

    if ((NULL == record_details) || (NULL == payload))
    {
        return IFX_ERROR(IFX_RECORD_HANDLER_BLE, IFX_RECORD_HANDLER_BLE_DECODE,
                         IFX_ILLEGAL_ARGUMENT);
    }

    ifx_record_ad_data_t dev_addr_ad_data;
    status = decode_ad_types_from_payload(&payload[index], &index,
                                          &dev_addr_ad_data);
    if (IFX_SUCCESS == status)
    {
        if (0 == dev_addr_ad_data.data_length)
        {
            status =
                IFX_ERROR(IFX_RECORD_HANDLER_BLE, IFX_RECORD_HANDLER_BLE_DECODE,
                          IFX_INVALID_STATE);
        }
        else
        {
            IFX_MEMCPY(&btle_record->device_addr, &dev_addr_ad_data,
                       sizeof(dev_addr_ad_data));
        }
    }

    if (IFX_SUCCESS == status)
    {
        ifx_record_ad_data_t role_ad_data;
        status = decode_ad_types_from_payload(&payload[index], &index,
                                              &role_ad_data);
        if (IFX_SUCCESS == status)
        {
            if (0 == role_ad_data.data_length)
            {
                status =
                    IFX_ERROR(IFX_RECORD_HANDLER_BLE,
                              IFX_RECORD_HANDLER_BLE_DECODE, IFX_INVALID_STATE);
            }
            else
            {
                IFX_MEMCPY(&btle_record->role, &role_ad_data,
                           sizeof(role_ad_data));
            }
        }
    }

    while ((index < payload_length) && (IFX_SUCCESS == status))
    {
        ifx_record_ad_data_t ad_data;
        status =
            decode_ad_types_from_payload(&payload[index], &index, &ad_data);
        if (IFX_SUCCESS == status)
        {
            switch (ad_data.data_type)
            {
            case IFX_BT_SECURITY_MANAGER_TK_VALUE:
                IFX_MEMCPY(
                    &btle_record->optional_ad_types.security_manager_tk_val,
                    &ad_data, sizeof(ifx_record_ad_data_t));
                break;

            case IFX_BLE_SECURE_CONN_CONFIRM_VALUE:
                IFX_MEMCPY(&btle_record->optional_ad_types
                                .secure_conn_confirmation_val,
                           &ad_data, sizeof(ifx_record_ad_data_t));
                break;

            case IFX_BLE_SECURE_CONN_RANDOM_VALUE:
                IFX_MEMCPY(
                    &btle_record->optional_ad_types.secure_conn_random_val,
                    &ad_data, sizeof(ifx_record_ad_data_t));
                break;

            case IFX_BT_APPEARANCE:
                IFX_MEMCPY(&btle_record->optional_ad_types.appearance, &ad_data,
                           sizeof(ifx_record_ad_data_t));
                break;

            case IFX_BT_FLAGS:
                IFX_MEMCPY(&btle_record->optional_ad_types.flags, &ad_data,
                           sizeof(ifx_record_ad_data_t));
                break;

            case IFX_BT_SHORTENED_LOCAL_NAME:
            case IFX_BT_COMPLETE_LOCAL_NAME:
                IFX_MEMCPY(&btle_record->optional_ad_types.local_name, &ad_data,
                           sizeof(ifx_record_ad_data_t));
                break;

            default:
                IFX_MEMCPY(&btle_record->optional_ad_types
                                .additional_ad_types[count++],
                           &ad_data, sizeof(ifx_record_ad_data_t));
                break;
            }
        }
    }

    if (IFX_SUCCESS == status)
    {
        btle_record->optional_ad_types.count_of_additional_ad_types = count;
    }

    return status;
}
