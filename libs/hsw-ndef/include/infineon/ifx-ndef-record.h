// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/ifx-ndef-record.h
 * \brief Model interface to create record types and set/get record fields.
 */
#ifndef IFX_NDEF_RECORD_H
#define IFX_NDEF_RECORD_H

#include <stddef.h>
#include <stdint.h>
#include "infineon/ifx-error.h"
#include "infineon/ifx-utils.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Macro definitions */
/* Definitions of function identifiers */

/**
 * \brief Identifier for model set data ID
 */
#define IFX_RECORD_SET                 UINT8_C(0x01)

/**
 * \brief Identifier for model get data ID
 */
#define IFX_RECORD_GET                 UINT8_C(0x02)

/**
 * \brief Identifier for model register record ID
 */
#define IFX_RECORD_REGISTER            UINT8_C(0x03)

/**
 * \brief Identifier for model retrieve record ID
 */
#define IFX_RECORD_RETRIEVE            UINT8_C(0x04)

/**
 * \brief Identifier for model deregister record ID
 */
#define IFX_RECORD_DEREGISTER          UINT8_C(0x05)

/**
 * \brief Error id for unavailable data field in record handle
 */
#define IFX_RECORD_DATA_FIELD_NA       UINT8_C(0xF1)

/**
 * \brief Mask value to extract SR bit in header
 */
#define IFX_RECORD_HEADER_MASK_SR_FLAG UINT8_C(0x10)

/**
 * \brief Mask value to extract ID bit in header
 */
#define IFX_RECORD_HEADER_MASK_ID_FLAG UINT8_C(0x08)

/**
 * \brief Mask value of TNF field in header
 */
#define IFX_RECORD_TNF_MASK            UINT8_C(0x27)

/**
 * \brief TNF of known NDEF records such as URI, Text, Smart poster
 */
#define IFX_RECORD_TNF_TYPE_KNOWN      UINT8_C(0x01)

/**
 * \brief TNF of Media NDEF record types
 */
#define IFX_RECORD_TNF_TYPE_MEDIA      UINT8_C(0x02)

/**
 * \brief TNF of External NDEF record types
 */
#define IFX_RECORD_TNF_TYPE_EXT        UINT8_C(0x04)

/* enum definitions */

/**
 * \brief Defines list of available record types
 */
typedef enum
{
    IFX_RECORD_TYPE_URI,             /**< URI record */
    IFX_RECORD_TYPE_HANDOVER_SELECT, /**< Handover select record*/
    IFX_RECORD_TYPE_ALT_CARRIER,     /**< Alternative carrier record  */
    IFX_RECORD_TYPE_BT,              /**< Bluetooth record */
    IFX_RECORD_TYPE_BLE,             /**< Bluetooth low energy record*/
    IFX_RECORD_TYPE_ERROR,           /**< Error record */
    IFX_RECORD_MAX                   /**< Record Maximum */
} ifx_ndef_record_type;

/**
 * \brief Data storage for NDEF Records
 */
typedef struct
{
    uint8_t tnf;         /**< Type Name Format value for NDEF records*/
    uint8_t type_length; /**< Length of the type of the NDEF record */
    uint32_t
        payload_length; /**< Length of the actual NDEF record data (payload) */
    uint8_t id_length;  /**< Length of the id field data */
    uint8_t *type;      /**< Actual type data */
    uint8_t *id;        /**< Actual id field data */
    uint8_t
        *payload; /**< Actual NDEF record payload data (might be \c NULL ) */
} ifx_ndef_record_t;

/* function prototype declarations */

/**
 * \brief Function prototype declaration for specific record encode operation
 * \param[in]   record_details  pointer to details of respective record
 * \param[out]  payload         pointer to encoded payload
 * \param[out]  payload_length  pointer to encoded payload length
 * \return      ifx_status_t \c IFX_SUCCESS, if encode successful,
 *                              error information in case of error.
 */
typedef ifx_status_t (*ifx_record_encoder_t)(const void *record_details,
                                             uint8_t **payload,
                                             uint32_t *payload_length);

/**
 * \brief Function prototype declaration for specific record decode operation
 * \param[in]  payload         pointer to payload to be decoded
 * \param[in]  payload_length  pointer to payload length
 * \param[out] record_details  pointer to details of respective record
 * \return      ifx_status_t \c IFX_SUCCESS, if encode successful,
 *                              error information in case of error.
 */
typedef ifx_status_t (*ifx_record_decoder_t)(const uint8_t *payload,
                                             uint32_t payload_length,
                                             void *record_details);

/**
 * \brief Function prototype declaration for specific record's data release
 * memory operation
 * \param[in] record_data        Pointer to the record data field of the record
 * handle.
 * \return  ifx_status_t \c IFX_SUCCESS, if memory release successful,
 *                              error information in case of error.
 */
typedef ifx_status_t (*ifx_record_deinit_t)(void *record_data);

/* Structure definitions */

/**
 * \brief Defines the handle for specific record type
 */
typedef struct
{
    uint8_t tnf;     /**< Type Name Forma field of specific record*/
    ifx_blob_t id;   /**< Record id field */
    ifx_blob_t type; /**< Record type field */
    ifx_record_encoder_t
        encode_record; /**< Map to specific record encode function */
    ifx_record_decoder_t
        decode_record; /**< Map to specific record decode function */
    ifx_record_deinit_t
        deinit_record; /**< Map to specific record memory release function */
    void *record_data; /**< Pointer to specific record details */
} ifx_record_handle_t;

/**
 * \brief Function prototype declarations that map to creation of new record
 * handle APIs
 */
typedef ifx_status_t (*ifx_record_init_handler_t)(ifx_record_handle_t *handle);

/**
 * \brief This Structure holds record's type information and function pointer
 * which retrieves record handle based on type.
 */
typedef struct
{
    uint8_t *type;        /**< Record type data */
    uint32_t type_length; /**< Record type data length */
    ifx_record_init_handler_t
        get_handle; /**< Function pointer to map model_new_record API()*/
} ifx_record_init_t;

/* public functions */

/**
 * \brief Sets the record ID to the handle of record.
 * \param[out] handle       Pointer to the record handle.
 * \param[in] record_id            Pointer to the record ID.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the record ID is set to the handle successfully
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t ifx_ndef_record_set_record_id(ifx_record_handle_t *handle,
                                           const ifx_blob_t *record_id);

/**
 * \brief Gets the record ID from the handle of record.
 * \param[in] handle        Pointer to the record handle.
 * \param[out] record_id           Pointer to the record ID.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the record ID is read from the handle successfully
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t ifx_ndef_record_get_record_id(const ifx_record_handle_t *handle,
                                           ifx_blob_t *record_id);

/**
 * \brief Retrieves the record handle for the given type of record type
 * information.
 * \param[in] tnf              Type name format (TNF) value of the record.
 * \param[in] type             Pointer to the record type data.
 * \param[in] type_length      Record type data length
 * \param[out] handle          Record handle that will be retrieved through the
 *                             type information.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If handler is retrieved properly
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t ifx_ndef_record_retrieve_handle(uint8_t tnf, const uint8_t *type,
                                             uint32_t type_length,
                                             ifx_record_handle_t *handle);

/**
 * \brief Register a new record service to the record init handler list based on
 * the type data.
 *
 * \param[in] init_handler  Handler that holds the type and handle retrieval
 * APIs.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If init handler is registered properly to the record init
 * handler list ER_INFO_ALREADY_REGISTERED_RECORD if already same record type is
 * registered
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 *
 * \note Use this API for registering the record service whose type value is
 * known and fixed.
 */
ifx_status_t ifx_ndef_record_register_handle(ifx_record_init_t *init_handler);

/**
 * \brief This method will free-up the internally allocated memory for ndef
 * registered records.
 *
 * \return ifx_status_t
 * \retval IFX_SUCCESS if freeing up of memory is done successfully
 */
ifx_status_t ifx_ndef_record_release_resource(void);

/**
 * \brief This method will free-up the internally allocated memory of a record.
 *
 * \param[in] record_handle Pointer to the record handle.
 * \return ifx_status_t
 * \retval IFX_SUCCESS if releasing memory resources is done successfully
 */
ifx_status_t ifx_ndef_record_dispose(ifx_record_handle_t *record_handle);

/**
 * \brief  This method will free-up the internally allocated memory for the list
 * of records.
 *
 * \param[in] record_handles Pointer to the array of NDEF record handles.
 * \param[in] number_of_records Number of NDEF records
 * \return ifx_status_t
 * \retval IFX_SUCCESS if releasing memory resources is done successfully
 */
ifx_status_t ifx_ndef_record_dispose_list(ifx_record_handle_t *record_handles,
                                          uint32_t number_of_records);

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* IFX_NDEF_RECORD_H */
