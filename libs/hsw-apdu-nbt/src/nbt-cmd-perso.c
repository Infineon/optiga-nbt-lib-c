// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file nbt-cmd-perso.c
 * \brief Collection of the NBT personalization commands.
 */
#include "infineon/nbt-cmd-perso.h"

#include "infineon/ifx-utils.h"
#include "infineon/nbt-apdu-lib.h"
#include "nbt-build-apdu-perso.h"

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
