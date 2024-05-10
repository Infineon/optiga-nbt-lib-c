// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file ifx-error.c
 * \brief Infineon specific error code creation and parsing.
 */
#include "infineon/ifx-error.h"

/**
 * \brief Checks if status code indicates error.
 *
 * \param[in] status_code Status code to be checked.
 * \return bool \c true if \p status_code indicates error.
 */
bool ifx_error_check(ifx_status_t status_code)
{
    return (status_code & IFX_ERROR_INDICATOR) != 0;
}

/**
 * \brief Extracts library identifier from error code.
 *
 * \details No checks for error indicator are performed, so ifx_error_check()
 * must have been called beforehand.
 *
 * \param[in] error_code Error code to extract module identifier from.
 * \return uint8_t Library identifier.
 */
uint8_t ifx_error_get_library(ifx_status_t error_code)
{
    return (uint8_t) ((error_code & 0x7F000000U) >> 24);
}

/**
 * \brief Extracts module identifier from error code.
 *
 * \details No checks for error indicator are performed, so ifx_error_check()
 * must have been called beforehand.
 *
 * \param[in] error_code Error code to extract module identifier from.
 * \return uint8_t Module identifier.
 */
uint8_t ifx_error_get_module(ifx_status_t error_code)
{
    return (uint8_t) ((error_code & 0x00ff0000U) >> 16);
}

/**
 * \brief Extracts function identifier from error code.
 *
 * \details No checks for error indicator are performed, so ifx_error_check()
 * must have been called beforehand.
 *
 * \param[in] error_code Error code to extract function identifier from.
 * \return uint8_t Function identifier.
 */
uint8_t ifx_error_get_function(ifx_status_t error_code)
{
    return (uint8_t) ((error_code & 0x0000ff00U) >> 8);
}

/**
 * \brief Extracts function specific reason from error code.
 *
 * \details No checks for error indicator are performed, so ifx_error_check()
 * must have been called beforehand.
 *
 * \param[in] error_code Error code to extract function specific reason from.
 * \return uint8_t Function specific reason.
 */
uint8_t ifx_error_get_reason(ifx_status_t error_code)
{
    return (uint8_t) (error_code & 0x000000ffU);
}
