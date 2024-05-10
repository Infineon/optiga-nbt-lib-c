// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file include/nbt-build-apdu.h
 * \brief Provides the function declaration for building command for NBT
 * operational command set.
 */
#ifndef NBT_BUILD_APDU_H
#define NBT_BUILD_APDU_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "infineon/ifx-apdu.h"
#include "infineon/ifx-utils.h"
#include "infineon/nbt-apdu.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Function identifiers */

/**
 * \brief Identifier for build command authenticate tag
 */
#define NBT_BUILD_AUTHENTICATE_TAG                UINT8_C(0x01)

/**
 * \brief Identifier for build command change password
 */
#define NBT_BUILD_CHANGE_PASSWORD                 UINT8_C(0x02)

/**
 * \brief Identifier for build command create password
 */
#define NBT_BUILD_CREATE_PASSWORD                 UINT8_C(0x03)

/**
 * \brief Identifier for build command delete password
 */
#define NBT_BUILD_DELETE_PASSWORD                 UINT8_C(0x04)

/**
 * \brief Identifier for command builder get data
 */
#define NBT_BUILD_GET_DATA                        UINT8_C(0x05)

/**
 * \brief Identifier for build command pass-through fetch data
 */
#define NBT_BUILD_PASS_THROUGH_FETCH_DATA         UINT8_C(0x06)

/**
 * \brief Identifier for build command pass-through put data
 */
#define NBT_BUILD_PASS_THROUGH_PUT_RESPONSE       UINT8_C(0x07)

/**
 * \brief Identifier for build command read binary
 */
#define NBT_BUILD_READ_BINARY                     UINT8_C(0x08)

/**
 * \brief Identifier for build command select application
 */
#define NBT_BUILD_SELECT_APPLICATION              UINT8_C(0x09)

/**
 * \brief Identifier for build command select file by ID
 */
#define NBT_BUILD_SELECT_FILE                     UINT8_C(0x0A)

/**
 * \brief Identifier for build command update binary
 */
#define NBT_BUILD_UPDATE_BINARY                   UINT8_C(0x0B)

/**
 *  \brief Identifier for command builder unblock password
 */
#define NBT_BUILD_UNBLOCK_PASSWORD                UINT8_C(0x0C)

/**
 * \brief Create password APDU command data field header size.
 */
#define NBT_CREATE_PASSWORD_APDU_DATA_HEADER_SIZE UINT8_C(0x05)

/**
 * \brief The command expected length data for NBT command: If data is absent.
 */
#define NBT_APDU_DATA_NULL                        NULL

/**
 * \brief Helper macro for memory copy: Minimum incremental value.
 */
#define NBT_OFFSET_MEMORY_INCREMENT               UINT8_C(0x01)

/**
 * \brief Password header length.
 */
#define NBT_LEN_PASSWORD_HEADER                   UINT8_C(0x02)

/**
 * \brief INS type for NBT command: Read binary.
 */
#define NBT_INS_READ_BINARY                       UINT8_C(0xB0)

/**
 * \brief INS type for NBT command: Select file.
 */
#define NBT_INS_SELECT                            UINT8_C(0xA4)

/**
 * \brief INS type for NBT command: Update binary.
 */
#define NBT_INS_UPDATE_BINARY                     UINT8_C(0xD6)

/**
 * \brief INS type for NBT command: Unblock password.
 */
#define NBT_INS_CHANGE_UNBLOCK_PASSWORD           UINT8_C(0x24)

/**
 * \brief INS type for NBT command: Create password.
 */
#define NBT_INS_CREATE_PASSWORD                   UINT8_C(0xE1)

/**
 * \brief INS type for NBT command: Delete password.
 */
#define NBT_INS_DELETE_PASSWORD                   UINT8_C(0xE4)

/**
 * \brief INS type for NBT command: Get data.
 */
#define NBT_INS_GET_DATA                          UINT8_C(0x30)

/**
 * \brief INS type for NBT command: Pass-through fetch data.
 */
#define NBT_INS_PASS_THROUGH_FETCH_DATA           UINT8_C(0xCA)

/**
 * \brief INS type for NBT command: Pass-through put response.
 */
#define NBT_INS_PASS_THROUGH_PUT_RESPONSE         UINT8_C(0xDA)

/**
 * \brief The command expected length Le for NBT command: if expected length
 * is absent.
 */
#define NBT_LE_ABSENT                             UINT8_C(0x00)

/**
 * \brief The reference control parameter P2 for NBT command: Select only first
 * occurrence.
 */
#define NBT_P2_SELECT_FIRST_ONLY                  UINT8_C(0x0C)

/**
 * \brief The command expected length Le for NBT command: if command length
 * is absent.
 */
#define NBT_LC_ABSENT                             UINT8_C(0x00)

/**
 * \brief The reference control parameter P2 for NBT command: P2 (bit 7 and
 * 8) indicates whether to change the password.
 */
#define NBT_PASSWORD_CHANGE_MASK                  UINT8_C(0x40)

/**
 * \brief TAG defines password as read password.
 */
#define NBT_TAG_PASSWORD_READ                     UINT8_C(0x52)

/**
 * \brief TAG defines password as write password.
 */
#define NBT_TAG_PASSWORD_WRITE                    UINT8_C(0x54)

/**
 * \brief INS type for NBT command: Authenticate tag.
 */
#define NBT_INS_AUTHENTICATE_TAG                  UINT8_C(0x88)

/**
 * \brief Maximum number of bits to be stored in the P2 field for unblock
 * password command.
 */
#define NBT_PASSWORD_UNBLOCK_MASK                 UINT8_C(0x1F)

/**
 * \brief  Minimum length for pass-through response data in
 * pass_through_put_response command.
 */
#define NBT_MIN_PT_RESPONSE_LEN                   UINT8_C(0x02)

/**
 * \brief RFU byte of pass-through put response APDU
 */
#define NBT_PT_PUT_RESP_RFU                       UINT8_C(0x00)

/**
 * \brief Get data reference control parameters
 * \details User can directly use the enumerations to pass the reference control
 * parameters for get data command.
 */
typedef enum
{
    /**
     * \brief Offset for applet version
     */
    NBT_GET_DATA_APPLET_VERSION = UINT16_C(0xDF3A),

    /**
     * \brief Offset for available memory
     */
    NBT_GET_DATA_AVAILABLE_MEMORY = UINT16_C(0xDF3B)
} nbt_get_data_rcp;

/**
 * \brief Builds the select AID command. This command is used to select
 * the application instance. Note that the status word of the command is not
 * checked by this API.
 *
 * \param[out] apdu APDU reference to store the select file APDU command.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t build_select_application(ifx_apdu_t *apdu);

/**
 * \brief Builds the select file APDU command. This command is used to select
 * the personalized elementary file (EF). Note that the status word of the
 * command is not checked by this API.
 *
 * \param[in] file_id FileID to select an EF.
 * \param[out] apdu APDU reference to store the select file APDU command.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t build_select_file(uint16_t file_id, ifx_apdu_t *apdu);

/**
 * \brief Builds the select file APDU command to select the elementary file with
 * FileID and read or write password. This command is used to select the
 * personalized EF.
 * Note that the status word of the command is not checked by this API.
 *
 * \param[in] file_id FileID to select an EF.
 * \param[in] read_password Blob_t 4-byte password for read operation (Optional)
 * \param[in] write_password Blob_t 4-byte password for write operation
 * (Optional)
 * \param[out] apdu APDU reference to store the select file APDU command.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t build_select_file_with_password(uint16_t file_id,
                                             const ifx_blob_t *read_password,
                                             const ifx_blob_t *write_password,
                                             ifx_apdu_t *apdu);

/**
 * \brief Builds the read binary command.
 *
 * \param[in] offset Offset to read binary.
 * \param[in] read_data_length Data length to read.
 * \param[out] apdu APDU reference to store the read binary APDU command.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t build_read_binary(uint16_t offset, uint8_t read_data_length,
                               ifx_apdu_t *apdu);

/**
 * \brief Builds the update binary command.
 *
 * \details This command is used to update the personalized EF.
 *
 * \param[in] offset Offset from where data to be updated in file.
 * \param[in] data_length Length of the data to be updated in the file.
 * \param[in] data Pointer to the data to be updated in the file.
 * \param[out] apdu APDU reference to store the update binary APDU command.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t build_update_binary(uint16_t offset, uint8_t data_length,
                                 const uint8_t *data, ifx_apdu_t *apdu);

/**
 * \brief Builds the change password command.
 *
 * \param[in] new_password_id 5-bit password ID (Between 01 to 1F).
 * \param[in] master_password Master password for verification. Required if this
 * password is used with password-protected access condition (Optional).
 * \param[in] new_password New password to be changed.
 * \param[out] apdu APDU reference to store the change password APDU command.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t build_change_password(uint8_t new_password_id,
                                   const ifx_blob_t *master_password,
                                   const ifx_blob_t *new_password,
                                   ifx_apdu_t *apdu);

/**
 * \brief Builds the command to unblock a password.
 *
 * \param[in] pwd_id 5-bit password ID (Between 01 to 1F).
 * \param[in] master_password Master password for verification. Required if this
 * password is used with password-protected access condition (Optional).
 * \param[out] apdu APDU reference to store the unblock password APDU command.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t build_unblock_password(uint8_t pwd_id,
                                    const ifx_blob_t *master_password,
                                    ifx_apdu_t *apdu);

/**
 * \brief Builds the authenticate tag command.
 * \param[in] challenge Instance holds the challenge data and size.
 * \param[out] apdu APDU reference to store the authenticate tag APDU command.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t build_authenticate_tag(const ifx_blob_t *challenge,
                                    ifx_apdu_t *apdu);

/**
 * \brief Builds the create password command.
 *
 * \param[in] master_password Master password for verification. Required if this
 * password is already used with password-protected access condition. Pass NULL,
 * if master_password is not needed (Optional).
 * \param[in] new_password_id 5-bit password ID (Between 01 to 1F).
 * \param[in] new_password Password value to be updated.
 * \param[in] pwd_resp 2-byte success response, which will be sent on successful
 * password verification.
 * \param[in] pwd_limit 2-byte password retry limit (Between 0001 to 007F). If
 * limit set to 0xFFFF, applet will treat this as an infinite try limit.
 * \param[out] apdu APDU reference to store create password APDU command.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t build_create_password(const ifx_blob_t *master_password,
                                   uint8_t new_password_id,
                                   const ifx_blob_t *new_password,
                                   uint16_t pwd_resp, uint16_t pwd_limit,
                                   ifx_apdu_t *apdu);

/**
 * \brief Builds the delete password command.
 *
 * \param[in] master_password Master password for verification. Required if this
 * password is already used with password-protected access condition (Optional).
 * \param[in] password_id Offset value for p2.
 * \param[out] apdu APDU reference to store the delete password APDU command.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t build_delete_password(const ifx_blob_t *master_password,
                                   uint8_t password_id, ifx_apdu_t *apdu);

/**
 * \brief Builds the get data command.
 *
 * \param[in] get_data_rcp Get data reference control parameters.
 * \param[out] apdu APDU reference to store the get data APDU command.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t build_get_data(uint16_t get_data_rcp, ifx_apdu_t *apdu);

/**
 * \brief Builds the pass-through fetch data command.
 *
 * \param[out] apdu APDU reference to store pass-through fetch data command.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 */
ifx_status_t build_pass_through_fetch_data(ifx_apdu_t *apdu);

/**
 * \brief Builds the pass-through put response command.
 *
 * \param[in] pass_through_response     Pass-through response data that has to
 * be provided for pass-through fetch data command.
 * \param[out] apdu_bytes APDU byte array that stores proprietary
 * APDU format of PT put response command.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t build_pass_through_put_response(
    const ifx_apdu_response_t *pass_through_response, ifx_blob_t *apdu_bytes);

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* NBT_BUILD_APDU_H */
