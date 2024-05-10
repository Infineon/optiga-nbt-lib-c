// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/ifx-record-bluetooth-le.h
 * \brief Model interface to create the Bluetooth low energy (BLE) carrier
 * configuration record types and set/get record fields.
 */
#ifndef IFX_RECORD_BLUETOOTH_LE_H
#define IFX_RECORD_BLUETOOTH_LE_H

#include <stddef.h>
#include <stdint.h>
#include "infineon/ifx-utils.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Macro definitions */
/* Definitions of function identifiers */

/**
 * \brief Identifier for model get Bluetooth LE record
 */
#define IFX_RECORD_BLE_GET               UINT8_C(0x01)

/**
 * \brief Identifier for model set bluetooth LE record ID
 */
#define IFX_RECORD_BLE_SET               UINT8_C(0x02)

/**
 * \brief Identifier for model bluetooth LE record release memory
 */
#define IFX_RECORD_BLE_RELEASE_MEMORY    UINT8_C(0x03)

/**
 * \brief Type information for bluetooth record
 */
#define IFX_RECORD_BLE_TYPE              "application/vnd.bluetooth.le.oob"

/**
 * \brief Length of device address type field
 */
#define IFX_RECORD_TYPE_LEN_BLE_DEV_ADDR UINT8_C(0x1)

/* enum definitions */

/**
 * \brief Enumeration defines the bluetooth low energy device address type.
 */
typedef enum
{
    IFX_PUBLIC_DEVICE_ADDRESS,
    IFX_RANDOM_DEVICE_ADDRESS
} ifx_ble_device_addr_type;

/* Structure definitions*/

/**
 * \brief Structure defines the data structure of bluetooth low energy record
 * such as data length and actual config data field.
 */
typedef struct
{
    uint8_t data_len; /**< Length of config data field */
    uint8_t *data;    /**< Actual config data field */
} ifx_ble_config_field_t;

/**
 * \brief Structure defines advertising and scan response data (AD)
 * format. Each AD structure consists of an AD Length field of 1 octet, an AD
 * Type field and an AD Data field.
 */
typedef struct
{
    uint8_t data_length; /**< AD data length field */
    uint8_t data_type;   /**< AD data type field */
    uint8_t *data;       /**< AD data field */
} ifx_record_ad_data_t;

/* public functions */

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
ifx_status_t ifx_record_ble_new(ifx_record_handle_t *handle);

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
    ifx_ble_device_addr_type device_addr_type);

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
                                     const ifx_ble_config_field_t *role);

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
    const ifx_ble_config_field_t *security_manager_tk_val);

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
    const ifx_ble_config_field_t *secure_conn_confirmation_val);

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
    const ifx_ble_config_field_t *secure_conn_random_val);

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
    ifx_record_handle_t *handle, const ifx_ble_config_field_t *appearance);

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
                                      const ifx_ble_config_field_t *flags);

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
    const ifx_ble_config_field_t *local_name);

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
    uint32_t count);

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
    ifx_ble_device_addr_type *device_addr_type);

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
                                     ifx_ble_config_field_t *role);

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
    ifx_ble_config_field_t *security_manager_tk_val);

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
    ifx_ble_config_field_t *secure_conn_confirmation_val);

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
    ifx_ble_config_field_t *secure_conn_random_val);

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
                                           ifx_ble_config_field_t *appearance);

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
                                      ifx_ble_config_field_t *flags);

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
                                           ifx_ble_config_field_t *local_name);

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
    uint32_t *count);

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* IFX_RECORD_BLUETOOTH_LE_H */
