// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file include/nbt-build-apdu-config.h
 * \brief Provides the function declaration for building command for NBT
 * configuration command set.
 */
#ifndef NBT_BUILD_APDU_CONFIG_H
#define NBT_BUILD_APDU_CONFIG_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "infineon/ifx-apdu.h"
#include "infineon/ifx-utils.h"
#include "infineon/nbt-apdu.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Function identifiers */

/**
 * \brief Identifier for command builder get configuration
 */
#define NBT_BUILD_GET_CONFIGURATION   UINT8_C(0x01)

/**
 * \brief Identifier for build command select configurator application
 */
#define NBT_BUILD_SELECT_CONFIGURATOR UINT8_C(0x02)

/**
 * \brief Identifier for build command set configuration
 */
#define NBT_BUILD_SET_CONFIGURATION   UINT8_C(0x03)

/**
 * \brief NBT_CLA type for NBT command: Get/Set configuration.
 */
#define NBT_CLA_SET_GET_CONFIG        UINT8_C(0x20)

/**
 * \brief Constant defines a length of tag field of configuration data.
 */
#define NBT_LEN_CONFIG_DATA_TAG       UINT8_C(0x02)

/**
 * \brief Constant defines a length of tag length field of configuration data.
 */
#define NBT_LEN_OF_TAG_LEN_FIELD      UINT8_C(0x01)

/**
 * \brief INS type for NBT command: Set configuration.
 */
#define NBT_INS_SET_CONFIG            UINT8_C(0x20)

/**
 * \brief INS type for NBT command: Get configuration.
 */
#define NBT_INS_GET_CONFIG            UINT8_C(0x30)

/**
 * \brief The command length Lc for NBT command: Get configuration.
 */
#define NBT_LC_GET_CONFIG             UINT8_C(0x02)

/**
 * \brief Builds the select command to select the NBT configurator application.
 *
 * \param[out] apdu APDU reference to store the select file APDU command.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t build_select_configurator_application(ifx_apdu_t *apdu);

/**
 * \brief Builds the set configuration command.
 *
 * \details This command can be used to set a specific product configuration
 * data.
 *
 * \param[in] config_data_tag Tag value of the configuration field to be set.
 * \param[in] config_value Product specific configuration parameters that needs
 * to be set.
 * \param[out] apdu APDU reference to store the set configuration APDU command.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t build_set_configuration(uint16_t config_data_tag,
                                     const ifx_blob_t *config_value,
                                     ifx_apdu_t *apdu);

/**
 * \brief Builds the get configuration command.
 *
 * \details This command can be used to get a specific product configuration
 * data.
 * \param[in] config_data_tag Tag value of the configuration field to be read.
 * \param[out] apdu APDU reference to store the get configuration APDU command.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t build_get_configuration(uint16_t config_data_tag,
                                     ifx_apdu_t *apdu);

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* NBT_BUILD_APDU_CONFIG_H */
