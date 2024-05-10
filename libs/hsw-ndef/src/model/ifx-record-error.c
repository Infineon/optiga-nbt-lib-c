// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file model/ifx-record-error.c
 * \brief Model interface to create error record types and set/get record
 * fields.
 * \details This record is used only by the handover select and not
 * intended to use for any other records.
 */
#include "ifx-record-handler-error.h"
#include "infineon/ifx-ndef-errors.h"
#include "infineon/ifx-ndef-lib.h"
#include "infineon/ifx-ndef-record.h"
#include "infineon/ifx-record-error.h"
#include "infineon/ifx-record-handler.h"

/**
 * @brief Release all the allocated memory for the created error record data
 *
 * @param[in] record_data    data of the error record
 * \retval IFX_SUCCESS If memory release operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 */
static ifx_status_t record_error_deinit(void *record_data)
{
    if (NULL == record_data)
    {
        return IFX_ERROR(IFX_RECORD_ERROR, IFX_RECORD_ERROR_RELEASE_MEMORY,
                         IFX_ILLEGAL_ARGUMENT);
    }
    ifx_record_error_t *error_record = (ifx_record_error_t *) (record_data);
    if (error_record->error->buffer != NULL)
    {
        IFX_FREE(error_record->error->buffer);
        error_record->error->buffer = NULL;
    }
    if (error_record->error != NULL)
    {
        IFX_FREE(error_record->error);
        error_record->error = NULL;
    }
    return IFX_SUCCESS;
}

/**
 * \brief Creates an error record and handle of the created record.
 * This handle holds the values of the record needed for encode and decode
 * operations.
 * \param[out] handle    Handle of the created error record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the record creation operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation fails
 */
ifx_status_t ifx_record_error_new(ifx_record_handle_t *handle)
{
    ifx_status_t status = IFX_SUCCESS;
    if (NULL != handle)
    {
        uint8_t type[] = IFX_RECORD_ERROR_TYPE;
        handle->tnf = IFX_RECORD_TNF_TYPE_KNOWN;
        handle->type.length = sizeof(type);
        handle->type.buffer = (uint8_t *) malloc(handle->type.length);
        if (NULL != handle->type.buffer)
        {
            handle->id.buffer = NULL;
            handle->id.length = IFX_NDEF_ID_LEN_FIELD_NONE;
            IFX_MEMCPY(handle->type.buffer, type, handle->type.length);
            handle->encode_record = record_handler_error_encode;
            handle->decode_record = record_handler_error_decode;
            handle->deinit_record = record_error_deinit;

            ifx_record_error_t *error_rec =
                (ifx_record_error_t *) malloc(sizeof(ifx_record_error_t));
            if (NULL == error_rec)
            {
                IFX_FREE(handle->type.buffer);
                handle->type.buffer = NULL;
                return IFX_ERROR(IFX_RECORD_ERROR, IFX_RECORD_ERROR_SET,
                                 IFX_OUT_OF_MEMORY);
            }
            handle->record_data = (void *) error_rec;
        }
        else
        {
            status = IFX_ERROR(IFX_RECORD_ERROR, IFX_RECORD_ERROR_SET,
                               IFX_OUT_OF_MEMORY);
        }
    }
    else
    {
        status = IFX_ERROR(IFX_RECORD_ERROR, IFX_RECORD_ERROR_SET,
                           IFX_ILLEGAL_ARGUMENT);
    }

    return status;
}

/**
 * \brief Sets the error reason in the error record details for the given record
 * handle.
 * \param[out] handle      Pointer to the record handle obtained while creating
 *                         the record.
 * \param[in] error_reason Error reason that defines the failure cause of the
 *                         handover selector.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_error_set_reason(ifx_record_handle_t *handle,
                                         uint8_t error_reason)
{
    ifx_status_t status = IFX_SUCCESS;
    if (NULL == handle)
    {
        return IFX_ERROR(IFX_RECORD_ERROR, IFX_RECORD_ERROR_SET,
                         IFX_ILLEGAL_ARGUMENT);
    }
    const uint8_t type[] = IFX_RECORD_ERROR_TYPE;
    if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
    {
        ((ifx_record_error_t *) handle->record_data)->error_reason =
            error_reason;
    }
    else
    {
        status = IFX_ERROR(IFX_RECORD_ERROR, IFX_RECORD_ERROR_SET,
                           IFX_RECORD_INVALID);
    }

    return status;
}

/**
 * \brief Sets the error data in the error record details for the given record
 * handle.
 * \param[out] handle     Pointer to the record handle obtained while
 *                        creating the record.
 * \param[in] error  Pointer to error data.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_error_set_error_data(ifx_record_handle_t *handle,
                                             const ifx_blob_t *error)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == error))
    {
        return IFX_ERROR(IFX_RECORD_ERROR, IFX_RECORD_ERROR_SET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    const uint8_t type[] = IFX_RECORD_ERROR_TYPE;
    if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
    {
        ((ifx_record_error_t *) handle->record_data)->error =
            (ifx_blob_t *) malloc(sizeof(ifx_blob_t));
        if (NULL != ((ifx_record_error_t *) handle->record_data)->error)
        {
            ((ifx_record_error_t *) handle->record_data)->error->length =
                error->length;
            ((ifx_record_error_t *) handle->record_data)->error->buffer =
                (uint8_t *) malloc(error->length);
            if (NULL !=
                ((ifx_record_error_t *) handle->record_data)->error->buffer)
            {
                IFX_MEMCPY(
                    ((ifx_record_error_t *) handle->record_data)->error->buffer,
                    error->buffer, error->length);
            }
            else
            {
                IFX_FREE(((ifx_record_error_t *) handle->record_data)->error);
                ((ifx_record_error_t *) handle->record_data)->error = NULL;
                status = IFX_ERROR(IFX_RECORD_ERROR, IFX_RECORD_ERROR_SET,
                                   IFX_OUT_OF_MEMORY);
            }
        }
        else
        {
            status = IFX_ERROR(IFX_RECORD_ERROR, IFX_RECORD_ERROR_SET,
                               IFX_OUT_OF_MEMORY);
        }
    }
    else
    {
        status = IFX_ERROR(IFX_RECORD_ERROR, IFX_RECORD_ERROR_SET,
                           IFX_RECORD_INVALID);
    }

    return status;
}

/**
 * \brief Gets an error reason of an error record from the given record handle.
 * \param[in] handle        Pointer to the record handle obtained while
 *                          creating the record.
 * \param[out] error_reason Pointer to the error reason that defines
 *                          failure cause of the handover selector.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_error_get_reason(const ifx_record_handle_t *handle,
                                         uint8_t *error_reason)
{
    ifx_status_t status = IFX_SUCCESS;

    if ((NULL == handle) || (NULL == error_reason))
    {
        return IFX_ERROR(IFX_RECORD_ERROR, IFX_RECORD_ERROR_GET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    const uint8_t type[] = IFX_RECORD_ERROR_TYPE;
    if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
    {
        *error_reason =
            ((ifx_record_error_t *) handle->record_data)->error_reason;
    }
    else
    {
        status = IFX_ERROR(IFX_RECORD_ERROR, IFX_RECORD_ERROR_GET,
                           IFX_RECORD_INVALID);
    }

    return status;
}

/**
 * \brief Gets an error data of an error record from the given record handle.
 * \param[in] handle            Pointer to the record handle obtained while
 * creating the record.
 * \param[out] error       Pointer to error data.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t ifx_record_error_get_error_data(const ifx_record_handle_t *handle,
                                             ifx_blob_t *error)
{
    ifx_status_t status = IFX_SUCCESS;

    if ((NULL == handle) || (NULL == error))
    {
        return IFX_ERROR(IFX_RECORD_ERROR, IFX_RECORD_ERROR_GET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    const uint8_t type[] = IFX_RECORD_ERROR_TYPE;
    if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
    {
        error->length =
            ((ifx_record_error_t *) handle->record_data)->error->length;
        error->buffer = (uint8_t *) malloc(error->length);
        if (NULL != error->buffer)
        {
            IFX_MEMCPY(
                error->buffer,
                ((ifx_record_error_t *) handle->record_data)->error->buffer,
                ((ifx_record_error_t *) handle->record_data)->error->length);
        }
        else
        {
            status = IFX_ERROR(IFX_RECORD_ERROR, IFX_RECORD_ERROR_GET,
                               IFX_OUT_OF_MEMORY);
        }
    }
    else
    {
        status = IFX_ERROR(IFX_RECORD_ERROR, IFX_RECORD_ERROR_GET,
                           IFX_RECORD_INVALID);
    }

    return status;
}
