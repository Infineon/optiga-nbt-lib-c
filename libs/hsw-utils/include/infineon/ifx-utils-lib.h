// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/ifx-utils-lib.h
 * \brief Provides the module, method and error identifiers of the library.
 */
#ifndef IFX_UTILS_LIB_H
#define IFX_UTILS_LIB_H

#include "infineon/ifx-error.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Utils library module ID */
#define LIB_UTILS UINT8_C(0x43)

/**
 * \brief Enumeration defines the utils module IDs.
 */
typedef enum
{
    /**
     * \brief TLV module ID.
     */
    IFX_TLV = IFX_MODULE(LIB_UTILS, 0x01),

    /**
     * \brief Utils module ID.
     */
    IFX_UTILS
} ifx_utils_module_id;

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* IFX_UTILS_LIB_H */
