// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file include/nbt-build-apdu-perso.h
 * \brief Provides the implementation of command builder function for
 * personalize data and finalize the personalization command.
 */
#ifndef NBT_BUILD_APDU_PERSO_H
#define NBT_BUILD_APDU_PERSO_H

#include "infineon/ifx-utils.h"
#include "infineon/nbt-apdu-lib.h"
#include "infineon/nbt-apdu.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Function identifiers */

/**
 * \brief Identifier for command builder finalize personalization
 */
#define NBT_BUILD_FINALIZE_PERSONALIZATION UINT8_C(0x01)

/**
 * \brief Identifier for command builder personalize data
 */
#define NBT_BUILD_PERSONALIZE_DATA         UINT8_C(0x02)

/**
 * \brief Identifier for command builder to enable backend test
 */
#define NBT_BUILD_BACKEND_TEST             UINT8_C(0x03)

/**
 * \brief Identifier for command builder to disable backend test
 */
#define NBT_BUILD_BACKEND_TEST_DISABLE     UINT8_C(0x04)

/**
 * \brief INS type for NBT command: Personalize data.
 */
#define NBT_INS_PERSO_DATA                 UINT8_C(0xE2)

/**
 * \brief INS type for NBT command: Backend test enable/disable.
 */
#define NBT_INS_BACKEND_TEST               UINT8_C(0xBE)

/**
 * \brief P1 for NBT command: Backend test disable.
 */
#define NBT_P1_DISABLE_BACKEND_TEST        UINT8_C(0xFF)

/**
 * \brief DGI value of finalize personalization command data.
 */
#define NBT_DGI_FINALIZE_PERSO             UINT16_C(0xBF63)

/**
 * \brief Finalize personalization APDU command data length Lc value.
 */
#define NBT_LC_FINALIZE_PERSO              UINT8_C(0x00)

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
                                    ifx_apdu_t *apdu);

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
ifx_status_t build_finalize_personalization(ifx_apdu_t *apdu);

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
 * | Bit 1 | ECDSA sign|
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
 *
 * \param[out] apdu            APDU reference to enable the given backend tests.
 * \return                     ifx_status_t IFX_SUCCESS if successful.
 *                             Error status IFX_ILLEGAL_ARGUMENT if function
 * parameter is NULL.
 */
ifx_status_t build_backend_test(uint8_t test_request, ifx_apdu_t *apdu);

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
ifx_status_t build_backend_test_disable(ifx_apdu_t *apdu);

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* NBT_BUILD_APDU_PERSO_H */
