// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/ifx-ndef-errors.h
 * \brief Contains the error identifier codes of NDEF library.
 */
#ifndef IFX_NDEF_ERRORS_H
#define IFX_NDEF_ERRORS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Macros defines custom error reason codes */

/**
 * \brief Invalid Identifier Code status
 */
#define IFX_RECORD_URI_IDENTIFIER_CODE_INVALID UINT8_C(0x01)

/**
 * \brief Invalid Identifier bytes status
 */
#define IFX_RECORD_URI_IDENTIFIER_INVALID      UINT8_C(0x02)

/**
 * \brief Invalid record
 */
#define IFX_RECORD_INVALID                     UINT8_C(0x03)

/**
 * \brief Info id for already registered record in record init handler list
 */
#define IFX_RECORD_INFO_ALREADY_REGISTERED     UINT8_C(0x04)

/**
 * \brief Error code, Unsupported record type while retrieving record
 */
#define IFX_RECORD_UNSUPPORTED                 UINT8_C(0x05)

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* IFX_NDEF_ERRORS_H */
