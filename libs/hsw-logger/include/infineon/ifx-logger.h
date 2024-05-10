// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/ifx-logger.h
 * \brief Generic logging API.
 */
#ifndef IFX_LOGGER_H
#define IFX_LOGGER_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "infineon/ifx-error.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Logger library identifier ID.
 */
#define LIB_LOGGER              UINT8_C(0x0C)

/**
 * \brief Reusable IFX error encoding function identifier for any function
 * initializing a Logger.
 */
#define IFX_LOGGER_INITIALIZE   UINT8_C(0x01)

/**
 * \brief Reusable IFX error encoding function identifier for all functions
 * logging data.
 */
#define IFX_LOGGER_LOG          UINT8_C(0x02)

/**
 * \brief IFX error encoding function identifier for ifx_logger_set_level().
 */
#define IFX_LOGGER_SET_LEVEL    UINT8_C(0x03)

/**
 * \brief IFX error encoding error reason if error occurred formatting string in
 * ifx_logger_log().
 *
 * \details Must be used in combination with \ref IFX_LOGGER_LOG.
 */
#define IFX_LOGGER_FORMAT_ERROR 0x01u

// Forward declaration only
typedef struct ifx_logger ifx_logger_t;

/** \enum ifx_log_level
 * \brief Log level for filtering messages to actually be logged.
 */
typedef enum
{
    /**
     * \brief Debug information for finding problems in library.
     */
    IFX_LOG_DEBUG = 0,

    /**
     * \brief Information that help trace the program's normal execution flow.
     */
    IFX_LOG_INFO = 1,

    /**
     * \brief Information that warns of potential problems.
     */
    IFX_LOG_WARN = 2,

    /**
     * \brief Information about (recoverable) errors.
     */
    IFX_LOG_ERROR = 3,

    /**
     * \brief Information about non-recoverable errors.
     */
    IFX_LOG_FATAL = 4
} ifx_log_level;

/**
 * \brief Initializes Logger object by setting all members to valid (but
 * potentially unusable) values.
 *
 * \details Concrete implementations can call this function to ensure all
 * members are set to their correct default values before populating the
 * required fields.
 *
 * \param[in] self Logger object to be initialized.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 * \relates ifx_logger
 */
ifx_status_t ifx_logger_initialize(ifx_logger_t *self);

/**
 * \brief Log formatted message.
 *
 * \details Uses printf syntax for message formatting.
 *
 * \code
 *     ifx_logger_log(&logger,
 *               "example",
 *               IFX_LOG_INFO,
 *               "The answer to life, the universe, and everything is: %d",
 *               42)
 * \endcode
 *
 * \param[in] self Logger object holding concrete implementation. Use
 * \ref ifx_logger_default to use default logger set via
 * ifx_logger_set_default().
 * \param[in] source String with information where the log originated from.
 * \param[in] level ifx_log_level of message (used for filtering).
 * \param[in] formatter Format string (printf syntax).
 * \param[in] ... String format arguments (printf syntax).
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other value in
 * case of error.
 * \relates ifx_logger
 */
ifx_status_t ifx_logger_log(const ifx_logger_t *self, const char *source,
                            ifx_log_level level, const char *formatter, ...);

/**
 * \brief Extension of ifx_logger_log() for logging byte arrays.
 *
 * \code
 *     uint8_t data[] = {0x01u, 0x02u, 0x03u, 0x04u};
 *     size_t data_len = sizeof(data);
 *     ifx_logger_log_bytes(&logger, TAG, IFX_LOG_INFO, ">> ", data, data_len, "
 * ");
 *     // >> 01 02 03 04
 * \endcode
 *
 * \param[in] self Logger object holding concrete implementation. Use
 * \ref ifx_logger_default to use default logger set via
 * ifx_logger_set_default().
 * \param[in] source String with information where the log originated from.
 * \param[in] level ifx_log_level of message (used for filtering).
 * \param[in] msg Optional message to be prepended to byte array (might be
 * \c NULL).
 * \param[in] data Data to be logged.
 * \param[in] data_len Number of bytes in \p data.
 * \param[in] delimiter Delimiter to be used between bytes (might be \c NULL for
 * empty string).
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other value in
 * case of error.
 * \relates ifx_logger
 */
ifx_status_t ifx_logger_log_bytes(const ifx_logger_t *self, const char *source,
                                  ifx_log_level level, const char *msg,
                                  const uint8_t *data, size_t data_len,
                                  const char *delimiter);

/**
 * \brief Implementation specific log function.
 *
 * \details Used by ifx_logger_log() to call concrete implementation.
 *
 * \param[in] self Logger object the function was called for.
 * \param[in] source String with information where the log originated from.
 * \param[in] level ifx_log_level of message (used for filtering).
 * \param[in] message Formatted string to be logged.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other value in
 * case of error.
 */
typedef ifx_status_t (*ifx_logger_log_callback_t)(const ifx_logger_t *self,
                                                  const char *source,
                                                  ifx_log_level level,
                                                  const char *message);

/**
 * \brief All messages with level lower than \p level shall be discarded by
 * the logger.
 *
 * \param[in] self Logger object to set log level for.
 * \param[in] level Minimum log level of interest.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 * \relates ifx_logger
 */
ifx_status_t ifx_logger_set_level(ifx_logger_t *self, ifx_log_level level);

/**
 * \brief Implementation specific log level setter.
 *
 * \details Used by ifx_logger_set_level() to call concrete implementation.
 * Depending on the logging mechanism used this might wrap to an external
 * library function, etc.
 *
 * \param[in] self Logger object to set log level for.
 * \param[in] level Minimum log level of interest.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
typedef ifx_status_t (*ifx_logger_set_level_callback_t)(ifx_logger_t *self,
                                                        ifx_log_level level);

/**
 * \brief Frees memory associated with Logger object (but not object itself).
 *
 * \details Logger objects might contain data that needs to be released (e.g.
 * open file handles). Users would need to know the concrete type based on the
 * used implementation. This would negate all benefits of having a generic
 * interface. Calling this function will ensure that all dynamically allocated
 * members have been freed.
 *
 * \param[in] self Logger object whose data shall be freed.
 * \relates ifx_logger
 */
void ifx_logger_destroy(ifx_logger_t *self);

/**
 * \brief Implementation specific destructor.
 *
 * \details Different loggers may need different clean-up functionality. This
 * function type gives a generic interface for performing clean-up.
 *
 * \param[in] self Logger object being destroyed.
 */
typedef void (*ifx_logger_destroy_callback_t)(ifx_logger_t *self);

/**
 * \brief Sets default Logger to be used.
 *
 * \details Libraries that cannot keep track of the desired logger themselves
 * can call ifx_logger_log() (or any variation thereof) with the \c logger
 * parameter set to \ref ifx_logger_default to use the Logger given to this
 * function.
 *
 * \param[in] logger Default logger instance to be used.
 * \relates ifx_logger
 */
void ifx_logger_set_default(ifx_logger_t *logger);

/**
 * \brief Default Logger instance to be used.
 *
 * \details Set via ifx_logger_set_default(), do **NOT** edit manually.
 */
extern ifx_logger_t *ifx_logger_default;

/** \struct ifx_logger
 * \brief Generic Logger object used to decapsulate concrete implementation from
 * interface.
 */
struct ifx_logger
{
    /**
     * \brief Private logging function for concrete implementation.
     *
     * \details Set by implementation's initialization function, do **NOT** set
     * manually!
     */
    ifx_logger_log_callback_t _log;

    /**
     * \brief Private function to set log level.
     *
     * \details Set by implementation's initialization function, do **NOT** set
     * manually. By default ifx_logger_set_level() will simply update
     * Logger._level. If further logic is required implementations can set this
     * function to a more complex setter. If no custom setter is required use \c
     * NULL.
     */
    ifx_logger_set_level_callback_t _set_level;

    /**
     * \brief Private destructor if further cleanup is necessary.
     *
     * \details Set by implementation's initialization function, do **NOT** set
     * manually. ifx_logger_destroy(ifx_logger_t*) will call `free()` for
     * Logger._data . If any further cleanup is necessary implement it in this
     * function. Otherwise use \c NULL.
     */
    ifx_logger_destroy_callback_t _destructor;

    /**
     * \brief Private member for minimum log level used for filtering messages.
     *
     * \details Set by ifx_logger_set_level(ifx_logger_t*, ifx_log_level), do
     * **NOT** set manually.
     */
    ifx_log_level _level;

    /**
     * \brief Private member for generic logger data as \c void*.
     *
     * \details Only used internally, do **NOT** set manually. Might be \c NULL.
     */
    void *_data;
};

#ifdef __cplusplus
}
#endif

#endif // IFX_LOGGER_H