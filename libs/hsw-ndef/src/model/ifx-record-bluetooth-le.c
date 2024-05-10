// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file model/ifx-record-bluetooth-le.c
 * \brief Model interface to create the Bluetooth low energy (BLE) carrier
 * configuration record types and set/get record fields.
 */
#include "ifx-bluetooth-core-config.h"
#include "ifx-record-handler-bluetooth-le.h"
#include "infineon/ifx-ndef-errors.h"
#include "infineon/ifx-ndef-lib.h"
#include "infineon/ifx-ndef-record.h"
#include "infineon/ifx-record-bluetooth-le.h"
#include "infineon/ifx-record-handler.h"

/**
 * \brief Macro to check if advertising and scan response data (AD) parameters
 * are invalid.
 */
#define CHECK_IF_AD_DATA_IS_INVALID(ad)                                        \
    (((ad).data_length == 0) || ((ad).data == NULL))

/* Static functions */

/**
 * \brief Frames AD data type from the Bluetooth configuration data.
 * \param[out] ad_data     AD data field
 * \param[in] data_type    Type field of Bluetooth configuration data.
 * \param[in] config       Data field of Bluetooth configuration data.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If framing of AD data operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation fails
 */
static ifx_status_t frame_ad_data(ifx_record_ad_data_t *ad_data,
                                  uint8_t data_type,
                                  const ifx_ble_config_field_t *config)

{
    if ((NULL == ad_data) || (NULL == config))
    {
        return IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_SET,
                         IFX_ILLEGAL_ARGUMENT);
    }
    ad_data->data_type = data_type;

    // Additional 0x01 byte for size of data length field.
    ad_data->data_length = config->data_len + 0x01;
    ad_data->data = (uint8_t *) malloc(config->data_len);
    if (NULL == ad_data->data)
    {
        return IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_SET, IFX_OUT_OF_MEMORY);
    }

    IFX_MEMCPY(ad_data->data, config->data, config->data_len);

    return IFX_SUCCESS;
}

/**
 * \brief Extracts the Bluetooth configuration data from the AD data.
 * \param[in] ad_data      AD data field
 * \param[out] data_type   Type field of Bluetooth configuration data
 * \param[out] config      Data field of Bluetooth configuration data
 * \return ifx_status_t
 * \retval IFX_SUCCESS If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation fails
 */
static ifx_status_t extract_data_from_ad_type(
    const ifx_record_ad_data_t *ad_data, uint8_t *data_type,
    ifx_ble_config_field_t *config)
{
    if ((NULL == ad_data) || (NULL == config) || (NULL == data_type))
    {
        return IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET,
                         IFX_ILLEGAL_ARGUMENT);
    }
    *data_type = ad_data->data_type;

    // Reduced 0x01 byte for size of data length field.
    config->data_len = ad_data->data_length - 0x01;
    config->data = (uint8_t *) malloc(config->data_len);
    if (NULL == config->data)
    {
        return IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET, IFX_OUT_OF_MEMORY);
    }

    IFX_MEMCPY(config->data, ad_data->data, config->data_len);

    return IFX_SUCCESS;
}

/**
 * @brief Release all the allocated memory for the created bluetooth low energy
 * record data
 *
 * @param[in] record_data    data of the bluetooth low energy record
 * \retval IFX_SUCCESS If memory release operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 */
static ifx_status_t record_ble_deinit(void *record_data)
{
    if (NULL == record_data)
    {
        return IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_RELEASE_MEMORY,
                         IFX_ILLEGAL_ARGUMENT);
    }
    ifx_record_ble_t *ble_record = (ifx_record_ble_t *) (record_data);

    if (ble_record->device_addr.data != NULL)
    {
        IFX_FREE(ble_record->device_addr.data);
        ble_record->device_addr.data = NULL;
    }

    if (ble_record->role.data != NULL)
    {
        IFX_FREE(ble_record->role.data);
        ble_record->role.data = NULL;
    }

    if (ble_record->optional_ad_types.appearance.data != NULL)
    {
        IFX_FREE(ble_record->optional_ad_types.appearance.data);
        ble_record->optional_ad_types.appearance.data = NULL;
    }

    if (ble_record->optional_ad_types.flags.data != NULL)
    {
        IFX_FREE(ble_record->optional_ad_types.flags.data);
        ble_record->optional_ad_types.flags.data = NULL;
    }

    if (ble_record->optional_ad_types.local_name.data != NULL)
    {
        IFX_FREE(ble_record->optional_ad_types.local_name.data);
        ble_record->optional_ad_types.local_name.data = NULL;
    }

    if (ble_record->optional_ad_types.secure_conn_confirmation_val.data != NULL)
    {
        IFX_FREE(
            ble_record->optional_ad_types.secure_conn_confirmation_val.data);
        ble_record->optional_ad_types.secure_conn_confirmation_val.data = NULL;
    }

    if (ble_record->optional_ad_types.secure_conn_random_val.data != NULL)
    {
        IFX_FREE(ble_record->optional_ad_types.secure_conn_random_val.data);
        ble_record->optional_ad_types.secure_conn_random_val.data = NULL;
    }

    if (ble_record->optional_ad_types.security_manager_tk_val.data != NULL)
    {
        IFX_FREE(ble_record->optional_ad_types.security_manager_tk_val.data);
        ble_record->optional_ad_types.security_manager_tk_val.data = NULL;
    }
    uint32_t index = 0;
    while (index < ble_record->optional_ad_types.count_of_additional_ad_types)
    {
        if (ble_record->optional_ad_types.additional_ad_types[index].data !=
            NULL)
        {
            IFX_FREE(
                ble_record->optional_ad_types.additional_ad_types[index].data);
            ble_record->optional_ad_types.additional_ad_types[index].data =
                NULL;
        }
        index++;
    }
    if (ble_record->optional_ad_types.additional_ad_types != NULL)
    {
        IFX_FREE(ble_record->optional_ad_types.additional_ad_types);
        ble_record->optional_ad_types.additional_ad_types = NULL;
    }

    return IFX_SUCCESS;
}

/* Public functions */

/**
 * \brief Creates the Bluetooth low energy carrier configuration record and
 * handle of the created record. This handle holds the values of the record
 * needed for encode and decode operations.
 *
 * \param[out] handle Handle of the created Bluetooth low energy carrier
 * configuration record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If record creation operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation fails
 */
ifx_status_t ifx_record_ble_new(ifx_record_handle_t *handle)
{
    ifx_status_t status = IFX_SUCCESS;
    if (NULL != handle)
    {
        const uint8_t type[] = IFX_RECORD_BLE_TYPE;
        handle->tnf = IFX_RECORD_TNF_TYPE_MEDIA;
        handle->type.length = sizeof(type) - 1;
        handle->type.buffer = (uint8_t *) malloc(handle->type.length);
        if (NULL != handle->type.buffer)
        {
            handle->id.buffer = NULL;
            handle->id.length = IFX_NDEF_ID_LEN_FIELD_NONE;
            IFX_MEMCPY(handle->type.buffer, type, handle->type.length);
            handle->encode_record = record_handler_ble_encode;
            handle->decode_record = record_handler_ble_decode;
            handle->deinit_record = record_ble_deinit;

            ifx_record_ble_t *btle_record =
                (ifx_record_ble_t *) malloc(sizeof(ifx_record_ble_t));
            if (NULL != btle_record)
            {
                IFX_MEMSET(&btle_record->optional_ad_types, 0,
                           sizeof(btle_record->optional_ad_types));
                handle->record_data = (void *) btle_record;
            }
            else
            {
                IFX_FREE(handle->type.buffer);
                handle->type.buffer = NULL;
                status = IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_SET,
                                   IFX_OUT_OF_MEMORY);
            }
        }
        else
        {
            status = IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_SET,
                               IFX_OUT_OF_MEMORY);
        }
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_SET, IFX_ILLEGAL_ARGUMENT);
    }

    return status;
}

/**
 * \brief Sets the device address in the Bluetooth low energy (BLE) carrier
 * configuration record for the given record handle.
 * \param[out] handle           Pointer to the record handle obtained while
 * creating the record.
 * \param[in] device_addr       6 octets of the Bluetooth low energy device
 * address are encoded in the Little Endian order.
 * \param[in] device_addr_type  Specifies whether the device address is
 * public(0) or random(1).
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation fails
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_ble_set_device_addr(
    ifx_record_handle_t *handle, const uint8_t *device_addr,
    ifx_ble_device_addr_type device_addr_type)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == device_addr))
    {
        status =
            IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_SET, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        const uint8_t type[] = IFX_RECORD_BLE_TYPE;
        if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
        {
            ((ifx_record_ble_t *) handle->record_data)
                ->device_addr.data_length =
                IFX_RECORD_TYPE_LEN_BLE_DEV_ADDR + IFX_BLE_DEV_ADDR_LEN;
            ((ifx_record_ble_t *) handle->record_data)->device_addr.data_type =
                IFX_BT_LE_DEVICE_ADDRESS;
            ((ifx_record_ble_t *) handle->record_data)->device_addr.data =
                (uint8_t *) malloc(IFX_BLE_DEV_ADDR_LEN);
            if (NULL !=
                ((ifx_record_ble_t *) handle->record_data)->device_addr.data)
            {
                IFX_MEMCPY(((ifx_record_ble_t *) handle->record_data)
                               ->device_addr.data,
                           device_addr, IFX_BLE_DEV_ADDR_LEN - 0x01);
                ((ifx_record_ble_t *) handle->record_data)
                    ->device_addr.data[IFX_BLE_DEV_ADDR_LEN - 0x01] =
                    (uint8_t) device_addr_type;
            }
            else
            {
                status = IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_SET,
                                   IFX_OUT_OF_MEMORY);
            }
        }
        else
        {
            status = IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_SET,
                               IFX_RECORD_INVALID);
        }
    }

    return status;
}

/**
 * \brief Sets the low energy (LE) role in the Bluetooth low energy carrier
 * configuration record for the given record handle.
 * \param[out] handle           Pointer to the record handle obtained while
 * creating the record.
 * \param[in] role              Pointer to the low energy role.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_ble_set_role(ifx_record_handle_t *handle,
                                     const ifx_ble_config_field_t *role)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == role))
    {
        return IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_SET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    const uint8_t type[] = IFX_RECORD_BLE_TYPE;
    if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
    {
        status =
            frame_ad_data(&((ifx_record_ble_t *) handle->record_data)->role,
                          IFX_BLE_ROLE, role);
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_SET, IFX_RECORD_INVALID);
    }

    return status;
}

/**
 * \brief Sets the security manager temporary key (TK) value in the Bluetooth
 * low energy carrier configuration record for the given record handle.
 * \param[out] handle                 Pointer to the record handle obtained
 * while creating the record.
 * \param[in] security_manager_tk_val Pointer to the low energy (BLE) security
 * manager TK value.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_ble_set_security_tk_val(
    ifx_record_handle_t *handle,
    const ifx_ble_config_field_t *security_manager_tk_val)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == security_manager_tk_val))
    {
        return IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_SET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    const uint8_t type[] = IFX_RECORD_BLE_TYPE;
    if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
    {
        status = frame_ad_data(&((ifx_record_ble_t *) handle->record_data)
                                    ->optional_ad_types.security_manager_tk_val,
                               IFX_BT_SECURITY_MANAGER_TK_VALUE,
                               security_manager_tk_val);
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_SET, IFX_RECORD_INVALID);
    }

    return status;
}

/**
 * \brief Sets the secure connections confirmation value in the Bluetooth low
 * energy carrier configuration record for the given record handle.
 * \param[out] handle                      Pointer to the record handle obtained
 * while creating the record.
 * \param[in] secure_conn_confirmation_val Pointer to the BLE secure connections
 * confirmation value
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_ble_set_secure_conn_confirm(
    ifx_record_handle_t *handle,
    const ifx_ble_config_field_t *secure_conn_confirmation_val)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == secure_conn_confirmation_val))
    {
        return IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_SET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    const uint8_t type[] = IFX_RECORD_BLE_TYPE;
    if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
    {
        status = frame_ad_data(
            &((ifx_record_ble_t *) handle->record_data)
                 ->optional_ad_types.secure_conn_confirmation_val,
            IFX_BLE_SECURE_CONN_CONFIRM_VALUE, secure_conn_confirmation_val);
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_SET, IFX_RECORD_INVALID);
    }

    return status;
}

/**
 * \brief Sets the secure connections random value in the Bluetooth low energy
 * carrier configuration record for the given record handle.
 * \param[out] handle                   Pointer to the record handle obtained
 * while creating the record.
 * \param[in] secure_conn_random_val    Pointer to the BLE secure connections
 * random value.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_ble_set_secure_conn_random(
    ifx_record_handle_t *handle,
    const ifx_ble_config_field_t *secure_conn_random_val)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == secure_conn_random_val))
    {
        return IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_SET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    const uint8_t type[] = IFX_RECORD_BLE_TYPE;
    if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
    {
        status = frame_ad_data(&((ifx_record_ble_t *) handle->record_data)
                                    ->optional_ad_types.secure_conn_random_val,
                               IFX_BLE_SECURE_CONN_RANDOM_VALUE,
                               secure_conn_random_val);
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_SET, IFX_RECORD_INVALID);
    }

    return status;
}

/**
 * \brief Sets an appearance in the Bluetooth low energy carrier configuration
 * record for the given record handle.
 * \param[out] handle     Pointer to the record handle obtained while creating
 * the record.
 * \param[in] appearance  Pointer to the BLE appearance.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_ble_set_appearance(
    ifx_record_handle_t *handle, const ifx_ble_config_field_t *appearance)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == appearance))
    {
        return IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_SET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    const uint8_t type[] = IFX_RECORD_BLE_TYPE;
    if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
    {
        status = frame_ad_data(&((ifx_record_ble_t *) handle->record_data)
                                    ->optional_ad_types.appearance,
                               IFX_BT_APPEARANCE, appearance);
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_SET, IFX_RECORD_INVALID);
    }

    return status;
}

/**
 * \brief Sets the flags in the Bluetooth low energy carrier configuration
 * record for the given record handle.
 * \param[out] handle               Pointer to the record handle obtained while
 * creating the record.
 * \param[in] flags                 Pointer to the LE flags.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_ble_set_flags(ifx_record_handle_t *handle,
                                      const ifx_ble_config_field_t *flags)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == flags))
    {
        return IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_SET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    const uint8_t type[] = IFX_RECORD_BLE_TYPE;
    if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
    {
        status = frame_ad_data(&((ifx_record_ble_t *) handle->record_data)
                                    ->optional_ad_types.flags,
                               IFX_BT_FLAGS, flags);
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_SET, IFX_RECORD_INVALID);
    }

    return status;
}

/**
 * \brief Sets the local name in the Bluetooth low energy carrier configuration
 * record for the given record handle.
 * \param[out] handle                       Pointer to the record handle
 * obtained while creating the record.
 * \param[in] config_type                   Type of the local name
 * (SHORTENED_LOCAL_NAME or COMPLETE_LOCAL_NAME)
 * \param[in] local_name                    Pointer to the Bluetooth low energy
 * local name.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_ble_set_local_name(
    ifx_record_handle_t *handle, uint8_t config_type,
    const ifx_ble_config_field_t *local_name)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == local_name) ||
        !((config_type == IFX_BT_SHORTENED_LOCAL_NAME) ||
          (config_type == IFX_BT_COMPLETE_LOCAL_NAME)))
    {
        return IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_SET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    const uint8_t type[] = IFX_RECORD_BLE_TYPE;
    if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
    {
        status = frame_ad_data(&((ifx_record_ble_t *) handle->record_data)
                                    ->optional_ad_types.local_name,
                               config_type, local_name);
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_SET, IFX_RECORD_INVALID);
    }

    return status;
}

/**
 * \brief Sets the array of additional AD data in the Bluetooth low energy
 * carrier configuration record for the given record handle.
 * \param[out] handle                       Pointer to the record handle
 * obtained while creating the record.
 * \param[in] additional_data               Pointer to the array of additional
 * AD data.
 * \param[in] count                         Count of the additional data.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation fails
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_ble_set_additional_ad_data(
    ifx_record_handle_t *handle, const ifx_record_ad_data_t *additional_data,
    uint32_t count)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == additional_data))
    {
        return IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_SET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    const uint8_t type[] = IFX_RECORD_BLE_TYPE;
    if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
    {
        uint32_t index = UINT32_C(0);
        uint32_t size = UINT32_C(0);
        IFX_UNUSED_VARIABLE(size);
        while (index < count)
        {
            size += sizeof(additional_data[index]);
            index++;
        }
        ((ifx_record_ble_t *) handle->record_data)
            ->optional_ad_types.additional_ad_types =
            (ifx_record_ad_data_t *) malloc(size);
        if (NULL != ((ifx_record_ble_t *) handle->record_data)
                        ->optional_ad_types.additional_ad_types)
        {
            index = 0;
            while (index < count)
            {
                ((ifx_record_ble_t *) handle->record_data)
                    ->optional_ad_types.additional_ad_types[index]
                    .data_length = additional_data[index].data_length;
                ((ifx_record_ble_t *) handle->record_data)
                    ->optional_ad_types.additional_ad_types[index]
                    .data_type = additional_data[index].data_type;
                ((ifx_record_ble_t *) handle->record_data)
                    ->optional_ad_types.additional_ad_types[index]
                    .data =
                    (uint8_t *) malloc(additional_data[index].data_length - 1);
                if (NULL != ((ifx_record_ble_t *) handle->record_data)
                                ->optional_ad_types.additional_ad_types[index]
                                .data)
                {
                    IFX_MEMCPY(
                        ((ifx_record_ble_t *) handle->record_data)
                            ->optional_ad_types.additional_ad_types[index]
                            .data,
                        additional_data[index].data,
                        additional_data[index].data_length - 1);
                    index++;
                }
                else
                {
                    IFX_FREE(((ifx_record_ble_t *) handle->record_data)
                                 ->optional_ad_types.additional_ad_types);
                    ((ifx_record_ble_t *) handle->record_data)
                        ->optional_ad_types.additional_ad_types = NULL;
                    status = IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_SET,
                                       IFX_OUT_OF_MEMORY);
                }
            }
            ((ifx_record_ble_t *) handle->record_data)
                ->optional_ad_types.count_of_additional_ad_types = count;
        }
        else
        {
            status = IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_SET,
                               IFX_OUT_OF_MEMORY);
        }
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_SET, IFX_RECORD_INVALID);
    }

    return status;
}

/**
 * \brief Gets the device address of the Bluetooth low energy carrier
 * configuration record from the given record handle.
 * \param[in] handle                Pointer to the record handle obtained while
 *                                  creating the record.
 * \param[out] device_addr          6 octets of the Bluetooth low energy device
 *                                  address are encoded in the Little Endian
 * order.
 * \param[out] device_addr_type     Pointer to the type that specifies whether
 *                                  the device address is public(0) or random(1)
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_DATA_FIELD_NA If data field is invalid
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_ble_get_device_addr(
    const ifx_record_handle_t *handle, uint8_t *device_addr,
    ifx_ble_device_addr_type *device_addr_type)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == device_addr) || (NULL == device_addr_type))
    {
        status =
            IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        const uint8_t type[] = IFX_RECORD_BLE_TYPE;
        if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
        {
            if (CHECK_IF_AD_DATA_IS_INVALID(
                    ((ifx_record_ble_t *) handle->record_data)->device_addr))
            {
                status = IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET,
                                   IFX_RECORD_DATA_FIELD_NA);
            }
            else
            {
                uint8_t config_type = UINT8_C(0);
                ifx_ble_config_field_t device_addr_config_field;
                IFX_MEMSET(&device_addr_config_field, 0x00,
                           sizeof(ifx_ble_config_field_t));
                status = extract_data_from_ad_type(
                    &((ifx_record_ble_t *) handle->record_data)->device_addr,
                    &config_type, &device_addr_config_field);
                if ((config_type != IFX_BT_LE_DEVICE_ADDRESS) &&
                    (IFX_SUCCESS == status))
                {
                    IFX_FREE(device_addr_config_field.data);
                    device_addr_config_field.data = NULL;
                    status = IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET,
                                       IFX_INVALID_STATE);
                }
                else
                {
                    IFX_MEMCPY(device_addr, device_addr_config_field.data,
                               device_addr_config_field.data_len - 1);
                    *device_addr_type =
                        (ifx_ble_device_addr_type) ((ifx_record_ble_t *)
                                                        handle->record_data)
                            ->device_addr
                            .data[device_addr_config_field.data_len - 1];
                    IFX_FREE(device_addr_config_field.data);
                    device_addr_config_field.data = NULL;
                }
            }
        }
        else
        {
            status = IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET,
                               IFX_RECORD_INVALID);
        }
    }

    return status;
}

/**
 * \brief Gets the low energy (LE) role of the Bluetooth low energy carrier
 * configuration record from the given record handle.
 * \param[in] handle            Pointer to the record handle obtained while
 *                              creating the record.
 * \param[out] role             Pointer to the low energy role.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_DATA_FIELD_NA If the data field is invalid
 * \retval IFX_RECORD_INVALID If record type is invalid
 * \retval IFX_INVALID_STATE If the object is in an invalid state
 */
ifx_status_t ifx_record_ble_get_role(const ifx_record_handle_t *handle,
                                     ifx_ble_config_field_t *role)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == role))
    {
        status =
            IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        const uint8_t type[] = IFX_RECORD_BLE_TYPE;
        if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
        {
            if (CHECK_IF_AD_DATA_IS_INVALID(
                    ((ifx_record_ble_t *) handle->record_data)->role))
            {
                status = IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET,
                                   IFX_RECORD_DATA_FIELD_NA);
            }
            else
            {
                uint8_t config_type = UINT8_C(0);
                status = extract_data_from_ad_type(
                    &((ifx_record_ble_t *) handle->record_data)->role,
                    &config_type, role);
                if ((config_type != IFX_BLE_ROLE) && (IFX_SUCCESS == status))
                {
                    status = IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET,
                                       IFX_INVALID_STATE);
                }
            }
        }
        else
        {
            status = IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET,
                               IFX_RECORD_INVALID);
        }
    }

    return status;
}

/**
 * \brief Gets the security manager TK value of the Bluetooth low energy carrier
 * configuration record from the given record handle.
 * \param[in] handle                   Pointer to the record handle obtained
 *                                     while creating the record.
 * \param[out] security_manager_tk_val Pointer to the BLE security manager TK
 * value.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_DATA_FIELD_NA If the data field is invalid
 * \retval IFX_RECORD_INVALID If record type is invalid
 * \retval IFX_INVALID_STATE If the object is in an invalid state
 */
ifx_status_t ifx_record_ble_get_security_tk_val(
    const ifx_record_handle_t *handle,
    ifx_ble_config_field_t *security_manager_tk_val)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == security_manager_tk_val))
    {
        return IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    const uint8_t type[] = IFX_RECORD_BLE_TYPE;
    if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
    {
        if (CHECK_IF_AD_DATA_IS_INVALID(
                ((ifx_record_ble_t *) handle->record_data)
                    ->optional_ad_types.security_manager_tk_val))
        {
            status = IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET,
                               IFX_RECORD_DATA_FIELD_NA);
        }
        else
        {
            uint8_t config_type = UINT8_C(0);
            status = extract_data_from_ad_type(
                &((ifx_record_ble_t *) handle->record_data)
                     ->optional_ad_types.security_manager_tk_val,
                &config_type, security_manager_tk_val);
            if ((config_type != IFX_BT_SECURITY_MANAGER_TK_VALUE) &&
                (IFX_SUCCESS == status))
            {
                status = IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET,
                                   IFX_INVALID_STATE);
            }
        }
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET, IFX_RECORD_INVALID);
    }

    return status;
}

/**
 * \brief Gets the secure connections confirmation value of the Bluetooth low
 * energy carrier configuration record from the given record handle.
 * \param[in] handle                           Pointer to the record handle
 * obtained while creating the record.
 * \param[out] secure_conn_confirmation_val    Pointer to the BLE secure
 * connections confirmation value.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_DATA_FIELD_NA If the data field is invalid
 * \retval IFX_RECORD_INVALID If record type is invalid
 * \retval IFX_INVALID_STATE If the object is in an invalid state
 */
ifx_status_t ifx_record_ble_get_secure_conn_confirm(
    const ifx_record_handle_t *handle,
    ifx_ble_config_field_t *secure_conn_confirmation_val)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == secure_conn_confirmation_val))
    {
        return IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    const uint8_t type[] = IFX_RECORD_BLE_TYPE;
    if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
    {
        if (CHECK_IF_AD_DATA_IS_INVALID(
                ((ifx_record_ble_t *) handle->record_data)
                    ->optional_ad_types.secure_conn_confirmation_val))
        {
            status = IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET,
                               IFX_RECORD_DATA_FIELD_NA);
        }
        else
        {
            uint8_t config_type = UINT8_C(0);
            status = extract_data_from_ad_type(
                &((ifx_record_ble_t *) handle->record_data)
                     ->optional_ad_types.secure_conn_confirmation_val,
                &config_type, secure_conn_confirmation_val);
            if ((config_type != IFX_BLE_SECURE_CONN_CONFIRM_VALUE) &&
                (IFX_SUCCESS == status))
            {
                status = IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET,
                                   IFX_INVALID_STATE);
            }
        }
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET, IFX_RECORD_INVALID);
    }

    return status;
}

/**
 * \brief Gets the secure connections random value of the Bluetooth low energy
 * carrier configuration record from the given record handle.
 * \param[in] handle                  Pointer to the record handle obtained
 *                                    while creating the record.
 * \param[out] secure_conn_random_val Pointer to the BLE secure connections
 * random value.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_DATA_FIELD_NA If the data field is invalid
 * \retval IFX_RECORD_INVALID If record type is invalid
 * \retval IFX_INVALID_STATE If the object is in an invalid state
 */
ifx_status_t ifx_record_ble_get_secure_conn_random(
    const ifx_record_handle_t *handle,
    ifx_ble_config_field_t *secure_conn_random_val)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == secure_conn_random_val))
    {
        return IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    const uint8_t type[] = IFX_RECORD_BLE_TYPE;
    if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
    {
        if (CHECK_IF_AD_DATA_IS_INVALID(
                ((ifx_record_ble_t *) handle->record_data)
                    ->optional_ad_types.secure_conn_random_val))
        {
            status = IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET,
                               IFX_RECORD_DATA_FIELD_NA);
        }
        else
        {
            uint8_t config_type = UINT8_C(0);
            status = extract_data_from_ad_type(
                &((ifx_record_ble_t *) handle->record_data)
                     ->optional_ad_types.secure_conn_random_val,
                &config_type, secure_conn_random_val);
            if ((config_type != IFX_BLE_SECURE_CONN_RANDOM_VALUE) &&
                (IFX_SUCCESS == status))
            {
                status = IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET,
                                   IFX_INVALID_STATE);
            }
        }
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET, IFX_RECORD_INVALID);
    }

    return status;
}

/**
 * \brief Gets the appearance of the Bluetooth low energy carrier configuration
 * record from the given record handle.
 * \param[in] handle                Pointer to the record handle obtained while
 *                                  creating the record.
 * \param[out] appearance           Pointer to the BLE appearance.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_DATA_FIELD_NA If the data field is invalid
 * \retval IFX_RECORD_INVALID If record type is invalid
 * \retval IFX_INVALID_STATE If the object is in an invalid state
 */
ifx_status_t ifx_record_ble_get_appearance(const ifx_record_handle_t *handle,
                                           ifx_ble_config_field_t *appearance)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == appearance))
    {
        return IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    const uint8_t type[] = IFX_RECORD_BLE_TYPE;
    if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
    {
        if (CHECK_IF_AD_DATA_IS_INVALID(
                ((ifx_record_ble_t *) handle->record_data)
                    ->optional_ad_types.appearance))
        {
            status = IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET,
                               IFX_RECORD_DATA_FIELD_NA);
        }
        else
        {
            uint8_t config_type = UINT8_C(0);
            status = extract_data_from_ad_type(
                &((ifx_record_ble_t *) handle->record_data)
                     ->optional_ad_types.appearance,
                &config_type, appearance);
            if ((config_type != IFX_BT_APPEARANCE) && (IFX_SUCCESS == status))
            {
                status = IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET,
                                   IFX_INVALID_STATE);
            }
        }
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET, IFX_RECORD_INVALID);
    }

    return status;
}

/**
 * \brief Gets the flags of the Bluetooth low energy carrier configuration
 * record from the given record handle.
 * \param[in] handle                Pointer to the record handle obtained while
 *                                  creating the record.
 * \param[out] flags                Pointer to the LE flags.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_DATA_FIELD_NA If the data field is invalid
 * \retval IFX_RECORD_INVALID If record type is invalid
 * \retval IFX_INVALID_STATE If the object is in an invalid state
 */
ifx_status_t ifx_record_ble_get_flags(const ifx_record_handle_t *handle,
                                      ifx_ble_config_field_t *flags)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == flags))
    {
        return IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    const uint8_t type[] = IFX_RECORD_BLE_TYPE;
    if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
    {
        if (CHECK_IF_AD_DATA_IS_INVALID(
                ((ifx_record_ble_t *) handle->record_data)
                    ->optional_ad_types.flags))
        {
            status = IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET,
                               IFX_RECORD_DATA_FIELD_NA);
        }
        else
        {
            uint8_t config_type = UINT8_C(0);
            status = extract_data_from_ad_type(
                &((ifx_record_ble_t *) handle->record_data)
                     ->optional_ad_types.flags,
                &config_type, flags);
            if ((config_type != IFX_BT_FLAGS) && (IFX_SUCCESS == status))
            {
                status = IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET,
                                   IFX_INVALID_STATE);
            }
        }
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET, IFX_RECORD_INVALID);
    }

    return status;
}

/**
 * \brief Gets the local name of the Bluetooth low energy carrier configuration
 * record from the given record handle.
 * \param[in] handle                Pointer to the record handle obtained while
 *                                  creating the record.
 * \param[out] config_type          Type of the local name (SHORTENED_LOCAL_NAME
 * or COMPLETE_LOCAL_NAME)
 * \param[out] local_name           Pointer to the Bluetooth low energy local
 * name.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_DATA_FIELD_NA If the data field is invalid
 * \retval IFX_RECORD_INVALID If record type is invalid
 * \retval IFX_INVALID_STATE If the object is in an invalid state
 */
ifx_status_t ifx_record_ble_get_local_name(const ifx_record_handle_t *handle,
                                           uint8_t *config_type,
                                           ifx_ble_config_field_t *local_name)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == local_name) || (NULL == config_type))
    {
        return IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    const uint8_t type[] = IFX_RECORD_BLE_TYPE;
    if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
    {
        if (CHECK_IF_AD_DATA_IS_INVALID(
                ((ifx_record_ble_t *) handle->record_data)
                    ->optional_ad_types.local_name))
        {
            status = IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET,
                               IFX_RECORD_DATA_FIELD_NA);
        }
        else
        {
            status = extract_data_from_ad_type(
                &((ifx_record_ble_t *) handle->record_data)
                     ->optional_ad_types.local_name,
                config_type, local_name);
            if ((IFX_SUCCESS == status) &&
                !((IFX_BT_SHORTENED_LOCAL_NAME == *config_type) ||
                  (IFX_BT_COMPLETE_LOCAL_NAME == *config_type)))
            {
                status = IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET,
                                   IFX_INVALID_STATE);
            }
        }
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET, IFX_RECORD_INVALID);
    }

    return status;
}

/**
 * \brief Gets the array of additional AD data of the Bluetooth low energy
 * carrier configuration record from the given record handle.
 * \param[in] handle                        Pointer to the record handle
 * obtained while creating the record.
 * \param[out] additional_data              Pointer to the array of additional
 * AD data.
 * \param[out] count                        Pointer to the count of additional
 * data.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_DATA_FIELD_NA If the data field is invalid
 * \retval IFX_OUT_OF_MEMORY If memory allocation fails
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_ble_get_additional_ad_data(
    const ifx_record_handle_t *handle, ifx_record_ad_data_t *additional_data,
    uint32_t *count)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == additional_data) || (NULL == count))
    {
        status =
            IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        const uint8_t type[] = IFX_RECORD_BLE_TYPE;
        if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
        {
            uint32_t index = UINT32_C(0);
            *count = ((ifx_record_ble_t *) handle->record_data)
                         ->optional_ad_types.count_of_additional_ad_types;
            while ((index < *count) && (IFX_SUCCESS == status))
            {
                if (CHECK_IF_AD_DATA_IS_INVALID(
                        ((ifx_record_ble_t *) handle->record_data)
                            ->optional_ad_types.additional_ad_types[index]))
                {
                    status = IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET,
                                       IFX_RECORD_DATA_FIELD_NA);
                }
                else
                {
                    additional_data[index].data_length =
                        (uint8_t) ((ifx_record_ble_t *) handle->record_data)
                            ->optional_ad_types.additional_ad_types[index]
                            .data_length;
                    additional_data[index].data_type =
                        ((ifx_record_ble_t *) handle->record_data)
                            ->optional_ad_types.additional_ad_types[index]
                            .data_type;
                    additional_data[index].data = (uint8_t *) malloc(
                        ((ifx_record_ble_t *) handle->record_data)
                            ->optional_ad_types.additional_ad_types[index]
                            .data_length);
                    if (NULL != additional_data[index].data)
                    {
                        IFX_MEMCPY(
                            additional_data[index].data,
                            ((ifx_record_ble_t *) handle->record_data)
                                ->optional_ad_types.additional_ad_types[index]
                                .data,
                            additional_data[index].data_length - 1);
                        index++;
                    }
                    else
                    {
                        status = IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET,
                                           IFX_OUT_OF_MEMORY);
                    }
                }
            }
        }
        else
        {
            status = IFX_ERROR(IFX_RECORD_BLE, IFX_RECORD_BLE_GET,
                               IFX_RECORD_INVALID);
        }
    }

    return status;
}
