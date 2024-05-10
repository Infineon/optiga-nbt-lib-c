// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file ifx-apdu-error.c
 * \brief APDU (response) en-/decoding utility.
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "infineon/ifx-error.h"
#include "infineon/ifx-apdu.h"
#include "infineon/ifx-apdu-error.h"

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
                                    const ifx_apdu_response_t *response)
{

    uint8_t index = UINT8_C(0);
    uint8_t *apdu_error = (uint8_t *) IFX_ERROR_MSG_UNAVAILABLE;

    if (IFX_CHECK_SW_OK(response->sw))
    {
        return (uint8_t *) IFX_EMPTY_STRING;
    }
    if (apdu_error_map_list == NULL)
    {
        return (uint8_t *) IFX_ERROR_MSG_EMPTY_MAP_LIST;
    }
    while (index < apdu_error_map_list_length)
    {
        if ((apdu_error_map_list[index].ins == apdu->ins) &&
            (apdu_error_map_list[index].sw == response->sw))
        {
            apdu_error = apdu_error_map_list[index].message;
            break;
        }
        index++;
    }
    return apdu_error;
}
