// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/nbt-cmd-config.h
 * \brief Collection of the NBT configuration commands.
 */
#ifndef NBT_CMD_CONFIG_H
#define NBT_CMD_CONFIG_H

#include "infineon/ifx-apdu-protocol.h"
#include "infineon/ifx-protocol.h"
#include "infineon/ifx-utils.h"
#include "infineon/nbt-apdu-lib.h"
#include "infineon/nbt-apdu.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Function identifiers */

/**
 * \brief Identifier for command get configuration
 */
#define NBT_GET_CONFIGURATION   UINT8_C(0x01)

/**
 * \brief Identifier for command set configuration
 */
#define NBT_SET_CONFIGURATION   UINT8_C(0x02)

/**
 * \brief Identifier for command select configurator
 */
#define NBT_SELECT_CONFIGURATOR UINT8_C(0x03)

/**
 * \brief Enumeration defines the list of tags available for configuration
 * option.
 */
typedef enum
{
    NBT_TAG_PRODUCT_SHORT_NAME =
        UINT16_C(0xC020), /**< Tag ID for product short name */

    NBT_TAG_PRODUCT_LIFE_CYCLE =
        UINT16_C(0xC021), /**< Tag ID for product Life Cycle */

    NBT_TAG_SW_VERSION_INFO =
        UINT16_C(0xC022), /**< Tag ID for software version information */

    NBT_TAG_FLASH_LOADER = UINT16_C(0xC02F), /**< Tag ID for Flash Loader */

    NBT_TAG_GPIO_FUNCTION = UINT16_C(0xC030), /**< Tag ID for GPIO function */

    NBT_TAG_GPIO_ASSERT_LEVEL =
        UINT16_C(0xC031), /**< Tag ID for GPIO assert level */

    NBT_TAG_GPIO_OUTPUT_TYPE =
        UINT16_C(0xC032), /**< Tag ID for GPIO output type */

    NBT_TAG_GPIO_PULL_TYPE = UINT16_C(0xC033), /**< Tag ID for GPIO pull type */

    NBT_TAG_I2C_IDLE_TIMEOUT =
        UINT16_C(0xC040), /**< Tag ID for I2C idle timeout */

    NBT_TAG_I2C_DRIVE_STRENGTH =
        UINT16_C(0xC041), /**< Tag ID for I2C drive strength */

    NBT_TAG_I2C_SPEED = UINT16_C(0xC042), /**< Tag ID for I2C speed */

    NBT_TAG_NFC_IRQ_EVENT_TYPE =
        UINT16_C(0xC034), /**< Tag ID for NFC IRQ event type */

    NBT_TAG_NFC_ATS_CONFIG =
        UINT16_C(0xC050), /**< Tag ID for NFC ATS configuration */

    NBT_TAG_NFC_WTX_MODE = UINT16_C(0xC051), /**< Tag ID for NFC WTX mode */

    NBT_TAG_NFC_RF_HW_CONFIG =
        UINT16_C(0xC052), /**< Tag ID for NFC RF hardware configuration */

    NBT_TAG_NFC_UID_TYPE_FOR_ANTI_COLLISION =
        UINT16_C(0xC053), /**< Tag ID for NFC UID type for anti collision */

    NBT_TAG_COMMUNICATION_INTERFACE_ENABLE =
        UINT16_C(0xC060), /**< Tag ID for communication interface */

    NBT_TAG_PM_CURRENT_LIMIT_ENABLE =
        UINT16_C(0xC061), /**< Tag ID for power management clock scaling
                             (current limitation) enable */

    NBT_TAG_PM_CURRENT_LIMIT_CONFIG =
        UINT16_C(0xC062) /**<  Tag ID for power management clock scaling */
} nbt_tag_configurations;

/**
 * \brief Enumeration defines the tag values of product life cycle.
 */
typedef enum
{
    NBT_PRODUCT_LIFE_CYCLE_OPERATIONAL =
        UINT16_C(0xC33C), /**< Tag value of product life cycle - Operational */

    NBT_PRODUCT_LIFE_CYCLE_PERSONALIZATION = UINT16_C(
        0x5AA5) /**< Tag value of product life cycle - Personalization */
} nbt_product_life_cycle_tags;

/**
 * \brief Enumeration defines the tag values of Flash Loader.
 *
 * \note Use this tag value setting cautiously as this makes device go into
 * Flash Loader mode. Reverting to application mode is possible only in
 * engineering samples and not in any other samples.
 */
typedef enum
{
    NBT_FLASH_LOADER_ENABLE =
        UINT16_C(0xAC95), /**< Tag value of Flash Loader - Enable */

    NBT_FLASH_LOADER_DISABLE =
        UINT16_C(0xFFFF) /**< Tag value of Flash Loader - Disable */
} nbt_flash_loader_tags;

/**
 * \brief Enumeration defines the tag values of GPIO function.
 */
typedef enum
{
    NBT_GPIO_FUNCTION_DISABLED =
        UINT8_C(0x01), /**< Tag value for GPIO function - Disabled */

    NBT_GPIO_FUNCTION_NFC_IRQ_OUTPUT =
        UINT8_C(0x02), /**< Tag value for GPIO function - NFC IRQ output */

    NBT_GPIO_FUNCTION_I2C_DATA_READY_IRQ_OUTPUT = UINT8_C(
        0x03), /**< Tag value for GPIO function - I2C Data ready IRQ output */

    NBT_GPIO_FUNCTION_NFC_I2C_PASS_THROUGH_IRQ_OUTPUT =
        UINT8_C(0x04) /**< Tag value for GPIO function - NFC I2C pass-through
                          IRQ output */
} nbt_gpio_function_tags;

/**
 * \brief Enumeration defines the tag values of GPIO assert level.
 */
typedef enum
{
    NBT_GPIO_ASSERT_LOW_LEVEL_ACTIVE =
        UINT8_C(0x01), /**< Tag value for GPIO assert level - Low active */

    NBT_GPIO_ASSERT_HIGH_LEVEL_ACTIVE =
        UINT8_C(0x02), /**< Tag value for GPIO assert level - High active */
} nbt_gpio_assert_tags;

/**
 * \brief Enumeration defines the tag values of GPIO output type.
 */
typedef enum
{
    NBT_GPIO_OUTPUT_TYPE_PUSH_PULL =
        UINT8_C(0x01), /**< Tag value for GPIO output type - Push pull */

    NBT_GPIO_OUTPUT_TYPE_OPEN_DRAIN =
        UINT8_C(0x02), /**< Tag value for GPIO output type - Open drain */
} nbt_gpio_output_tags;

/**
 * \brief Enumeration defines the tag values of GPIO pull type.
 */
typedef enum
{
    NBT_GPIO_NO_PULL =
        UINT8_C(0x01), /**< Tag value for GPIO pull type - No pull */

    NBT_GPIO_PULL_UP =
        UINT8_C(0x02), /**< Tag value for GPIO pull type - Pull up */

    NBT_GPIO_PULL_DOWN =
        UINT8_C(0x03) /**< Tag value for GPIO pull type - Pull down */
} nbt_gpio_pull_tags;

/**
 * \brief Enumeration defines the tag values of NFC IRQ event.
 */
typedef enum
{
    NBT_NFC_IRQ_EVENT_SIGNAL_FIELD_PRESENCE = UINT8_C(
        0x01), /**< Tag value for NFC IRQ event type - Signal field presence */

    NBT_NFC_IRQ_EVENT_SIGNAL_LAYER_4_ENTRY = UINT8_C(
        0x02), /**< Tag value for NFC IRQ event type - Signal layer 4 entry */

    NBT_NFC_IRQ_EVENT_SIGNAL_APDU_PROCESSING_STAGE =
        UINT8_C(0x03) /**< Tag value for NFC IRQ event type - Signal APDU
                          processing stage */
} nbt_nfc_irq_event_tags;

/**
 * \brief Enumeration defines the tag values of I2C drive strength
 */
typedef enum
{
    NBT_I2C_DRIVE_STRENGTH_WEAK =
        UINT8_C(0x01), /**< Tag value for I2C drive strength - Weak */

    NBT_I2C_DRIVE_STRENGTH_STRONG =
        UINT8_C(0x02) /**< Tag value for I2C drive strength - Strong */
} nbt_i2c_drive_strength_tags;

/**
 * \brief Enumeration defines the tag values of I2C speed.
 */
typedef enum
{
    NBT_I2C_SPEED_400_KHZ =
        UINT8_C(0x01), /**< Tag value for I2C speed - 400kHz */

    NBT_I2C_SPEED_1000_KHZ =
        UINT8_C(0x02) /**< Tag value for I2C speed - 1000kHz */
} nbt_i2c_speed_tags;

/**
 * \brief Enumeration defines the tag values of NFC UID type for anti collision.
 */
typedef enum
{
    NBT_NFC_UID_UNIQUE_DEV_SPECIFIC_7_BYTE =
        UINT8_C(0x00), /**< Tag value for unique 7-byte device specific NFC UID
                          type for anti collision */

    NBT_NFC_UID_RANDOM_4_BYTE = UINT8_C(0x01), /**< Tag value for random 4-byte
                                              NFC UID type for anti collision */
} nbt_nfc_uid_type_for_anti_collision_tags;

/**
 * \brief Enumeration defines the tag values of communication interface.
 */
typedef enum
{
    NBT_COMM_INTF_NFC_DISABLED_I2C_ENABLED =
        UINT8_C(0x01), /**< Tag value for communication interface - NFC
                          disabled, I2C enabled */

    NBT_COMM_INTF_NFC_ENABLED_I2C_DISABLED =
        UINT8_C(0x10), /**< Tag value for communication interface - NFC enabled,
                          I2C disabled */

    NBT_COMM_INTF_NFC_ENABLED_I2C_ENABLED =
        UINT8_C(0x11) /**< Tag value for communication interface - NFC enabled,
                          I2C enabled */
} nbt_communication_interface_tags;

/**
 * \brief Selects the NBT configurator application.
 *
 * \param[out] self Command set with communication protocol and response.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_select_configurator_application(nbt_cmd_t *self);

/**
 * \brief Issues the set configuration command with configuration data as
 * byte array.
 *
 * \details This command can be used to set a specific product configuration
 * data.
 *
 * \param[out] self Command set with communication protocol and response.
 * \param[in] config_tag   Tag value of the configuration field to be set.
 * \param[in] config_value      configuration parameters of mentioned Tag value
 * that needs to be set as a byte array
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_set_configuration_bytes(nbt_cmd_t *self, uint16_t config_tag,
                                         const ifx_blob_t *config_value);

/**
 * \brief Issues the set configuration command with configuration data as byte.
 *
 * \details This command configures the system and lock the configuration by
 * setting product life cycle to operational state.
 *
 * \param[in,out] self Command set with communication protocol and response.
 * \param[in] config_tag   Tag value of the configuration field to be set.
 * \param[in] config_value      Configuration parameters of mentioned tag value
 * that needs to be set as a byte.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_set_configuration(nbt_cmd_t *self, uint16_t config_tag,
                                   uint8_t config_value);

/**
 * \brief Issues the get configuration command.
 *
 * \details This command can be used to get a specific product configuration
 * data.
 *
 * \param[in,out] self Command set with communication protocol and response.
 * \param[in] config_tag   Tag value of the configuration field to be read.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_get_configuration(nbt_cmd_t *self, uint16_t config_tag);

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* NBT_CMD_CONFIG_H */
