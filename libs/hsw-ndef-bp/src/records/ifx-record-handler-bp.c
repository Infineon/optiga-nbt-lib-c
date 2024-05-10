// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file records/ifx-record-handler-bp.c
 * \brief Brand protection record encoding/decoding utility.
 */
#include <stdlib.h>
#include "ifx-record-handler-bp.h"
#include "infineon/ifx-record-handler.h"

/* public functions */
/**
 * \brief Encodes the brand protection record data into payload bytes.
 *
 * \param[in]   record_details      Pointer to brand protection record data (
 * \ref ifx_record_bp_t )
 * \param[out]  payload             Pointer to payload byte array of the brand
 * protection record
 * \param[out]  payload_length      Pointer to payload length of the brand
 * protection record
 * \return ifx_status_t
 * \retval IFX_SUCCESS If encode operation is successful
 * \retval IFX_OUT_OF_MEMORY If memory allocation fails
 * \retval IFX_ILLEGAL_ARGUMENT If input parameters are NULL or invalid
 */
ifx_status_t record_handler_bp_encode(const void *record_details,
                                      uint8_t **payload,
                                      uint32_t *payload_length)
{
    if ((NULL == record_details) || (NULL == payload) ||
        (NULL == payload_length))
    {
        return IFX_ERROR(IFX_RECORD_HANDLER_BP, IFX_RECORD_HANDLER_BP_ENCODE,
                         IFX_ILLEGAL_ARGUMENT);
    }

    ifx_record_bp_t *brandprotection_rec = (ifx_record_bp_t *) record_details;

    if (NULL == brandprotection_rec->payload)
    {
        return IFX_ERROR(IFX_RECORD_HANDLER_BP, IFX_RECORD_HANDLER_BP_ENCODE,
                         IFX_INVALID_STATE);
    }

    /* Storing brand protection record payload data. */
    *payload = (uint8_t *) malloc(brandprotection_rec->payload->length);
    if (NULL == *payload)
    {
        return IFX_ERROR(IFX_RECORD_HANDLER_BP, IFX_RECORD_HANDLER_BP_ENCODE,
                         IFX_OUT_OF_MEMORY);
    }

    IFX_MEMCPY(*payload, brandprotection_rec->payload->buffer,
               brandprotection_rec->payload->length);
    *payload_length = brandprotection_rec->payload->length;

    return IFX_SUCCESS;
}

/**
 * \brief Decodes the record payload bytes to brand protection record details.
 *
 * \param[in]   payload             Pointer to payload byte array of the brand
 * protection record
 * \param[in]   payload_length      Pointer to payload length of the brand
 * protection record
 * \param[out]  record_details      Pointer to brand protection record data (
 * \ref ifx_record_bp_t ) where the decoded payload will be copied
 * \return ifx_status_t
 * \retval IFX_SUCCESS If decode operation is successful
 * \retval IFX_OUT_OF_MEMORY If memory allocation fails
 * \retval IFX_ILLEGAL_ARGUMENT If input parameters are NULL or invalid
 */
ifx_status_t record_handler_bp_decode(const uint8_t *payload,
                                      uint32_t payload_length,
                                      void *record_details)
{
    if ((NULL == record_details) || (NULL == payload))
    {
        return IFX_ERROR(IFX_RECORD_HANDLER_BP, IFX_RECORD_HANDLER_BP_DECODE,
                         IFX_ILLEGAL_ARGUMENT);
    }

    ifx_record_bp_t *brandprotection_rec = (ifx_record_bp_t *) record_details;

    /* Storing payload data to brandprotection record. */
    brandprotection_rec->payload = (ifx_blob_t *) malloc(sizeof(ifx_blob_t));
    if (NULL == brandprotection_rec->payload)
    {
        return IFX_ERROR(IFX_RECORD_HANDLER_BP, IFX_RECORD_HANDLER_BP_DECODE,
                         IFX_OUT_OF_MEMORY);
    }

    brandprotection_rec->payload->buffer = (uint8_t *) malloc(payload_length);
    if (NULL == brandprotection_rec->payload->buffer)
    {
        IFX_FREE(brandprotection_rec->payload);
        brandprotection_rec->payload = NULL;
        return IFX_ERROR(IFX_RECORD_HANDLER_BP, IFX_RECORD_HANDLER_BP_DECODE,
                         IFX_OUT_OF_MEMORY);
    }

    IFX_MEMCPY(brandprotection_rec->payload->buffer, payload, payload_length);
    brandprotection_rec->payload->length = payload_length;

    return IFX_SUCCESS;
}
