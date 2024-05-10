// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file nbt-build-apdu-config.c
 * \brief Provides the function implementation for command for NBT Configuration
 * command builder.
 */
#include "nbt-build-apdu-config.h"

#include "infineon/nbt-apdu-lib.h"
#include "nbt-build-apdu.h"

/**
 * \brief Configurator AID of the NBT configurator application.
 */
const uint8_t nbt_aid_config[] = {0xD2, 0x76, 0x00, 0x00, 0x04, 0x15, 0x02,
                                  0x00, 0x00, 0x0B, 0x00, 0x01, 0x01};

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
ifx_status_t build_select_configurator_application(ifx_apdu_t *apdu)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu))
    {
        return IFX_ERROR(NBT_BUILD_APDU_CONFIG, NBT_BUILD_SELECT_CONFIGURATOR,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    apdu->data = (uint8_t *) malloc(sizeof(nbt_aid_config));
    if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu->data))
    {
        return IFX_ERROR(NBT_BUILD_APDU_CONFIG, NBT_BUILD_SELECT_CONFIGURATOR,
                         IFX_OUT_OF_MEMORY);
    }

    apdu->cla = NBT_CLA;
    apdu->ins = NBT_INS_SELECT;
    apdu->p1 = NBT_P1_SELECT_BY_DF;
    apdu->p2 = NBT_P2_DEFAULT;
    apdu->lc = sizeof(nbt_aid_config);
    IFX_MEMCPY(apdu->data, nbt_aid_config, apdu->lc);
    apdu->le = IFX_APDU_LE_ANY;

    return IFX_SUCCESS;
}

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
                                     ifx_apdu_t *apdu)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu))
    {
        return IFX_ERROR(NBT_BUILD_APDU_CONFIG, NBT_BUILD_SET_CONFIGURATION,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    if (IFX_VALIDATE_NULL_PTR_BLOB(config_value))
    {
        apdu->lc = 0x00;
    }
    else
    {
        apdu->lc = config_value->length + NBT_LEN_CONFIG_DATA_TAG +
                   NBT_LEN_OF_TAG_LEN_FIELD;
    }

    apdu->cla = NBT_CLA_SET_GET_CONFIG;
    apdu->ins = NBT_INS_SET_CONFIG;
    apdu->p1 = NBT_P1_DEFAULT;
    apdu->p2 = NBT_P2_DEFAULT;
    if (apdu->lc > 0x00) // Configuration data present
    {
        apdu->data = (uint8_t *) malloc(apdu->lc);
        if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu->data))
        {
            return IFX_ERROR(NBT_BUILD_APDU_CONFIG, NBT_BUILD_SET_CONFIGURATION,
                             IFX_OUT_OF_MEMORY);
        }
        IFX_UPDATE_U16(apdu->data, config_data_tag);
        apdu->data[NBT_LEN_CONFIG_DATA_TAG] = config_value->length;
        IFX_MEMCPY(
            &apdu->data[NBT_LEN_CONFIG_DATA_TAG + NBT_LEN_OF_TAG_LEN_FIELD],
            config_value->buffer, config_value->length);
    }
    else
    {
        apdu->data = NULL;
    }

    apdu->le = NBT_LE_NONE;

    return IFX_SUCCESS;
}

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
ifx_status_t build_get_configuration(uint16_t config_data_tag, ifx_apdu_t *apdu)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu))
    {
        return IFX_ERROR(NBT_BUILD_APDU_CONFIG, NBT_BUILD_GET_CONFIGURATION,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif
    apdu->cla = NBT_CLA_SET_GET_CONFIG;
    apdu->ins = NBT_INS_GET_CONFIG;
    apdu->p1 = NBT_P1_DEFAULT;
    apdu->p2 = NBT_P2_DEFAULT;
    apdu->lc = NBT_LC_GET_CONFIG;

    apdu->data = (uint8_t *) malloc(apdu->lc);
    if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu->data))
    {
        return IFX_ERROR(NBT_BUILD_APDU_CONFIG, NBT_BUILD_GET_CONFIGURATION,
                         IFX_OUT_OF_MEMORY);
    }
    IFX_UPDATE_U16(apdu->data, config_data_tag);
    apdu->le = NBT_LE_NONE;

    return IFX_SUCCESS;
}
