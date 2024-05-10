// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file ifx-apdu.c
 * \brief APDU (response) en-/decoding utility.
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "infineon/ifx-error.h"
#include "infineon/ifx-apdu.h"

/**
 * \brief Decodes binary data to its member representation in APDU object.
 *
 * \param[out] apdu APDU object to store values in.
 * \param[in] data Binary data to be decoded.
 * \param[in] data_len Number of bytes in \p data.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other value in
 * case of error.
 * \relates APDU
 */
ifx_status_t ifx_apdu_decode(ifx_apdu_t *apdu, const uint8_t *data,
                             size_t data_len)
{
    // Validate parameters
    if ((apdu == NULL) || (data == NULL))
    {
        return IFX_ERROR(LIB_APDU, IFX_APDU_DECODE, IFX_ILLEGAL_ARGUMENT);
    }

    // Minimum APDU length 4 bytes -> header only
    if (data_len < 4U)
    {
        return IFX_ERROR(LIB_APDU, IFX_APDU_DECODE, IFX_TOO_LITTLE_DATA);
    }

    // Parse out header information
    apdu->cla = data[0];
    apdu->ins = data[1];
    apdu->p1 = data[2];
    apdu->p2 = data[3];

    // Set default values just to be sure
    apdu->lc = 0U;
    apdu->data = NULL;
    apdu->le = 0U;

    // Parse body information
    data += 4;
    data_len -= 4U;

    // ISO7816-3: Case 1
    if (data_len == 0U)
    {
        return IFX_SUCCESS;
    }

    // ISO7816-3: Case 2S
    if (data_len == 1U)
    {
        // Special case {0x00} extends to 0x100
        if (data[0] == 0x00U)
        {
            apdu->le = IFX_APDU_LE_ANY;
        }
        else
        {
            apdu->le = data[0];
        }
        return IFX_SUCCESS;
    }

    // ISO7816-3: Case 2E
    if (data_len == 3U && data[0] == 0x00U)
    {
        uint16_t parsed_le = (data[1] << 8) | data[2];

        // Special case {0x00, 0x00} extends to 0x10000
        if (parsed_le == 0x0000U)
        {
            apdu->le = IFX_APDU_LE_ANY_EXTENDED;
        }
        else
        {
            apdu->le = parsed_le;
        }
        return IFX_SUCCESS;
    }

    // ISO7816-3: Case 3S or Case 4S
    bool extended_length = false;
    if (data[0] != 0x00U)
    {
        apdu->lc = data[0];
        data += 1;
        data_len -= 1;
    }
    // ISO7816-3: Case 3E or Case 4E
    else
    {
        if (data_len < 3U)
        {
            return IFX_ERROR(LIB_APDU, IFX_APDU_DECODE, IFX_LC_MISMATCH);
        }
        apdu->lc = (data[1] << 8) | data[2];
        data += 3;
        data_len -= 3U;
        extended_length = true;
    }

    // Check if LC matches data
    if (data_len < apdu->lc)
    {
        return IFX_ERROR(LIB_APDU, IFX_APDU_DECODE, IFX_LC_MISMATCH);
    }

    // Copy data
    apdu->data = (uint8_t *) malloc(apdu->lc);
    if (apdu->data == NULL)
    {
        // clang-format off
        return IFX_ERROR(LIB_APDU, IFX_APDU_DECODE, IFX_OUT_OF_MEMORY); // LCOV_EXCL_LINE
        // clang-format on
    }
    memcpy(apdu->data, data, apdu->lc); // Flawfinder: ignore
    data += apdu->lc;
    data_len -= apdu->lc;

    // ISO7816-3: Case 3S or Case 3E
    if (data_len == 0U)
    {
        return IFX_SUCCESS;
    }

    // ISO7816-3: Case 4S
    if (data_len == 1U)
    {
        // ISO7816-3 Case 4S requires LC to also have short form
        if (extended_length)
        {
            free(apdu->data);
            apdu->data = NULL;
            return IFX_ERROR(LIB_APDU, IFX_APDU_DECODE,
                             IFX_EXTENDED_LEN_MISMATCH);
        }

        // Special case {0x00} extends to 0x100
        if (data[0] == 0x00U)
        {
            apdu->le = IFX_APDU_LE_ANY;
        }
        else
        {
            apdu->le = data[0];
        }
        return IFX_SUCCESS;
    }

    // ISO7816-3: Case 4E
    if (data_len == 2U)
    {
        // ISO7816-3 Case 4E requires LC to also have extended form
        if (!extended_length)
        {
            free(apdu->data);
            apdu->data = NULL;
            return IFX_ERROR(LIB_APDU, IFX_APDU_DECODE,
                             IFX_EXTENDED_LEN_MISMATCH);
        }

        uint16_t parsed_le = (data[0] << 8) | data[1];
        // Special case {0x00, 0x00} extends to 0x10000
        if (parsed_le == 0x00U)
        {
            apdu->le = IFX_APDU_LE_ANY_EXTENDED;
        }
        else
        {
            apdu->le = parsed_le;
        }
        return IFX_SUCCESS;
    }

    // Otherwise incorrect data
    free(apdu->data);
    apdu->data = NULL;

    return IFX_ERROR(LIB_APDU, IFX_APDU_DECODE, IFX_LC_MISMATCH);
}

/**
 * \brief Encodes APDU to its binary representation.
 *
 * \param[in] apdu APDU to be encoded.
 * \param[out] buffer Buffer to store encoded data in.
 * \param[out] buffer_len Pointer for storing number of bytes in \p buffer.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other value in
 * case of error.
 * \relates APDU
 */
ifx_status_t ifx_apdu_encode(const ifx_apdu_t *apdu, uint8_t **buffer,
                             size_t *buffer_len)
{
    // Calculate required buffer size (minimum 4 bytes for header)
    size_t buffer_size = 4U + apdu->lc;
    bool extended_length = (apdu->lc > 0xffU) || (apdu->le > IFX_APDU_LE_ANY);
    if (extended_length)
    {
        // ISO7816-3 Case 3E or 4E
        if (apdu->lc > 0U)
        {
            buffer_size += 3U;

            // ISO7816-3 Case 4E
            if (apdu->le > 0U)
            {
                buffer_size += 2U;
            }
        }
        // ISO7816-3 Case 2E
        else
        {
            buffer_size += 3U;
        }
    }
    else
    {
        // ISO7816-3 Case 3S or 4S
        if (apdu->lc > 0U)
        {
            buffer_size += 1U;
        }

        // ISO7816-3 Case 2S or 4S
        if (apdu->le > 0U)
        {
            buffer_size += 1U;
        }
    }

    // Allocate memory for buffer
    *buffer = (uint8_t *) malloc(buffer_size);
    if (*buffer == NULL)
    {
        // clang-format off
        return IFX_ERROR(LIB_APDU, IFX_APDU_ENCODE, IFX_OUT_OF_MEMORY); // LCOV_EXCL_LINE
        // clang-format on
    }

    // Encode header information
    (*buffer)[0] = apdu->cla;
    (*buffer)[1] = apdu->ins;
    (*buffer)[2] = apdu->p1;
    (*buffer)[3] = apdu->p2;

    // ISO7816-3 Case 3 or Case 4
    if (apdu->lc > 0x00U)
    {
        size_t offset = 4U;

        // ISO7816-3 Case 3E or Case 4E
        if (extended_length)
        {
            (*buffer)[offset] = 0x00U;
            (*buffer)[offset + 1] = (apdu->lc & 0xff00U) >> 8;
            (*buffer)[offset + 2] = apdu->lc & 0xffU;
            offset += 3;
        }
        // ISO7816-3 Case 3S or Case 4S
        else
        {
            (*buffer)[offset] = apdu->lc & 0xffU;
            offset += 1U;
        }
        memcpy((*buffer) + offset, apdu->data, apdu->lc); // Flawfinder: ignore
        offset += apdu->lc;

        // ISO7816-3 Case 4
        if (apdu->le > 0U)
        {
            // ISO7816-3 Case 4E
            if (extended_length)
            {
                // Special case 0x10000 extends to {0x00, 0x00}
                if (apdu->le == IFX_APDU_LE_ANY_EXTENDED)
                {
                    (*buffer)[offset] = 0x00U;
                    (*buffer)[offset + 1] = 0x00U;
                }
                else
                {
                    (*buffer)[offset] = (apdu->le & 0xff00U) >> 8;
                    (*buffer)[offset + 1] = apdu->le & 0xffU;
                }
            }
            // ISO7816-3 Case 4S
            else
            {
                // Special case 0x100 extends to {0x00}
                if (apdu->le == IFX_APDU_LE_ANY)
                {
                    (*buffer)[offset] = 0x00U;
                }
                else
                {
                    (*buffer)[offset] = apdu->le & 0xffU;
                }
            }
        }
    }
    // ISO7816-3 Case 1 or Case 2
    else
    {
        // ISO7816-3 Case 2
        if (apdu->le > 0U)
        {
            // ISO7816-3 Case 2E
            if (extended_length)
            {
                (*buffer)[4] = 0x00U;
                // Special case 0x10000 extends to {0x00, 0x00}
                if (apdu->le == IFX_APDU_LE_ANY_EXTENDED)
                {
                    (*buffer)[5] = 0x00U;
                    (*buffer)[6] = 0x00U;
                }
                else
                {
                    (*buffer)[5] = (apdu->le & 0xff00U) >> 8;
                    (*buffer)[6] = apdu->le & 0xffU;
                }
            }
            // ISO7816-3 Case 2S
            else
            {
                // Special case 0x100 extends to {0x00}
                if (apdu->le == IFX_APDU_LE_ANY)
                {
                    (*buffer)[4] = 0x00U;
                }
                else
                {
                    (*buffer)[4] = apdu->le & 0xffU;
                }
            }
        }
    }

    *buffer_len = buffer_size;
    return IFX_SUCCESS;
}

/**
 * \brief Frees memory associated with APDU object (but not object itself).
 *
 * \details APDU objects may contain dynamically allocated data (e.g.
 * APDU.data). Users would need to manually check which members have been
 * dynamically allocated and free them themselves. Calling this function will
 * ensure that all dynamically allocated members have been freed.
 *
 * \param[in] apdu APDU object whose data shall be freed.
 * \relates APDU
 */
void ifx_apdu_destroy(ifx_apdu_t *apdu)
{
    if (apdu != NULL)
    {
        if ((apdu->lc > 0U) && (apdu->data != NULL))
        {
            free(apdu->data);
        }
        apdu->data = NULL;
        apdu->lc = 0U;
    }
}

/**
 * \brief Decodes binary data to its member representation in
 * ifx_apdu_response_t object.
 *
 * \param[out] response ifx_apdu_response_t object to store values in.
 * \param[in] data Binary data to be decoded.
 * \param[in] data_len Number of bytes in \p data.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 * \relates ifx_apdu_response_t
 */
ifx_status_t ifx_apdu_response_decode(ifx_apdu_response_t *response,
                                      const uint8_t *data, size_t data_len)
{
    // Minimum APDU response length 2 bytes -> status word only
    if (data_len < 2U)
    {
        return IFX_ERROR(LIB_APDU, IFX_APDU_RESPONSE_DECODE,
                         IFX_TOO_LITTLE_DATA);
    }

    // Copy data
    response->len = data_len - 2U;
    if (data_len > 2U)
    {
        response->data = (uint8_t *) malloc(response->len);
        if (response->data == NULL)
        {
            // clang-format off
            return IFX_ERROR(LIB_APDU, IFX_APDU_RESPONSE_DECODE, IFX_OUT_OF_MEMORY); // LCOV_EXCL_LINE
            // clang-format on
        }
        memcpy(response->data, data, response->len); // Flawfinder: ignore
    }
    else
    {
        response->data = NULL;
    }

    // Decode status word
    response->sw = (data[response->len] << 8) | data[response->len + 1];
    return IFX_SUCCESS;
}

/**
 * \brief Encodes ifx_apdu_response_t to its binary representation.
 *
 * \param[in] response ifx_apdu_response_t to be encoded.
 * \param[out] buffer Buffer to store encoded data in.
 * \param[out] buffer_len Pointer for storing number of bytes in \p buffer.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 * \relates ifx_apdu_response_t
 */
ifx_status_t ifx_apdu_response_encode(const ifx_apdu_response_t *response,
                                      uint8_t **buffer, size_t *buffer_len)
{
    // Allocate memory for buffer
    *buffer = (uint8_t *) malloc(response->len + 2U);
    if (*buffer == NULL)
    {
        // clang-format off
        return IFX_ERROR(LIB_APDU, IFX_APDU_RESPONSE_ENCODE, IFX_OUT_OF_MEMORY); // LCOV_EXCL_LINE
        // clang-format on
    }

    // Add data
    if (response->len > 0U)
    {
        memcpy(*buffer, response->data, response->len); // Flawfinder: ignore
    }

    // Add status word
    (*buffer)[response->len] = (response->sw & 0xff00U) >> 8;
    (*buffer)[response->len + 1] = response->sw & 0xffU;

    *buffer_len = (response->len + 2U) & 0x1ffffU;
    return IFX_SUCCESS;
}

/**
 * \brief Frees memory associated with ifx_apdu_response_t object (but not
 * object itself).
 *
 * \details ifx_apdu_response_t objects will most likely be populated by
 * ifx_apdu_response_decode(). Users would need to manually check which members
 * have been dynamically allocated and free them themselves. Calling this
 * function will ensure that all dynamically allocated members have been freed.
 *
 * \param[in] response ifx_apdu_response_t object whose data shall be freed.
 * \relates ifx_apdu_response_t
 */
void ifx_apdu_response_destroy(ifx_apdu_response_t *response)
{
    if (response != NULL)
    {
        if ((response->len > 0U) && (response->data != NULL))
        {
            free(response->data);
        }
        response->data = NULL;
        response->len = 0U;
    }
}
