// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file nbt-cmd.c
 * \brief Collection of the NBT operational commands.
 */
#include "infineon/nbt-cmd.h"

#include "infineon/ifx-logger.h"
#include "infineon/nbt-apdu-lib.h"
#include "infineon/nbt-errors.h"
#include "nbt-build-apdu.h"

/**
 * \brief Offset for the pass-through status word in pass-through fetch data
 * response.
 */
#define NBT_OFFSET_OF_PT_SW_IN_FETCH_DATA_RESP    UINT8_C(0)

/**
 * \brief Offset for the pass-through NFC APDU in pass-through fetch data
 * response.
 */
#define NBT_OFFSET_OF_NFC_APDU_IN_FETCH_DATA_RESP UINT8_C(2)

/**
 * \brief Checks if the NFC data available bit is set in pass-through status
 * word.
 */
#define NBT_IS_NFC_DATA_AVAILABLE_IN_PT_SW(status_word)                        \
    (((status_word) & NBT_BIT_MASK_PASS_THROUGH_APDU_AVAILABLE) ==             \
     NBT_BIT_MASK_PASS_THROUGH_APDU_AVAILABLE)

/**
 * \brief Offset of INS byte in put response command byte array.
 */
#define NBT_INS_BYTE_OFFSET UINT8_C(1)

/**
 * \brief Sends APDU of pass-through put response to secure element reads back
 * its APDU response.
 *
 * \note Since pass-through put response command follows a proprietary-APDU
 * structure, this method will directly invoke the ifx_protocol_transceive()
 * method, bypassing the APDU protocol layer.
 *
 * \details Encodes pass-through put response APDU, then sends it through
 * ISO/OSI protocol. Reads back its response and stores it in APDU response.
 *
 * \param[in] self          Protocol stack for performing necessary operations.
 * \param[in] apdu_bytes          Pass-through put response APDU to be sent to
 * secure element.
 * \param[out] response     Response received from the secure element.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
static ifx_status_t pass_through_apdu_tranceive(ifx_protocol_t *self,
                                                const ifx_blob_t *apdu_bytes,
                                                ifx_apdu_response_t *response)
{
    if ((IFX_VALIDATE_NULL_PTR_MEMORY(self)) ||
        (IFX_VALIDATE_NULL_PTR_MEMORY(apdu_bytes)) ||
        (IFX_VALIDATE_NULL_PTR_MEMORY(response)))
    {
        return IFX_ERROR(NBT_CMD, NBT_PASS_THROUGH_PUT_RESPONSE,
                         IFX_ILLEGAL_ARGUMENT);
    }

    // Exchange data with secure element
    uint8_t *response_buffer = NULL;
    size_t response_len = 0;
    ifx_status_t status =
        ifx_protocol_transceive(self, apdu_bytes->buffer, apdu_bytes->length,
                                &response_buffer, &response_len);
    if (ifx_error_check(status))
    {
        NBT_APDU_LOG(self->_logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                     "protocol transceive error");
    }

    if (IFX_SUCCESS == status)
    {
        // Log transmitted data
        NBT_APDU_LOG_BYTES(self->_logger, NBT_CMD_LOG_TAG, IFX_LOG_INFO, ">> ",
                           apdu_bytes->buffer, apdu_bytes->length, " ");
        // Decode APDU response
        status =
            ifx_apdu_response_decode(response, response_buffer, response_len);
        if (status != IFX_SUCCESS)
        {
            NBT_APDU_LOG_BYTES(self->_logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                               "received invalid APDU response: ",
                               response_buffer, response_len, " ");
        }
        else
        {
            NBT_APDU_LOG_BYTES(self->_logger, NBT_CMD_LOG_TAG, IFX_LOG_INFO,
                               "<< ", response_buffer, response_len, " ");
        }
        IFX_FREE(response_buffer);
        response_buffer = NULL;
    }

    return status;
}

/**
 * \brief Get the fap bytes from nbt_file_access_policy_t type handler's field.
 * \param[in] fap_policy        Pointer to nbt_file_access_policy_t type object
 * \param[out] fap_bytes        Pointer to blob type object
 *                              Format of FAP: < FileID (2B) > < Config byte for
 * I2C read(1B) > < Config byte for I2C write (1B) > <Config byte for NFC
 * read(1B) > < Config byte for NFC write (1B) >
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
static ifx_status_t fap_encode(const nbt_file_access_policy_t *fap_policy,
                               ifx_blob_t *fap_bytes)
{
    uint8_t buffer_offset;

#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(fap_policy) ||
        IFX_VALIDATE_NULL_PTR_BLOB(fap_bytes))
    {
        return IFX_ERROR(NBT_CMD, NBT_GET_FAP_BYTES, IFX_ILLEGAL_ARGUMENT);
    }
#endif

    fap_bytes->buffer = (uint8_t *) malloc(NBT_FAP_ACCESS_CONDITION_LENGTH);
    if (IFX_VALIDATE_NULL_PTR_MEMORY(fap_bytes->buffer))
    {
        return IFX_ERROR(NBT_CMD, NBT_GET_FAP_BYTES, IFX_OUT_OF_MEMORY);
    }

    /* Convert handler's FileID to bytes array. */
    IFX_UPDATE_U16(fap_bytes->buffer, fap_policy->file_id);
    buffer_offset = NBT_FILE_ID_LENGTH;

    /* Convert handler's I2C and NFC access conditions to bytes array. */
    fap_bytes->buffer[buffer_offset++] = fap_policy->i2c_read_access_condition;
    fap_bytes->buffer[buffer_offset++] = fap_policy->i2c_write_access_condition;
    fap_bytes->buffer[buffer_offset++] = fap_policy->nfc_read_access_condition;
    fap_bytes->buffer[buffer_offset++] = fap_policy->nfc_write_access_condition;
    fap_bytes->length = buffer_offset;

    return IFX_SUCCESS;
}

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
ifx_status_t nbt_select_application(nbt_cmd_t *self)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(self))
    {
        return IFX_ERROR(NBT_CMD, NBT_SELECT_APPLICATION, IFX_ILLEGAL_ARGUMENT);
    }
#endif

    ifx_status_t status = build_select_application(self->apdu);
    if (ifx_error_check(status))
    {
        NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                     "build_select_application unable to build command ");
    }
    else
    {
        status = ifx_apdu_protocol_transceive(self->protocol, self->apdu,
                                              self->response);
        if (ifx_error_check(status))
        {
            NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                         "apdu transceive error");
        }
    }

    return status;
}

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
ifx_status_t nbt_select_file(nbt_cmd_t *self, uint16_t file_id)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(self))
    {
        return IFX_ERROR(NBT_CMD, NBT_SELECT_FILE_BY_ID, IFX_ILLEGAL_ARGUMENT);
    }
#endif

    ifx_status_t status = build_select_file(file_id, self->apdu);
    if (ifx_error_check(status))
    {
        NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                     "build_cmd_select_file_by_ID unable to build command ");
    }
    else
    {
        status = ifx_apdu_protocol_transceive(self->protocol, self->apdu,
                                              self->response);
        if (ifx_error_check(status))
        {
            NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                         "apdu transceive error");
        }
    }

    return status;
}

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
                                           const ifx_blob_t *write_password)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(self))
    {
        return IFX_ERROR(NBT_CMD, NBT_SELECT_FILE_BY_ID, IFX_ILLEGAL_ARGUMENT);
    }
#endif

    ifx_status_t status = build_select_file_with_password(
        file_id, read_password, write_password, self->apdu);
    if (ifx_error_check(status))
    {
        NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                     "build_cmd_select_file_by_ID unable to build command ");
    }
    else
    {
        status = ifx_apdu_protocol_transceive(self->protocol, self->apdu,
                                              self->response);
        if (ifx_error_check(status))
        {
            NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                         "apdu transceive error");
        }
    }

    return status;
}

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
                             uint8_t binary_data_length)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(self))
    {
        return IFX_ERROR(NBT_CMD, NBT_READ_BINARY, IFX_ILLEGAL_ARGUMENT);
    }
#endif

    ifx_status_t status =
        build_read_binary(offset, binary_data_length, self->apdu);
    if (ifx_error_check(status))
    {
        NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                     "build_read_binary unable to build command ");
    }
    else
    {
        status = ifx_apdu_protocol_transceive(self->protocol, self->apdu,
                                              self->response);
        if (ifx_error_check(status))
        {
            NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                         "apdu transceive error");
        }
    }

    return status;
}

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
                               uint32_t data_length, const uint8_t *data)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(self))
    {
        return IFX_ERROR(NBT_CMD, NBT_UPDATE_BINARY, IFX_ILLEGAL_ARGUMENT);
    }
#endif

    ifx_status_t status =
        build_update_binary(offset, data_length, data, self->apdu);
    if (ifx_error_check(status))
    {
        NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                     "build_update_binary unable to build command ");
    }
    else
    {
        status = ifx_apdu_protocol_transceive(self->protocol, self->apdu,
                                              self->response);
        if (ifx_error_check(status))
        {
            NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                         "apdu transceive error");
        }
    }

    return status;
}

/**
 * \brief Changes an existing password with a new password. If the FAP file
 * update operation is password protected, the master password is required to
 * change the password.
 *
 * \param[in,out] self Command set with communication protocol and response.
 * \param[in] pwd_id 5-bits password ID (Between 01 to 1F).
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
                                 const ifx_blob_t *new_password)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(self))
    {
        return IFX_ERROR(NBT_CMD, NBT_CHANGE_PASSWORD, IFX_ILLEGAL_ARGUMENT);
    }
#endif

    ifx_status_t status = build_change_password(pwd_id, master_password,
                                                new_password, self->apdu);
    if (ifx_error_check(status))
    {
        NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                     "build_change_password unable to build command ");
    }
    else
    {
        status = ifx_apdu_protocol_transceive(self->protocol, self->apdu,
                                              self->response);
        if (ifx_error_check(status))
        {
            NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                         "apdu transceive error");
        }
    }

    return status;
}

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
                                  const ifx_blob_t *master_password)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(self))
    {
        return IFX_ERROR(NBT_CMD, NBT_UNBLOCK_PASSWORD, IFX_ILLEGAL_ARGUMENT);
    }
#endif

    ifx_status_t status =
        build_unblock_password(pwd_id, master_password, self->apdu);
    if (ifx_error_check(status))
    {
        NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                     "build_unblock_password unable to build command ");
    }
    else
    {
        status = ifx_apdu_protocol_transceive(self->protocol, self->apdu,
                                              self->response);
        if (ifx_error_check(status))
        {
            NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                         "apdu transceive error");
        }
    }

    return status;
}

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
ifx_status_t nbt_authenticate_tag(nbt_cmd_t *self, const ifx_blob_t *challenge)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(self))
    {
        return IFX_ERROR(NBT_CMD, NBT_AUTHENTICATE_TAG, IFX_ILLEGAL_ARGUMENT);
    }
#endif

    ifx_status_t status = build_authenticate_tag(challenge, self->apdu);
    if (ifx_error_check(status))
    {
        NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                     "build_authenticate_tag unable to build command ");
    }
    else
    {
        status = ifx_apdu_protocol_transceive(self->protocol, self->apdu,
                                              self->response);

        if (ifx_error_check(status))
        {
            NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                         "apdu transceive error");
        }
    }

    return status;
}

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
                                 uint16_t pwd_resp, uint16_t pwd_limit)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(self))
    {
        return IFX_ERROR(NBT_CMD, NBT_CREATE_PASSWORD, IFX_ILLEGAL_ARGUMENT);
    }
#endif

    ifx_status_t status =
        build_create_password(master_password, new_password_id, new_password,
                              pwd_resp, pwd_limit, self->apdu);
    if (ifx_error_check(status))
    {
        NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                     "build_cmd_select_file_by_ID unable to build command ");
    }
    else
    {
        status = ifx_apdu_protocol_transceive(self->protocol, self->apdu,
                                              self->response);
        if (ifx_error_check(status))
        {
            NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                         "apdu transceive error");
        }
    }

    return status;
}

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
                                 uint8_t password_id)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(self))
    {
        return IFX_ERROR(NBT_CMD, NBT_DELETE_PASSWORD, IFX_ILLEGAL_ARGUMENT);
    }
#endif

    ifx_status_t status =
        build_delete_password(master_password, password_id, self->apdu);
    if (ifx_error_check(status))
    {
        NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                     "build_delete_password unable to build command ");
    }
    else
    {
        status = ifx_apdu_protocol_transceive(self->protocol, self->apdu,
                                              self->response);
        if (ifx_error_check(status))
        {
            NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                         "apdu transceive error");
        }
    }

    return status;
}

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
ifx_status_t nbt_get_data(nbt_cmd_t *self, uint16_t get_data_rcp)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(self))
    {
        return IFX_ERROR(NBT_CMD, NBT_GET_DATA, IFX_ILLEGAL_ARGUMENT);
    }
#endif

    ifx_status_t status = build_get_data(get_data_rcp, self->apdu);
    if (ifx_error_check(status))
    {
        NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                     "build_get_data unable to build command ");
    }
    else
    {
        status = ifx_apdu_protocol_transceive(self->protocol, self->apdu,
                                              self->response);
        if (ifx_error_check(status))
        {
            NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                         "apdu transceive error");
        }
    }

    return status;
}

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
                                         nbt_applet_version_t *applet_version)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(self))
    {
        return IFX_ERROR(NBT_CMD, NBT_APPLET_VERSION, IFX_ILLEGAL_ARGUMENT);
    }
#endif

    ifx_status_t status = nbt_get_data(self, NBT_GET_DATA_APPLET_VERSION);
    if (ifx_error_check(status))
    {
        NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                     "get_data_applet_version unable to get data ");
    }
    status = nbt_parse_applet_version(self->response, applet_version);

    return status;
}

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
    nbt_cmd_t *self, nbt_available_memory_t *available_memory)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(self) ||
        IFX_VALIDATE_NULL_PTR_MEMORY(available_memory))
    {
        return IFX_ERROR(NBT_CMD, NBT_AVAILABLE_MEMORY, IFX_ILLEGAL_ARGUMENT);
    }
#endif

    ifx_status_t status = nbt_get_data(self, NBT_GET_DATA_AVAILABLE_MEMORY);
    if (ifx_error_check(status))
    {
        NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                     "get_data_available_memory unable to get data ");
    }
    status = nbt_parse_available_memory(self->response, available_memory);

    return status;
}

/**
 * \brief Get the access condition policy byte for NFC read/write and I2C
 * read/write interface password ID as NULL, if nbt_access_condition is ALWAYS
 * and NEVER, otherwise pass valid password ID (0x01 - 0x1F) with
 * nbt_access_condition PASSWORD_PROTECTED
 * \param[in] access_type           Access_condition type object (ALWAYS,
 * NEVER_ACCESS and PASSWORD_PROTECTED)
 * \param[in] password_id           Pointer to store password ID
 * \return uint8_t              Returns a valid access byte, if password_id is
 * NULL and nbt_access_condition is ALWAYS, NEVER_ACCESS return valid access
 * byte, if password_id (0x01 - 0x0F) and nbt_access_condition is
 * PASSWORD_PROTECTED return invalid access byte (0x80), if password_id is NULL
 * & nbt_access_condition is PASSWORD_PROTECTED.
 */
uint8_t nbt_access_condition_get(nbt_access_condition access_type,
                                 const uint8_t *password_id)
{
    uint8_t access_condition_byte = access_type;

    if (access_type == NBT_ACCESS_PASSWORD_PROTECTED)
    {
        if (password_id != NULL)
        {
            access_condition_byte |= (*password_id) & NBT_PASSWORD_ID_MASK;
        }
    }

    return access_condition_byte;
}

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
                            const nbt_file_access_policy_t *fap_policy)
{
    /* Master password is NULL, means updating the FAP file (not protected with
     * password) without master password. */
    ifx_status_t status = nbt_update_fap_with_password(self, fap_policy, NULL);
    return status;
}

/**
 * \brief Updates file access conditions in FAP file. when FAP file is password
 * protected(optional). If FAP file is not password protected, then no need to
 * authenticate with the master password, pass the master password as NULL
 * \param[in,out] self Command set with communication protocol and response.
 * \param[in] fap_policy        Pointer to nbt_file_access_policy_t handler to
 * store fap file access conditions for I2C and NFC interface
 * \param[in] master_password   Pointer to ifx_blob_t handler to store FAP file
 * master password(4 bytes), if FAP file is password protected(optional)
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_update_fap_with_password(
    nbt_cmd_t *self, const nbt_file_access_policy_t *fap_policy,
    const ifx_blob_t *master_password)
{
    ifx_blob_t fap_bytes;
    ifx_status_t status;

    /* Convert handler's field into bytes array. */
    status = fap_encode(fap_policy, &fap_bytes);
    if (!ifx_error_check(status))
    {
        /* Update FAP bytes, if FAP file is password protected. */
        status = nbt_update_fap_bytes_with_password(self, &fap_bytes,
                                                    master_password);
    }

    return status;
}

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
                                  const ifx_blob_t *policy_bytes)
{
    /* Master password is NULL, means updating the FAP file (not protected with
     * password) without master password. */
    ifx_status_t status =
        nbt_update_fap_bytes_with_password(self, policy_bytes, NULL);
    return status;
}

/**
 * \brief Updates file access conditions in FAP file from policy bytes. When FAP
 * file is password protected (optional). If FAP file is not password protected,
 * then no need to authenticate with the master password, pass the master
 * password as NULL.
 * \param[in,out] self Command set with communication protocol and response.
 * \param[in] policy_bytes      Pointer to ifx_blob_t handler to store access
 * condition for I2C and NFC read/write operation along with FileID.
 * \param[in] master_password   Pointer to ifx_blob_t handler to store FAP file
 * write password(4 bytes), if FAP file write operation is password
 * protected(optional).
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_update_fap_bytes_with_password(
    nbt_cmd_t *self, const ifx_blob_t *policy_bytes,
    const ifx_blob_t *master_password)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_BLOB(policy_bytes))
    {
        return IFX_ERROR(NBT_CMD, NBT_UPDATE_FAP_BYTES_WITH_PASSWORD,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    ifx_status_t status = IFX_SUCCESS;

    if (master_password != NULL)
    {
        /* Select file for update (write) operation, read password as NULL means
        we are using master password for file update (write) operation. */
        status = nbt_select_file_with_password(self, NBT_FAP_FILE_ID, NULL,
                                               master_password);
    }
    else
    {
        status = nbt_select_file(self, NBT_FAP_FILE_ID);
    }

    if (!ifx_error_check(status) && IFX_CHECK_SW_OK(self->response->sw))
    {
        IFX_FREE(self->apdu->data);
        IFX_FREE(self->response->data);
        self->apdu->data = NULL;
        self->response->data = NULL;
        ifx_blob_t fap_content;
        IFX_MEMSET(&fap_content, 0, sizeof(fap_content));
        fap_content.buffer =
            (uint8_t *) malloc(NBT_FAP_ACCESS_CONDITION_LENGTH);
        if (IFX_VALIDATE_NULL_PTR_MEMORY(fap_content.buffer))
        {
            return IFX_ERROR(NBT_CMD, NBT_UPDATE_FAP_BYTES_WITH_PASSWORD,
                             IFX_OUT_OF_MEMORY);
        }

        IFX_MEMCPY(fap_content.buffer, policy_bytes->buffer,
                   policy_bytes->length);
        fap_content.length = NBT_FAP_ACCESS_CONDITION_LENGTH;

        /* If the current selected file is FAP file, then the offset other than
         * ‘0000’ will be ignored by applet. */
        status = nbt_update_binary(self, 0x0000, fap_content.length,
                                   fap_content.buffer);
        IFX_FREE(fap_content.buffer);
        fap_content.buffer = NULL;
    }

    return status;
}

/**
 * \brief Parse response (APDU response type) bytes to array of
 * nbt_file_access_policy_t type object of FAP file. \note :Memory should be
 * allocated for the 'fap_policies' array parameter externally before passing to
 * this function. Example: nbt_file_access_policy_t policies[7];  // 7 =>
 * NBT_TOTAL_FILES
 * \param[in] fap_bytes          Pointer to blob handler to store fap bytes
 *                               Format of FAP:<FileID (2B)> <Config byte for
 * I2C read(1B)> <Config byte for I2C write(1B)> <Config byte for NFC read(1B)>
 * <Config byte for NFC write(1B)>
 * \param[in] no_of_fap_policies Number of FAP policies
 * (nbt_file_access_policy_t object type) that's to be parsed. Make sure memory
 * should be allocated for nbt_file_access_policy_t object (0x00 <
 * no_of_fap_policies <= NBT_TOTAL_FILE(7))
 * \param[out] fap_policies      Pointer to nbt_file_access_policy_t handler to
 * store FAP file access conditions for I2C and NFC interface along with FileID.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If function parameter is NULL or
 * no_of_fap_policies is zero
 * \retval NBT_FAP_PARSE_ERROR : If expected(NBT_FAP_ACCESS_CONDITION_LENGTH(6
 * bytes))fap bytes is not available
 */
ifx_status_t nbt_parse_fap_bytes(const ifx_blob_t *fap_bytes,
                                 uint8_t no_of_fap_policies,
                                 nbt_file_access_policy_t *fap_policies)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(fap_policies) ||
        IFX_VALIDATE_NULL_PTR_BLOB(fap_bytes) || (no_of_fap_policies == 0x00))
    {
        return IFX_ERROR(NBT_CMD, NBT_PARSE_FAP_BYTES, IFX_ILLEGAL_ARGUMENT);
    }
#endif

    uint8_t buffer_offset;
    uint8_t fap_access_policy_count;
    buffer_offset = 0x00;

    /* Parse response byte into array of structure's fields. */
    for (fap_access_policy_count = 0x00;
         fap_access_policy_count < no_of_fap_policies;
         fap_access_policy_count++)
    {
        /* Check whether expected fap bytes is available or not.
        <FileID (2B)><Config byte for I2C read (1B)><Config byte for I2C
        write (1B)><Config byte for NFC read (1B)><Config byte for NFC write
        (1B)> */
        if ((uint32_t) (buffer_offset + NBT_FAP_ACCESS_CONDITION_LENGTH) >
            fap_bytes->length)
        {
            return IFX_ERROR(NBT_CMD, NBT_PARSE_FAP_BYTES, NBT_FAP_PARSE_ERROR);
        }

        /* Read fap bytes and populates into structure array's fields. */
        IFX_READ_U16(&fap_bytes->buffer[buffer_offset],
                     fap_policies[fap_access_policy_count].file_id);
        buffer_offset += NBT_FILE_ID_LENGTH;
        fap_policies[fap_access_policy_count].i2c_read_access_condition =
            fap_bytes->buffer[buffer_offset++];
        fap_policies[fap_access_policy_count].i2c_write_access_condition =
            fap_bytes->buffer[buffer_offset++];
        fap_policies[fap_access_policy_count].nfc_read_access_condition =
            fap_bytes->buffer[buffer_offset++];
        fap_policies[fap_access_policy_count].nfc_write_access_condition =
            fap_bytes->buffer[buffer_offset++];
    }

    return IFX_SUCCESS;
}

/**
 * \brief Reads binary data (fap bytes of elementary files) from FAP file and
 * store the fap bytes into nbt_file_access_policy_t type object array. \note
 * :Memory should be allocated for the 'fap_policies' array parameter externally
 * before passing to this function. Example: nbt_file_access_policy_t
 * policies[7];  // 7 => NBT_TOTAL_FILES.
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
                          nbt_file_access_policy_t *fap_policies)
{
    /* Master password is NULL, means read the FAP file (unprotected with
     * password) content without master password. */
    ifx_status_t status = nbt_read_fap_with_password(self, NULL, fap_policies);
    return status;
}

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
 * write password(4 bytes), if FAP file write operation is password
 * protected(optional)
 * \param[out] fap_policies     Pointer to nbt_file_access_policy_t handler to
 * store fap file access conditions for I2C and NFC interface
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_read_fap_with_password(nbt_cmd_t *self,
                                        const ifx_blob_t *master_password,
                                        nbt_file_access_policy_t *fap_policies)
{
    ifx_status_t status;
    ifx_blob_t fap_bytes;
    uint8_t number_of_policies = NBT_TOTAL_FILE;

    status = nbt_read_fap_bytes_with_password(self, master_password);
    if (!ifx_error_check(status) && IFX_CHECK_SW_OK(self->response->sw) &&
        (self->response->len == NBT_SIZE_OF_FAP_FILE))
    {
        fap_bytes.buffer = (uint8_t *) malloc(NBT_SIZE_OF_FAP_FILE);
        if (IFX_VALIDATE_NULL_PTR_MEMORY(fap_bytes.buffer))
        {
            IFX_FREE(self->apdu->data);
            IFX_FREE(self->response->data);
            self->apdu->data = NULL;
            self->response->data = NULL;
            return IFX_ERROR(NBT_CMD, NBT_READ_FAP_WITH_PASSWORD,
                             IFX_OUT_OF_MEMORY);
        }

        IFX_MEMCPY(fap_bytes.buffer, self->response->data,
                   NBT_SIZE_OF_FAP_FILE);
        fap_bytes.length = NBT_SIZE_OF_FAP_FILE;
        status =
            nbt_parse_fap_bytes(&fap_bytes, number_of_policies, fap_policies);
        IFX_FREE(fap_bytes.buffer);
        fap_bytes.buffer = NULL;
    }
    return status;
}

/**
 * \brief Reads binary data (fap bytes of elementary files) from FAP file.
 * \param[in,out] self Command set with communication protocol and response.

 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_read_fap_bytes(nbt_cmd_t *self)
{
    /* Master password is NULL, means read the FAP file (unprotected with
     * password) content without master password. */
    ifx_status_t status = nbt_read_fap_bytes_with_password(self, NULL);
    return status;
}

/**
 * \brief Reads binary data (fap bytes of elementary files) from FAP file.
 * If FAP file is not password protected, then no need to authenticate with the
 * master password, pass the master password as NULL.
 * \param[in,out] self Command set with communication protocol and response.
 * \param[in] master_password   Pointer to ifx_blob_t handler to store FAP file
 * write password(4 bytes), if FAP file write operation is password
 * protected(optional).
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_read_fap_bytes_with_password(nbt_cmd_t *self,
                                              const ifx_blob_t *master_password)
{
    ifx_status_t status;

    /* Check master password, if master password is NULL then select file with
    ID without master password, otherwise select file with master password. */
    if (master_password != NULL)
    {
        /* Select FAP file for read operation with master password. passing NULL
        as read password, if not selecting FAP file for write operation with
        master password. */
        status = nbt_select_file_with_password(self, NBT_FAP_FILE_ID,
                                               master_password, NULL);
    }
    else
    {
        status = nbt_select_file(self, NBT_FAP_FILE_ID);
    }

    if (!ifx_error_check(status) && IFX_CHECK_SW_OK(self->response->sw))
    {
        /* If the current selected file is FAP file, then the offset other than
         * ‘0000’ will be ignored by applet. */
        IFX_FREE(self->apdu->data);
        IFX_FREE(self->response->data);
        self->apdu->data = NULL;
        self->response->data = NULL;
        status = nbt_read_binary(self, 0x0000, NBT_SIZE_OF_FAP_FILE);
    }

    return status;
}

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
 * \param[in] self Command set with communication protocol.
 * \param[out] response     Buffer to store response data
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_pass_through_fetch_data(nbt_cmd_t *self,
                                         ifx_apdu_response_t *response)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(self) ||
        IFX_VALIDATE_NULL_PTR_MEMORY(response))
    {
        return IFX_ERROR(NBT_CMD, NBT_PASS_THROUGH_FETCH_DATA,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    ifx_status_t status = build_pass_through_fetch_data(self->apdu);
    if (ifx_error_check(status))
    {
        NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                     "build_pass_through_fetch_data unable to build command ");
    }
    else
    {
        status =
            ifx_apdu_protocol_transceive(self->protocol, self->apdu, response);
        if (ifx_error_check(status))
        {
            NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                         "apdu transceive error");
        }
    }

    return status;
}

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
                                        uint16_t *pass_through_status_word)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(response) ||
        IFX_VALIDATE_NULL_PTR_MEMORY(pass_through_status_word))
    {
        return IFX_ERROR(NBT_CMD, NBT_PASS_THROUGH_FETCH_DATA,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    IFX_READ_U16(&response->data[NBT_OFFSET_OF_PT_SW_IN_FETCH_DATA_RESP],
                 *pass_through_status_word);

    return IFX_SUCCESS;
}

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
    const ifx_apdu_response_t *response, ifx_blob_t *nfc_apdu)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(response) ||
        IFX_VALIDATE_NULL_PTR_MEMORY(nfc_apdu))
    {
        return IFX_ERROR(NBT_CMD, NBT_PASS_THROUGH_FETCH_DATA,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    uint16_t status_word = UINT16_C(0);
    ifx_status_t status = nbt_pass_through_decode_sw(response, &status_word);
    if (IFX_SUCCESS == status)
    {
        if (NBT_IS_NFC_DATA_AVAILABLE_IN_PT_SW(status_word))
        {
            nfc_apdu->length = (uint32_t) (response->len) -
                               NBT_OFFSET_OF_NFC_APDU_IN_FETCH_DATA_RESP;
            nfc_apdu->buffer = (uint8_t *) malloc(nfc_apdu->length);
            if (NULL == nfc_apdu->buffer)
            {
                return IFX_ERROR(NBT_CMD, NBT_PASS_THROUGH_FETCH_DATA,
                                 IFX_OUT_OF_MEMORY);
            }
            IFX_MEMCPY(
                nfc_apdu->buffer,
                &response->data[NBT_OFFSET_OF_NFC_APDU_IN_FETCH_DATA_RESP],
                nfc_apdu->length);
        }
        else
        {
            status = IFX_ERROR(NBT_CMD, NBT_PASS_THROUGH_FETCH_DATA,
                               NBT_NFC_APDU_NOT_AVAILABLE);
        }
    }

    return status;
}

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
                                          ifx_apdu_t *nfc_apdu)
{
    ifx_status_t status = IFX_SUCCESS;
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(response) ||
        IFX_VALIDATE_NULL_PTR_MEMORY(nfc_apdu))
    {
        return IFX_ERROR(NBT_CMD, NBT_PASS_THROUGH_FETCH_DATA,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    uint16_t status_word = UINT16_C(0);
    status = nbt_pass_through_decode_sw(response, &status_word);
    if (IFX_SUCCESS == status)
    {
        if (NBT_IS_NFC_DATA_AVAILABLE_IN_PT_SW(status_word))
        {
            status = ifx_apdu_decode(
                nfc_apdu,
                &response->data[NBT_OFFSET_OF_NFC_APDU_IN_FETCH_DATA_RESP],
                response->len - NBT_OFFSET_OF_NFC_APDU_IN_FETCH_DATA_RESP);
        }
        else
        {
            status = IFX_ERROR(NBT_CMD, NBT_PASS_THROUGH_FETCH_DATA,
                               NBT_NFC_APDU_NOT_AVAILABLE);
        }
    }

    return status;
}

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
    ifx_apdu_response_t *response)
{
    ifx_status_t status = IFX_SUCCESS;
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(self) ||
        IFX_VALIDATE_NULL_PTR_MEMORY(response) ||
        (IFX_VALIDATE_NULL_PTR_MEMORY(pass_through_response_data)))
    {
        return IFX_ERROR(NBT_CMD, NBT_PASS_THROUGH_PUT_RESPONSE,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif
    ifx_blob_t apdu_bytes;
    // Initializing apdu_bytes to zero
    apdu_bytes.buffer = NULL;
    apdu_bytes.length = 0;

    status = build_pass_through_put_response(pass_through_response_data,
                                             &apdu_bytes);
    // Set APDU INS field, as it is required for error mapping.
    self->apdu->ins = apdu_bytes.buffer[NBT_INS_BYTE_OFFSET];

    if (ifx_error_check(status))
    {
        NBT_APDU_LOG(
            self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
            "build_pass_through_put_response unable to build command ");
    }
    else
    {
        status =
            pass_through_apdu_tranceive(self->protocol, &apdu_bytes, response);
        if (ifx_error_check(status))
        {
            NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                         "transceive put response error");
        }
    }

    IFX_FREE(apdu_bytes.buffer);
    apdu_bytes.buffer = NULL;
    return status;
}

/**
 * \brief  Append new response to existing response. This method is useful if
 * the response dat has to be concatenated from the content of multiple APDU
 * responses (GET RESPONSE). The status word of the existing response will be
 * replaced by the status word of the new response.
 *
 * \param[in,out] self Reference to existing response on which response should
 * append.
 * \param[in] response New response to be appended to existing response.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
static ifx_status_t append_apdu_response(ifx_apdu_response_t *self,
                                         const ifx_apdu_response_t *response)
{
    // Validate parameters
#if (IFX_VALIDATE_NULL_PTR)
    if ((self == NULL) || (response == NULL))
    {
        return IFX_ERROR(LIB_APDU, IFX_APDU_DECODE, IFX_ILLEGAL_ARGUMENT);
    }
#endif
    if (response->len < 2)
    {
        return IFX_ERROR(LIB_APDU, IFX_APDU_DECODE, IFX_ILLEGAL_ARGUMENT);
    }

    // Append response data and overwrite status word of existing response.
    uint8_t *buffer_temp = (uint8_t *) malloc(self->len);
    if (buffer_temp == NULL)
    {
        return IFX_ERROR(LIB_APDU, IFX_APDU_RESPONSE_ENCODE, IFX_OUT_OF_MEMORY);
    }
    IFX_MEMCPY(buffer_temp, self->data, self->len);
    self->data = (uint8_t *) malloc(self->len + response->len);
    if (self->data == NULL)
    {
        IFX_FREE(buffer_temp);
        buffer_temp = NULL;
        return IFX_ERROR(LIB_APDU, IFX_APDU_RESPONSE_ENCODE, IFX_OUT_OF_MEMORY);
    }
    IFX_MEMCPY(self->data, buffer_temp, self->len);
    IFX_MEMCPY(self->data + self->len, response->data, response->len);
    self->len = self->len + response->len;
    self->sw = response->sw;
    IFX_FREE(buffer_temp);
    buffer_temp = NULL;
    return IFX_SUCCESS;
}

/**
 * \brief Method to perform readv binary command in recursive pattern. In first
 * time call to method user has to pass offset from where file has to read.
 *
 *
 * \param[in,out] self NBT command set object, which will contains NDEF
 *                     message bytes including status word after successfully
 *                     read message.
 * \param[in] offset  Offset value from where start reading the binary file, we
 *                    can use micros NBT_RECURSIVE_READ_INIT_OFFSET.
 *
 * \param[in] total_bytes_to_read Size of data to be read from file including
 *                                the total bytes field, we can use micros
 *                                NBT_RECURSIVE_READ_INIT_MSG_LEN.
 *
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
static ifx_status_t recursive_ndef_read(nbt_cmd_t *self, uint16_t offset,
                                        uint16_t total_bytes_to_read)
{
    uint16_t total_bytes_remains_to_read = total_bytes_to_read - offset;
    ifx_status_t status =
        nbt_read_binary(self, offset,
                        (total_bytes_remains_to_read > NBT_MAX_LE)
                            ? NBT_MAX_LE
                            : total_bytes_remains_to_read);
    ifx_apdu_response_t response = *(self->response);

    if (ifx_error_check(status) || !IFX_CHECK_SW_OK(self->response->sw))
    {
        return status;
    }

    // Calculate total bytes to be read from first two bytes of NDEF file.
    if (offset == 0x00)
    {
        // Update the total_bytes_to_read with first 2 bytes from file.
        IFX_READ_U16(self->response->data, total_bytes_to_read);

        uint16_t data_length = (uint16_t) (response.len - 2);
        // If NDEF message size is less than the response length.
        if (total_bytes_to_read < (response.len))
        {
            data_length = (total_bytes_to_read);
        }

        // Shift all data by 2-byte left in response data.
        IFX_MEMCPY(response.data, response.data + 2U, data_length);

        // Calculate next iteration expected length and offset.
        offset = offset + (uint16_t) (data_length + 2U);
        response.len = data_length;
        // +2 for adding 2 bytes of length
        total_bytes_to_read += 0x02;
    }
    else
    {
        // Calculate next iteration expected length and offset.
        offset = offset + (uint16_t) (self->response->len);
    }
    if (total_bytes_to_read > offset)
    {
        recursive_ndef_read(self, offset, total_bytes_to_read);
        append_apdu_response(&response, self->response);
    }
    *(self->response) = response;

    return status;
}

/**
 * \brief Method to perform update binary command in recursive pattern. In first
 * time call to method user has to pass offset from where file has to start
 * write data.
 *
 *
 * \param[in,out] self NBT command set object, which will status word after
 *                     success.
 * \param[in] offset  Offset value from where start writing the binary file, we
 *                    can use micros RECURSIVE_WRITE_INIT_OFFSET.
 * \param[in] ndef_bytes NDEF data bytes to be write. ndef_bytes should without
 *                       NDEF length method will inside calculate ndef length
 *                       and add to file internally.
 *
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
static ifx_status_t recursive_ndef_update(nbt_cmd_t *self, uint16_t offset,
                                          ifx_blob_t *ndef_bytes)
{
    ifx_status_t status;
    // Validate data is null.
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_BLOB(ndef_bytes))
    {
        return IFX_ERROR(NBT_CMD, NBT_UPDATE_RECURSIVE_BINARY,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    // Adding file size at the beginning of file data.
    if (offset == 0)
    {
        // fap_content.length, fap_content.buffer

        uint8_t *buffer_data = (uint8_t *) malloc(ndef_bytes->length + 2);
        if (buffer_data == NULL)
        {
            return IFX_ERROR(NBT_CMD, NBT_UPDATE_RECURSIVE_BINARY,
                             IFX_OUT_OF_MEMORY);
        }
        // Adding 2 bytes NLEN in buffer.
        IFX_UPDATE_U16(buffer_data, (ndef_bytes->length));
        IFX_MEMCPY(buffer_data + 2U, ndef_bytes->buffer, ndef_bytes->length);

        // Update buffer size including 2 bytes NLEN field
        ndef_bytes->length = ndef_bytes->length + 2;

        // Copy updated data to ndef_bytes
        ndef_bytes->buffer = (uint8_t *) malloc(ndef_bytes->length);
        if (ndef_bytes->buffer == NULL)
        {
            return IFX_ERROR(NBT_CMD, NBT_UPDATE_RECURSIVE_BINARY,
                             IFX_OUT_OF_MEMORY);
        }
        IFX_MEMCPY(ndef_bytes->buffer, buffer_data, ndef_bytes->length);
        IFX_FREE(buffer_data);
        buffer_data = NULL;
    }

    // Extracting block of NDEF message to be updated.
    uint32_t total_remaining_data_size = ndef_bytes->length - offset;
    uint32_t block_size = (total_remaining_data_size > NBT_MAX_LC)
                              ? NBT_MAX_LC
                              : total_remaining_data_size;

    // Update the sub set of data.
    status = nbt_update_binary(self, offset, block_size,
                               ndef_bytes->buffer + offset);
    if (ifx_error_check(status) || !(IFX_CHECK_SW_OK(self->response->sw)))
    {
        return status;
    }
    // Calculate next iteration expected offset and total remaining data size.
    offset = (short) (offset + block_size);
    total_remaining_data_size = ndef_bytes->length - offset;
    if (total_remaining_data_size > 0)
    {
        status = recursive_ndef_update(self, offset, ndef_bytes);
    }
    return status;
}

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
 * Null if not required)
 * \param[in] ndef_bytes  Data to be write
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_ndef_update_with_id_password(nbt_cmd_t *self, uint16_t file_id,
                                              const ifx_blob_t *write_password,
                                              ifx_blob_t *ndef_bytes)
{
    ifx_status_t status;

    /* Check read password, if read password is NULL then select file with id
    without read password, otherwise select file with read password. */
    if (write_password != NULL)
    {
        /* Select NDEF file for read operation with read password. Passing NULL
         * as write password. */
        status =
            nbt_select_file_with_password(self, file_id, NULL, write_password);
    }
    else
    {
        status = nbt_select_file(self, file_id);
    }
    if (!ifx_error_check(status) && IFX_CHECK_SW_OK(self->response->sw))
    {
        status = recursive_ndef_update(self, NBT_RECURSIVE_UPDATE_INIT_OFFSET,
                                       ndef_bytes);
    }
    return status;
}

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
                                     ifx_blob_t *ndef_bytes)
{
    return nbt_ndef_update_with_id_password(self, file_id, NULL, ndef_bytes);
}

/**
 * \brief  Updates NDEF file with 4-byte password (Optional) and default NDEF
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
                                           ifx_blob_t *ndef_bytes)
{
    return nbt_ndef_update_with_id_password(self, NBT_NDEF_FILE_ID,
                                            write_password, ndef_bytes);
}

/**
 * \brief  Updates NDEF file default ndef FileID (E104), Method performs the
 * select file with file_id and then update binary until data is available.
 *
 * Note: Application must be selected already with selectApplication() API
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
ifx_status_t nbt_ndef_update(nbt_cmd_t *self, ifx_blob_t *ndef_bytes)
{
    return nbt_ndef_update_with_id_password(self, NBT_NDEF_FILE_ID, NULL,
                                            ndef_bytes);
}

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
                                            const ifx_blob_t *read_password)
{
    ifx_status_t status;

    /* check read password, if read password is NULL then select file with id
    without read password, otherwise select file with read password */
    if (read_password != NULL)
    {
        /* select ndef file for read operation with read password. passing NULL
         * as write password */
        status =
            nbt_select_file_with_password(self, file_id, read_password, NULL);
    }
    else
    {
        status = nbt_select_file(self, file_id);
    }

    if (!ifx_error_check(status) && IFX_CHECK_SW_OK(self->response->sw))
    {
        status = recursive_ndef_read(self, NBT_RECURSIVE_READ_INIT_OFFSET,
                                     NBT_MAX_LE);
    }

    return status;
}

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
ifx_status_t nbt_ndef_read_with_id(nbt_cmd_t *self, uint16_t file_id)
{
    return nbt_ndef_read_with_id_password(self, file_id, NULL);
}

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
 * Null if not required).
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_ndef_read_with_password(nbt_cmd_t *self,
                                         const ifx_blob_t *read_password)
{
    return nbt_ndef_read_with_id_password(self, NBT_NDEF_FILE_ID,
                                          read_password);
}

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
ifx_status_t nbt_ndef_read(nbt_cmd_t *self)
{
    return nbt_ndef_read_with_id_password(self, NBT_NDEF_FILE_ID, NULL);
}
