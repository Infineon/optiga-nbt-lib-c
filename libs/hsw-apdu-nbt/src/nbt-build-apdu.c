// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file nbt-build-apdu.c
 * \brief Provides the function implementation for command for NBT operational
 * command builder.
 */
#include "nbt-build-apdu.h"

#include "infineon/ifx-utils.h"
#include "infineon/nbt-apdu-lib.h"
#include "infineon/nbt-errors.h"

/** \brief Put response header length -> NBT_CLA(1byte) + INS(1byte) +
 * RFU(1byte) + LC(2bytes) */
#define LENGTH_OF_PUT_RESPONSE_HEADER UINT8_C(0x05)

/**
 * \brief AID of NBT application
 */
const uint8_t nbt_aid[] = {0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01};

/**
 * \brief Support function for select file command. It appends the password to
 * the command data of select file command.
 *
 * \param password Password to be append
 * \param tag Tag for password Read/Write
 * \param apdu APDU command to which the password has to be appended.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
static ifx_status_t append_password(const ifx_blob_t *password, uint8_t tag,
                                    ifx_apdu_t *apdu)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu) ||
        IFX_VALIDATE_NULL_PTR_MEMORY(password))
    {
        return IFX_ERROR(NBT_BUILD_APDU, NBT_BUILD_SELECT_FILE,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    size_t offset = UINT8_C(0x00);

    uint8_t *data = (uint8_t *) malloc(apdu->lc);
    if (IFX_VALIDATE_NULL_PTR_MEMORY(data))
    {
        return IFX_ERROR(NBT_BUILD_APDU, NBT_BUILD_SELECT_FILE,
                         IFX_OUT_OF_MEMORY);
    }

    IFX_MEMCPY(data, apdu->data, apdu->lc);
    IFX_FREE(apdu->data);
    apdu->data = NULL;
    apdu->data = (uint8_t *) malloc(apdu->lc + password->length +
                                    NBT_LEN_PASSWORD_HEADER);
    if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu->data))
    {
        IFX_FREE(data);
        data = NULL;
        return IFX_ERROR(NBT_BUILD_APDU, NBT_BUILD_SELECT_FILE,
                         IFX_OUT_OF_MEMORY);
    }

    IFX_MEMCPY(apdu->data, data, apdu->lc);
    offset += apdu->lc;
    *(apdu->data + offset) = tag;
    *(apdu->data + offset + NBT_OFFSET_MEMORY_INCREMENT) = password->length;
    offset += NBT_LEN_PASSWORD_HEADER;
    IFX_MEMCPY(apdu->data + offset, password->buffer, password->length);
    offset += password->length;
    apdu->lc = offset;
    IFX_FREE(data);
    data = NULL;
    return IFX_SUCCESS;
}

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
ifx_status_t build_select_application(ifx_apdu_t *apdu)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu))
    {
        return IFX_ERROR(NBT_BUILD_APDU, NBT_BUILD_SELECT_APPLICATION,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    apdu->data = (uint8_t *) malloc(sizeof(nbt_aid));
    if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu->data))
    {
        return IFX_ERROR(NBT_BUILD_APDU, NBT_BUILD_SELECT_APPLICATION,
                         IFX_OUT_OF_MEMORY);
    }
    apdu->cla = NBT_CLA;
    apdu->ins = NBT_INS_SELECT;
    apdu->p1 = NBT_P1_SELECT_BY_DF;
    apdu->p2 = NBT_P2_DEFAULT;
    apdu->lc = sizeof(nbt_aid);
    IFX_MEMCPY(apdu->data, nbt_aid, apdu->lc);
    apdu->le = IFX_APDU_LE_ANY;

    return IFX_SUCCESS;
}

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
ifx_status_t build_select_file(uint16_t file_id, ifx_apdu_t *apdu)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu))
    {
        return IFX_ERROR(NBT_BUILD_APDU, NBT_BUILD_SELECT_FILE,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    apdu->cla = NBT_CLA;
    apdu->ins = NBT_INS_SELECT;
    apdu->p1 = NBT_P1_DEFAULT;
    apdu->p2 = NBT_P2_SELECT_FIRST_ONLY;
    apdu->lc = sizeof(file_id);
    apdu->data = (uint8_t *) malloc(apdu->lc);
    if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu->data))
    {
        return IFX_ERROR(NBT_BUILD_APDU, NBT_BUILD_SELECT_FILE,
                         IFX_OUT_OF_MEMORY);
    }
    IFX_UPDATE_U16(apdu->data, file_id);
    apdu->le = IFX_APDU_LE_ANY;

    return IFX_SUCCESS;
}

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
                                             ifx_apdu_t *apdu)
{
    ifx_status_t status = build_select_file(file_id, apdu);
    if (!ifx_error_check(status))
    {
        if (!IFX_VALIDATE_NULL_PTR_BLOB(read_password))
        {
            status =
                append_password(read_password, NBT_TAG_PASSWORD_READ, apdu);
        }
        if (!ifx_error_check(status))
        {
            if (!IFX_VALIDATE_NULL_PTR_BLOB(write_password))
            {
                status = append_password(write_password, NBT_TAG_PASSWORD_WRITE,
                                         apdu);
            }
        }
    }
    return status;
}

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
                               ifx_apdu_t *apdu)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu))
    {
        return IFX_ERROR(NBT_BUILD_APDU, NBT_BUILD_READ_BINARY,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    apdu->cla = NBT_CLA;
    apdu->ins = NBT_INS_READ_BINARY;
    IFX_GET_UPPER_BYTE(apdu->p1, offset);
    IFX_GET_LOWER_BYTE(apdu->p2, offset);
    apdu->lc = NBT_LC_ABSENT;
    apdu->data = NBT_APDU_DATA_NULL;
    apdu->le = read_data_length;

    return IFX_SUCCESS;
}

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
                                 const uint8_t *data, ifx_apdu_t *apdu)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu))
    {
        return IFX_ERROR(NBT_BUILD_APDU, NBT_BUILD_UPDATE_BINARY,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    apdu->data = (uint8_t *) malloc(data_length);
    if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu->data))
    {
        return IFX_ERROR(NBT_BUILD_APDU, NBT_BUILD_UPDATE_BINARY,
                         IFX_OUT_OF_MEMORY);
    }

    apdu->cla = NBT_CLA;
    apdu->ins = NBT_INS_UPDATE_BINARY;
    IFX_GET_UPPER_BYTE(apdu->p1, offset);
    IFX_GET_LOWER_BYTE(apdu->p2, offset);
    apdu->lc = data_length;
    IFX_MEMCPY(apdu->data, data, apdu->lc);
    apdu->le = NBT_LE_ABSENT;
    return IFX_SUCCESS;
}

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
                                   ifx_apdu_t *apdu)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_BLOB(new_password) ||
        IFX_VALIDATE_NULL_PTR_MEMORY(apdu))
    {
        return IFX_ERROR(NBT_BUILD_APDU, NBT_BUILD_CHANGE_PASSWORD,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif
    uint8_t offset = 0x00;
    uint16_t data_length = new_password->length;
    if (!IFX_VALIDATE_NULL_PTR_BLOB(master_password))
    {
        data_length += master_password->length;
    }

    apdu->data = (uint8_t *) malloc(data_length);
    if (apdu->data == NULL)
    {
        return IFX_ERROR(NBT_BUILD_APDU, NBT_BUILD_CHANGE_PASSWORD,
                         IFX_OUT_OF_MEMORY);
    }

    if (!IFX_VALIDATE_NULL_PTR_BLOB(master_password))
    {
        IFX_MEMCPY(&apdu->data[offset], master_password->buffer,
                   master_password->length);
        offset += master_password->length;
    }
    IFX_MEMCPY(&apdu->data[offset], new_password->buffer, new_password->length);

    apdu->cla = NBT_CLA;
    apdu->ins = NBT_INS_CHANGE_UNBLOCK_PASSWORD;
    apdu->p1 = NBT_P1_DEFAULT;
    apdu->p2 = (NBT_PASSWORD_CHANGE_MASK | new_password_id);
    apdu->le = NBT_LE_ABSENT;
    apdu->lc = data_length;
    return IFX_SUCCESS;
}

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
                                    ifx_apdu_t *apdu)
{
    ifx_status_t status = IFX_SUCCESS;
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu))
    {
        return IFX_ERROR(NBT_BUILD_APDU, NBT_BUILD_UNBLOCK_PASSWORD,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    apdu->lc = 0x00;
    apdu->data = NULL;

    if (NULL != master_password)
    {
        if (NULL != master_password->buffer)
        {
            apdu->lc = master_password->length;
            apdu->data = (uint8_t *) malloc(master_password->length);
            if (apdu->data == NULL)
            {
                return IFX_ERROR(NBT_BUILD_APDU, NBT_BUILD_UNBLOCK_PASSWORD,
                                 IFX_OUT_OF_MEMORY);
            }
            IFX_MEMCPY(apdu->data, master_password->buffer, apdu->lc);
        }
    }

    apdu->cla = NBT_CLA;
    apdu->ins = NBT_INS_CHANGE_UNBLOCK_PASSWORD;
    apdu->p1 = NBT_P1_DEFAULT;
    apdu->p2 = (NBT_PASSWORD_UNBLOCK_MASK & pwd_id);
    apdu->le = NBT_LE_ABSENT;

    return status;
}

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
                                    ifx_apdu_t *apdu)
{
    /* Check for NULL type parameter error */
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu) ||
        IFX_VALIDATE_NULL_PTR_MEMORY(challenge))
    {
        return IFX_ERROR(NBT_BUILD_APDU, NBT_BUILD_AUTHENTICATE_TAG,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    /* Validate the challenge length */
    if (challenge->length == 0)
    {
        return IFX_ERROR(NBT_BUILD_APDU, NBT_BUILD_AUTHENTICATE_TAG,
                         NBT_LC_INVALID);
    }

    /* Prepare the frame for authenticate tag command */
    apdu->cla = NBT_CLA;
    apdu->ins = NBT_INS_AUTHENTICATE_TAG;
    apdu->p1 = NBT_P1_DEFAULT;
    apdu->p2 = NBT_P2_DEFAULT;
    apdu->lc = challenge->length;
    apdu->le = IFX_APDU_LE_ANY;

    /* Allocate memory block as per challenge length */
    apdu->data = (uint8_t *) malloc(challenge->length);
    if (apdu->data == NULL)
    {
        return IFX_ERROR(NBT_BUILD_APDU, NBT_BUILD_AUTHENTICATE_TAG,
                         IFX_OUT_OF_MEMORY);
    }

    IFX_MEMCPY(apdu->data, challenge->buffer, challenge->length);
    /* Successfully command prepared */
    return IFX_SUCCESS;
}

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
                                   ifx_apdu_t *apdu)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_BLOB(new_password) ||
        IFX_VALIDATE_NULL_PTR_MEMORY(apdu))
    {
        return IFX_ERROR(NBT_BUILD_APDU, NBT_BUILD_CREATE_PASSWORD,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    uint8_t offset = UINT8_C(0x00);
    if (IFX_VALIDATE_NULL_PTR_BLOB(master_password))
    {
        apdu->lc =
            NBT_CREATE_PASSWORD_APDU_DATA_HEADER_SIZE + new_password->length;
    }
    else
    {
        apdu->lc = NBT_CREATE_PASSWORD_APDU_DATA_HEADER_SIZE +
                   new_password->length + master_password->length;
    }

    apdu->data = (uint8_t *) malloc(apdu->lc);
    if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu->data))
    {
        return IFX_ERROR(NBT_BUILD_APDU, NBT_BUILD_CREATE_PASSWORD,
                         IFX_OUT_OF_MEMORY);
    }

    if (apdu->lc >
        NBT_CREATE_PASSWORD_APDU_DATA_HEADER_SIZE + new_password->length)
    { // Master password (4B) present
        IFX_MEMCPY(apdu->data + offset, master_password->buffer,
                   master_password->length);
        offset = master_password->length;
    }

    // New password ID (1B)
    *((apdu->data) + offset) = new_password_id;
    offset++;
    // New password (4B)
    IFX_MEMCPY((apdu->data) + offset, new_password->buffer,
               new_password->length);
    offset += new_password->length;

    // Password response (2B)
    IFX_UPDATE_U16((apdu->data + offset), pwd_resp);

    // Password limit  (2B)
    IFX_UPDATE_U16((apdu->data + offset + NBT_LEN_PASSWORD_HEADER), pwd_limit);

    apdu->cla = NBT_CLA;
    apdu->ins = NBT_INS_CREATE_PASSWORD;
    apdu->p1 = NBT_P1_DEFAULT;
    apdu->p2 = NBT_P2_DEFAULT;
    apdu->le = NBT_LE_ABSENT;

    return IFX_SUCCESS;
}

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
                                   uint8_t password_id, ifx_apdu_t *apdu)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu))
    {
        return IFX_ERROR(NBT_BUILD_APDU, NBT_BUILD_DELETE_PASSWORD,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif
    if (IFX_VALIDATE_NULL_PTR_BLOB(master_password))
    {
        apdu->lc = 0;
    }
    else
    {
        apdu->lc = master_password->length;
    }

    apdu->cla = NBT_CLA;
    apdu->ins = NBT_INS_DELETE_PASSWORD;
    apdu->p1 = NBT_P1_DEFAULT;
    apdu->p2 = password_id;

    if (apdu->lc > 0) // Master password present
    {
        apdu->data = (uint8_t *) malloc(apdu->lc);
        if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu->data))
        {
            return IFX_ERROR(NBT_BUILD_APDU, NBT_BUILD_DELETE_PASSWORD,
                             IFX_OUT_OF_MEMORY);
        }
        IFX_MEMCPY(apdu->data, master_password->buffer, apdu->lc);
    }
    else
    {
        apdu->data = NULL;
    }
    apdu->le = NBT_LE_ABSENT;

    return IFX_SUCCESS;
}

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
ifx_status_t build_get_data(uint16_t get_data_rcp, ifx_apdu_t *apdu)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu))
    {
        return IFX_ERROR(NBT_BUILD_APDU, NBT_BUILD_GET_DATA,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    apdu->cla = NBT_CLA;
    apdu->ins = NBT_INS_GET_DATA;
    IFX_GET_UPPER_BYTE(apdu->p1, get_data_rcp);
    IFX_GET_LOWER_BYTE(apdu->p2, get_data_rcp);
    apdu->lc = NBT_LC_ABSENT;
    apdu->data = NBT_APDU_DATA_NULL;
    apdu->le = IFX_APDU_LE_ANY;

    return IFX_SUCCESS;
}

/**
 * \brief Builds the pass-through fetch data command.
 *
 * \param[out] apdu APDU reference to store pass-through fetch data command.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 */
ifx_status_t build_pass_through_fetch_data(ifx_apdu_t *apdu)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu))
    {
        return IFX_ERROR(NBT_BUILD_APDU, NBT_BUILD_PASS_THROUGH_FETCH_DATA,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    apdu->cla = NBT_CLA_PASS_THROUGH;
    apdu->ins = NBT_INS_PASS_THROUGH_FETCH_DATA;
    apdu->p1 = NBT_P1_DEFAULT;
    apdu->p2 = NBT_P2_DEFAULT;
    apdu->lc = NBT_LC_ABSENT;
    apdu->data = NULL;
    apdu->le = NBT_LE_ABSENT;

    return IFX_SUCCESS;
}

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
    const ifx_apdu_response_t *pass_through_response, ifx_blob_t *apdu_bytes)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu_bytes) ||
        IFX_VALIDATE_NULL_PTR_MEMORY(pass_through_response))
    {
        return IFX_ERROR(NBT_BUILD_APDU, NBT_BUILD_PASS_THROUGH_PUT_RESPONSE,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    ifx_status_t status = IFX_SUCCESS;
    uint16_t index = UINT16_C(0);

    /* Encoding put response APDU as byte array directly, bypassing APDU
    structure since pass-through put response has proprietary APDU format */
    uint8_t *encoded_response_data = NULL;
    size_t encoded_response_data_len = 0;
    status =
        ifx_apdu_response_encode(pass_through_response, &encoded_response_data,
                                 &encoded_response_data_len);
    if (IFX_SUCCESS == status)
    {
        if (encoded_response_data_len >= NBT_MIN_PT_RESPONSE_LEN)
        {
            apdu_bytes->length = (uint32_t) encoded_response_data_len +
                                 LENGTH_OF_PUT_RESPONSE_HEADER;
            apdu_bytes->buffer = (uint8_t *) malloc(apdu_bytes->length);
            if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu_bytes->buffer))
            {
                IFX_FREE(encoded_response_data);
                encoded_response_data = NULL;
                return IFX_ERROR(NBT_BUILD_APDU,
                                 NBT_BUILD_PASS_THROUGH_PUT_RESPONSE,
                                 IFX_OUT_OF_MEMORY);
            }
            apdu_bytes->buffer[index++] = NBT_CLA_PASS_THROUGH;
            apdu_bytes->buffer[index++] = NBT_INS_PASS_THROUGH_PUT_RESPONSE;
            apdu_bytes->buffer[index++] =
                NBT_PT_PUT_RESP_RFU; // RFU byte of pass-through put response
            // apdu
            IFX_GET_UPPER_BYTE(apdu_bytes->buffer[index++],
                               encoded_response_data_len);
            IFX_GET_LOWER_BYTE(apdu_bytes->buffer[index++],
                               encoded_response_data_len);
            IFX_MEMCPY(&apdu_bytes->buffer[index], encoded_response_data,
                       encoded_response_data_len);
        }
        else
        {
            status =
                IFX_ERROR(NBT_BUILD_APDU, NBT_BUILD_PASS_THROUGH_PUT_RESPONSE,
                          IFX_TOO_LITTLE_DATA);
        }
    }

    if ((NULL != encoded_response_data) && (encoded_response_data_len > 0))
    {
        IFX_FREE(encoded_response_data);
        encoded_response_data = NULL;
        encoded_response_data_len = 0;
    }
    return status;
}
