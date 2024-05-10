// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/ifx-apdu-error.h
 * \brief APDU (response) error utility.
 */
#ifndef IFX_APDU_ERROR_H
#define IFX_APDU_ERROR_H

#include <stddef.h>
#include <stdint.h>
#include "infineon/ifx-error.h"
#include "infineon/ifx-apdu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief IFX error encoding function identifier for
 * apdu_response_get_error_message().
 */
#define IFX_ERROR_GET_APDU        0x05u

/**
 * \brief IFX register error message identifier if errors are not register
 */
#define IFX_ERRORS_NOT_REGISTERED 0x07u

/**
 * \brief IFX error message error map list is empty or null
 */
#define IFX_ERROR_MSG_EMPTY_MAP_LIST                                           \
    "Invalid error map list input. Cannot extract the error message."

/**
 * \brief IFX error message error map list is empty or null
 */
#define IFX_ERROR_MSG_UNAVAILABLE "Error description unavailable"

/**
 * \brief Defines empty string ("\n")
 */
#define IFX_EMPTY_STRING          ""

/**
 * \brief Struct that holds APDU Error's message with respective to the INS and
 * SW
 */
typedef struct
{
    uint8_t ins;      /**<the INS byte */
    uint16_t sw;      /**<SW byte */
    uint8_t *message; /**< APDU Error's message with NULL terminated string*/
} ifx_apdu_error_map_t;

/**
 * \brief Returns the error message with respect to the INS and SW
 * \param[in] apdu_error_map_list pointer to apdu error map
 * \param[in] apdu_error_map_list_length length of the apdu error map list list
 * \param[in] apdu pointer to APDU command for which the error message to be
 * return.
 * \param[in] response pointer to ifx_apdu_response_t for which the error
 * message to be return.
 * \return uint8_t \c Error message with null terminated string. it will return
 * empty string ("\n") if status word is 0x9000.
 */
uint8_t *ifx_apdu_error_get_message(ifx_apdu_error_map_t *apdu_error_map_list,
                                    uint8_t apdu_error_map_list_length,
                                    const ifx_apdu_t *apdu,
                                    const ifx_apdu_response_t *response);

#ifdef __cplusplus
}

#endif
#endif // IFX_APDU_ERROR_H
