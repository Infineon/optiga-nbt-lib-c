// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file nbt-cmd-config.c
 * \brief Collection of the NBT configuration commands.
 */
#include "infineon/nbt-cmd-config.h"

#include "infineon/nbt-apdu-lib.h"
#include "nbt-build-apdu-config.h"

/**
 * \brief Selects the NBT configurator application.
 *
 * \param[in,out] self Command set with communication protocol and response.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_select_configurator_application(nbt_cmd_t *self)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(self))
    {
        return IFX_ERROR(NBT_CMD_CONFIG, NBT_SELECT_CONFIGURATOR,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif
    ifx_status_t status = build_select_configurator_application(self->apdu);
    if (ifx_error_check(status))
    {
        NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                     "build_select_configurator_application unable to init "
                     "command ");
    }
    else
    {
        status = ifx_apdu_protocol_transceive(self->protocol, self->apdu,
                                              self->response);
        if (ifx_error_check(status))
        {
            NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                         "apdu transceive error");
        }
    }

    return status;
}

/**
 * \brief Issues the set configuration command with configuration data as
 * byte array.
 *
 * \details This command can be used to set a specific product configuration
 * data.
 *
 * \param[in,out] self Command set with communication protocol and response.
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
                                         const ifx_blob_t *config_value)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(self) ||
        (IFX_VALIDATE_NULL_PTR_MEMORY(config_value)))
    {
        return IFX_ERROR(NBT_CMD_CONFIG, NBT_SET_CONFIGURATION,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    ifx_status_t status =
        build_set_configuration(config_tag, config_value, self->apdu);
    if (ifx_error_check(status))
    {
        NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                     "build_set_configuration unable to build command ");
    }
    else
    {
        status = ifx_apdu_protocol_transceive(self->protocol, self->apdu,
                                              self->response);
        if (ifx_error_check(status))
        {
            NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                         "apdu transceive error");
        }
    }

    return status;
}

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
                                   uint8_t config_value)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(self))
    {
        return IFX_ERROR(NBT_CMD_CONFIG, NBT_SET_CONFIGURATION,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    ifx_blob_t config_data;
    config_data.buffer = &config_value;
    config_data.length = UINT32_C(0x01);
    ifx_status_t status =
        build_set_configuration(config_tag, &config_data, self->apdu);

    if (ifx_error_check(status))
    {
        NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                     "build_set_configuration unable to build command ");
    }
    else
    {
        status = ifx_apdu_protocol_transceive(self->protocol, self->apdu,
                                              self->response);
        if (ifx_error_check(status))
        {
            NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                         "apdu transceive error");
        }
    }

    return status;
}

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
ifx_status_t nbt_get_configuration(nbt_cmd_t *self, uint16_t config_tag)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(self))
    {
        return IFX_ERROR(NBT_CMD_CONFIG, NBT_GET_CONFIGURATION,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    ifx_status_t status = build_get_configuration(config_tag, self->apdu);
    if (ifx_error_check(status))
    {
        NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                     "build_get_configuration unable to build command ");
    }
    else
    {
        status = ifx_apdu_protocol_transceive(self->protocol, self->apdu,
                                              self->response);
        if (ifx_error_check(status))
        {
            NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                         "apdu transceive error");
        }
    }

    return status;
}
