// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/ifx-t1prime.h
 * \brief Global Platform T=1' protocol.
 */
#ifndef IFX_T1PRIME_H
#define IFX_T1PRIME_H

#include <stddef.h>
#include <stdint.h>

#include "infineon/ifx-error.h"
#include "infineon/ifx-protocol.h"
#include "infineon/ifx-t1prime-lib.h"

/**
 * @def IFX_T1PRIME_LOG
 * @brief T1PRIME library data log macro.
 *
 * @param[in] logger_object Logger object in which logging to be done.
 * @param[in] source_information Source information from where logs are
 * originated.
 * @param[in] logger_level Log level
 * @param[in] formatter String with formatter for data passed as variable
 * argument
 *
 * It maps to the ifx_logger_log function if IFX_T1PRIME_LOG_ENABLE macro is
 * defined other wise it defines IFX_SUCCESS.
 *
 */

/**
 * @def IFX_T1PRIME_LOG_BYTES
 * @brief T1PRIME library array of bytes data log macro.
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
 * It maps to the ifx_logger_log_bytes function if IFX_T1PRIME_LOG_ENABLE macro
 * is defined other wise it defines IFX_SUCCESS.
 *
 */

#ifdef IFX_T1PRIME_LOG_ENABLE
#define IFX_T1PRIME_LOG(logger_object, source_information, logger_level,       \
                        formatter, ...)                                        \
    ifx_logger_log(logger_object, source_information, logger_level, formatter, \
                   ##__VA_ARGS__)
#define IFX_T1PRIME_LOG_BYTES(logger_object, source_information, logger_level, \
                              delimeter_1, data, data_len, delimeter_2)        \
    ifx_logger_log_bytes(logger_object, source_information, logger_level,      \
                         delimeter_1, data, data_len, delimeter_2)
#else
#define IFX_T1PRIME_LOG(logger_object, source_information, logger_level,       \
                        formatter, ...)

#define IFX_T1PRIME_LOG_BYTES(logger_object, source_information, logger_level, \
                              delimeter_1, data, data_len, delimeter_2)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief String used as source information for logging.
 */
#define IFX_T1PRIME_LOG_TAG           "T=1'"

/**
 * \brief Return code for successful calls to \ref ifx_t1prime_irq_handler_t.
 */
#define IFX_T1PRIME_IRQ_TRIGGERED     IFX_SUCCESS

/**
 * \brief IFX error encoding function identifier for \ref
 * ifx_t1prime_irq_handler_t.
 */
#define IFX_T1PRIME_IRQ               0x09u

/**
 * \brief Error reason if interrupt did not trigger in time during
 * ifx_t1prime_irq_handler_t.
 */
#define IFX_T1PRIME_IRQ_NOT_TRIGGERED 0x01u

/**
 * \brief Initializes Protocol object for Global Platform T=1' protocol.
 *
 * \param[in] self Protocol object to be initialized.
 * \param[in] driver Physical (driver) layer used for communication.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any
 * other value in case of error.
 * \cond FULL_DOCUMENTATION_BUILD
 * \relates ifx_protocol
 * \endcond
 */
ifx_status_t ifx_t1prime_initialize(ifx_protocol_t *self,
                                    ifx_protocol_t *driver);

/**
 * \brief Sets maximum information field size of the host device (IFSD).
 *
 * \param[in] self T=1' protocol stack to set IFSD for.
 * \param[in] ifsd IFS value to be used.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 * \cond FULL_DOCUMENTATION_BUILD
 * \relates ifx_protocol
 * \endcond
 */
ifx_status_t ifx_t1prime_set_ifsd(ifx_protocol_t *self, size_t ifsd);

/**
 * \brief Returns current block waiting time (BWT) in [ms].
 *
 * \param[in] self T=1' protocol stack to get BWT for.
 * \param[out] bwt_ms_buffer Buffer to store BWT value in.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 * \cond FULL_DOCUMENTATION_BUILD
 * \relates ifx_protocol
 * \endcond
 */
ifx_status_t ifx_t1prime_get_bwt(ifx_protocol_t *self, uint16_t *bwt_ms_buffer);

/**
 * \brief Sets block waiting time (BWT) in [ms].
 *
 * \param[in] self T=1' protocol stack to set BWT for.
 * \param[in] bwt_ms BWT value to be used.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 * \cond FULL_DOCUMENTATION_BUILD
 * \relates ifx_protocol
 * \endcond
 */
ifx_status_t ifx_t1prime_set_bwt(ifx_protocol_t *self, uint16_t bwt_ms);

/**
 * \brief Custom function type used to wait for T=1' data interrupt.
 *
 * \details Set using ifx_t1prime_set_irq_handler() to enable IRQ mode.
 *
 * \param[in] self T=1' protocol stack waiting for data interrupt.
 * \param[in] timeout_us Timeout until interrupt has to have triggered (in
 * [us]).
 * \return ifx_status_t \c IFX_T1PRIME_IRQ_TRIGGERED if successful, any other
 * value in case of error.
 */
typedef ifx_status_t (*ifx_t1prime_irq_handler_t)(ifx_protocol_t *self,
                                                  uint32_t timeout_us);

/**
 * \brief Sets T=1' interrupt handler function.
 *
 * \details If value is not set (\c NULL - default) then polling mode will be
 * used.
 *
 * \param[in] self T=1' protocol stack to set interrupt handler for.
 * \param[in] irq Interrupt handler to be used.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 * \cond FULL_DOCUMENTATION_BUILD
 * \relates ifx_protocol
 * \endcond
 */
ifx_status_t ifx_t1prime_set_irq_handler(ifx_protocol_t *self,
                                         ifx_t1prime_irq_handler_t irq);

/**
 * \brief Getter for T=1' interrupt handler function.
 *
 * \details If value is not set then polling mode is used and \c NULL is
 * returned.
 *
 * \param[in] self T=1' protocol stack to get interrupt handler for.
 * \param[in] irq_buffer Buffer to store interrupt handler in.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other value in case of
 * * error. \cond FULL_DOCUMENTATION_BUILD \relates ifx_protocol \endcond
 */
ifx_status_t ifx_t1prime_get_irq_handler(ifx_protocol_t *self,
                                         ifx_t1prime_irq_handler_t *irq_buffer);

/**
 * \brief Performs Global Platform T=1' power on reset (POR).
 *
 * \details Sends S(POR request).
 *
 * \param[in] self Protocol stack for performing necessary operations.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
ifx_status_t ifx_t1prime_s_por(ifx_protocol_t *self);

/**
 * \brief Performs Global Platform T=1' software reset (SWR).
 *
 * \details Sends S(SWR request) and expects S(SWR response).
 *
 * \param[in] self Protocol stack for performing necessary operations.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
ifx_status_t ifx_t1prime_s_swr(ifx_protocol_t *self);

#ifdef __cplusplus
}
#endif

#endif // IFX_T1PRIME_H
