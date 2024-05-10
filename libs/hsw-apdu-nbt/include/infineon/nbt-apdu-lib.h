// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/nbt-apdu-lib.h
 * \brief Provides the library and module identifiers code of the library.
 */
#ifndef NBT_APDU_LIB_H
#define NBT_APDU_LIB_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "infineon/ifx-error.h"

/**
 * @def NBT_APDU_LOG
 * @brief APDU NBT library data log macro.
 *
 * @param[in] logger_object Logger object in which logging to be done.
 * @param[in] source_information Source information from where logs are
 * originated.
 * @param[in] logger_level Log level
 * @param[in] formatter String with formatter for data passed as variable
 * argument
 *
 * It maps to the ifx_logger_log function if NBT_APDU_LOG_ENABLE macro is
 * defined other wise it defines IFX_SUCCESS.
 *
 */

/**
 * @def NBT_APDU_LOG_BYTES
 * @brief APDU NBT library array of bytes data log macro.
 *
 * @param[in] logger_object Logger object in which logging to be done.
 * @param[in] source_information Source information from where logs are
 * originated.
 * @param[in] logger_level Log level
 * @param[in] delimeter_1 Delimeter to differentiate log information and data.
 * @param[in] data Data to be log.
 * @param[in] data_len Length of the data
 * @param[in] delimeter_2 Delimeter to differentiate each data byte
 *
 * It maps to the ifx_logger_log_bytes function if NBT_APDU_LOG_ENABLE macro is
 * defined other wise it defines IFX_SUCCESS.
 *
 */

#ifdef NBT_APDU_LOG_ENABLE
#define NBT_APDU_LOG(logger_object, source_information, logger_level,          \
                     formatter, ...)                                           \
    ifx_logger_log(logger_object, source_information, logger_level, formatter, \
                   ##__VA_ARGS__)
#define NBT_APDU_LOG_BYTES(logger_object, source_information, logger_level,    \
                           delimeter_1, data, data_len, delimeter_2)           \
    ifx_logger_log_bytes(logger_object, source_information, logger_level,      \
                         delimeter_1, data, data_len, delimeter_2)
#else
#define NBT_APDU_LOG(logger_object, source_information, logger_level,          \
                     formatter, ...)

#define NBT_APDU_LOG_BYTES(logger_object, source_information, logger_level,    \
                           delimeter_1, data, data_len, delimeter_2)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** \brief apdu-nbt library module ID */
#define LIB_NBT_APDU UINT8_C(0x33)

/**
 * \brief Enumeration defines the NBT library module IDs.
 */
typedef enum
{
    /**
     * \brief NBT APDU module ID.
     */
    NBT_APDU = IFX_MODULE(LIB_NBT_APDU, 0x01),

    /**
     * \brief NBT APDU command builder module ID.
     */
    NBT_BUILD_APDU,

    /**
     * \brief NBT APDU configuration command builder module ID.
     */
    NBT_BUILD_APDU_CONFIG,

    /**
     * \brief NBT APDU personalization command builder module ID.
     */
    NBT_BUILD_APDU_PERSO,

    /**
     * \brief NBT APDU response parser module ID.
     */
    NBT_PARSE_RESP,

    /**
     * \brief NBT configuration command set module ID.
     */
    NBT_CMD_CONFIG,

    /**
     * \brief NBT operational command set module ID.
     */
    NBT_CMD,

    /**
     * \brief NBT personalization command set module ID.
     */
    NBT_CMD_PERSO
} nbt_module_id;

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* NBT_APDU_LIB_H */
