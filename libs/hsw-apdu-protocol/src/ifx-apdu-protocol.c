// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file ifx-apdu-protocol.c
 * \brief Generic protocol API for exchanging APDUs with secure elements
 */
#include "infineon/ifx-apdu-protocol.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "infineon/ifx-logger.h"
#include "infineon/ifx-protocol.h"

/**
 * \brief String used as source information for logging.
 */
#define IFX_LOG_TAG IFX_APDU_PROTOCOL_LOG_TAG

/**
 * \brief Sends APDU to secure element reads back APDU response.
 *
 * \details Encodes APDU, then sends it through ISO/OSI protocol.
 *          Reads back response and stores it in APDU response.
 *
 * \param[in] self  Protocol stack for performing necessary operations.
 * \param[in] apdu APDU to be send to secure element.
 * \param[out] response Buffer to store response information in.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 * \cond FULL_DOCUMENTATION_BUILD
 * \relates ifx_protocol
 * \endcond
 */
ifx_status_t ifx_apdu_protocol_transceive(ifx_protocol_t *self,
                                          const ifx_apdu_t *apdu,
                                          ifx_apdu_response_t *response)
{
    // Validate parameters
    if (self == NULL)
    {
        return IFX_ERROR(LIB_APDU_PROTOCOL, IFX_PROTOCOL_TRANSCEIVE,
                         IFX_ILLEGAL_ARGUMENT);
    }
    if ((apdu == NULL) || (response == NULL))
    {
        IFX_APDU_PROTOCOL_LOG(
            self->_logger, IFX_LOG_TAG, IFX_LOG_ERROR,
            "ifx_apdu_protocol_transceive() called with illegal NULL argument");
        return IFX_ERROR(LIB_APDU_PROTOCOL, IFX_PROTOCOL_TRANSCEIVE,
                         IFX_ILLEGAL_ARGUMENT);
    }

    // Encode APDU
    uint8_t *encoded = NULL;
    size_t encoded_len = 0U;
    ifx_status_t status = ifx_apdu_encode(apdu, &encoded, &encoded_len);
    if (status != IFX_SUCCESS)
    {
        // clang-format off
        return status; // LCOV_EXCL_LINE
        // clang-format on
    }

    // Log transmitted data
    IFX_APDU_PROTOCOL_LOG_BYTES(self->_logger, IFX_LOG_TAG, IFX_LOG_INFO, ">> ",
                                encoded, encoded_len, " ");

    // Exchange data with secure element
    uint8_t *response_buffer = NULL;
    size_t response_len = 0;
    status = ifx_protocol_transceive(self, encoded, encoded_len,
                                     &response_buffer, &response_len);
    if ((encoded != NULL) && (encoded_len > 0U))
    {
        free(encoded);
        encoded = NULL;
    }
    if (status != IFX_SUCCESS)
    {
        return status;
    }

    // Decode APDU response
    status = ifx_apdu_response_decode(response, response_buffer, response_len);
    if (status != IFX_SUCCESS)
    {
        IFX_APDU_PROTOCOL_LOG_BYTES(self->_logger, IFX_LOG_TAG, IFX_LOG_ERROR,
                                    "received invalid APDU response: ",
                                    response_buffer, response_len, " ");
        free(response_buffer);
        response_buffer = NULL;
        return status;
    }

    IFX_APDU_PROTOCOL_LOG_BYTES(self->_logger, IFX_LOG_TAG, IFX_LOG_INFO, "<< ",
                                response_buffer, response_len, " ");
    free(response_buffer);
    response_buffer = NULL;
    return IFX_SUCCESS;
}

/**
 * \brief Extension of ifx_logger_log() for logging APDU objects.
 *
 * \code
 *     ifx_apdu_t apdu = {
 *         .cla = 0x00u,
 *         .ins = 0xA4u
 *         .p1 = 0x04u,
 *         .p2 = 0x00u
 *     };
 *     ifx_apdu_protocol_log(&logger, TAG, IFX_LOG_INFO, "Sending: ", &apdu);
 *     // Sending: 00 A4 04 00
 * \endcode
 *
 * \param[in] logger Logger object holding concrete implementation.
 * \param[in] source String with information where the log originated from.
 * \param[in] level ifx_log_level of message (used for filtering).
 * \param[in] msg Optional message to be prepended to APDU (might be \c NULL ).
 * \param[in] apdu APDU to be logged.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other value in
 * case of error.
 * \cond FULL_DOCUMENTATION_BUILD
 * \relates ifx_logger
 * \endcond
 */
ifx_status_t ifx_apdu_protocol_log(const ifx_logger_t *logger,
                                   const char *source, ifx_log_level level,
                                   const char *msg, const ifx_apdu_t *apdu)
{
    // Validate parameters
    if ((logger == NULL) || (logger->_log == NULL) || (apdu == NULL))
    {
        return IFX_ERROR(LIB_APDU_PROTOCOL, IFX_LOGGER_LOG,
                         IFX_ILLEGAL_ARGUMENT);
    }

    // Pre-check level
    if (level < logger->_level)
    {
        return IFX_SUCCESS;
    }

    // Encode APDU response to its binary representation
    uint8_t *encoded = NULL;
    size_t encoded_len = 0U;
    ifx_status_t status = ifx_apdu_encode(apdu, &encoded, &encoded_len);
    if (status != IFX_SUCCESS)
    {
        // clang-format off
        return status; // LCOV_EXCL_LINE
        // clang-format on
    }

#ifndef IFX_APDU_PROTOCOL_LOG_ENABLE
    (void) msg;
    (void) source;
#endif

    // Actually log message
    IFX_APDU_PROTOCOL_LOG_BYTES(logger, source, level, msg, encoded,
                                encoded_len, " ");

    // Free temporary data
    if ((encoded != NULL) && (encoded_len > 0U))
    {
        free(encoded);
        encoded = NULL;
    }

    return status;
}

/**
 * \brief Extension of ifx_logger_log() for logging APDU response objects.
 *
 * \code
 *     ifx_apdu_response_t response = {
 *         .sw = 0x9000u
 *     };
 *     ifx_apdu_protocol_log_response(&logger, TAG, IFX_LOG_INFO, "Received: ",
 * &response);
 *     // Received: 90 00
 * \endcode
 *
 * \param[in] logger Logger object holding concrete implementation.
 * \param[in] source String with information where the log originated from.
 * \param[in] level ifx_log_level of message (used for filtering).
 * \param[in] msg Optional message to be prepended to APDU response (might be \c
 * NULL ).
 * \param[in] response ifx_apdu_response_t to be logged.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other value in
 * case of error.
 * \cond FULL_DOCUMENTATION_BUILD
 * \relates ifx_logger
 * \endcond
 */
ifx_status_t ifx_apdu_protocol_log_response(const ifx_logger_t *logger,
                                            const char *source,
                                            ifx_log_level level,
                                            const char *msg,
                                            const ifx_apdu_response_t *response)
{
    // Validate parameters
    if ((logger == NULL) || (logger->_log == NULL) || (response == NULL))
    {
        return IFX_ERROR(LIB_APDU_PROTOCOL, IFX_LOGGER_LOG,
                         IFX_ILLEGAL_ARGUMENT);
    }

    // Pre-check level
    if (level < logger->_level)
    {
        return IFX_SUCCESS;
    }

    // Encode APDU response to its binary representation
    uint8_t *encoded = NULL;
    size_t encoded_len = 0U;
    ifx_status_t status =
        ifx_apdu_response_encode(response, &encoded, &encoded_len);
    if (status != IFX_SUCCESS)
    {
        // clang-format off
        return status; // LCOV_EXCL_LINE
        // clang-format on
    }

#ifndef IFX_APDU_PROTOCOL_LOG_ENABLE
    (void) msg;
    (void) source;
#endif

    // Actually log message
    IFX_APDU_PROTOCOL_LOG_BYTES(logger, source, level, msg, encoded,
                                encoded_len, " ");

    // Free temporary data
    if ((encoded != NULL) && (encoded_len > 0U))
    {
        free(encoded);
        encoded = NULL;
    }

    return status;
}
