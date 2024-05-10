// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/nbt-errors.h
 * \brief Provides generic error message used in the NBT library.
 */
#ifndef NBT_ERRORS_H
#define NBT_ERRORS_H

#include <stdint.h>

#include "infineon/ifx-apdu-error.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Incorrect LC value
 */
#define NBT_SW_INCORRECT_LC_LE               UINT16_C(0x6700)

/**
 * \brief Application or file not found.
 */
#define NBT_SW_APPLICATION_OR_FILE_NOT_FOUND UINT16_C(0x6A82)

/**
 * \brief Wrong TLV data format for select file with password data.
 */
#define NBT_SW_SELECT_FILE_WRONG_TLV         UINT16_C(0x6A80)

/**
 * \brief Incorrect parameters in the command data field.
 */
#define NBT_SW_INCORRECT_DATA_PARAMETERS     UINT16_C(0x6A80)

/**
 * \brief Unknown or unsupported data group, data object.
 */
#define NBT_SW_UNSUPPORTED_DATA              UINT16_C(0x6A88)

/**
 * \brief Use of condition not satisfied. If during personalization of password,
 * password ID already exists. If trying to personalize more than 28 passwords.
 * If personalize data command is sent during operational phase. If personalize
 * data command is for Asymmetric ECC key and sales code is NBT 1000.
 */
#define NBT_SW_SECURITY_ACCESS_DENIED        UINT16_C(0x6985)

/**
 * \brief Incorrect P1-P2.
 */
#define NBT_SW_WRONG_P1_P2                   UINT16_C(0x6A86)

/**
 * \brief Security condition not satisfied.
 */
#define NBT_SW_SECURITY_NOT_SATISFIED        UINT16_C(0x6982)

/**
 * \brief CC file is updated for ‘02’ to ‘0E’ offset is not allowed.
 */
#define NBT_SW_UPDATE_ACCESS_DENIED          UINT16_C(0x6985)

/**
 * \brief Command not allowed.
 */
#define NBT_SW_COMMAND_NOT_ALLOWED           UINT16_C(0x6986)

/**
 * \brief Conditions of use not satisfied. Password ID already present, if
 * trying to personalize more than 28 passwords.
 */
#define NBT_SW_CONDITIONS_NOT_SATISFIED      UINT16_C(0x6985)

/**
 * \brief Wrong data, invalid FileID or format mismatch in FAP policy.
 */
#define NBT_SW_INCORRECT_DATA                UINT16_C(0x6A80)

/**
 * \brief Incorrect parameters in the command data field
 */
#define NBT_SW_CONFIG_SELECT_INCORRECT_DATA  UINT16_C(0x6A80)

/**
 * \brief Incorrect Lc value for configurator select
 */
#define NBT_SW_CONFIG_SELECT_INCORRECT_LC    UINT16_C(0x6700)

/**
 * \brief Wrong length, if Lc is present or if Le not equal to ‘00’.
 */
#define NBT_SW_GET_DATA_WRONG_LE             UINT16_C(0x6700)

/**
 * \brief Reference data not found.
 */
#define NBT_SW_DATA_NOT_FOUND                UINT16_C(0x6A88)

/**
 * \brief APDU error message list length (30 bytes)
 */
#define NBT_MESSAGE_ERROR_COUNTS             UINT8_C(0x1E)

/* Error identifiers */

/**
 * \brief Lc must not be zero.
 */
#define NBT_LC_INVALID                       UINT8_C(0x01)

/**
 * \brief NFC APDU is not available in pass-through fetch data command response.
 */
#define NBT_NFC_APDU_NOT_AVAILABLE           UINT8_C(0x02)

/**
 * \brief FAP bytes are improper or not have FAP bytes of length (6 bytes).
 */
#define NBT_FAP_PARSE_ERROR                  UINT8_C(0x03)

/**
 * \brief APDU error message list
 */
extern ifx_apdu_error_map_t nbt_apdu_errors[];

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* NBT_ERRORS_H */
