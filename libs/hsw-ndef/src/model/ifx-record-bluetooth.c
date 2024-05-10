// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file model/ifx-record-bluetooth.c
 * \brief Model interface to create the Bluetooth carrier configuration record
 * types and set/get record fields.
 */
#include "ifx-bluetooth-core-config.h"
#include "ifx-record-handler-bluetooth.h"
#include "infineon/ifx-ndef-errors.h"
#include "infineon/ifx-ndef-lib.h"
#include "infineon/ifx-ndef-record.h"
#include "infineon/ifx-record-bluetooth.h"
#include "infineon/ifx-record-handler.h"

/** \brief Macro to check if extended inquiry response (EIR) data parameters are
 * invalid. */
#define CHECK_IF_EIR_DATA_IS_INVALID(eir)                                      \
    (((eir).data_length == 0) || ((eir).data == NULL))

/* Static functions */

/**
 * \brief Frames the EIR data type from the Bluetooth configuration data.
 * \param[in] data_type     Type field of Bluetooth configuration data.
 * \param[in] config        Data field of Bluetooth configuration data.
 * \param[out] eir_data     EIR data field
 * \return ifx_status_t
 * \retval IFX_SUCCESS If framing of the EIR data is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation fails
 */
static ifx_status_t frame_eir_data(uint8_t data_type,
                                   const ifx_bt_config_field_t *config,
                                   ifx_record_eir_data_t *eir_data)
{
    if ((NULL == eir_data) || (NULL == config))
    {
        return IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_SET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    eir_data->data_type = data_type;
    eir_data->data_length = config->data_len + 1;
    eir_data->data = (uint8_t *) malloc(config->data_len);
    if (NULL == eir_data->data)
    {
        return IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_SET, IFX_OUT_OF_MEMORY);
    }
    IFX_MEMCPY(eir_data->data, config->data, config->data_len);

    return IFX_SUCCESS;
}

/**
 * \brief Extracts the Bluetooth configuration data from the EIR data.
 * \param[in] eir_data      EIR data field
 * \param[out] data_type    Type field of Bluetooth configuration data.
 * \param[out] config       Data field of Bluetooth configuration data.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation fails
 */
static ifx_status_t extract_data_from_eir_type(
    const ifx_record_eir_data_t *eir_data, uint8_t *data_type,
    ifx_bt_config_field_t *config)
{
    if ((NULL == eir_data) || (NULL == config) || (NULL == data_type))
    {
        return IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_GET,
                         IFX_ILLEGAL_ARGUMENT);
    }
    *data_type = eir_data->data_type;
    config->data_len = eir_data->data_length - 1;
    config->data = (uint8_t *) malloc(config->data_len);
    if (NULL == config->data)
    {
        return IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_GET, IFX_OUT_OF_MEMORY);
    }
    IFX_MEMCPY(config->data, eir_data->data, config->data_len);
    return IFX_SUCCESS;
}

/**
 * @brief Release all the allocated memory for the created bluetooth record data
 *
 * @param[in] record_data    data of the bluetooth record
 * \retval IFX_SUCCESS If memory release operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 */
static ifx_status_t record_bt_deinit(void *record_data)
{
    if (NULL == record_data)
    {
        return IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_RELEASE_MEMORY,
                         IFX_ILLEGAL_ARGUMENT);
    }
    ifx_record_bt_t *bt_record = (ifx_record_bt_t *) (record_data);

    if (bt_record->optional_eir_types.device_class.data != NULL)
    {
        IFX_FREE(bt_record->optional_eir_types.device_class.data);
        bt_record->optional_eir_types.device_class.data = NULL;
    }
    if (bt_record->optional_eir_types.local_name.data != NULL)
    {
        IFX_FREE(bt_record->optional_eir_types.local_name.data);
        bt_record->optional_eir_types.local_name.data = NULL;
    }
    if (bt_record->optional_eir_types.service_class_uuid.data != NULL)
    {
        IFX_FREE(bt_record->optional_eir_types.service_class_uuid.data);
        bt_record->optional_eir_types.service_class_uuid.data = NULL;
    }
    if (bt_record->optional_eir_types.simple_pairing_hash_c.data != NULL)
    {
        IFX_FREE(bt_record->optional_eir_types.simple_pairing_hash_c.data);
        bt_record->optional_eir_types.simple_pairing_hash_c.data = NULL;
    }
    if (bt_record->optional_eir_types.simple_pairing_randomizer_r.data != NULL)
    {
        IFX_FREE(
            bt_record->optional_eir_types.simple_pairing_randomizer_r.data);
        bt_record->optional_eir_types.simple_pairing_randomizer_r.data = NULL;
    }

    uint32_t index = 0;
    while (index < bt_record->optional_eir_types.count_of_additional_eir_types)
    {
        if (bt_record->optional_eir_types.additional_eir_types[index].data !=
            NULL)
        {
            IFX_FREE(
                bt_record->optional_eir_types.additional_eir_types[index].data);
            bt_record->optional_eir_types.additional_eir_types[index].data =
                NULL;
        }
        index++;
    }
    if (bt_record->optional_eir_types.additional_eir_types != NULL)
    {
        IFX_FREE(bt_record->optional_eir_types.additional_eir_types);
        bt_record->optional_eir_types.additional_eir_types = NULL;
    }
    return IFX_SUCCESS;
}

/* Public functions */

/**
 * \brief Creates the Bluetooth record and handle of the created record.
 * This handle holds values of the record needed for encode and decode
 * operations.
 * \param[out] handle    Handle of the created Bluetooth carrier configuration
 * record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If record creation operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation fails
 */
ifx_status_t ifx_record_bt_new(ifx_record_handle_t *handle)
{
    ifx_status_t status = IFX_SUCCESS;

    if (NULL != handle)
    {
        const uint8_t type[] = IFX_RECORD_BT_TYPE;
        handle->tnf = IFX_RECORD_TNF_TYPE_MEDIA;
        handle->type.length = sizeof(type) - 0x01;
        handle->type.buffer = (uint8_t *) malloc(handle->type.length);
        if (NULL != handle->type.buffer)
        {
            handle->id.buffer = NULL;
            handle->id.length = IFX_NDEF_ID_LEN_FIELD_NONE;
            IFX_MEMCPY(handle->type.buffer, type, handle->type.length);
            handle->encode_record = record_handler_bt_encode;
            handle->decode_record = record_handler_bt_decode;
            handle->deinit_record = record_bt_deinit;

            ifx_record_bt_t *bt_record =
                (ifx_record_bt_t *) malloc(sizeof(ifx_record_bt_t));
            if (NULL != bt_record)
            {
                IFX_MEMSET(bt_record, 0, sizeof(ifx_record_bt_t));
                IFX_MEMSET(&bt_record->optional_eir_types, 0,
                           sizeof(bt_record->optional_eir_types));
                handle->record_data = (void *) bt_record;
            }
            else
            {
                IFX_FREE(handle->type.buffer);
                handle->type.buffer = NULL;
                status = IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_SET,
                                   IFX_OUT_OF_MEMORY);
            }
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_SET, IFX_OUT_OF_MEMORY);
        }
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_SET, IFX_ILLEGAL_ARGUMENT);
    }

    return status;
}

/**
 * \brief Sets the device address in the Bluetooth carrier configuration record
 * for the given record handle.
 * \param[out] handle           Pointer to the record handle obtained while
 *                              creating the record.
 * \param[in] device_addr       6 octets of the Bluetooth device address are
 *                              encoded in the little endian order.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_bt_set_device_addr(ifx_record_handle_t *handle,
                                           const uint8_t *device_addr)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == device_addr))
    {
        status =
            IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_SET, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        const uint8_t type[] = IFX_RECORD_BT_TYPE;
        if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
        {
            IFX_MEMCPY(((ifx_record_bt_t *) handle->record_data)->device_addr,
                       device_addr, IFX_RECORD_BT_DEV_ADDR_LEN);
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_SET, IFX_RECORD_INVALID);
        }
    }

    return status;
}

/**
 * \brief Sets the device class in the Bluetooth carrier configuration record
 * for the given record handle.
 * \param[out] handle           Pointer to the record handle obtained while
 *                              creating the record.
 * \param[in] device_class      Pointer to the device class.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_bt_set_device_class(
    ifx_record_handle_t *handle, const ifx_bt_config_field_t *device_class)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == device_class))
    {
        return IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_SET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    const uint8_t type[] = IFX_RECORD_BT_TYPE;
    if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
    {
        status = frame_eir_data(IFX_BT_DEVICE_CLASS, device_class,
                                &((ifx_record_bt_t *) handle->record_data)
                                     ->optional_eir_types.device_class);
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_SET, IFX_RECORD_INVALID);
    }

    return status;
}

/**
 * \brief Sets the simple pairing hash in the Bluetooth carrier configuration
 * record for the given record handle.
 * \param[out] handle                   Pointer to the record handle obtained
 *                                      while creating the record.
 * \param[in] config_type                  Pointer to the simple pairing hash.
 * \param[in] simple_pairing_hash_c     Pointer to the simple pairing hash.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_bt_set_simple_pairing_hash_c(
    ifx_record_handle_t *handle, uint8_t config_type,
    const ifx_bt_config_field_t *simple_pairing_hash_c)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == simple_pairing_hash_c) ||
        (!((IFX_BT_SIMPLE_PAIRING_HASH_C_256 == config_type) ||
           (IFX_BT_SIMPLE_PAIRING_HASH_C_192 == config_type))))
    {
        return IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_SET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    const uint8_t type[] = IFX_RECORD_BT_TYPE;
    if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
    {
        status =
            frame_eir_data(config_type, simple_pairing_hash_c,
                           &((ifx_record_bt_t *) handle->record_data)
                                ->optional_eir_types.simple_pairing_hash_c);
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_SET, IFX_RECORD_INVALID);
    }

    return status;
}

/**
 * \brief Sets the simple pairing randomizer R in the Bluetooth carrier
 * configuration record for the given record handle.
 * \param[out] handle                       Pointer to the record handle
 * obtained while creating the record.
 * \param[in] config_type                  Pointer to the simple pairing
 * randomizer.
 * \param[in] simple_pairing_randomizer_r   Pointer to the simple pairing
 * randomizer R.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_bt_set_simple_pairing_randomizer_r(
    ifx_record_handle_t *handle, uint8_t config_type,
    const ifx_bt_config_field_t *simple_pairing_randomizer_r)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == simple_pairing_randomizer_r) ||
        !((config_type == IFX_BT_SIMPLE_PAIRING_RANDOMIZER_R_192) ||
          (config_type == IFX_BT_SIMPLE_PAIRING_RANDOMIZER_R_256)))
    {
        return IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_SET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    const uint8_t type[] = IFX_RECORD_BT_TYPE;
    if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
    {
        status = frame_eir_data(
            config_type, simple_pairing_randomizer_r,
            &((ifx_record_bt_t *) handle->record_data)
                 ->optional_eir_types.simple_pairing_randomizer_r);
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_SET, IFX_RECORD_INVALID);
    }

    return status;
}

/**
 * \brief Sets the service class universal unique identifier (UUID) in the
 * Bluetooth carrier configuration record for the given record handle.
 * \param[out] handle                       Pointer to the record handle
 * obtained while creating the record.
 * \param[in] config_type                   Pointer to the type of the service
 * class UUID.
 * \param[in] service_class_uuid            Pointer to the service class UUID.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_bt_set_service_class_uuid(
    ifx_record_handle_t *handle, uint8_t config_type,
    const ifx_bt_config_field_t *service_class_uuid)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == service_class_uuid) ||
        !((config_type == IFX_BT_INCOMPLETE_SERVICE_CLASS_UUID_16_BIT) ||
          (config_type == IFX_BT_COMPLETE_SERVICE_CLASS_UUID_16_BIT) ||
          (config_type == IFX_BT_INCOMPLETE_SERVICE_CLASS_UUID_32_BIT) ||
          (config_type == IFX_BT_COMPLETE_SERVICE_CLASS_UUID_32_BIT) ||
          (config_type == IFX_BT_INCOMPLETE_SERVICE_CLASS_UUID_128_BIT) ||
          (config_type == IFX_BT_COMPLETE_SERVICE_CLASS_UUID_128_BIT)))
    {
        return IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_SET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    const uint8_t type[] = IFX_RECORD_BT_TYPE;
    if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
    {
        status = frame_eir_data(config_type, service_class_uuid,
                                &((ifx_record_bt_t *) handle->record_data)
                                     ->optional_eir_types.service_class_uuid);
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_SET, IFX_RECORD_INVALID);
    }

    return status;
}

/**
 * \brief Sets the local name in the Bluetooth carrier configuration record for
 * the given record handle.
 * \param[out] handle    Pointer to the record handle obtained while creating
 *                       the record.
 * \param[in] config_type Type of the local name
 * (SHORTENED_LOCAL_NAME or COMPLETE_LOCAL_NAME)
 * \param[in] local_name Pointer to the Bluetooth local name.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_bt_set_local_name(
    ifx_record_handle_t *handle, uint8_t config_type,
    const ifx_bt_config_field_t *local_name)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == local_name) ||
        !((config_type == IFX_BT_SHORTENED_LOCAL_NAME) ||
          (config_type == IFX_BT_COMPLETE_LOCAL_NAME)))
    {
        return IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_SET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    const uint8_t type[] = IFX_RECORD_BT_TYPE;
    if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
    {
        status = frame_eir_data(config_type, local_name,
                                &((ifx_record_bt_t *) handle->record_data)
                                     ->optional_eir_types.local_name);
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_SET, IFX_RECORD_INVALID);
    }

    return status;
}

/**
 * \brief Sets the array of additional EIR data in Bluetooth carrier
 * configuration record for the given record handle.
 * \param[out] handle         Pointer to the record handle obtained while
 *                            creating the record.
 * \param[in] additional_data Pointer to the array of additional EIR data.
 * \param[in] count           Count of additional data.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t ifx_record_bt_set_additional_eir_data(
    ifx_record_handle_t *handle, const ifx_record_eir_data_t *additional_data,
    uint32_t count)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == additional_data))
    {
        return IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_SET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    const uint8_t type[] = IFX_RECORD_BT_TYPE;
    if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
    {
        uint32_t index = UINT32_C(0);
        uint32_t size = 0;
        while (index < count)
        {
            size += sizeof(additional_data[index]);
            index++;
        }

        ((ifx_record_bt_t *) handle->record_data)
            ->optional_eir_types.additional_eir_types =
            (ifx_record_eir_data_t *) malloc(size);
        if (NULL != ((ifx_record_bt_t *) handle->record_data)
                        ->optional_eir_types.additional_eir_types)
        {
            index = 0;
            while (index < count)
            {
                ((ifx_record_bt_t *) handle->record_data)
                    ->optional_eir_types.additional_eir_types[index]
                    .data_length = additional_data[index].data_length;
                ((ifx_record_bt_t *) handle->record_data)
                    ->optional_eir_types.additional_eir_types[index]
                    .data_type = additional_data[index].data_type;
                ((ifx_record_bt_t *) handle->record_data)
                    ->optional_eir_types.additional_eir_types[index]
                    .data = (uint8_t *) malloc(
                    additional_data[index].data_length - 0x01);
                if (NULL != ((ifx_record_bt_t *) handle->record_data)
                                ->optional_eir_types.additional_eir_types[index]
                                .data)
                {
                    IFX_MEMCPY(
                        ((ifx_record_bt_t *) handle->record_data)
                            ->optional_eir_types.additional_eir_types[index]
                            .data,
                        additional_data[index].data,
                        additional_data[index].data_length - 0x01);
                    index++;
                }
                else
                {
                    IFX_FREE(((ifx_record_bt_t *) handle->record_data)
                                 ->optional_eir_types.additional_eir_types);
                    ((ifx_record_bt_t *) handle->record_data)
                        ->optional_eir_types.additional_eir_types = NULL;
                    status = IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_SET,
                                       IFX_OUT_OF_MEMORY);
                }
            }
            ((ifx_record_bt_t *) handle->record_data)
                ->optional_eir_types.count_of_additional_eir_types = count;
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_SET, IFX_OUT_OF_MEMORY);
        }
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_SET, IFX_RECORD_INVALID);
    }

    return status;
}

/**
 * \brief Gets the device address of the Bluetooth carrier configuration record
 * from the given record handle.
 * \param[in] handle       Pointer to the record handle obtained while creating
 *                         the record.
 * \param[out] device_addr 6 octets of the Bluetooth device address are encoded
 *                         in the Little Endian order.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_bt_get_device_addr(const ifx_record_handle_t *handle,
                                           uint8_t *device_addr)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == device_addr) ||
        (NULL == (ifx_record_bt_t *) handle->record_data))
    {
        status =
            IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_GET, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        const uint8_t type[] = IFX_RECORD_BT_TYPE;
        if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
        {
            IFX_MEMCPY(device_addr,
                       ((ifx_record_bt_t *) handle->record_data)->device_addr,
                       IFX_RECORD_BT_DEV_ADDR_LEN);
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_GET, IFX_RECORD_INVALID);
        }
    }

    return status;
}

/**
 * \brief Gets the device class of Bluetooth carrier configuration record from
 * the given record handle.
 * \param[in] handle        Pointer to the record handle obtained while
 *                          creating the record.
 * \param[out] device_class Pointer to the device class.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 * \retval IFX_INVALID_STATE If the object is in an invalid state
 */
ifx_status_t ifx_record_bt_get_device_class(const ifx_record_handle_t *handle,
                                            ifx_bt_config_field_t *device_class)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == device_class))
    {
        status =
            IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_GET, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        const uint8_t type[] = IFX_RECORD_BT_TYPE;
        if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
        {
            if (CHECK_IF_EIR_DATA_IS_INVALID(
                    ((ifx_record_bt_t *) handle->record_data)
                        ->optional_eir_types.device_class))
            {
                status = IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_GET,
                                   IFX_RECORD_DATA_FIELD_NA);
            }
            else
            {
                uint8_t config_type = UINT8_C(0);
                status = extract_data_from_eir_type(
                    &((ifx_record_bt_t *) handle->record_data)
                         ->optional_eir_types.device_class,
                    &config_type, device_class);
                if ((config_type != IFX_BT_DEVICE_CLASS) &&
                    (IFX_SUCCESS == status))
                {
                    status = IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_GET,
                                       IFX_INVALID_STATE);
                }
            }
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_GET, IFX_RECORD_INVALID);
        }
    }

    return status;
}

/**
 * \brief Gets the simple pairing hash of the Bluetooth carrier configuration
 * record from the given record handle.
 * \param[in] handle                        Pointer to the record handle
 * obtained while creating the record.
 * \param[out] config_type                  Pointer to the type of the simple
 * pairing hash.
 * \param[out] simple_pairing_hash_c        Pointer to the simple pairing hash
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 * \retval IFX_INVALID_STATE If the object is in an invalid state
 */
ifx_status_t ifx_record_bt_get_simple_pairing_hash_c(
    const ifx_record_handle_t *handle, uint8_t *config_type,
    ifx_bt_config_field_t *simple_pairing_hash_c)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == simple_pairing_hash_c) ||
        (NULL == config_type))
    {
        status =
            IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_GET, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        const uint8_t type[] = IFX_RECORD_BT_TYPE;
        if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
        {
            if (CHECK_IF_EIR_DATA_IS_INVALID(
                    ((ifx_record_bt_t *) handle->record_data)
                        ->optional_eir_types.simple_pairing_hash_c))
            {
                status = IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_GET,
                                   IFX_RECORD_DATA_FIELD_NA);
            }
            else
            {
                status = extract_data_from_eir_type(
                    &((ifx_record_bt_t *) handle->record_data)
                         ->optional_eir_types.simple_pairing_hash_c,
                    config_type, simple_pairing_hash_c);
                if ((IFX_SUCCESS == status) &&
                    (((IFX_BT_SIMPLE_PAIRING_HASH_C_256 != *config_type) &&
                      (IFX_BT_SIMPLE_PAIRING_HASH_C_192 != *config_type))))
                {
                    status = IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_GET,
                                       IFX_INVALID_STATE);
                }
            }
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_GET, IFX_RECORD_INVALID);
        }
    }

    return status;
}

/**
 * \brief Gets the simple pairing randomizer R of the Bluetooth carrier
 * configuration record from the given record handle.
 * \param[in] handle                        Pointer to the record handle
 *                                          obtained while creating the record.
 * \param[out] config_type                  Pointer to the type of the simple
 * pairing randomizer.
 * \param[out] simple_pairing_randomizer_r  Pointer to the simple pairing
 * randomizer R.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 * \retval IFX_INVALID_STATE If the object is in an invalid state
 */
ifx_status_t ifx_record_bt_get_simple_pairing_randomizer_r(
    const ifx_record_handle_t *handle, uint8_t *config_type,
    ifx_bt_config_field_t *simple_pairing_randomizer_r)
{
    ifx_status_t status = IFX_SUCCESS;

    if ((NULL == handle) || (NULL == simple_pairing_randomizer_r) ||
        (NULL == config_type))
    {
        status =
            IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_GET, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        const uint8_t type[] = IFX_RECORD_BT_TYPE;
        if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
        {
            if (CHECK_IF_EIR_DATA_IS_INVALID(
                    ((ifx_record_bt_t *) handle->record_data)
                        ->optional_eir_types.simple_pairing_randomizer_r))
            {
                status = IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_GET,
                                   IFX_RECORD_DATA_FIELD_NA);
            }
            else
            {
                status = extract_data_from_eir_type(
                    &((ifx_record_bt_t *) handle->record_data)
                         ->optional_eir_types.simple_pairing_randomizer_r,
                    config_type, simple_pairing_randomizer_r);
                if ((IFX_SUCCESS == status) &&
                    (((IFX_BT_SIMPLE_PAIRING_RANDOMIZER_R_256 !=
                       *config_type) &&
                      (IFX_BT_SIMPLE_PAIRING_RANDOMIZER_R_192 !=
                       *config_type))))
                {
                    status = IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_GET,
                                       IFX_INVALID_STATE);
                }
            }
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_GET, IFX_RECORD_INVALID);
        }
    }

    return status;
}

/**
 * \brief Gets the service class UUID of the Bluetooth carrier configuration
 * record from the given record handle.
 * \param[in] handle                        Pointer to the record handle
 *                                          obtained while creating the record.
 * \param[out] config_type                  Pointer to the type of the service
 * class UUID.
 * \param[out] service_class_uuid           Pointer to the service class UUID.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 * \retval IFX_INVALID_STATE If the object is in an invalid state
 */
ifx_status_t ifx_record_bt_get_service_class_uuid(
    const ifx_record_handle_t *handle, uint8_t *config_type,
    ifx_bt_config_field_t *service_class_uuid)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == service_class_uuid) ||
        (NULL == config_type))
    {
        status =
            IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_GET, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        const uint8_t type[] = IFX_RECORD_BT_TYPE;
        if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
        {
            if (CHECK_IF_EIR_DATA_IS_INVALID(
                    ((ifx_record_bt_t *) handle->record_data)
                        ->optional_eir_types.service_class_uuid))
            {
                status = IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_GET,
                                   IFX_RECORD_DATA_FIELD_NA);
            }
            else
            {
                status = extract_data_from_eir_type(
                    &((ifx_record_bt_t *) handle->record_data)
                         ->optional_eir_types.service_class_uuid,
                    config_type, service_class_uuid);
                if ((IFX_SUCCESS == status) &&
                    (((IFX_BT_INCOMPLETE_SERVICE_CLASS_UUID_16_BIT !=
                       *config_type) &&
                      (IFX_BT_COMPLETE_SERVICE_CLASS_UUID_16_BIT !=
                       *config_type) &&
                      (IFX_BT_INCOMPLETE_SERVICE_CLASS_UUID_32_BIT !=
                       *config_type) &&
                      (IFX_BT_COMPLETE_SERVICE_CLASS_UUID_32_BIT !=
                       *config_type) &&
                      (IFX_BT_INCOMPLETE_SERVICE_CLASS_UUID_128_BIT !=
                       *config_type) &&
                      (IFX_BT_COMPLETE_SERVICE_CLASS_UUID_128_BIT !=
                       *config_type))))
                {
                    status = IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_GET,
                                       IFX_INVALID_STATE);
                }
            }
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_GET, IFX_RECORD_INVALID);
        }
    }

    return status;
}

/**
 * \brief Gets the local name in the Bluetooth carrier configuration record from
 * the given record handle.
 * \param[in] handle       Pointer to the record handle obtained
 *                         while creating the record.
 * \param[out] config_type Type of the local name
 * (SHORTENED_LOCAL_NAME or COMPLETE_LOCAL_NAME)
 * \param[out] local_name  Pointer to the Bluetooth local name.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 * \retval IFX_INVALID_STATE If the object is in an invalid state
 */
ifx_status_t ifx_record_bt_get_local_name(const ifx_record_handle_t *handle,
                                          uint8_t *config_type,
                                          ifx_bt_config_field_t *local_name)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == local_name) || (NULL == config_type))
    {
        status =
            IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_GET, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        const uint8_t type[] = IFX_RECORD_BT_TYPE;
        if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
        {
            if (CHECK_IF_EIR_DATA_IS_INVALID(
                    ((ifx_record_bt_t *) handle->record_data)
                        ->optional_eir_types.local_name))
            {
                status = IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_GET,
                                   IFX_RECORD_DATA_FIELD_NA);
            }
            else
            {
                status = extract_data_from_eir_type(
                    &((ifx_record_bt_t *) handle->record_data)
                         ->optional_eir_types.local_name,
                    config_type, local_name);
                if ((IFX_SUCCESS == status) &&
                    !((IFX_BT_SHORTENED_LOCAL_NAME == *config_type) ||
                      (IFX_BT_COMPLETE_LOCAL_NAME == *config_type)))
                {
                    status = IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_GET,
                                       IFX_INVALID_STATE);
                }
            }
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_GET, IFX_RECORD_INVALID);
        }
    }

    return status;
}

/**
 * \brief Gets an array of additional EIR data of the Bluetooth carrier
 * configuration record from the given record handle.
 * \param[in] handle                  Pointer to the record handle obtained
 *                                    while creating the record.
 * \param[out] additional_data        Pointer to the array of additional data.
 * \param[out] count                  Count of additional data.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 * \retval IFX_INVALID_STATE If the object is in an invalid state
 */
ifx_status_t ifx_record_bt_get_additional_eir_data(
    const ifx_record_handle_t *handle, ifx_record_eir_data_t *additional_data,
    uint32_t *count)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == additional_data) || (NULL == count) ||
        (0 == ((ifx_record_bt_t *) handle->record_data)
                  ->optional_eir_types.count_of_additional_eir_types))
    {
        status =
            IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_GET, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        const uint8_t type[] = IFX_RECORD_BT_TYPE;
        if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
        {
            uint32_t index = UINT32_C(0);
            *count = ((ifx_record_bt_t *) handle->record_data)
                         ->optional_eir_types.count_of_additional_eir_types;
            while ((index < *count) && (IFX_SUCCESS == status))
            {
                if (CHECK_IF_EIR_DATA_IS_INVALID(
                        ((ifx_record_bt_t *) handle->record_data)
                            ->optional_eir_types.additional_eir_types[index]))
                {
                    status = IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_GET,
                                       IFX_RECORD_DATA_FIELD_NA);
                }
                else
                {
                    additional_data[index].data_length =
                        ((ifx_record_bt_t *) handle->record_data)
                            ->optional_eir_types.additional_eir_types[index]
                            .data_length;
                    additional_data[index].data_type =
                        ((ifx_record_bt_t *) handle->record_data)
                            ->optional_eir_types.additional_eir_types[index]
                            .data_type;
                    additional_data[index].data =
                        (uint8_t *) malloc(additional_data[index].data_length);
                    if (NULL != additional_data[index].data)
                    {
                        IFX_MEMCPY(
                            additional_data[index].data,
                            ((ifx_record_bt_t *) handle->record_data)
                                ->optional_eir_types.additional_eir_types[index]
                                .data,
                            additional_data[index].data_length - 0x01);
                        index++;
                    }
                    else
                    {
                        status = IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_GET,
                                           IFX_OUT_OF_MEMORY);
                    }
                }
            }
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_BT, IFX_RECORD_BT_GET, IFX_RECORD_INVALID);
        }
    }

    return status;
}
