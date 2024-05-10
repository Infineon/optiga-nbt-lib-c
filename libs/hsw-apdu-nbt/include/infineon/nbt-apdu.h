// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/nbt-apdu.h
 * \brief NBT protocol API for exchanging APDUs with NBT product.
 */
#ifndef NBT_APDU_H
#define NBT_APDU_H

#include "infineon/ifx-apdu-error.h"
#include "infineon/ifx-apdu-protocol.h"
#include "infineon/ifx-apdu.h"
#include "infineon/ifx-protocol.h"
#include "infineon/nbt-apdu-lib.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Identifier for command set initialize
 */
#define NBT_INIT             UINT8_C(0x01)

/**
 * \brief String used as source information for logging.
 */
#define NBT_CMD_LOG_TAG      "NBT_COMMAND"

/**
 * \brief NBT_CLA type for NBT command: General.
 */
#define NBT_CLA              UINT8_C(0x00)

/**
 * \brief NBT_CLA type for NBT commands: pass-through fetch data and put
 * response.
 */
#define NBT_CLA_PASS_THROUGH UINT8_C(0x38)

/**
 * \brief INS type for NBT command: Select file.
 */
#define NBT_INS_SELECT       UINT8_C(0xA4)

/**
 * \brief The reference control parameter P1 for NBT command: Wherever 00 is
 * passed.
 */
#define NBT_P1_DEFAULT       UINT8_C(0x00)

/**
 * \brief The reference control parameter P2 for NBT command: Wherever 00 is
 * passed.
 */
#define NBT_P2_DEFAULT       UINT8_C(0x00)

/**
 * \brief The reference control parameter P1 for NBT command: Select by DF
 * name.
 */
#define NBT_P1_SELECT_BY_DF  UINT8_C(0x04)

/**
 * \brief The command expected length Le for NBT command: if expected length
 * is absent.
 */
#define NBT_LE_NONE          UINT8_C(0x00)

/**
 * \brief Generic NBT command set structure for building and performing NBT
 * commands.
 */
typedef struct
{
    /**
     * \brief Private base layer in ISO/OSI stack
     * \details Set by the initialization function of the concrete
     * implementation. Do **NOT** set manually!.
     */
    ifx_protocol_t *protocol;

    /**
     * \brief Private member for optional Logger
     * \details Might be \c NULL.
     */
    ifx_logger_t *logger;

    /**
     * \brief Private member holds the command-APDU.
     */
    ifx_apdu_t *apdu;

    /**
     * \brief Private member holds the response-APDU.
     */
    ifx_apdu_response_t *response;

    /**
     * \brief Private member holds an APDU error message map.
     */
    ifx_apdu_error_map_t *apdu_error_map_list;

    /**
     * \brief Private member holds an APDU error message map length.
     */
    uint8_t apdu_error_map_list_length;
} nbt_cmd_t;

/**
 * \brief Initializes NBT command set object by setting protocol members to
 * valid values.
 *
 * \details This function is for NBT command set stack developers to start from
 * a clean base, when initializing the command set. It initializes the given NBT
 * command set structure to start with NBT command set.
 *
 * \param[in] self NBT command set object to be initialized.
 * \param[in] protocol Protocol to handle the communication with NBT tag.
 * \param[in] logger Logger to handle the logging the logs.
 * \return ifx_status_t
 * \retval IFX_SUCCESS : If API operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t nbt_initialize(nbt_cmd_t *self, ifx_protocol_t *protocol,
                            ifx_logger_t *logger);

/**
 * \brief Releases the memory associated with NBT command set object (but not
 * object itself).
 *
 * \param[in] self NBT command set object whose data shall be released.
 */
void nbt_destroy(nbt_cmd_t *self);

/**
 * \brief Returns the error message for last command executed by NBT command
 * set.
 * \param[in] self NBT command set object
 * \return uint8_t \c pointer to error message
 */
uint8_t *nbt_error_message_get(const nbt_cmd_t *self);

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* NBT_APDU_H */
