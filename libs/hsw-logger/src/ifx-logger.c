// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file ifx-logger.c
 * \brief Generic logging API.
 */
#include "infineon/ifx-logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
 * \relates ifx_logger_t
 */
ifx_status_t ifx_logger_initialize(ifx_logger_t *self)
{
    // Validate parameters
    if (self == NULL)
    {
        return IFX_ERROR(LIB_LOGGER, IFX_LOGGER_INITIALIZE,
                         IFX_ILLEGAL_ARGUMENT);
    }

    // Populate values
    self->_log = NULL;
    self->_set_level = NULL;
    self->_destructor = NULL;
    self->_level = IFX_LOG_FATAL;
    self->_data = NULL;

    return IFX_SUCCESS;
}

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
 * \relates ifx_logger_t
 */
ifx_status_t ifx_logger_log(const ifx_logger_t *self, const char *source,
                            ifx_log_level level, const char *formatter, ...)
{
// Logging disabled at compile time
#ifndef IFX_DISABLE_LOGGING

    // Validate parameters
    if ((self == NULL) || (self->_log == NULL) || (formatter == NULL))
    {
        return IFX_ERROR(LIB_LOGGER, IFX_LOGGER_LOG, IFX_ILLEGAL_ARGUMENT);
    }

    // Pre-check level
    if (level < self->_level)
    {
        return IFX_SUCCESS;
    }

    // Prepare varargs
    va_list args_len;
    va_list args_format;
    va_start(args_len, formatter);
    va_copy(args_format, args_len);
    ifx_status_t status =
        IFX_ERROR(LIB_LOGGER, IFX_LOGGER_LOG, IFX_PROGRAMMING_ERROR);

    do
    {
        // Calculate length of output
        // clang-format off
        size_t output_length = vsnprintf(NULL, 0U, formatter, args_len); // Flawfinder: ignore
        // clang-format on
        char *output = malloc(output_length + 1U);
        if (output == NULL)
        {
            // clang-format off
            status = IFX_ERROR(LIB_LOGGER, IFX_LOGGER_LOG, IFX_OUT_OF_MEMORY); // LCOV_EXCL_LINE
            break; // LCOV_EXCL_LINE
            // clang-format on
        }

        // Disable vsprintf warning because msbuild warning not valid in C99
        int bytes_written = -1;
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996)
#endif
        // clang-format off
        bytes_written = vsprintf(output, formatter, args_format); // Flawfinder: ignore
        // clang-format on
#ifdef _MSC_VER
#pragma warning(pop)
#endif
        if (bytes_written < 0)
        {
            // clang-format off
            status = IFX_ERROR(LIB_LOGGER, IFX_LOGGER_LOG, IFX_LOGGER_FORMAT_ERROR); // LCOV_EXCL_LINE
            break; // LCOV_EXCL_LINE
            // clang-format on
        }

        // Actually dispatch call
        status = self->_log(self, source, level, output);

        // Clean up
        free(output);
        output = NULL;
    } while (0);

    // Cleanup
    va_end(args_len);
    va_end(args_format);
    return status;
#else // IFX_DISABLE_LOGGING
    return IFX_SUCCESS;
#endif
}

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
 * \relates ifx_logger_t
 */
ifx_status_t ifx_logger_log_bytes(const ifx_logger_t *self, const char *source,
                                  ifx_log_level level, const char *msg,
                                  const uint8_t *data, size_t data_len,
                                  const char *delimiter)
{
// Logging disabled at compile time
#ifndef IFX_DISABLE_LOGGING
    // Validate parameters
    if ((self == NULL) || (self->_log == NULL) ||
        ((data == NULL) && (data_len != 0U)))
    {
        return IFX_ERROR(LIB_LOGGER, IFX_LOGGER_LOG, IFX_ILLEGAL_ARGUMENT);
    }

    // Pre-check level
    if (level < self->_level)
    {
        return IFX_SUCCESS;
    }

    // Manually format string
    // clang-format off
    size_t msg_len = (msg != NULL) ? strlen(msg) : 0U; // Flawfinder: ignore
    size_t delimiter_len = (delimiter != NULL) ? strlen(delimiter) : 0U; // Flawfinder: ignore
    // clang-format on
    size_t formatted_len = msg_len + (data_len * 2U);
    if (data_len > 0U)
    {
        formatted_len += (data_len - 1U) * delimiter_len;
    }
    char *formatted = malloc(formatted_len + 1U);
    if (formatted == NULL)
    {
        // clang-format off
        return IFX_ERROR(LIB_LOGGER, IFX_LOGGER_LOG, IFX_OUT_OF_MEMORY); // LCOV_EXCL_LINE
        // clang-format on
    }
    if (msg_len > 0U)
    {
        // clang-format off
        memcpy(formatted, msg, msg_len); // Flawfinder: ignore
        // clang-format on
    }
    for (size_t i = 0U; i < data_len; i++)
    {
// Disable sprintf warning because msbuild warning not valid in C99
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996)
#endif
        // Print byte as 2 character hex value
        // clang-format off
        sprintf(formatted + msg_len + (i * (2 + delimiter_len)), "%02x", data[i]); // Flawfinder: ignore
        // clang-format on
#ifdef _MSC_VER
#pragma warning(pop)
#endif
        // Print delimiter
        if ((delimiter_len > 0U) && ((i + 1U) != data_len))
        {
            // clang-format off
            memcpy(formatted + msg_len + (i * (2 + delimiter_len)) + 2, delimiter, delimiter_len); // Flawfinder: ignore
            // clang-format on
        }
    }
    formatted[formatted_len] = '\x00';

    // Actually log message
    ifx_status_t status = self->_log(self, source, level, formatted);
    free(formatted);
    formatted = NULL;
    return status;
#else // IFX_DISABLE_LOGGING
    return IFX_SUCCESS;
#endif
}

/**
 * \brief Sets minimum log level of interest.
 *
 * \details All messages with level lower than \p level shall be discarded by
 * the logger.
 *
 * \param[in] self Logger object to set log level for.
 * \param[in] level Mimimum log level of interest.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 * \relates ifx_logger_t
 */
ifx_status_t ifx_logger_set_level(ifx_logger_t *self, ifx_log_level level)
{
    // Validate parameters
    if (self == NULL)
    {
        return IFX_ERROR(LIB_LOGGER, IFX_LOGGER_SET_LEVEL,
                         IFX_ILLEGAL_ARGUMENT);
    }

    // Call implementation specific function if available
    ifx_status_t status = IFX_SUCCESS;
    if (self->_set_level != NULL)
    {
        status = self->_set_level(self, level);
    }

    // Cache level for later use
    if (!ifx_error_check(status))
    {
        self->_level = level;
    }
    return status;
}

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
 * \relates ifx_logger_t
 */
void ifx_logger_destroy(ifx_logger_t *self)
{
    if (self != NULL)
    {
        // Check if logger has custom cleanup function
        if (self->_destructor != NULL)
        {
            self->_destructor(self);
        }

        // Check if properties have been missed by concrete implementation
        if (self->_data != NULL)
        {
            free(self->_data);
            self->_data = NULL;
        }

        // Clear rest of members
        self->_log = NULL;
        self->_set_level = NULL;
        self->_destructor = NULL;
        self->_level = IFX_LOG_FATAL;
    }
}

/**
 * \brief Sets default Logger to be used.
 *
 * \details Libraries that cannot keep track of the desired logger themselves
 * can call ifx_logger_log() (or any variation thereof) with the \c logger
 * parameter set to \ref ifx_logger_default to use the Logger given to this
 * function.
 *
 * \param[in] logger Default logger instance to be used.
 * \relates ifx_logger_t
 */
void ifx_logger_set_default(ifx_logger_t *logger)
{
    ifx_logger_default = logger;
}

/**
 * \brief Default (default) Logger instance to be used - does nothing.
 */
static ifx_logger_t ifx_logger_noop = {._log = NULL,
                                       ._set_level = NULL,
                                       ._destructor = NULL,
                                       ._level = IFX_LOG_FATAL,
                                       ._data = NULL};

/**
 * \brief Default Logger instance to be used.
 *
 * \details Set via ifx_logger_set_default(), do **NOT** edit manually.
 */
ifx_logger_t *ifx_logger_default = &ifx_logger_noop;
