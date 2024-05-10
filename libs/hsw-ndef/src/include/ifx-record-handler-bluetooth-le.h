// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file include/ifx-record-handler-bluetooth-le.h
 * \brief Bluetooth Low Energy Record encoding/decoding utility.
 * \details Bluetooth LE Carrier Configuration Record is a record that stores
 * the bluetooth LE Security Manager OOB Required Data Types that can be
 * exchanged in Connection Handover Request and/or Select messages as
 * Alternative Carrier information. For more details refer to application
 * document for Bluetooth® Secure Simple Pairing Using NFC(Version 1.3)
 */
#ifndef IFX_RECORD_HANDLER_BLUETOOTH_LE_H
#define IFX_RECORD_HANDLER_BLUETOOTH_LE_H

#include <stddef.h>
#include <stdint.h>
#include "infineon/ifx-error.h"
#include "infineon/ifx-ndef-record.h"
#include "infineon/ifx-record-bluetooth-le.h"
#include "infineon/ifx-utils.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Macro definitions */
/* Definitions of function identifiers */

/**
 * \brief Identifier for Bluetooth LE record encode ID
 */
#define IFX_RECORD_HANDLER_BLE_ENCODE UINT8_C(0x01)

/**
 * \brief Identifier for Bluetooth LE record decode ID
 */
#define IFX_RECORD_HANDLER_BLE_DECODE UINT8_C(0x02)

/**
 * \brief  Bluetooth le device address is a unique 48 bit identifier.
 * 6 octets device address and MSB byte specifies whether device address is
 * random or public
 */
#define IFX_BLE_DEV_ADDR_LEN          UINT8_C(0x7)

/* Structure definitions */

/**
 * \brief Structure contains the list of optional AD types required for secure
 * simple pairing of bluetooth low energy
 */
typedef struct
{
    /**
     * The security manager TK value.
     * If the OOB association model and LE legacy pairing are used, the TK value
     * might be exchanged over the OOB channel, in this case NFC.
     */
    ifx_record_ad_data_t security_manager_tk_val;

    /**
     * LE Security connections confirmation value is used by security manager.
     * The value is encoded in Little Endian order.
     * If the OOB association model and LE Secure Connections pairing are used,
     * the LE Secure Connections Confirmation Value might be exchanged over the
     * OOB channel, in this case NFC
     */
    ifx_record_ad_data_t secure_conn_confirmation_val;

    /**
     * LE Secure Connections Random Value isis used by security manager.
     * The value is encoded in Little Endian order.
     * If the OOB association model and LE Secure Connections pairing are used,
     * the LE Secure Connections Random Value might be exchanged over the
     * OOB channel, in this case NFC
     */
    ifx_record_ad_data_t secure_conn_random_val;

    /**
     * The appearance data type. The appearance characteristic defines the
     * representation of the external appearance of the device, for example, a
     * mouse, generic remote control, or keyboard.
     */
    ifx_record_ad_data_t appearance;

    /**
     * Flags contains information on which discoverable mode to use and the
     * BR/EDR support and capability
     */
    ifx_record_ad_data_t flags;

    /**
     * The Local Name is the user-friendly name presented over Bluetooth Low
     * Energy technology.
     */
    ifx_record_ad_data_t local_name;

    /**
     * List of other optional advertising and scan response (AD) format data.
     */
    ifx_record_ad_data_t *additional_ad_types;

    /**
     * Count of additional AD format data
     */
    uint32_t count_of_additional_ad_types;
} ifx_record_ble_optional_ad_types_t;

/**
 * \brief Structure defines list of record details respective to bluetooth low
 * energy carrier configuration record
 */
typedef struct
{
    /**
     * The 7 octets Bluetooth low energy device address encoded in Little Endian
     * order. Device address is 6 octets field encoded in Little endian. The
     * lease significant bit of Most significant byte decided the type of device
     * address Random(1) and Public(0) device address
     */
    ifx_record_ad_data_t device_addr;

    /**
     * The LE Role data type defines the LE role capabilities of the device
     * is to be used to provide a graphical representation to the user as part
     * of UI involving operations with Bluetooth devices. For example, it may
     * provide a particular icon to present the device
     */
    ifx_record_ad_data_t role;

    /**
     * List of optional advertising and scan response (AD) format data
     * required for secure simple pairing of bluetooth low energy
     */
    ifx_record_ble_optional_ad_types_t optional_ad_types;

} ifx_record_ble_t;

/* public functions */
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
                                       uint32_t *payload_length);

/**
 * \brief Decodes the NDEF record payload to Bluetooth low energy carrier
 * configuration record.
 * \param[in] payload           Pointer to the payload byte array of Bluetooth
 * low energy carrier configuration record
 * \param[in] payload_length    Pointer to the payload length of Bluetooth low
 * energy carrier configuration record
 *  \param[out] record_details   Pointer to the Bluetooth low energy carrier
 * configuration record details that needs to be updated from record handle
 * \return ifx_status_t
 * \retval IFX_SUCCESS If decoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_INVALID_STATE If object is in an invalid state
 */
ifx_status_t record_handler_ble_decode(const uint8_t *payload,
                                       uint32_t payload_length,
                                       void *record_details);

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* IFX_RECORD_HANDLER_BLUETOOTH_LE_H */
