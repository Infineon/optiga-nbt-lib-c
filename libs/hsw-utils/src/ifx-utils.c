// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file ifx-utils.h
 * \brief Provides utility functions
 */
#include "infineon/ifx-utils.h"

#include <stdlib.h>

/**
 * \brief Concatenates the buffers of two ifx_blob_t type data structures.
 * Buffer field of append_data will be appended at end of the buffer field of
 * result.
 *
 * \note In this API, param 'result->buffer' will be reallocated with the memory
 * needed to store the concatenated buffer value. User need to take care to free
 * the memory if required.
 *
 * \param[in] append_data Data to be appended.
 * \param[in,out] result Destination in which the data will be appended.
 * \return ifx_status_t
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t ifx_utils_concat(const ifx_blob_t *append_data, ifx_blob_t *result)
{
#if (IFX_VALIDATE_NULL_PTR)
    if (IFX_VALIDATE_NULL_PTR_BLOB(result) ||
        IFX_VALIDATE_NULL_PTR_BLOB(append_data))
    {
        return IFX_ERROR(IFX_UTILS, IFX_UTILS_CONCAT, IFX_ILLEGAL_ARGUMENT);
    }
#endif

    uint8_t *data = (uint8_t *) malloc(result->length + append_data->length);
    if (IFX_VALIDATE_NULL_PTR_MEMORY(data))
    {
        return IFX_ERROR(IFX_UTILS, IFX_UTILS_CONCAT, IFX_OUT_OF_MEMORY);
    }

    IFX_MEMCPY(data, result->buffer, result->length);
    IFX_MEMCPY(data + result->length, append_data->buffer, append_data->length);
    result->length = result->length + append_data->length;
    result->buffer = NULL;
    result->buffer = (uint8_t *) malloc(result->length);
    if (IFX_VALIDATE_NULL_PTR_MEMORY(result->buffer))
    {
        IFX_FREE(data);
        data = NULL;
        return IFX_ERROR(IFX_UTILS, IFX_UTILS_CONCAT, IFX_OUT_OF_MEMORY);
    }

    IFX_MEMCPY(result->buffer, data, result->length);

    IFX_FREE(data);
    data = NULL;
    return IFX_SUCCESS;
}
