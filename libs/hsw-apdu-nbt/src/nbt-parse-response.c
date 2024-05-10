// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file nbt-parse-response.c
 * \brief Response parser for apdu-nbt commands
 */
#include "infineon/nbt-parse-response.h"

#include "infineon/nbt-apdu-lib.h"

/**
 * \brief Gets the response data and returns the applet version.
 *
 * \param[in] response APDU response object in parameter contains response data.
 * \param[out] applet_version Applet version out parameter object to store
 * applet version.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_parse_applet_version(const ifx_apdu_response_t *response,
                                      nbt_applet_version_t *applet_version)
{
    uint8_t buffer_offset;
// Memory check for applet version
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(applet_version))
    {
        return IFX_ERROR(NBT_PARSE_RESP, NBT_GET_DATA_RESP_PARSER,
                         IFX_ILLEGAL_ARGUMENT);
    }

    if (IFX_VALIDATE_NULL_PTR_MEMORY(response))
    {
        return IFX_ERROR(NBT_PARSE_RESP, NBT_GET_DATA_RESP_PARSER,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    buffer_offset = UINT8_C(0x00);
    if ((0x00 == response->len) ||
        (NBT_GET_DATA_DEFAULT_RESP_TAG != response->data[buffer_offset++]) ||
        (NBT_LEN_FILE_CONTROL_INFO != response->data[buffer_offset++]) ||
        (NBT_TAG_DEFAULT_GET_DATA != response->data[buffer_offset++]) ||
        (NBT_TAG_APPLET_VERSION != response->data[buffer_offset++]) ||
        (NBT_LEN_APPLET_VERSION != response->data[buffer_offset++]))
    {
        return IFX_ERROR(NBT_PARSE_RESP, NBT_GET_DATA_RESP_PARSER,
                         IFX_INVALID_STATE);
    }

    applet_version->version_major = response->data[buffer_offset++];
    applet_version->version_minor = response->data[buffer_offset++];
    IFX_READ_U16(&response->data[buffer_offset],
                 applet_version->version_build_number);

    return IFX_SUCCESS;
}

/**
 * \brief Gets the response data and returns available memory.
 *
 * \param[in] response APDU response object in parameter contains response data.
 * \param[out] available_memory Available memory out parameter object to store
 * available memory.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_parse_available_memory(
    const ifx_apdu_response_t *response,
    nbt_available_memory_t *available_memory)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(available_memory))
    {
        return IFX_ERROR(NBT_PARSE_RESP, NBT_GET_DATA_RESP_PARSER,
                         IFX_ILLEGAL_ARGUMENT);
    }

    if (IFX_VALIDATE_NULL_PTR_MEMORY(response))
    {
        return IFX_ERROR(NBT_PARSE_RESP, NBT_GET_DATA_RESP_PARSER,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    uint8_t buffer_offset = UINT8_C(0x00);

    if ((UINT8_C(0x00) == response->len) ||
        (NBT_GET_DATA_DEFAULT_RESP_TAG != (response->data[buffer_offset++])) ||
        (NBT_LEN_AVAILABLE_MEMORY_INFO != (response->data[buffer_offset++])) ||
        (NBT_TAG_DEFAULT_GET_DATA != (response->data[buffer_offset++])) ||
        (NBT_TAG_AVAILABLE_MEMORY != (response->data[buffer_offset++])) ||
        (NBT_LEN_AVAILABLE_MEMORY != (response->data[buffer_offset++])))
    {
        return IFX_ERROR(NBT_PARSE_RESP, NBT_GET_DATA_RESP_PARSER,
                         IFX_INVALID_STATE);
    }

    uint8_t byte_count =
        (uint8_t) response->len - NBT_GET_DATA_MEMORY_RESP_HEADER_LEN;
    while (byte_count)
    {
        if (NBT_GET_DATA_TAG_AVAILABLE_NVM_MEMORY ==
                ((response->data[buffer_offset])) ||
            (NBT_GET_DATA_TAG_AVAILABLE_TRANSIENT_COD ==
             (response->data[buffer_offset])) ||
            (NBT_GET_DATA_TAG_AVAILABLE_TRANSIENT_COR ==
             (response->data[buffer_offset])))
        {
            uint8_t tag_value = (response->data[buffer_offset++]);
            if (NBT_GET_DATA_TLV_LEN_VALUE != (response->data[buffer_offset++]))
            {
                return IFX_ERROR(NBT_PARSE_RESP, NBT_GET_DATA_RESP_PARSER,
                                 IFX_INVALID_STATE);
            }

            if (tag_value == NBT_GET_DATA_TAG_AVAILABLE_NVM_MEMORY)
            {
                IFX_READ_U16(&response->data[buffer_offset],
                             available_memory->available_memory_size);
            }

            if (tag_value == NBT_GET_DATA_TAG_AVAILABLE_TRANSIENT_COR)
            {
                IFX_READ_U16(&response->data[buffer_offset],
                             available_memory->clear_on_reset);
            }

            if (tag_value == NBT_GET_DATA_TAG_AVAILABLE_TRANSIENT_COD)
            {
                IFX_READ_U16(&response->data[buffer_offset],
                             available_memory->clear_on_deselect);
            }

            buffer_offset += NBT_GET_DATA_TLV_LEN_VALUE;
            byte_count -= NBT_LENGTH_TLV;
        }
        else
        {
            return IFX_ERROR(NBT_PARSE_RESP, NBT_GET_DATA_RESP_PARSER,
                             IFX_INVALID_STATE);
        }
    }

    return IFX_SUCCESS;
}
