// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/ifx-ndef-bp-lib.h
 * \brief Stores the module details and function identifiers
 */
#ifndef IFX_NDEF_BP_LIB_H
#define IFX_NDEF_BP_LIB_H

#include <stddef.h>
#include <stdint.h>

#include "infineon/ifx-error.h"
#include "infineon/ifx-utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Brand protection NDEF library module ID.
 */
#define LIB_NDEF_BP UINT8_C(0x36)

/**
 * \brief Enumeration defines the NDEF library module IDs.
 */
typedef enum
{
    /**
     * \brief NDEF brandprotection record module ID
     */
    IFX_RECORD_HANDLER_BP = IFX_MODULE(LIB_NDEF_BP, 0x01),

    /**
     * \brief NDEF model brandprotection record module ID
     */
    IFX_RECORD_BP
} ifx_record_bp_module_id;

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* IFX_NDEF_BP_LIB_H */