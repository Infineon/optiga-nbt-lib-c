// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/nbt-cmd.h
 * \brief Collection of the NBT operational commands.
 */
#ifndef NBT_CMD_H
#define NBT_CMD_H

#include "infineon/nbt-apdu-lib.h"
#include "infineon/nbt-apdu.h"
#include "infineon/nbt-parse-response.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Function identifiers */

/**
 * \brief Identifier for command authenticate tag
 */
#define NBT_AUTHENTICATE_TAG               UINT8_C(0x01)

/**
 * \brief Identifier for command get data applet version
 */
#define NBT_APPLET_VERSION                 UINT8_C(0x02)

/**
 * \brief Identifier for command get data available memory
 */
#define NBT_AVAILABLE_MEMORY               UINT8_C(0x03)

/**
 * \brief Identifier for command change password
 */
#define NBT_CHANGE_PASSWORD                UINT8_C(0x04)

/**
 * \brief Identifier for command create password
 */
#define NBT_CREATE_PASSWORD                UINT8_C(0x05)

/**
 * \brief Identifier for command delete password
 */
#define NBT_DELETE_PASSWORD                UINT8_C(0x06)

/**
 * \brief Identifier for command get data
 */
#define NBT_GET_DATA                       UINT8_C(0x07)

/**
 * \brief Identifier for command get FAP bytes
 */
#define NBT_GET_FAP_BYTES                  UINT8_C(0x08)

/**
 * \brief Identifier for command read binary
 */
#define NBT_READ_BINARY                    UINT8_C(0x09)

/**
 * \brief Identifier for command read FAP with password
 */
#define NBT_READ_FAP_WITH_PASSWORD         UINT8_C(0x0A)

/**
 * \brief Identifier for command parse FAP bytes
 */
#define NBT_PARSE_FAP_BYTES                UINT8_C(0x0B)

/**
 * \brief Identifier for command pass-through fetch data
 */
#define NBT_PASS_THROUGH_FETCH_DATA        UINT8_C(0x0C)

/**
 * \brief Identifier for command pass-through put response
 */
#define NBT_PASS_THROUGH_PUT_RESPONSE      UINT8_C(0x0D)

/**
 * \brief Identifier for command select AID
 */
#define NBT_SELECT_APPLICATION             UINT8_C(0x0E)

/**
 * \brief Identifier for command select file by ID
 */
#define NBT_SELECT_FILE_BY_ID              UINT8_C(0x0F)

/**
 * \brief Identifier for command unblock password
 */
#define NBT_UNBLOCK_PASSWORD               UINT8_C(0x10)

/**
 * \brief Identifier for command update binary
 */
#define NBT_UPDATE_BINARY                  UINT8_C(0x11)

/**
 * \brief Identifier for command update FAP bytes with password
 */
#define NBT_UPDATE_FAP_BYTES_WITH_PASSWORD UINT8_C(0x12)

/**
 * \brief Identifier for command recursive update binary
 */
#define NBT_UPDATE_RECURSIVE_BINARY        UINT8_C(0x13)

/**
 * \brief FileID of FAP file
 */
#define NBT_FAP_FILE_ID                    UINT16_C(0xE1AF)

/**
 * \brief FileID of NDEF file
 */
#define NBT_NDEF_FILE_ID                   UINT16_C(0xE104)

/**
 * \brief Total number of file available in NBT
 */
#define NBT_TOTAL_FILE                     UINT8_C(0x07)

/**
 * \brief Length of access condition, including FileID, NFC read/write and I2C
 * read/write access condition
 */
#define NBT_FAP_ACCESS_CONDITION_LENGTH    UINT8_C(0x06)

/**
 * \brief length of FileID
 */
#define NBT_FILE_ID_LENGTH                 UINT8_C(0x02)

/**
 * \brief Constant defines maximum possible LE value.
 */
#define NBT_MAX_LE                         UINT16_C(0x00FF)

/**
 * \brief Constant defines maximum possible LC value.
 */
#define NBT_MAX_LC                         UINT16_C(0x00FF)

/**
 * \brief Masking for access condition byte with password protected
 */
#define NBT_PASSWORD_ID_MASK               UINT8_C(0x1F)

/**
 * \brief Size of FAP file in bytes
 */
#define NBT_SIZE_OF_FAP_FILE               UINT8_C(0x2A)

/**
 * \brief  Constant defines initial offset value for recursive read binary
 * function.
 */
#define NBT_RECURSIVE_READ_INIT_OFFSET     UINT16_C(0x0000)

/**
 * \brief  Constant defines initial offset value for recursive update binary
 * function.
 */
#define NBT_RECURSIVE_UPDATE_INIT_OFFSET   UINT16_C(0x0000)

/**
 * \brief  Constant defines initial LE value for recursive read binary
 * function.
 */
#define NBT_RECURSIVE_READ_INIT_MSG_LEN    UINT8_C(0xFF)

/**
 * \brief Store the file access condition for both NFC and I2C (read/write
 * operation) interface with FileID.
 */
typedef struct
{
    /**
     * \brief 2 bytes of FileID
     */
    uint16_t file_id;

    /**
     * \brief I2C read access condition (always, never access condition, and
     * password protected with password ID)
     */
    uint8_t i2c_read_access_condition;

    /**
     * \brief I2C write access condition (always, never access condition, and
     * password protected with password ID)
     */
    uint8_t i2c_write_access_condition;

    /**
     * \brief NFC read access condition (always, never access condition, and
     * password protected with password ID)
     */
    uint8_t nfc_read_access_condition;

    /**
     * \brief NFC write access condition (always, never access condition, and
     * password protected with password ID)
     */
    uint8_t nfc_write_access_condition;
} nbt_file_access_policy_t;

/**
 * \brief Enumeration defines the access condition of file access policy.
 */
typedef enum
{
    /**
     * \brief No password verification is required, if the file is configured
     * with ALWAYS access condition.
     */
    NBT_ACCESS_ALWAYS = UINT8_C(0x40),

    /**
     * \brief Access is not allowed to the file, if file is configured with
     * NEVER access condition
     */
    NBT_ACCESS_NEVER = UINT8_C(0x00),

    /**
     * \brief Access is allowed only after password verification, if the file is
     * configured with PASSWORD_PROTECTED access condition. This config byte has
     * to be appended with password ID (5 bits).
     */
    NBT_ACCESS_PASSWORD_PROTECTED = UINT8_C(0x80)
} nbt_access_condition;

/**
 * \brief Enumeration provides the bit masks to verify the status of pass-though
 * status words.
 * \note Use the output pass_through_status_word parameter of
 * get_pt_fetch_data_response_pt_sw() API.
 */
typedef enum
{
    NBT_BIT_MASK_STATE_L4_EXIT = UINT16_C(0x0040), /**< State L4 exit since last
                  status bit field retrieval by host(1) / No change(0) */

    NBT_BIT_MASK_STATE_L4_ENTRY = UINT16_C(0x0020), /**< State L4 exit since
                  last status bit field retrieval by host(1) / No change(0) */

    NBT_BIT_MASK_STATE_FIELD_EXIT = UINT16_C(0x0010), /**< State L4 exit since
                  last status bit field retrieval by host(1) / No change(0) */

    NBT_BIT_MASK_STATE_FIELD_ENTRY =
        UINT16_C(0x0008), /**< State L4 exit since last status bit field
                  retrieval by host(1) / No change(0) */

    NBT_BIT_MASK_PASS_THROUGH_APDU_AVAILABLE =
        UINT16_C(0x0004), /**< NFC APDU Available(1) / Not available(0)*/

    NBT_BIT_MASK_LAYER_4_ACTIVE =
        UINT16_C(0x0002), /**< Layer 4 Active(1) / inactive(0) */

    NBT_BIT_MASK_NFC_FIELD_IS_PRESENT =
        UINT16_C(0x0001) /**< NFC field present(1) / off(0) */
} nbt_bit_mask_for_pass_through_status_word;

/**
 * \brief Selects the NBT application.
 *
 * \param[in,out] self Command set with communication protocol and response.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_select_application(nbt_cmd_t *self);

/**
 * \brief Selects the elementary file (EF) with the FileID.
 *
 * \param[in,out] self Command set with communication protocol and response.
 * \param[in]  file_id FileID to select an EF file.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_select_file(nbt_cmd_t *self, uint16_t file_id);

/**
 * \brief Selects the elementary file with FileID and read or write password.
 * Note that the status word of the command is not checked by this API.
 *
 * \param[in,out] self Command set with communication protocol and response.
 * \param[in] file_id FileID to select an EF file.
 * \param[in] read_password Blob_t 4-byte password for read operation (Optional)
 * \param[in] write_password Blob_t 4-byte password for write operation
 * (Optional)
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_select_file_with_password(nbt_cmd_t *self, uint16_t file_id,
                                           const ifx_blob_t *read_password,
                                           const ifx_blob_t *write_password);

/**
 * \brief Reads the binary data from the currently selected elementary file.
 *
 * \param[in,out] self Command set with communication protocol and response.
 * \param[in] offset Offset to read data from file
 * \param[in] binary_data_length Data length to be read from file.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_read_binary(nbt_cmd_t *self, uint16_t offset,
                             uint8_t binary_data_length);

/**
 * \brief Updates the binary data into the currently selected elementary file.
 *
 * \param[in,out] self Command set with communication protocol and response.
 * \param[in] offset Offset from where data to be updated in file.
 * \param[in] data_length Length of the data to be updated in the file.
 * \param[in] data Pointer to the data to be updated in the file.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_update_binary(nbt_cmd_t *self, uint16_t offset,
                               uint32_t data_length, const uint8_t *data);

/**
 * \brief Changes an existing password with a new password. If the FAP file
 * update operation is password protected, the master password is required to
 * change the password.
 *
 * \param[in,out] self Command set with communication protocol and response.
 * \param[in] pwd_id 5-bit password ID (Between 01 to 1F).
 * \param[in] master_password Master password for verification. Required if this
 * password is used with password-protected access condition (Optional).
 * \param[in] new_password New password to be changed.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_change_password(nbt_cmd_t *self, uint8_t pwd_id,
                                 const ifx_blob_t *master_password,
                                 const ifx_blob_t *new_password);

/**
 * \brief Unblocks the password, which is in blocked state because wrong
 * password limit exceeds.
 *
 * \param[in,out] self Command set with communication protocol and response.
 * \param[in] pwd_id 5-bit password ID (Between 01 to 1F).
 * \param[in] master_password Master password for verification. Required if this
 * password is used with password-protected access condition (Optional).
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_unblock_password(nbt_cmd_t *self, uint8_t pwd_id,
                                  const ifx_blob_t *master_password);

/**
 * \brief Issues an authenticate tag command, which generates the signature on
 * the challenge and can be used for brand protection use case in offline mode.
 *
 * \param[in,out] self Command set with communication protocol and response.
 * \param[in] challenge Instance holds challenge data.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_authenticate_tag(nbt_cmd_t *self, const ifx_blob_t *challenge);

/**
 * \brief Creates a new password. If create password command is
 * password-protected then the password to authenticate will be passed in the
 * command data as master password.
 *
 * \param[in,out] self Command set with communication protocol and response.
 * \param[in] master_password Master password for verification. Required if this
 * password is already used with password-protected access condition (Optional).
 * \param[in] new_password_id 5-bits password ID (Between 01 to 1F).
 * \param[in] new_password Password value to be updated.
 * \param[in] pwd_resp 2-bytes success response, which will be sent on
 * successful password verification.
 * \param[in] pwd_limit 2-bytes password retry limit (Between 0001 to 007F). if
 * limit set to FFFF, applet will treat this as an infinite try limit.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_create_password(nbt_cmd_t *self,
                                 const ifx_blob_t *master_password,
                                 uint8_t new_password_id,
                                 const ifx_blob_t *new_password,
                                 uint16_t pwd_resp, uint16_t pwd_limit);

/**
 * \brief Deletes an existing password, when the FAP file update operation
 * is allowed with ALWAYS access condition, then no need to authenticate with
 * the master password.
 *
 * \param[in,out] self Command set with communication protocol and response.
 * \param[in] master_password Master password for verification. Required if this
 * password is already used with password-protected access condition (Optional).
 * \param[in] password_id Offset value for p2 is 5-bit password ID (Between 01
 * to 1F).
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_delete_password(nbt_cmd_t *self,
                                 const ifx_blob_t *master_password,
                                 uint8_t password_id);

/**
 * \brief Issues a get data command to retrieve the NBT application specific
 * information like applet version and available memory. GET_DATA constants
 * that can be passed with an example: get data (TAG_AVAILABLE_MEMORY,
 * TAG_APPLET_VERSION)
 *
 * \param[in,out] self Command set with communication protocol and response.
 * \param[in] get_data_rcp get data reference control parameters.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_get_data(nbt_cmd_t *self, uint16_t get_data_rcp);

/**
 * \brief Issues a get data command to retrieve the applet version information.
 *
 * \param[in,out] self Command set with communication protocol and response.
 * \param[out] applet_version Buffer to store applet version.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_get_data_applet_version(nbt_cmd_t *self,
                                         nbt_applet_version_t *applet_version);

/**
 * \brief Issues a get data command to retrieve the available memory.
 *
 * \param[in,out] self Command set with communication protocol and response.
 * \param[out] available_memory Buffer to store available memory.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_get_data_available_memory(
    nbt_cmd_t *self, nbt_available_memory_t *available_memory);

/**
 * \brief Get the access condition policy byte for NFC read/write and I2C
 * read/write interface password ID as NULL, if nbt_access_condition is ALWAYS
 * and NEVER, otherwise pass valid password ID (0x01 - 0x1F) with
 * nbt_access_condition PASSWORD_PROTECTED
 * \param access_type           Access_condition type object (ALWAYS,
 * NEVER_ACCESS and PASSWORD_PROTECTED)
 * \param password_id           Pointer to store password ID
 * \return uint8_t              Returns a valid access byte, if password_id is
 * NULL and nbt_access_condition is ALWAYS, NEVER_ACCESS return valid access
 * byte, if password_id (0x01 - 0x0F) and nbt_access_condition is
 * PASSWORD_PROTECTED return invalid access byte (0x80), if password_id is NULL
 * & nbt_access_condition is PASSWORD_PROTECTED.
 */
uint8_t nbt_access_condition_get(nbt_access_condition access_type,
                                 const uint8_t *password_id);

/**
 * \brief Updates file access conditions in FAP file, if FAP file is update
 * always (not password protected).
 * \param[in,out] self Command set with communication protocol and response.
 * \param[in] fap_policy        Pointer to nbt_file_access_policy_t handler to
 * store FAP file access conditions for I2C and NGC interface.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_update_fap(nbt_cmd_t *self,
                            const nbt_file_access_policy_t *fap_policy);

/**
 * \brief Updates file access conditions in FAP file. when FAP file is password
 * protected(optional). If FAP file is not password protected, then no need to
 * authenticate with the master password, pass the master password as NULL
 * \param[in,out] self Command set with communication protocol and response.
 * \param[in] fap_policy        Pointer to nbt_file_access_policy_t handler to
 * store fap file access conditions for I2C and NFC interface
 * \param[in] master_password   Pointer to ifx_blob_t handler to store FAP file
 * master master password(4 bytes), if FAP file is password protected(optional)
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_update_fap_with_password(
    nbt_cmd_t *self, const nbt_file_access_policy_t *fap_policy,
    const ifx_blob_t *master_password);

/**
 * \brief Updates file access conditions in FAP file from policy bytes, if FAP
 * file is update always (not password protected).
 * \param[in,out] self Command set with communication protocol and response.
 * \param[in] policy_bytes      Pointer to ifx_blob_t handler to store access
 * condition for I2C and NFC read/write operation along with FileID.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_update_fap_bytes(nbt_cmd_t *self,
                                  const ifx_blob_t *policy_bytes);

/**
 * \brief Updates file access conditions in FAP file from policy bytes. When FAP
 * file is password protected (optional). If FAP file is not password protected,
 * then no need to authenticate with the master password, pass the master
 * password as NULL.
 * \param[in,out] self Command set with communication protocol and response.
 * \param[in] policy_bytes      Pointer to ifx_blob_t handler to store access
 * condition for I2C and NFC read/write operation along with FileID.
 * \param[in] master_password   Pointer to ifx_blob_t handler to store FAP file
 * write write password(4 bytes), if FAP file write operation is password
 * protected(optional).
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_update_fap_bytes_with_password(
    nbt_cmd_t *self, const ifx_blob_t *policy_bytes,
    const ifx_blob_t *master_password);

/**
 * \brief Parse response (APDU response type) bytes to array of
 * nbt_file_access_policy_t type object of FAP file. \note :Memory should be
 * allocated allocated for the 'fap_policies' array parameter externally before
 * passing to function. Example: nbt_file_access_policy_t policies[7];  // 7 =>
 * NBT_TOTAL_FILES NBT_TOTAL_FILES
 * \param[in] fap_bytes          Pointer to ifx_blob_t handler to store fap
 * bytes Format of FAP:<FileID (2B)> <Config byte for I2C read(1B)> <Config byte
 * for I2C write(1B)> <Config byte for NFC read(1B)> <Config byte for NFC
 * write(1B)>
 * \param[in] no_of_fap_policies Number of fap policies
 * (nbt_file_access_policy_t object type) that's to be parsed. Make sure memory
 * should be allocated for nbt_file_access_policy_t object (0x00 <
 * no_of_fap_policies <= NBT_TOTAL_FILE(7))
 * \param[out] fap_policies      Pointer to nbt_file_access_policy_t handler to
 * store store fap file access conditions for I2C and NFC interface along with
 * FileID.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If successful
 * \retval IFX_ILLEGAL_ARGUMENT : If function parameter is NULL or
 * no_of_fap_policies is zero
 * \retval NBT_FAP_PARSE_ERROR : If expected(NBT_FAP_ACCESS_CONDITION_LENGTH(6
 * bytes))fap bytes is not available
 */
ifx_status_t nbt_parse_fap_bytes(const ifx_blob_t *fap_bytes,
                                 uint8_t no_of_fap_policies,
                                 nbt_file_access_policy_t *fap_policies);

/**
 * \brief Reads binary data (fap bytes of elementary files) from FAP file and
 * store the fap bytes into nbt_file_access_policy_t type object array. \note
 * :Memory :Memory should be allocated for the 'fap_policies' array parameter
 * externally passing to this function. Example: nbt_file_access_policy_t
 * policies[7];  // 7 =>
 * // 7 => NBT_TOTAL_FILES.
 * \param[in,out] self Command set with communication protocol and response.
 * \param[out] fap_policies     Pointer to nbt_file_access_policy_t handler to
 * store fap file access conditions for I2C and NFC interface
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_read_fap(nbt_cmd_t *self,
                          nbt_file_access_policy_t *fap_policies);

/**
 * \brief Reads binary data (access condition policy bytes of elementary files)
 * from FAP file and store the fap bytes into nbt_file_access_policy_t type
 * object array. If FAP file is not password protected, then no need to
 * authenticate with the master password, pass the master password as NULL.
 * \note :Memory should be allocated for the 'fap_policies' array parameter
 * externally before passing to this function. Example: nbt_file_access_policy_t
 * policies[7];  // 7 => NBT_TOTAL_FILES.
 * \param[in,out] self Command set with communication protocol and response.
 * \param[in] master_password   Pointer to ifx_blob_t handler to store FAP file
 * write write password(4 bytes), if FAP file write operation is password
 * protected(optional)
 * \param[out] fap_policies     Pointer to nbt_file_access_policy_t handler to
 * store fap file access conditions for I2C and NFC interface
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_read_fap_with_password(nbt_cmd_t *self,
                                        const ifx_blob_t *master_password,
                                        nbt_file_access_policy_t *fap_policies);

/**
 * \brief Reads binary data (fap bytes of elementary files) from FAP file.
 * \param[in,out] self Command set with communication protocol and response.
 *
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_read_fap_bytes(nbt_cmd_t *self);

/**
 * \brief Reads binary data (fap bytes of elementary files) from FAP file.
 * If FAP file is not password protected, then no need to authenticate with the
 * master password, pass the master password as NULL.
 * \param[in,out] self Command set with communication protocol and response.
 * \param[in] master_password   Pointer to ifx_blob_t handler to store FAP file
 * write write password(4 bytes), if FAP file write operation is password
 * protected(optional).
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_read_fap_bytes_with_password(
    nbt_cmd_t *self, const ifx_blob_t *master_password);

/**
 * \brief Issues a pass-through fetch data APDU command.
 *  This command can be issued from host devices through I2C interfaces.
 *
 * \details This command can be used in NFC-I2C pass-through communication mode
 *  to fetch the NFC interface-specific status information concatenated with the
 * NFC received APDU over the I2C interface. When the NFC-I2C pass-through
 * communication mode is not active or the NFC APDU is received only the NFC
 * interface-specific status information is returned. This command is limited to
 * the I2C communication interface.
 *
 * \param[in,out] self Command set with communication protocol.
 * \param[out] response     Buffer to store response data
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_pass_through_fetch_data(nbt_cmd_t *self,
                                         ifx_apdu_response_t *response);

/**
 * \brief Gets the pass-through status word from the response of
 * pass_through_fetch_data() command.
 * \note This API can be called only after pass_through_fetch_data() is called.
 * This is a helper function to decode the response of pass_through_fetch_data()
 * command
 *
 * \param[in] response                      APDU response of
 * pass_through_fetch_data() command
 * \param[out] pass_through_status_word     pass-through status word - this
 * status word can be further decoded using
 *                                          nbt_bit_mask_for_pass_through_status_word
 * enumeration
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If pass-through status word is retrieved successfully
 * \retval INVALID_ARGUMENT : If input argument is invalid that is NULL input
 * parameter
 */
ifx_status_t nbt_pass_through_decode_sw(const ifx_apdu_response_t *response,
                                        uint16_t *pass_through_status_word);

/**
 * \brief Gets the pass-through NFC APDU in byte array from the response of
 * pass_through_fetch_data() command.
 * \note This API can be called only after pass_through_fetch_data() is called.
 * This is a helper function to decode the response of pass_through_fetch_data()
 * command
 *
 * \param[in] response                      APDU response of
 * pass_through_fetch_data() command
 * \param[out] nfc_apdu                     NFC APDU data in byte array
 *
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_pass_through_decode_apdu_bytes(
    const ifx_apdu_response_t *response, ifx_blob_t *nfc_apdu);

/**
 * \brief Gets the pass-through NFC APDU in APDU format from the response of
 * pass_through_fetch_data() command.
 * \note This API can be called only after pass_through_fetch_data() is called.
 * This is a helper function to decode the response of pass_through_fetch_data()
 * command
 *
 * \param[in] response                      APDU response of
 * pass_through_fetch_data() command
 * \param[out] nfc_apdu                     NFC APDU data in APDU format
 *
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If pass-through NFC APDU data is retrieved successfully
 * \retval INVALID_ARGUMENT : If input argument is invalid that is NULL input
 * parameter
 * \retval NFC_APDU_IS_NOT_AVAILABLE : If response does not contain NFC APDU
 * data IFX_OUT_OF_MEMORY if memory allocation fails
 */
ifx_status_t nbt_pass_through_decode_apdu(const ifx_apdu_response_t *response,
                                          ifx_apdu_t *nfc_apdu);

/**
 * \brief Sends the response to pass-through fetch data command, forwarding the
 * response over the NFC interface.
 *
 * \note This command is limited to the I2C communication interface. The command
 * can only be executed after entering PT-Mode and the execution of the PT Fetch
 * command. Otherwise an Exception is returned
 *
 * \details The Pass-through put response APDU has a proprietary format. The
 * data section of pass_through_response_data must include the response to be
 * forwarded over the NFC communication, including the status word SW1 and SW2.
 * The Data field must contain at least the status word (2 bytes) meant to be
 * sent over the NFC interface.
 *
 * \param[in] self            Command set with communication protocol.
 * \param[in] pass_through_response_data    Response to PT fetch
 * command,including SW
 * \param[out] response                     Buffer to store response data
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_pass_through_put_response(
    nbt_cmd_t *self, const ifx_apdu_response_t *pass_through_response_data,
    ifx_apdu_response_t *response);

/**
 * \brief Reads NDEF file with password and return the NDEF message byte data.
 * Method performs the select file with password if available and then
 * read binary until data is available in file.
 *
 * Note: Application must be selected already with selectApplication() API
 * before use this API.
 *
 * \param[in,out] self Command set with communication protocol and response.
 * \param[in] file_id FileID to select an NDEF file.
 * \param[in] read_password  4-byte password for read operation (Optional-
 * Null if not required).
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_ndef_read_with_id_password(nbt_cmd_t *self, uint16_t file_id,
                                            const ifx_blob_t *read_password);

/**
 * \brief Reads NDEF file and return the NDEF message byte data.
 * Method performs the select file and then read binary until data is available
 * in file.
 *
 * Note: Application must be selected already with selectApplication() API
 * before use this API.
 *
 * \param[in,out] self Command set with communication protocol and response.
 * \param[in] file_id FileID to select an EF file.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_ndef_read_with_id(nbt_cmd_t *self, uint16_t file_id);
/**
 * \brief Reads NDEF file with password and returns the NDEF message byte data.
 * Method performs the select file with default NDEF FileID (E104) and with
 * optional password and then read binary until data is available in file.
 *
 * Note: Application must be selected already with selectApplication() API
 * before use this API.
 *
 * \param[in,out] self Command set with communication protocol and response.
 * \param[in] read_password  4-byte password for read operation (Optional-
 *  Null if not required).
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_ndef_read_with_password(nbt_cmd_t *self,
                                         const ifx_blob_t *read_password);

/**
 * \brief Reads NDEF file with password and returns the NDEF message byte data.
 * Method performs the select file with default NDEF FileID (E104) and then read
 * binary until data is available in file.
 *
 * Note: Application must be selected already with selectApplication() API
 * before use this API
 *
 * Note: application must be selected already with selectApplication() API
 * before use this API
 *
 * \param[in,out] self Command set with communication protocol and response.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_ndef_read(nbt_cmd_t *self);

/**
 * \brief  Updates NDEF file with FileID, 4-byte password (Optional). Method
 * performs the select file with file_id and then update binary until data is
 * available.
 *
 * Note: Application must be selected already with selectApplication() API
 * before use this API
 *
 * \param[in,out] self  Command set with communication protocol and response.
 * \param[in] file_id FileID to select an EF file.
 * \param[in] write_password 4-byte password for write operation (Optional-
 *  Null if not required)
 * \param[in] ndef_bytes  Data to be write
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_ndef_update_with_id_password(nbt_cmd_t *self, uint16_t file_id,
                                              const ifx_blob_t *write_password,
                                              ifx_blob_t *ndef_bytes);

/**
 * \brief  Updates the NDEF file with FileID. Method performs the select file
 * with file_id and then update binary until data is available.
 *
 * Note: application must be selected already with selectApplication() API
 * before use this API
 *
 * \param[in,out] self  Command set with communication protocol and response.
 * \param[in] file_id FileID to select an EF file.
 * \param[in] ndef_bytes  NDEF message to be updated
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_ndef_update_with_id(nbt_cmd_t *self, uint16_t file_id,
                                     ifx_blob_t *ndef_bytes);

/**
 * \brief Updates NDEF file with 4-byte password (Optional) and default NDEF
 * file Id (E104). Method performs the select file with file_id and then update
 * binary until data is available.
 *
 * Note: Application must be selected already with selectApplication() API
 * before use this API.
 *
 * \param[in,out] self  Command set with communication protocol and response.
 * \param[in] write_password 4-byte password for write operation (Optional-
 * Null if not required)
 * \param[in] ndef_bytes  NDEF message to be updated
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_ndef_update_with_password(nbt_cmd_t *self,
                                           const ifx_blob_t *write_password,
                                           ifx_blob_t *ndef_bytes);

/**
 * \brief Updates NDEF file default ndef FileID (E104), Method performs the
 * select file with file_id and then update binary until data is available.
 *
 * \note: Application must be selected already with selectApplication() API
 * before use this API
 *
 * \param[in,out] self  Command set with communication protocol and response.
 * \param[in] ndef_bytes  NDEF message to be updated
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_ndef_update(nbt_cmd_t *self, ifx_blob_t *ndef_bytes);

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* NBT_CMD_H */
