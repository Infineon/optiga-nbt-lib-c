// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/ifx-ndef-lib.h
 * \brief Provides the library and module IDs definitions of the NDEF library.
 */
#ifndef IFX_NDEF_LIB_H
#define IFX_NDEF_LIB_H

#include <stdlib.h>
#include "infineon/ifx-error.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \brief NDEF library module ID */
#define LIB_NDEF UINT8_C(0x3D)

/**
 * \brief Enumeration defines the NDEF library module IDs.
 */
typedef enum
{
    /**
     * \brief NDEF record module ID
     */
    IFX_RECORD_HANDLER = IFX_MODULE(LIB_NDEF, 0x01),

    /**
     * \brief NDEF alternate carrier record module ID
     */
    IFX_RECORD_HANDLER_AC,

    /**
     * \brief NDEF bluetooth low energy(BLE) record module ID
     */
    IFX_RECORD_HANDLER_BLE,

    /**
     * \brief NDEF bluetooth record module ID
     */
    IFX_RECORD_HANDLER_BT,

    /**
     * \brief NDEF error record module ID
     */
    IFX_RECORD_HANDLER_ERROR,

    /**
     * \brief NDEF generic record module ID
     */
    IFX_RECORD_HANDLER_GENERIC,

    /**
     * \brief NDEF handover select module ID
     */
    IFX_RECORD_HANDLER_HS,

    /**
     * \brief NDEF URI record module ID
     */
    IFX_RECORD_HANDLER_URI,

    /**
     * \brief NDEF model alternate carrier module ID
     */
    IFX_RECORD_AC,

    /**
     * \brief NDEF model bluetooth low energy(BLE) record module ID
     */
    IFX_RECORD_BLE,

    /**
     * \brief NDEF model bluetooth record module ID
     */
    IFX_RECORD_BT,

    /**
     * \brief NDEF model error record module ID
     */
    IFX_RECORD_ERROR,

    /**
     * \brief NDEF model external record module ID
     */
    IFX_RECORD_EXTERNAL,

    /**
     * \brief NDEF model handover select record module ID
     */
    IFX_RECORD_HS,

    /**
     * \brief NDEF model multipurpose internet mail extensions (MIME) record
     * module ID
     */
    IFX_RECORD_MIME,

    /**
     * \brief NDEF model URI record module ID
     */
    IFX_RECORD_URI,

    /**
     * \brief NDEF model module ID
     */
    IFX_NDEF_RECORD,

    /**
     * \brief NDEF message module ID
     */
    IFX_NDEF_MESSAGE
} ifx_ndef_module_id;

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* IFX_NDEF_LIB_H */
