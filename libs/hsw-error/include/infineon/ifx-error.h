// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/ifx-error.h
 * \brief Infineon specific error code creation and parsing.
 */
#ifndef IFX_ERROR_H
#define IFX_ERROR_H

#include <stdbool.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Encoding of module id and library id into 2 bytes.
 * Bit: 15       Set in macro IFX_ERROR to denote error identifier.
 * Bits: 14-8    Library identifier code
 * Bits: 7-0     Module identifier code
 */
#define IFX_MODULE(library_id, module_id)                                      \
    (uint16_t)(((library_id & 0x7F) << 8) | (module_id & 0xFF))

/**
 * \brief Default status code for successful calls to any function.
 */
#define IFX_SUCCESS           ((ifx_status_t) 0x00000000)

/**
 * \brief Error indicator bitmask.
 */
#define IFX_ERROR_INDICATOR   ((ifx_status_t) 0x80000000u)

/**
 * \brief Function independent error reason for unspecified errors.
 */
#define IFX_UNSPECIFIED_ERROR ((uint8_t) 0xffu)

/**
 * \brief Function independent error reason for out of memory errors.
 */
#define IFX_OUT_OF_MEMORY     ((uint8_t) 0xfeu)

/**
 * \brief Function independent error reason for illegal argument value.
 */
#define IFX_ILLEGAL_ARGUMENT  ((uint8_t) 0xfdu)

/**
 * \brief Function independent error reason if too little data available.
 */
#define IFX_TOO_LITTLE_DATA   ((uint8_t) 0xfcu)

/**
 * \brief Function independent error reason if any object is in an invalid
 * state.
 */
#define IFX_INVALID_STATE     ((uint8_t) 0xfbu)

/**
 * \brief Function independent error reason for errors that should not occur
 * because they should be prevented by the code.
 */
#define IFX_PROGRAMMING_ERROR ((uint8_t) 0xfau)

/**
 * \brief Creates IFX encoded error code for given module, function and reason.
 *
 * \details IFX error codes have the following schema:
 *          * bit 31      Error indicator.
 *          * bits 30-24  Library identifier(e.g. LIB_NBT_APDU)
 *          * bits 23-16  Module identifier (e.g. NBT_BUILD_CMD_ID).
 *          * bits 15-8   Function identifier in module (e.g. apdu_decode).
 *          * bits 7-0    Function specific reason (e.g. apdu_decode too little
 * data).
 *
 * \param[in] module Library and module identifier.
 * \param[in] function Function identifier.
 * \param[in] reason Function specific reason.
 */
#define IFX_ERROR(module, function, reason)                                    \
    ((ifx_status_t) (IFX_ERROR_INDICATOR | (((module >> 8) & 0x7Fu) << 24) |   \
                     (((module) & 0xFFu) << 16) |                              \
                     (((function) & 0xFFu) << 8) | ((reason) & 0xFFu)))

/**
 * \brief Custom return code type used by all Infineon host software libraries.
 */
typedef uint32_t ifx_status_t;

/**
 * \brief Checks if status code indicates error.
 *
 * \param[in] status_code Status code to be checked.
 * \return bool \c true if \p status_code indicates error.
 */
bool ifx_error_check(ifx_status_t status_code);

/**
 * \brief Extracts library identifier from error code.
 *
 * \details No checks for error indicator are performed, so ifx_error_check()
 * must have been called beforehand.
 *
 * \param[in] error_code Error code to extract module identifier from.
 * \return uint8_t Library identifier.
 */
uint8_t ifx_error_get_library(ifx_status_t error_code);

/**
 * \brief Extracts module identifier from error code.
 *
 * \details No checks for error indicator are performed, so ifx_error_check()
 * must have been called beforehand.
 *
 * \param[in] error_code Error code to extract module identifier from.
 * \return uint8_t Module identifier.
 */
uint8_t ifx_error_get_module(ifx_status_t error_code);

/**
 * \brief Extracts function identifier from error code.
 *
 * \details No checks for error indicator are performed, so ifx_error_check()
 * must have been called beforehand.
 *
 * \param[in] error_code Error code to extract function identifier from.
 * \return uint8_t Function identifier.
 */
uint8_t ifx_error_get_function(ifx_status_t error_code);

/**
 * \brief Extracts function specific reason from error code.
 *
 * \details No checks for error indicator are performed, so ifx_error_check()
 * must have been called beforehand.
 *
 * \param[in] error_code Error code to extract function specific reason from.
 * \return uint8_t Function specific reason.
 */
uint8_t ifx_error_get_reason(ifx_status_t error_code);

#ifdef __cplusplus
}
#endif

#endif // IFX_ERROR_H
