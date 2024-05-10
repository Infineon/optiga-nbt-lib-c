// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file include/ifx-bluetooth-core-config.h
 * \brief Bluetooth Core Configuration utility.
 */
#ifndef IFX_BLUETOOTH_CORE_CONFIG_H
#define IFX_BLUETOOTH_CORE_CONFIG_H

#include <stddef.h>
#include <stdint.h>
#include "infineon/ifx-error.h"
#include "infineon/ifx-ndef-record.h"
#include "infineon/ifx-utils.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* enum definitions */

/**
 * \brief Defines or assigned numbers that are used in bluetooth core
 * specifications
 */
typedef enum
{
    /**
     * Bluetooth flags, contains information on which discoverable mode to use
     * and the BR/EDR support and capability.
     */
    IFX_BT_FLAGS = 0x01,

    /**
     * Incomplete list of 16-­bit service class UUIDs.
     */
    IFX_BT_INCOMPLETE_SERVICE_CLASS_UUID_16_BIT = 0x02,

    /**
     * Complete list of 16-­bit service class UUIDs.
     */
    IFX_BT_COMPLETE_SERVICE_CLASS_UUID_16_BIT = 0x03,

    /**
     * Incomplete list of 32-­bit service class UUIDs.
     */
    IFX_BT_INCOMPLETE_SERVICE_CLASS_UUID_32_BIT = 0x04,

    /**
     * Complete list of 32-­bit service class UUIDs.
     */
    IFX_BT_COMPLETE_SERVICE_CLASS_UUID_32_BIT = 0x05,

    /**
     * Incomplete list of 128­bit service class UUIDs.
     */
    IFX_BT_INCOMPLETE_SERVICE_CLASS_UUID_128_BIT = 0x06,

    /**
     * Complete list of 128­bit service class UUIDs.
     */
    IFX_BT_COMPLETE_SERVICE_CLASS_UUID_128_BIT = 0x07,

    /**
     * Shortened local name.
     */
    IFX_BT_SHORTENED_LOCAL_NAME = 0x08,

    /**
     * Complete local name.
     */
    IFX_BT_COMPLETE_LOCAL_NAME = 0x09,

    /**
     * Class of device.
     */
    IFX_BT_DEVICE_CLASS = 0x0D,

    /**
     * Simple pairing hash C-­192 .
     */
    IFX_BT_SIMPLE_PAIRING_HASH_C_192 = 0x0E,

    /**
     * Simple pairing randomizer R-­192
     */
    IFX_BT_SIMPLE_PAIRING_RANDOMIZER_R_192 = 0x0F,

    /**
     * Security manager TK value.
     */
    IFX_BT_SECURITY_MANAGER_TK_VALUE = 0x10,

    /**
     * Appearance.
     */
    IFX_BT_APPEARANCE = 0x19,

    /**
     * LE bluetooth device address.
     */
    IFX_BT_LE_DEVICE_ADDRESS = 0x1B,

    /**
     * LE bluetooth Role.
     */
    IFX_BLE_ROLE = 0x1C,

    /**
     * Simple pairing hash C-­256.
     */
    IFX_BT_SIMPLE_PAIRING_HASH_C_256 = 0x1D,

    /**
     * Simple pairing randomizer R-256
     */
    IFX_BT_SIMPLE_PAIRING_RANDOMIZER_R_256 = 0x1E,

    /**
     * LE secure connections confirmation Value
     */
    IFX_BLE_SECURE_CONN_CONFIRM_VALUE = 0x22,

    /**
     * LE secure connections random value
     */
    IFX_BLE_SECURE_CONN_RANDOM_VALUE = 0x23
} ifx_bt_core_spec_id;

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* IFX_BLUETOOTH_CORE_CONFIG_H */
