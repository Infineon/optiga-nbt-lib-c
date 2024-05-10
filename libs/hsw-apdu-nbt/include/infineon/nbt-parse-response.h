// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/nbt-parse-response.h
 * \brief Response parser for apdu-nbt commands
 */
#ifndef NBT_PARSE_RESPONSE_H
#define NBT_PARSE_RESPONSE_H

#include "infineon/ifx-apdu.h"
#include "infineon/ifx-utils.h"
#include "infineon/nbt-apdu-lib.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Function identifier */

/**
 * \brief Identifier for command get response parser available memory
 */
#define NBT_GET_DATA_RESP_PARSER                 UINT8_C(0x01)

/**
 * \brief Length of the header tags of the available memory
 */
#define NBT_GET_DATA_MEMORY_RESP_HEADER_LEN      UINT8_C(0x05)

/**
 * \brief Length of one memory TLV is present in the response.
 */
#define NBT_LENGTH_TLV                           UINT8_C(0x04)

/**
 * \brief Get data response will contain this tag as a reference.
 */
#define NBT_GET_DATA_DEFAULT_RESP_TAG            UINT8_C(0x6F)

/**
 * \brief Default get data tag
 */
#define NBT_TAG_DEFAULT_GET_DATA                 UINT8_C(0xDF)

/**
 * \brief Offset for applet version type in the get data.
 */
#define NBT_TAG_APPLET_VERSION                   UINT8_C(0x3A)

/**
 * \brief Offset for available memory type in the get data.
 */
#define NBT_TAG_AVAILABLE_MEMORY                 UINT8_C(0x3B)

/**
 * \brief File control information length for applet version
 */
#define NBT_LEN_FILE_CONTROL_INFO                UINT8_C(0x07)

/**
 * \brief Length of applet version
 */
#define NBT_LEN_APPLET_VERSION                   UINT8_C(0x04)

/**
 * \brief Available memory information length for the applet
 */
#define NBT_LEN_AVAILABLE_MEMORY_INFO            UINT8_C(0x0F)

/**
 * \brief Available memory length
 */
#define NBT_LEN_AVAILABLE_MEMORY                 UINT8_C(0x0C)

/**
 * \brief Length of memory data stored in the get data response.
 */
#define NBT_GET_DATA_TLV_LEN_VALUE               UINT8_C(0x02)

/**
 * \brief Available persistent or NVM memory
 */
#define NBT_GET_DATA_TAG_AVAILABLE_NVM_MEMORY    UINT8_C(0xC6)

/**
 * \brief Available transient of clear on reset (COR) type
 */
#define NBT_GET_DATA_TAG_AVAILABLE_TRANSIENT_COR UINT8_C(0xC7)

/**
 * \brief Available transient of clear on deselect (COD) type
 */
#define NBT_GET_DATA_TAG_AVAILABLE_TRANSIENT_COD UINT8_C(0xC8)

/**
 * \brief Structure for the applet version
 */
typedef struct
{
    /**
     * \brief Stores the major version.
     */
    uint8_t version_major;

    /**
     * \brief Stores the minor version.
     */
    uint8_t version_minor;

    /**
     * \brief Stores the build number.
     */
    uint16_t version_build_number;
} nbt_applet_version_t;

/**
 * \brief Structure for the available memory
 */
typedef struct
{
    /**
     * \brief Stores available persistent or NVM memory.
     */
    uint16_t available_memory_size;

    /**
     * \brief Stores available transient of clear on reset (COR) type.
     */
    uint16_t clear_on_reset;

    /**
     * \brief Stores available transient of clear on deselect (COD) type.
     */
    uint16_t clear_on_deselect;
} nbt_available_memory_t;

/**
 * \brief Gets the response data and returns the applet version.
 *
 * \param[in] response APDU response object in parameter contains the response
 * data.
 * \param[out] applet_version Applet version out parameter object to store the
 * applet version.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_parse_applet_version(const ifx_apdu_response_t *response,
                                      nbt_applet_version_t *applet_version);

/**
 * \brief Gets the response data and returns the available memory.
 *
 * \param[in] response APDU response object in parameter contains response data.
 * \param[out] available_memory Available memory out parameter object to store
 * the available memory.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_parse_available_memory(
    const ifx_apdu_response_t *response,
    nbt_available_memory_t *available_memory);

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* NBT_PARSE_RESPONSE_H */
