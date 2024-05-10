// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file nbt-apdu.c
 * \brief NBT protocol API for exchanging APDUs with NBT product.
 */
#include "infineon/nbt-apdu.h"

#include "infineon/ifx-utils.h"
#include "infineon/nbt-errors.h"

/**
 * \brief Initializes NBT command set object by setting protocol members to
 * valid values.
 *
 * \details This function is for NBT command set stack developers to start from
 * a clean base, when initializing the command set. It initializes the given NBT
 * command set structure to start with NBT command set.
 *
 * \param[out] self NBT command set object to be initialized.
 * \param[in]  protocol Protocol to handle the communication with NBT tag.
 * \param[in]  logger Logger to handle the logging the logs.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_initialize(nbt_cmd_t *self, ifx_protocol_t *protocol,
                            ifx_logger_t *logger)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(self) ||
        IFX_VALIDATE_NULL_PTR_MEMORY(protocol) ||
        (IFX_VALIDATE_NULL_PTR_MEMORY(logger)))
    {
        return IFX_ERROR(NBT_APDU, NBT_INIT, IFX_ILLEGAL_ARGUMENT);
    }
#endif

    self->protocol = protocol;
    self->logger = logger;
    self->apdu = (ifx_apdu_t *) malloc(sizeof(ifx_apdu_t));
    if (IFX_VALIDATE_NULL_PTR_MEMORY(self->apdu))
    {
        return IFX_ERROR(NBT_APDU, NBT_INIT, IFX_OUT_OF_MEMORY);
    }

    self->response =
        (ifx_apdu_response_t *) malloc(sizeof(ifx_apdu_response_t));
    if (IFX_VALIDATE_NULL_PTR_MEMORY(self->response))
    {
        IFX_FREE(self->apdu);
        self->apdu = NULL;
        return IFX_ERROR(NBT_APDU, NBT_INIT, IFX_OUT_OF_MEMORY);
    }

    self->apdu_error_map_list = nbt_apdu_errors;
    self->apdu_error_map_list_length = NBT_MESSAGE_ERROR_COUNTS;

    return IFX_SUCCESS;
}

/**
 * \brief Releases memory associated with NBT command set object (but not object
 * itself).
 *
 * \param[in] self NBT command set object whose data shall be released.
 */
void nbt_destroy(nbt_cmd_t *self)
{
    if (!IFX_VALIDATE_NULL_PTR_MEMORY(self))
    {
        IFX_FREE(self->apdu->data);
        IFX_FREE(self->apdu);
        IFX_FREE(self->response->data);
        IFX_FREE(self->response);
        IFX_FREE(self->protocol->_properties);
        self->protocol->_properties = NULL;
        self->protocol = NULL;
        self->logger = NULL;
        self->apdu->data = NULL;
        self->apdu = NULL;
        self->response->data = NULL;
        self->response = NULL;
        self->apdu_error_map_list = NULL;
    }
}

/**
 * \brief Returns the error message for last command executed by NBT command
 * set.
 * \param[in] apdu_error_map_list ifx_apdu_error_map_t list
 * \return uint8_t \c pointer to error message error message
 */
uint8_t *nbt_error_message_get(const nbt_cmd_t *self)
{
    return ifx_apdu_error_get_message(self->apdu_error_map_list,
                                      self->apdu_error_map_list_length,
                                      self->apdu, self->response);
}
