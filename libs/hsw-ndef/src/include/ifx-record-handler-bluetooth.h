// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/records/ifx-record-handler-bluetooth.h
 * \brief Bluetooth Record encoding/decoding utility.
 * \details Bluetooth Carrier Configuration Record is a record that stores the
 * bluetooth Secure Simple Pairing OOB data that can be exchanged in Connection
 * Handover Request and/or Select messages as Alternative Carrier information.
 * For more details refer to application document for Bluetooth® Secure Simple
 * Pairing Using NFC(Version 1.3)
 */
#ifndef IFX_RECORD_HANDLER_BLUETOOTH_H
#define IFX_RECORD_HANDLER_BLUETOOTH_H

#include <stddef.h>
#include <stdint.h>
#include "infineon/ifx-error.h"
#include "infineon/ifx-ndef-record.h"
#include "infineon/ifx-record-bluetooth.h"
#include "infineon/ifx-utils.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Macro definitions */
/* Definitions of function identifiers */

/**
 * \brief Identifier for Bluetooth record encode ID
 */
#define IFX_RECORD_HANDLER_BT_ENCODE UINT8_C(0x01)

/**
 * \brief Identifier for Bluetooth record decode ID
 */
#define IFX_RECORD_HANDLER_BT_DECODE UINT8_C(0x02)

/**
 * \brief Bluetooth device address is a unique 48 bit identifier
 */
#define IFX_RECORD_BT_DEV_ADDR_LEN   UINT8_C(0x06)

/* Structure definitions */

/**
 * \brief Structure contains list of optional EIR types required for secure
 * simple pairing of bluetooth
 */
typedef struct
{
    /**
     * The Class of Device information is to be used to provide a graphical
     * representation to the user as part of UI involving operations with
     * Bluetooth devices. For example, it may provide a particular icon to
     * present the device
     */
    ifx_record_eir_data_t device_class;

    /**
     * The simple pairing hash C.
     */
    ifx_record_eir_data_t simple_pairing_hash_c;

    /**
     * The simple pairing randomizer R.
     */
    ifx_record_eir_data_t simple_pairing_randomizer_r;

    /**
     * Service class UUID. Service class information is used to identify the
     * supported Bluetooth services of the device.
     */
    ifx_record_eir_data_t service_class_uuid;

    /**
     * The Local Name is the user-friendly name presented over Bluetooth
     * technology.
     */
    ifx_record_eir_data_t local_name;

    /**
     * List of additional extended inquiry response(EIR) format types
     */
    ifx_record_eir_data_t *additional_eir_types;

    /**
     * Count of additional extended inquiry response(EIR) format data
     */
    uint32_t count_of_additional_eir_types;
} ifx_record_bt_optional_eir_types_t;

/**
 * \brief Structure defines list of record details respective to bluetooth
 * carrier configuration record
 */
typedef struct
{
    /**
     * The value of this length field provides the absolute length of the total
     * OOB data block used for Bluetooth BR/EDR OOB communication, including the
     * length field itself and the Bluetooth Device Address. The minimum length
     * that can be represented in this field is 8.
     */
    uint16_t oob_data_length;

    /**
     * The 6 octets Bluetooth device address encoded in Little Endian order.
     */
    uint8_t device_addr[IFX_RECORD_BT_DEV_ADDR_LEN];

    /**
     * List of optional extended inquiry response(EIR) format data
     * required for secure simple pairing of bluetooth
     */
    ifx_record_bt_optional_eir_types_t optional_eir_types;

} ifx_record_bt_t;

/* public functions */

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
                                      uint32_t *payload_length);

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
                                      void *record_details);

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* IFX_RECORD_HANDLER_BLUETOOTH_H */
