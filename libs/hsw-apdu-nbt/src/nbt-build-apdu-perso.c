// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file nbt-build-apdu-perso.c
 * \brief Provides the implementation of command builder function for
 * personalize data and finalize personalization command.
 */
#include "nbt-build-apdu-perso.h"

#include "infineon/ifx-tlv.h"
#include "infineon/ifx-utils.h"
#include "infineon/nbt-apdu-lib.h"
#include "infineon/nbt-cmd-perso.h"
#include "nbt-build-apdu.h"

/**
 * \brief Builds the personalize data command.
 *
 * \param[in] dgi             Data group identifier (DGI) value
 * \param[in] dgi_data        Pointer to the data field of the respective DGI.
 *                            (Example: AES COTT Key, ECC Key, and NDEF file
 * content)
 * \param[out] apdu           APDU reference to store personalize data APDU
 * command.
 * \return                    ifx_status_t IFX_SUCCESS if successful.
 *                            Error status IFX_ILLEGAL_ARGUMENT if function
 * parameter is NULL. Error status IFX_OUT_OF_MEMORY if memory allocation is
 * invalid.
 */
ifx_status_t build_personalize_data(uint16_t dgi, const ifx_blob_t *dgi_data,
                                    ifx_apdu_t *apdu)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu) ||
        (IFX_VALIDATE_NULL_PTR_BLOB(dgi_data)))
    {
        return IFX_ERROR(NBT_BUILD_APDU_PERSO, NBT_BUILD_PERSONALIZE_DATA,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    ifx_status_t status;
    ifx_blob_t encoded_personalize_command;
    ifx_tlv_t tlv_data;
    uint8_t number_of_tlv = UINT8_C(0x01);

    /* Copy the dgi value, dgi data and data length into TLV type handler */
    tlv_data.tag = dgi;
    tlv_data.length = dgi_data->length;
    tlv_data.value = (uint8_t *) malloc(dgi_data->length);
    if (IFX_VALIDATE_NULL_PTR_MEMORY(tlv_data.value))
    {
        return IFX_ERROR(NBT_BUILD_APDU_PERSO, NBT_BUILD_PERSONALIZE_DATA,
                         IFX_OUT_OF_MEMORY);
    }

    IFX_MEMCPY(tlv_data.value, dgi_data->buffer, dgi_data->length);

    apdu->cla = NBT_CLA;
    apdu->ins = NBT_INS_PERSO_DATA;
    apdu->p1 = NBT_P1_DEFAULT;
    apdu->p2 = NBT_P2_DEFAULT;
    apdu->le = NBT_LE_NONE;

    /* Encode TLV type object into byte array ( <dgi> <length> <value> ).*/
    status = ifx_tlv_dgi_encode(&tlv_data, number_of_tlv,
                                &encoded_personalize_command);
    if (ifx_error_check(status))
    {
        IFX_FREE(tlv_data.value);
        tlv_data.value = NULL;
        return status;
    }

    /* Copy encoded personalize command data ( <dgi> <length> <value> ) into
     * APDU data fields. */
    apdu->data = (uint8_t *) malloc(encoded_personalize_command.length);
    if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu->data))
    {
        IFX_FREE(tlv_data.value);
        IFX_FREE(encoded_personalize_command.buffer);
        tlv_data.value = NULL;
        encoded_personalize_command.buffer = NULL;
        return IFX_ERROR(NBT_BUILD_APDU_PERSO, NBT_BUILD_PERSONALIZE_DATA,
                         IFX_OUT_OF_MEMORY);
    }

    IFX_MEMCPY(apdu->data, encoded_personalize_command.buffer,
               encoded_personalize_command.length);
    apdu->lc = encoded_personalize_command.length;
    IFX_FREE(encoded_personalize_command.buffer);
    IFX_FREE(tlv_data.value);
    tlv_data.value = NULL;
    encoded_personalize_command.buffer = NULL;
    return IFX_SUCCESS;
}

/**
 * \brief Builds the personalize data APDU command with the DGI to finalize
 * personalization.
 *
 * \param[out] apdu            APDU reference to store personalize data finalize
 * APDU command.
 * \return                     ifx_status_t IFX_SUCCESS if successful.
 *                             Error status IFX_ILLEGAL_ARGUMENT if function
 * parameter is NULL. Error status IFX_OUT_OF_MEMORY if memory allocation is
 * invalid.
 */
ifx_status_t build_finalize_personalization(ifx_apdu_t *apdu)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu))
    {
        return IFX_ERROR(NBT_BUILD_APDU_PERSO,
                         NBT_BUILD_FINALIZE_PERSONALIZATION,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    apdu->cla = NBT_CLA;
    apdu->ins = NBT_INS_PERSO_DATA;
    apdu->p1 = NBT_P1_DEFAULT;
    apdu->p2 = NBT_P2_DEFAULT;
    apdu->lc = IFX_TLV_DGI_TAG_SIZE + IFX_TLV_DGI_LEN_SIZE_1B;
    apdu->le = NBT_LE_NONE;

    apdu->data =
        (uint8_t *) malloc(IFX_TLV_DGI_TAG_SIZE + IFX_TLV_DGI_LEN_SIZE_1B);
    if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu->data))
    {
        return IFX_ERROR(NBT_BUILD_APDU_PERSO,
                         NBT_BUILD_FINALIZE_PERSONALIZATION, IFX_OUT_OF_MEMORY);
    }

    IFX_UPDATE_U16(apdu->data, NBT_DGI_FINALIZE_PERSO);
    apdu->data[IFX_TLV_DGI_TAG_SIZE] = NBT_LC_FINALIZE_PERSO;

    return IFX_SUCCESS;
}

/**
 * \brief Builds the APDU command to perform the requested backend tests.
 *
 * \param[in] test_request      8-bit field to request the backend tests to
 * perform.
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
 * variable to NBT_BACKEND_TEST_ECDSA_SIGN|BACKEND_TEST_ECDSA_VERIFY.
 * Use bit map combinations from #backend_test_request_t to request specific
 * backend tests. 7F(OR combination of all 7 tests) is the value of the
 * test_request_bitmap to request all tests to be performed and the expected
 * returned test_result_bitmap is 7F, i.e. is all tests performed successfully.
 *
 * C-APDU: 00 BE 00 7F (Enable all tests)
 * Expected R-APDU: 90 00 7F (7F is returned test_result bitmap in response data
 * if all tests are performed successfully)
 *
 * If the backend test command has been deactivated, response as (NBT_CLA/INS)
 * does not exist is sent This command is not needed on NFC interface, as the
 * back end testing is performed via I2C
 *
 * \param[out] apdu            APDU reference to enable the given backend tests.
 * \return                     ifx_status_t IFX_SUCCESS if successful.
 *                             Error status IFX_ILLEGAL_ARGUMENT if function
 * parameter is NULL.
 */
ifx_status_t build_backend_test(uint8_t test_request, ifx_apdu_t *apdu)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu))
    {
        return IFX_ERROR(NBT_BUILD_APDU_PERSO, NBT_BUILD_BACKEND_TEST,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    apdu->cla = NBT_CLA;
    apdu->ins = NBT_INS_BACKEND_TEST;
    apdu->p1 = NBT_P1_DEFAULT;
    apdu->p2 = test_request;
    apdu->lc = NBT_LC_ABSENT;
    apdu->le = NBT_LE_ABSENT;
    apdu->data = NULL;

    return IFX_SUCCESS;
}

/**
 * \brief Builds the APDU command that permanently disables the backend
 * test support in the chip.
 *
 * \warning This command permanently disables the backend test support and
 * cannot be enabled again.
 *
 * \param[out] apdu            APDU reference to disable the backend tests.
 * \return                     ifx_status_t IFX_SUCCESS if successful.
 *                             Error status IFX_ILLEGAL_ARGUMENT if function
 * parameter is NULL.
 */
ifx_status_t build_backend_test_disable(ifx_apdu_t *apdu)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_MEMORY(apdu))
    {
        return IFX_ERROR(NBT_BUILD_APDU_PERSO, NBT_BUILD_BACKEND_TEST_DISABLE,
                         IFX_ILLEGAL_ARGUMENT);
    }
#endif

    apdu->cla = NBT_CLA;
    apdu->ins = NBT_INS_BACKEND_TEST;
    apdu->p1 = NBT_P1_DISABLE_BACKEND_TEST;
    apdu->p2 = NBT_P2_DEFAULT;
    apdu->lc = NBT_LC_ABSENT;
    apdu->le = NBT_LE_ABSENT;
    apdu->data = NULL;

    return IFX_SUCCESS;
}