// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file nbt-errors.c
 * \brief Provides generic error message used in the NBT library.
 */
#include "infineon/nbt-errors.h"

#include <stdint.h>

#include "infineon/nbt-apdu.h"
#include "nbt-build-apdu.h"

/**
 * \brief APDU error message list
 */
ifx_apdu_error_map_t nbt_apdu_errors[NBT_MESSAGE_ERROR_COUNTS] = {
    {NBT_INS_SELECT, NBT_SW_INCORRECT_LC_LE,
     (uint8_t *) "Incorrect Lc/Le value"},
    {NBT_INS_SELECT, NBT_SW_APPLICATION_OR_FILE_NOT_FOUND,
     (uint8_t *) "File or application not found"},
    {NBT_INS_SELECT, NBT_SW_SELECT_FILE_WRONG_TLV,
     (uint8_t *) "Wrong TLV data format for select file with password data."},
    {NBT_INS_UPDATE_BINARY, NBT_SW_COMMAND_NOT_ALLOWED,
     (uint8_t *) "Command not Allowed (EF is not selected)"},
    {NBT_INS_UPDATE_BINARY, NBT_SW_INCORRECT_LC_LE,
     (uint8_t *) "Offset is greater than the file size or If Le is absent or "
                 "If Le is "
                 "coded on extended length"},
    {NBT_INS_UPDATE_BINARY, NBT_SW_SECURITY_NOT_SATISFIED,
     (uint8_t
          *) "Security condition not satisfied, authentication with respective "
             "password is not successful and file is locked "},
    {NBT_INS_UPDATE_BINARY, NBT_SW_UPDATE_ACCESS_DENIED,
     (uint8_t *) "CC file is updated for ‘02’ to ‘0E’ offset is not allowed."},
    {NBT_INS_UPDATE_BINARY, NBT_SW_INCORRECT_DATA,
     (uint8_t *) "Wrong data, Invalid FileID or format mismatch in FAP policy"},
    {NBT_INS_UPDATE_BINARY, NBT_SW_WRONG_P1_P2,
     (uint8_t *) "Incorrect P1-P2: If P1-P2 is greater than file size. Not "
                 "applicable for "
                 "FAP update."},
    {NBT_INS_READ_BINARY, NBT_SW_COMMAND_NOT_ALLOWED,
     (uint8_t *) "Command not Allowed (EF is not selected)"},
    {NBT_INS_READ_BINARY, NBT_SW_INCORRECT_LC_LE,
     (uint8_t *) "Offset is greater than the file size or If Le is absent or "
                 "If Le is "
                 "coded on extended length"},
    {NBT_INS_READ_BINARY, NBT_SW_SECURITY_NOT_SATISFIED,
     (uint8_t *) "Security status not satisfied, If master password is applied "
                 "for read "
                 "file and password is not verified."},
    {NBT_INS_CREATE_PASSWORD, NBT_SW_WRONG_P1_P2,
     (uint8_t *) "Offset is greater than the file size or If Le is absent or "
                 "If Le is "
                 "coded on extended length"},
    {NBT_INS_CREATE_PASSWORD, NBT_SW_SECURITY_NOT_SATISFIED,
     (uint8_t *) "Security status not satisfied"},
    {NBT_INS_CREATE_PASSWORD, NBT_SW_INCORRECT_LC_LE,
     (uint8_t *) "Wrong command length, If Lc is other than 09 & 0D bytes or "
                 "If Le is "
                 "present"},
    {NBT_INS_CREATE_PASSWORD, NBT_SW_INCORRECT_DATA_PARAMETERS,
     (uint8_t *) "Incorrect Data : Password response is 0000 and FFFF. or "
                 "password Id is "
                 "00 or Password limit value is 0000 or 0080 to FFFE."},
    {NBT_INS_CREATE_PASSWORD, NBT_SW_SECURITY_ACCESS_DENIED,
     (uint8_t *) "Conditions of use not satisfied Password ID already present "
                 "or If trying "
                 "to personalize more than 28 passwords"},
    {NBT_INS_DELETE_PASSWORD, NBT_SW_SECURITY_NOT_SATISFIED,
     (uint8_t *) "Security status not satisfied"},
    {NBT_INS_DELETE_PASSWORD, NBT_SW_UNSUPPORTED_DATA,
     (uint8_t *) "Password ID not present"},
    {NBT_INS_AUTHENTICATE_TAG, NBT_SW_CONDITIONS_NOT_SATISFIED,
     (uint8_t *) "If during personalization of password, password ID already "
                 "exists, If "
                 "trying to personalize more than 28 passwords ,If Personalize "
                 "Data "
                 "command is sent during operational phase, If Personalize "
                 "Data command is "
                 "for Asymmetric ECC key and sales code is NBT 1000"},
    {NBT_INS_AUTHENTICATE_TAG, NBT_SW_INCORRECT_LC_LE,
     (uint8_t *) "Wrong length: If Challenge length is 00 or If Le is 00 or If "
                 "Lc is coded "
                 "on Extended length"},
    {NBT_INS_AUTHENTICATE_TAG, NBT_SW_WRONG_P1_P2,
     (uint8_t *) "Incorrect parameters P1-P2, If P1-P2 is other than 00."},
    {NBT_INS_CHANGE_UNBLOCK_PASSWORD, NBT_SW_DATA_NOT_FOUND,
     (uint8_t *) "Reference of Data not found. Password ID not valid."},
    {NBT_INS_CHANGE_UNBLOCK_PASSWORD, NBT_SW_INCORRECT_LC_LE,
     (uint8_t *) "Wrong length, if Lc is other than 04 and 08 in case of "
                 "change password."},
    {NBT_INS_CHANGE_UNBLOCK_PASSWORD, NBT_SW_CONDITIONS_NOT_SATISFIED,
     (uint8_t
          *) "Conditions of use not satisfied. If Master password is applied "
             "on FAP "
             "update and master password is not present. If Master password is "
             "blocked. If requested password is blocked in case of Change "
             "Password "
             "command."},
    {NBT_INS_CHANGE_UNBLOCK_PASSWORD, NBT_SW_WRONG_P1_P2,
     (uint8_t *) "Incorrect P1-P2. If P1 is other than 00. If P2 has b8-b7 is "
                 "other than "
                 "00 and 40"},
    {NBT_INS_GET_DATA, NBT_SW_WRONG_P1_P2,
     (uint8_t *) "Wrong P1P2. If P1-P2 is other than DF3A and DF3B"},
    {NBT_INS_GET_DATA, NBT_SW_GET_DATA_WRONG_LE,
     (uint8_t *) "If FAP update content is other than 42 bytes ,If "
                 "Personalized password "
                 "length is other than 9 bytes ,AES Key length is other than16 "
                 "bytes ,ECC "
                 "key length is other than 32 bytes ,NDEF/Proprietary EF "
                 "personalization "
                 "is out of file size ,If Le is present ,If Additional data is "
                 "present in "
                 "command ,If Password Response is 0000 & FFFF"},
    {NBT_INS_SELECT, NBT_SW_CONFIG_SELECT_INCORRECT_LC,
     (uint8_t *) "Wrong length, Incorrect Lc value"},
    {NBT_INS_SELECT, NBT_SW_CONFIG_SELECT_INCORRECT_DATA,
     (uint8_t *) "Incorrect parameters in the command data field"},
};