// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/ifx-apdu-protocol.h
 * \brief Generic protocol API for exchanging APDUs with secure elements.
 */
#ifndef IFX_APDU_PROTOCOL_H
#define IFX_APDU_PROTOCOL_H

#include "infineon/ifx-apdu.h"
#include "infineon/ifx-logger.h"
#include "infineon/ifx-protocol.h"

/**
 * @def IFX_APDU_PROTOCOL_LOG
 * @brief APDU PROTOCOL library data log macro.
 *
 * @param[in] logger_object Logger object in which logging to be done.
 * @param[in] source_information Source information from where logs are
 * originated.
 * @param[in] logger_level Log level
 * @param[in] formatter String with formatter for data passed as variable
 * argument
 *
 * It maps to the ifx_logger_log function if IFX_APDU_PROTOCOL_LOG_ENABLE macro
 * is defined other wise it defines IFX_SUCCESS.
 *
 */

/**
 * @def IFX_APDU_PROTOCOL_LOG_BYTES
 * @brief APDU PROTOCOL library array of bytes data log macro.
 *
 * @param[in] logger_object Logger object in which logging to be done.
 * @param[in] source_information Source information from where logs are
 * originated.
 * @param[in] logger_level Log level
 * @param[in] delimeter_1 Delimeter to differentiate log information and data.
 * @param[in] data Data to be log.
 * @param[in] data_len Length of the data
 * @param[in] delimeter_2 Delimeter to differentiate each data byte
 *
 * It maps to the ifx_logger_log_bytes function if IFX_APDU_PROTOCOL_LOG_ENABLE
 * macro is defined other wise it defines IFX_SUCCESS.
 *
 */

#ifdef IFX_APDU_PROTOCOL_LOG_ENABLE
#define IFX_APDU_PROTOCOL_LOG(logger_object, source_information, logger_level, \
                              formatter, ...)                                  \
    ifx_logger_log(logger_object, source_information, logger_level, formatter, \
                   ##__VA_ARGS__)
#define IFX_APDU_PROTOCOL_LOG_BYTES(logger_object, source_information,         \
                                    logger_level, delimeter_1, data, data_len, \
                                    delimeter_2)                               \
    ifx_logger_log_bytes(logger_object, source_information, logger_level,      \
                         delimeter_1, data, data_len, delimeter_2)
#else
#define IFX_APDU_PROTOCOL_LOG(logger_object, source_information, logger_level, \
                              formatter, ...)

#define IFX_APDU_PROTOCOL_LOG_BYTES(logger_object, source_information,         \
                                    logger_level, delimeter_1, data, data_len, \
                                    delimeter_2)

#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief APDU protocol library identifier ID.
 */
#define LIB_APDU_PROTOCOL         UINT8_C(0x28)

/**
 * \brief String used as source information for logging.
 */
#define IFX_APDU_PROTOCOL_LOG_TAG "APDU-PROTOCOL"

/**
 * \brief Reusable error reason if status word returned during
 * ifx_apdu_protocol_transceive() indicates an error.
 */
#define IFX_SW_ERROR              UINT8_C(0xB0)

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
                                          ifx_apdu_response_t *response);

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
                                   const char *msg, const ifx_apdu_t *apdu);

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
ifx_status_t ifx_apdu_protocol_log_response(
    const ifx_logger_t *logger, const char *source, ifx_log_level level,
    const char *msg, const ifx_apdu_response_t *response);

#ifdef __cplusplus
}

#endif
#endif // IFX_APDU_PROTOCOL_H
