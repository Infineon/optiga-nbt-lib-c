// SPDX-FileCopyrightText: Copyright (c) 2024-2026 Infineon Technologies AG
// SPDX-License-Identifier: MIT

/**
 * \file nbt-cmd-perso.c
 * \brief Collection of the NBT personalization commands.
 */
#include "infineon/nbt-cmd-perso.h"

#include "infineon/ifx-utils.h"
#include "infineon/nbt-apdu-lib.h"
#include "infineon/nbt-cmd.h"
#include "infineon/ifx-tlv.h"
#include "nbt-build-apdu-perso.h"

/**
 * \brief Builds a single personalize data APDU for the given DGI and its data
 * field, sends it to the secure element, and stores the response.
 *
 * \details Encodes the personalize data command via build_personalize_data()
 * and, on success, exchanges it using the APDU protocol layer. Any build or
 * transceive failure is logged and returned to the caller.
 *
 * \param[in,out] self      Command set with communication protocol and
 * response.
 * \param[in] dgi           Data group identifier (DGI) value.
 * \param[in] dgi_data      Pointer to the data field of the respective DGI.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If any memory allocation fails
 */
static ifx_status_t nbt_personalize_data_transceive(nbt_cmd_t *self,
                                                    uint16_t dgi,
                                                    const ifx_blob_t *dgi_data)
{
    ifx_status_t status = build_personalize_data(dgi, dgi_data, self->apdu);
    if (ifx_error_check(status))
    {
        NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                     "build_personalize_data unable to build command");
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
 * \brief Issues multiple personalize data commands to personalize for data not
 * fitting in a single APDU command. This function is used for personalizing
 * data with variable length (e.g., NDEF file content) that exceeds the maximum
 * allowed length for a single APDU command.
 *
 * \param[in,out] self  Command set with communication protocol and response.
 * \param[in] dgi                Data group identifier (DGI) value
 * \param[in] dgi_data           Pointer to the data field of the respective
 * DGI. (Example: AES COTT Key, ECC Key, and NDEF file content)
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If any memory allocation fails
 */
static ifx_status_t nbt_personalize_data_chained(nbt_cmd_t *self, uint16_t dgi,
                                                 const ifx_blob_t *dgi_data)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(self) ||
        IFX_VALIDATE_NULL_PTR_MEMORY(dgi_data))
    {
        return IFX_ERROR(NBT_CMD_PERSO, NBT_PERSONALIZE_DATA,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    ifx_status_t status = IFX_SUCCESS;

    // Offset value as specified in the file data
    uint16_t file_offset = 0;
    IFX_READ_U16(dgi_data->buffer, file_offset);

    size_t transmission_offset = 2; // Skip the offset bytes in the data buffer
    const size_t header_length =
        IFX_TLV_DGI_LEN_WITH_ID_SIZE + 2; // TLV header + offset bytes

    ifx_blob_t dgi_data_chunk;
    dgi_data_chunk.buffer = malloc(NBT_MAX_LC);
    dgi_data_chunk.length = NBT_MAX_LC;

    size_t remaining_length = dgi_data->length - transmission_offset;

    if (IFX_VALIDATE_NULL_PTR_MEMORY(dgi_data_chunk.buffer))
    {
        return IFX_ERROR(NBT_CMD_PERSO, NBT_PERSONALIZE_DATA,
                         IFX_OUT_OF_MEMORY);
    }

    while (remaining_length > 0)
    {
        size_t chunk_size = (remaining_length + header_length > NBT_MAX_LC)
                                ? (NBT_MAX_LC - header_length)
                                : remaining_length;

        IFX_UPDATE_U16(dgi_data_chunk.buffer, file_offset);
        memcpy(&dgi_data_chunk.buffer[2],
               dgi_data->buffer + transmission_offset, chunk_size);
        dgi_data_chunk.length =
            chunk_size + 2; // Include the offset bytes in the length
        transmission_offset += chunk_size;

        file_offset += chunk_size;
        remaining_length -= chunk_size;

        status = nbt_personalize_data_transceive(self, dgi, &dgi_data_chunk);

        if (ifx_error_check(status) || !IFX_CHECK_SW_OK(self->response->sw))
        {
            ifx_apdu_destroy(self->apdu);
            break;
        }

        // Keep response for last chunk
        if (remaining_length > 0)
        {
            ifx_apdu_destroy(self->apdu);
            ifx_apdu_response_destroy(self->response);
        }
    }

    IFX_FREE(dgi_data_chunk.buffer);
    return status;
}
/**
 * \brief Issues finalize personalization command. Personalization is considered
 * completed after successful execution of finalize personalization.
 * On successful processing of this command, the applet transitions from the
 * 'personalization' state to the 'operational state'.
 * Refer the PERSONALIZE_DATA command section in the datasheet to know
 * the possible DGI and the data fields to be used.
 *
 * \param[in,out] self Command set with communication protocol and response.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 */
ifx_status_t nbt_finalize_personalization(nbt_cmd_t *self)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(self))
    {
        return IFX_ERROR(NBT_CMD_PERSO, NBT_FINALIZE_PERSONALIZATION,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    ifx_status_t status = build_finalize_personalization(self->apdu);
    if (ifx_error_check(status))
    {
        NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                     "build_finalize_personalization unable to build command");
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
 * \brief Issues personalize data command to personalize the data elements of
 * the applet. Applet personalization consists of storing data in the applet as
 * standard elementary files and personalizing proprietary key data objects
 * using the personalize data command.
 *
 * \param[in,out] self  Command set with communication protocol and response.
 * \param[in] dgi                Data group identifier (DGI) value
 * \param[in] dgi_data           Pointer to the data field of the respective
 * DGI. (Example: AES COTT Key, ECC Key, and NDEF file content)
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 */
ifx_status_t nbt_personalize_data(nbt_cmd_t *self, uint16_t dgi,
                                  const ifx_blob_t *dgi_data)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(self))
    {
        return IFX_ERROR(NBT_CMD_PERSO, NBT_PERSONALIZE_DATA,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    if (dgi_data == NULL || dgi_data->buffer == NULL || dgi_data->length == 0)
    {
        return IFX_ERROR(NBT_CMD_PERSO, NBT_PERSONALIZE_DATA,
                         IFX_ILLEGAL_ARGUMENT);
    }

    if (dgi_data->length + IFX_TLV_DGI_LEN_WITH_ID_SIZE > NBT_MAX_LC)
    {
        // Variable data length only allowed for specific DGI values
        if (dgi != NBT_DGI_E104 && (dgi < NBT_DGI_E1A1 || dgi > NBT_DGI_E1A4))
        {
            return IFX_ERROR(NBT_CMD_PERSO, NBT_PERSONALIZE_DATA,
                             IFX_ILLEGAL_ARGUMENT);
        }

        return nbt_personalize_data_chained(self, dgi, dgi_data);
    }

    return nbt_personalize_data_transceive(self, dgi, dgi_data);
}

/**
 * \brief Issues backend test command to perform the requested backend tests.
 *
 * \param[in,out] self  Command set with communication protocol and response.
 * \param[in] test_request  8-bit field to request the backend tests to perform.
 *
 * \note Set respective bit(s) of test_request parameter to request which
 * test(s) to perform
 *
 * | Bit 0 | Random number generation |
 * | Bit 1 | ECDSA sign |
 * | Bit 2 | Public key extraction from certificate |
 * | Bit 3 | ECDSA verify |
 * | Bit 4 | UID extraction from certificate |
 * | Bit 5 | UID comparison |
 * | Bit 6 | COTT computation |
 * | Bit 7 | RFU (ignored) |
 *
 * For example, to run ECDSA sign and ECDSA verify tests, set test_request
 * variable to NBT_BACKEND_TEST_ECDSA_SIGN|NBT_BACKEND_TEST_ECDSA_VERIFY.
 * Use bit map combinations from #nbt_backend_test_request to request specific
 * backend tests. 7F(OR combination of all 7 tests) is the value of the
 * test_request_bitmap to request all tests to be performed and the expected
 * returned test_result_bitmap is 7F, i.e. is all tests performed successfully.
 *
 * C-APDU: 00 BE 00 7F (Enable all tests)
 * Expected R-APDU: 90 00 7F (7F is returned test_result bitmap in response data
 * if all tests are performed successfully
 *
 * If the backend test command has been deactivated, response as (NBT_CLA/INS)
 * does not exist is sent. This command is not needed on NFC interface, as the
 * back end testing is performed via I2C
 * \return ifx_status_t
 *
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If any memory allocation fails
 */
ifx_status_t nbt_backend_test(nbt_cmd_t *self, uint8_t test_request)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(self))
    {
        return IFX_ERROR(NBT_CMD_PERSO, NBT_BACKEND_TEST, IFX_ILLEGAL_ARGUMENT);
    }
#endif
    ifx_status_t status = build_backend_test(test_request, self->apdu);
    if (ifx_error_check(status))
    {
        NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                     "build_backend_test unable to build command");
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
 * \brief Issues backend test command that permanently disables the backend
 * test support in the chip.
 *
 * \warning This command permanently disable the backend test support and
 * cannot be enabled again.
 * \param[in,out] self  Command set with communication protocol and response.
 * \return ifx_status_t
 *
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If any memory allocation fails
 */
ifx_status_t nbt_backend_test_disable(nbt_cmd_t *self)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(self))
    {
        return IFX_ERROR(NBT_CMD_PERSO, NBT_BACKEND_TEST_DISABLE,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif
    ifx_status_t status = build_backend_test_disable(self->apdu);
    if (ifx_error_check(status))
    {
        NBT_APDU_LOG(self->logger, NBT_CMD_LOG_TAG, IFX_LOG_ERROR,
                     "build_backend_test_disable unable to build command");
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
