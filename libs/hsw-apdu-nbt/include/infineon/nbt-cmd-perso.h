// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/nbt-cmd-perso.h
 * \brief Collection of the NBT personalization commands.
 */
#ifndef NBT_CMD_PERSO_H
#define NBT_CMD_PERSO_H

#include "infineon/ifx-utils.h"
#include "infineon/nbt-apdu-lib.h"
#include "infineon/nbt-apdu.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Function identifiers */

/**
 * \brief Identifier for command finalize personalization
 */
#define NBT_FINALIZE_PERSONALIZATION UINT8_C(0x01)

/**
 * \brief Identifier for command personalize data
 */
#define NBT_PERSONALIZE_DATA         UINT8_C(0x02)

/**
 * \brief Identifier for command to perform backend test
 */
#define NBT_BACKEND_TEST             UINT8_C(0x03)

/**
 * \brief Identifier for command to disable backend test
 */
#define NBT_BACKEND_TEST_DISABLE     UINT8_C(0x04)

/**
 * \brief Enumeration stores data group identifier (DGI) for personalize data
 * command.
 */
typedef enum
{
    /* DGI for personalize AES COTT key */
    NBT_DGI_A001 = UINT16_C(0xA001),

    /* DGI for personalize ECC key */
    NBT_DGI_A002 = UINT16_C(0xA002),

    /* DGI for personalize password data */
    NBT_DGI_A003 = UINT16_C(0xA003),

    /* DGI for personalize NDEF file content */
    NBT_DGI_E104 = UINT16_C(0xE104),

    /* DGI for personalize proprietary files (E1A1) content */
    NBT_DGI_E1A1 = UINT16_C(0xE1A1),

    /* DGI for personalize proprietary files (E1A2) content */
    NBT_DGI_E1A2 = UINT16_C(0xE1A2),

    /* DGI for personalize proprietary files (E1A3) content */
    NBT_DGI_E1A3 = UINT16_C(0xE1A3),

    /* DGI for personalize proprietary files (E1A4) content */
    NBT_DGI_E1A4 = UINT16_C(0xE1A4),

    /* DGI for personalize configuration data (File access policy) */
    NBT_DGI_E1AF = UINT16_C(0xE1AF),

    /* DGI for finalize personalization command */
    NBT_DGI_BF63 = UINT16_C(0xBF63)
} nbt_personalize_data_dgi;

/**
 * \brief Enumeration holds the bitmaps to perform backend tests
 */
typedef enum
{
    /* Backend Test - Random number generation */
    NBT_BACKEND_TEST_RANDOM_NUM_GENERATION = (1 << 0),

    /* Backend Test - ECDSA sign */
    NBT_BACKEND_TEST_ECDSA_SIGN = (1 << 1),

    /* Backend Test - Public key extraction from certificate */
    NBT_BACKEND_TEST_PUBLIC_KEY_EXTRACTION_FROM_CERT = (1 << 2),

    /* Backend Test - ECDSA verification */
    NBT_BACKEND_TEST_ECDSA_VERIFY = (1 << 3),

    /* Backend Test - UID Extraction from Certificate */
    NBT_BACKEND_TEST_UID_EXTRACTION_FROM_CERT = (1 << 4),

    /* Backend Test - UID comparison */
    NBT_BACKEND_TEST_UID_COMPARISON = (1 << 5),

    /* Backend Test - COTT computation */
    NBT_BACKEND_TEST_COTT_COMPUTATION = (1 << 6),

    /* Backend Test - Enable all */
    NBT_BACKEND_TEST_ALL = 0x7F,
} nbt_backend_test_request;

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
ifx_status_t nbt_finalize_personalization(nbt_cmd_t *self);

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
                                  const ifx_blob_t *dgi_data);
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
 * back end testing is performed via I2C.
 * \return ifx_status_t
 *
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If any memory allocation fails
 */
ifx_status_t nbt_backend_test(nbt_cmd_t *self, uint8_t test_request);

/**
 * \brief Issues backend test command that permanently disables the backend
 * test support in the chip.
 *
 * \warning This command permanently disable the backend test support and cannot
 * be enabled again.
 *
 * \param[in,out] self  Command set with communication protocol and response.
 * \return ifx_status_t
 *
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If any memory allocation fails
 */
ifx_status_t nbt_backend_test_disable(nbt_cmd_t *self);

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* NBT_CMD_PERSO_H */
