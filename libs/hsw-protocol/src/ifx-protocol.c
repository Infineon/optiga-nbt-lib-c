// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file ifx-protocol.c
 * \brief Generic protocol API (ISO/OSI stack).
 */
#include "infineon/ifx-protocol.h"

#include <stdlib.h>

/**
 * \brief Activates secure element and performs protocol negotiation.
 *
 * \details Depending on the protocol, data needs to be exchanged with a secure
 * element to negotiate certain protocol aspects like frame sizes, waiting
 * times, etc. This function will perform the initial parameter negotiation.
 *
 * \param[in] self Protocol stack for performing necessary operations.
 * \param[out] response Buffer to store response in (e.g. ATR, ATPO, ...).
 * \param[out] response_len Buffer to store number of received bytes in (number
 * of bytes in \p response).
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 * \relates ifx_protocol
 */
ifx_status_t ifx_protocol_activate(ifx_protocol_t *self, uint8_t **response,
                                   size_t *response_len)
{
    // Validate parameters
    if (self == NULL)
    {
        return IFX_ERROR(LIB_PROTOCOL, IFX_PROTOCOL_ACTIVATE,
                         IFX_ILLEGAL_ARGUMENT);
    }

    // Check if current layer has activation function
    if (self->_activate != NULL)
    {
        return self->_activate(self, response, response_len);
    }

    // Otherwise try next layer
    if (self->_base != NULL)
    {
        return ifx_protocol_activate(self->_base, response, response_len);
    }
    return IFX_SUCCESS;
}

/**
 * \brief Sends data via Protocol and reads back response.
 *
 * \details Goes through ISO/OSI protocol stack and performs necessary protocol
 * operations (chaining, crc, ...).
 *
 * \param[in] self Protocol stack for performing necessary operations.
 * \param[in] data Data to be send via protocol.
 * \param[in] data_len Number of bytes in \p data.
 * \param[out] response Buffer to store response in.
 * \param[out] response_len Buffer to store number of received bytes in (number
 * of bytes in \p response ).
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 * \relates ifx_protocol
 */
ifx_status_t ifx_protocol_transceive(ifx_protocol_t *self, const uint8_t *data,
                                     size_t data_len, uint8_t **response,
                                     size_t *response_len)
{
    // Validate parameters
    if (self == NULL)
    {
        return IFX_ERROR(LIB_PROTOCOL, IFX_PROTOCOL_TRANSCEIVE,
                         IFX_ILLEGAL_ARGUMENT);
    }

    // XXX: This check might be protocol layer specific
    if ((data == NULL) || (data_len == 0U) || (response == NULL) ||
        (response_len == NULL))
    {
        return IFX_ERROR(LIB_PROTOCOL, IFX_PROTOCOL_TRANSCEIVE,
                         IFX_ILLEGAL_ARGUMENT);
    }

    // If protocol defines transceive function then directly use it
    if (self->_transceive != NULL)
    {
        return self->_transceive(self, data, data_len, response, response_len);
    }

    // Otherwise fall back to transmit / receive
    if ((self->_transmit) == NULL || (self->_receive == NULL))
    {
        return IFX_ERROR(LIB_PROTOCOL, IFX_PROTOCOL_TRANSCEIVE,
                         IFX_PROTOCOL_STACK_INVALID);
    }
    ifx_status_t status = self->_transmit(self, data, data_len);
    if (ifx_error_check(status))
    {
        return status;
    }
    return self->_receive(self, IFX_PROTOCOL_RECEIVE_LEN_UNKOWN, response,
                          response_len);
}

/**
 * \brief Frees memory associated with Protocol object (but not object itself).
 *
 * \details Protocol objects can consist of several layers each of which might
 * hold dynamically allocated data that needs special clean-up functionality.
 * Users would need to manually check which members need special care and free
 * them themselves. Calling this function will ensure that all members have been
 * freed properly.
 *
 * \param[in] self Protocol object whose data shall be freed.
 * \relates ifx_protocol
 */
void ifx_protocol_destroy(ifx_protocol_t *self)
{
    if (self != NULL)
    {
        // Check if layer has custom cleanup function
        if (self->_destructor != NULL)
        {
            self->_destructor(self);
        }

        // Check if properties have been missed by protocol layer
        if (self->_properties != NULL)
        {
            free(self->_properties);
            self->_properties = NULL;
        }

        // Go down protocol stack
        if (self->_base != NULL)
        {
            ifx_protocol_destroy(self->_base);
        }
    }
}

/**
 * \brief Sets Logger to be used by Protocol.
 *
 * \details Sets logger for whole protocol stack, so all layers below will also
 * have the logger set.
 *
 * \param[in] self Protocol object to set logger for.
 * \param[in] logger Logger object to be used (might be \c NULL to clear
 * logger).
 * \relates ifx_protocol
 */
void ifx_protocol_set_logger(ifx_protocol_t *self, ifx_logger_t *logger)
{
    if (self != NULL)
    {
        // Set logger for current layer
        self->_logger = logger;

        // Go down protocol stack
        ifx_protocol_set_logger(self->_base, logger);
    }
}

/**
 * \brief Initializes Protocol object by setting all members to valid values.
 *
 * \details This function is for protocol stack developers to start from a clean
 * base when initializing a layer in their custom initialization function. It
 * initializes the given Protocol struct to a clean but unusable state (no
 * members set).
 *
 * \param[in] self Protocol object to be initialized.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any
 * other value in case of error.
 */
ifx_status_t ifx_protocol_layer_initialize(ifx_protocol_t *self)
{
    if (self == NULL)
    {
        return IFX_ERROR(LIB_PROTOCOL, IFX_PROTOCOL_LAYER_INITIALIZE,
                         IFX_ILLEGAL_ARGUMENT);
    }
    self->_base = NULL;
    self->_layer_id = 0U;
    self->_activate = NULL;
    self->_transceive = NULL;
    self->_transmit = NULL;
    self->_receive = NULL;
    self->_destructor = NULL;
    self->_logger = NULL;
    self->_properties = NULL;
    return IFX_SUCCESS;
}
