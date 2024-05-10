// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/ifx-record-bluetooth.h
 * \brief Model interface to create the Bluetooth carrier configuration record
 * types and set/get record fields.
 */
#ifndef IFX_RECORD_BLUETOOTH_H
#define IFX_RECORD_BLUETOOTH_H

#include <stddef.h>
#include <stdint.h>
#include "infineon/ifx-error.h"
#include "infineon/ifx-utils.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Macro definitions */
/* Definitions of function identifiers */

/**
 * \brief Identifier for model set Bluetooth record
 */
#define IFX_RECORD_BT_SET            UINT8_C(0x01)

/**
 * \brief Identifier for model get Bluetooth record ID
 */
#define IFX_RECORD_BT_GET            UINT8_C(0x02)

/**
 * \brief Identifier for model Bluetooth record ID release memory
 */
#define IFX_RECORD_BT_RELEASE_MEMORY UINT8_C(0x03)

/**
 * \brief Type information for bluetooth record
 */
#define IFX_RECORD_BT_TYPE           "application/vnd.bluetooth.ep.oob"

/**
 * \brief Defines the data structure of Bluetooth configuration field
 */
typedef struct
{
    uint8_t data_len; /**< Length of config data field */
    uint8_t *data;    /**< Actual config data field */
} ifx_bt_config_field_t;

/**
 * \brief Structure for extended inquiry response (EIR) data object. Each
 * EIR structure consists of an EIR Length field of 1 octet, an EIR Type field
 * and an EIR Data field.
 */
typedef struct
{
    uint8_t data_length; /**< EIR data length field */
    uint8_t data_type;   /**< EIR data type field */
    uint8_t *data;       /**< EIR data field */
} ifx_record_eir_data_t;

/* public functions */

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
ifx_status_t ifx_record_bt_new(ifx_record_handle_t *handle);

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
                                           const uint8_t *device_addr);

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
    ifx_record_handle_t *handle, const ifx_bt_config_field_t *device_class);

/**
 * \brief Sets the simple pairing hash in the Bluetooth carrier configuration
 * record for the given record handle.
 * \param[out] handle                   Pointer to the record handle obtained
 *                                      while creating the record.
 * \param[in] config_type               Type of the simple pairing hash.
 * \param[in] simple_pairing_hash_c     Pointer to the simple pairing hash.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_bt_set_simple_pairing_hash_c(
    ifx_record_handle_t *handle, uint8_t config_type,
    const ifx_bt_config_field_t *simple_pairing_hash_c);

/**
 * \brief Sets the simple pairing randomizer R in the Bluetooth carrier
 * configuration record for the given record handle.
 * \param[out] handle                       Pointer to the record handle
 * obtained while creating the record.
 * \param[in] config_type                   Type of the simple pairing
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
    const ifx_bt_config_field_t *simple_pairing_randomizer_r);

/**
 * \brief Sets the service class universal unique identifier (UUID) in the
 * Bluetooth carrier configuration record for the given record handle.
 * \param[out] handle                       Pointer to the record handle
 * obtained while creating the record.
 * \param[in] config_type                   Type of the service class UUID.
 * \param[in] service_class_uuid            Pointer to the service class UUID.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_bt_set_service_class_uuid(
    ifx_record_handle_t *handle, uint8_t config_type,
    const ifx_bt_config_field_t *service_class_uuid);

/**
 * \brief Sets the local name in the Bluetooth carrier configuration record for
 * the given record handle.
 * \param[out] handle    Pointer to the record handle obtained while creating
 *                       the record.
 * \param[in] config_type Type of the local name
 * (SHORTENED_LOCAL_NAME or COMPLETE_LOCAL_NAME).
 * \param[in] local_name Pointer to the Bluetooth local name.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_bt_set_local_name(
    ifx_record_handle_t *handle, uint8_t config_type,
    const ifx_bt_config_field_t *local_name);

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
    uint32_t count);

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
                                           uint8_t *device_addr);

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
ifx_status_t ifx_record_bt_get_device_class(
    const ifx_record_handle_t *handle, ifx_bt_config_field_t *device_class);

/**
 * \brief Gets the simple pairing hash of the Bluetooth carrier configuration
 * record from the given record handle.
 * \param[in] handle                        Pointer to the record handle
 * obtained while creating the record.
 * \param[out] config_type                  Type of the simple pairing hash.
 * \param[out] simple_pairing_hash_c        Pointer to the simple pairing hash
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 * \retval IFX_INVALID_STATE If the object is in an invalid state
 */
ifx_status_t ifx_record_bt_get_simple_pairing_hash_c(
    const ifx_record_handle_t *handle, uint8_t *config_type,
    ifx_bt_config_field_t *simple_pairing_hash_c);

/**
 * \brief Gets the simple pairing randomizer R of the Bluetooth carrier
 * configuration record from the given record handle.
 * \param[in] handle                        Pointer to the record handle
 *                                          obtained while creating the record.
 * \param[out] config_type                  Type of the simple pairing
 * randomizer.
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
    ifx_bt_config_field_t *simple_pairing_randomizer_r);

/**
 * \brief Gets the service class UUID of the Bluetooth carrier configuration
 * record from the given record handle.
 * \param[in] handle                        Pointer to the record handle
 *                                          obtained while creating the record.
 * \param[out] config_type                  Type of the simple pairing hash.
 * \param[out] service_class_uuid           Pointer to the service class UUID.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 * \retval IFX_INVALID_STATE If the object is in an invalid state
 */
ifx_status_t ifx_record_bt_get_service_class_uuid(
    const ifx_record_handle_t *handle, uint8_t *config_type,
    ifx_bt_config_field_t *service_class_uuid);

/**
 * \brief Gets the local name in the Bluetooth carrier configuration record from
 * the given record handle.
 * \param[in] handle       Pointer to the record handle obtained
 *                         while creating the record.
 * \param[out] config_type Type of the local name
 * (SHORTENED_LOCAL_NAME or COMPLETE_LOCAL_NAME).
 * \param[out] local_name  Pointer to the Bluetooth local name.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 * \retval IFX_INVALID_STATE If the object is in an invalid state
 */
ifx_status_t ifx_record_bt_get_local_name(const ifx_record_handle_t *handle,
                                          uint8_t *config_type,
                                          ifx_bt_config_field_t *local_name);

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
    uint32_t *count);

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* IFX_RECORD_BLUETOOTH_H */