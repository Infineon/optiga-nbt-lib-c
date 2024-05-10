// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file model/ifx-record-alt-carrier.c
 * \brief Model interface to create an alternative carrier record types and
 * set/get record fields.
 * \details Alternative Carrier Record is used within global
 * handover NDEF records to describe a single alternative carrier.
 */
#include "ifx-record-handler-alt-carrier.h"
#include "infineon/ifx-ndef-errors.h"
#include "infineon/ifx-ndef-lib.h"
#include "infineon/ifx-ndef-record.h"
#include "infineon/ifx-record-alt-carrier.h"
#include "infineon/ifx-record-handler.h"

/**
 * \brief Default auxiliary data reference count value 0
 */
#define DEFAULT_AUXILIARY_DATA_REF_COUNT UINT8_C(0)

/**
 * @brief Release all the allocated memory for the created alternative carrier
 * record data
 *
 * @param[in] record_data    data of the alternative carrier record
 * \retval IFX_SUCCESS If memory release operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 */
static ifx_status_t record_ac_deinit(void *record_data)
{
    if (NULL == record_data)
    {
        return IFX_ERROR(IFX_RECORD_AC, IFX_RECORD_AC_RELEASE_MEMORY,
                         IFX_ILLEGAL_ARGUMENT);
    }
    ifx_record_ac_t *ac_record = (ifx_record_ac_t *) (record_data);
    uint8_t index = UINT8_C(0);
    while (index < ac_record->auxiliary_data_ref_count)
    {
        if (ac_record->auxiliary_data_ref[index]->data != NULL)
        {
            IFX_FREE(ac_record->auxiliary_data_ref[index]->data);
            ac_record->auxiliary_data_ref[index]->data = NULL;
        }
        if (ac_record->auxiliary_data_ref[index] != NULL)
        {
            IFX_FREE(ac_record->auxiliary_data_ref[index]);
            ac_record->auxiliary_data_ref[index] = NULL;
        }
        index++;
    }
    if (ac_record->auxiliary_data_ref != NULL)
    {
        IFX_FREE(ac_record->auxiliary_data_ref);
        ac_record->auxiliary_data_ref = NULL;
    }
    if (ac_record->carrier_data_ref->data != NULL)
    {
        IFX_FREE(ac_record->carrier_data_ref->data);
        ac_record->carrier_data_ref->data = NULL;
    }
    if (ac_record->carrier_data_ref != NULL)
    {
        IFX_FREE(ac_record->carrier_data_ref);
        ac_record->carrier_data_ref = NULL;
    }

    return IFX_SUCCESS;
}

/**
 * \brief Creates an alternative carrier record and handle of the created
 * record. This handle holds the values of the record needed for encode and
 * decode operations.
 * \param[out] handle    Handle of the created alternative carrier record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If record creation operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation fails
 */
ifx_status_t ifx_record_ac_new(ifx_record_handle_t *handle)
{
    ifx_status_t status = IFX_SUCCESS;

    if (NULL != handle)
    {
        uint8_t type[] = IFX_RECORD_AC_TYPE;
        handle->tnf = IFX_RECORD_TNF_TYPE_KNOWN;
        handle->type.length = sizeof(type);
        handle->type.buffer = (uint8_t *) malloc(handle->type.length);
        if (NULL != handle->type.buffer)
        {
            handle->id.buffer = NULL;
            handle->id.length = IFX_NDEF_ID_LEN_FIELD_NONE;
            IFX_MEMCPY(handle->type.buffer, type, handle->type.length);
            handle->encode_record = record_handler_ac_encode;
            handle->decode_record = record_handler_ac_decode;
            handle->deinit_record = record_ac_deinit;
            handle->record_data =
                (ifx_record_ac_t *) malloc(sizeof(ifx_record_ac_t));
            if (NULL != handle->record_data)
            {
                ((ifx_record_ac_t *) (handle->record_data))
                    ->auxiliary_data_ref_count =
                    DEFAULT_AUXILIARY_DATA_REF_COUNT;
            }
            else
            {
                // Free any memory allocated by malloc() before returning
                IFX_FREE(handle->type.buffer);
                handle->type.buffer = NULL;
                handle->type.length = 0;
                status = IFX_ERROR(IFX_RECORD_AC, IFX_RECORD_AC_SET,
                                   IFX_OUT_OF_MEMORY);
            }
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_AC, IFX_RECORD_AC_SET, IFX_OUT_OF_MEMORY);
        }
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_AC, IFX_RECORD_AC_SET, IFX_ILLEGAL_ARGUMENT);
    }

    return status;
}

/**
 * \brief Sets a carrier power state (CPS) in the alternative carrier record
 * details for the given record handle.
 * \param[out] handle           Pointer to the record handle obtained while
 * creating the record.
 * \param[in] cps               Carrier power state of the alternative carrier
 * record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_ac_set_cps(ifx_record_handle_t *handle,
                                   ifx_record_ac_cps cps)
{
    ifx_status_t status = IFX_SUCCESS;

    if ((NULL == handle) || ((uint8_t) cps > IFX_CPS_UNKNOWN))
    {
        status =
            IFX_ERROR(IFX_RECORD_AC, IFX_RECORD_AC_SET, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        uint8_t type[] = IFX_RECORD_AC_TYPE;
        handle->type.length = sizeof(type);

        if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
        {
            ((ifx_record_ac_t *) handle->record_data)->cps = cps;
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_AC, IFX_RECORD_AC_SET, IFX_RECORD_INVALID);
        }
    }

    return status;
}

/**
 * \brief Sets a carrier data reference in alternative carrier record details
 * for the given record handle.
 * \param[out] handle           Pointer to the record handle obtained while
 * creating the record.
 * \param[in] carrier_data_ref  Pointer to the carrier data reference.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation fails
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_ac_set_carrier_ref(
    ifx_record_handle_t *handle, const ifx_record_data_ref_t *carrier_data_ref)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == carrier_data_ref))
    {
        status =
            IFX_ERROR(IFX_RECORD_AC, IFX_RECORD_AC_SET, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        uint8_t type[] = IFX_RECORD_AC_TYPE;
        handle->type.length = sizeof(type);

        if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
        {
            ((ifx_record_ac_t *) handle->record_data)->carrier_data_ref =
                (ifx_record_data_ref_t *) malloc(sizeof(ifx_record_data_ref_t));
            if (NULL ==
                ((ifx_record_ac_t *) handle->record_data)->carrier_data_ref)
            {
                status = IFX_ERROR(IFX_RECORD_AC, IFX_RECORD_AC_SET,
                                   IFX_OUT_OF_MEMORY);
            }
            else
            {
                ((ifx_record_ac_t *) handle->record_data)
                    ->carrier_data_ref->data = NULL;
                ((ifx_record_ac_t *) handle->record_data)
                    ->carrier_data_ref->data_length =
                    carrier_data_ref->data_length;
                ((ifx_record_ac_t *) handle->record_data)
                    ->carrier_data_ref->data =
                    (uint8_t *) malloc(carrier_data_ref->data_length);
                if (NULL != ((ifx_record_ac_t *) handle->record_data)
                                ->carrier_data_ref->data)
                {
                    IFX_MEMCPY(((ifx_record_ac_t *) handle->record_data)
                                   ->carrier_data_ref->data,
                               carrier_data_ref->data,
                               carrier_data_ref->data_length);
                }
                else
                {
                    status = IFX_ERROR(IFX_RECORD_AC, IFX_RECORD_AC_SET,
                                       IFX_OUT_OF_MEMORY);
                }
            }
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_AC, IFX_RECORD_AC_SET, IFX_RECORD_INVALID);
        }
    }

    return status;
}

/**
 * \brief Sets the list of auxiliary data references to alternative carrier
 * record for the given record handle.
 * \param[out] handle                    Pointer to the record handle obtained
 * while creating the record.
 * \param[in] auxiliary_data_ref         Pointer to the list of auxiliary data
 * references.
 * \param[in] auxiliary_data_ref_count   Count of auxiliary data references.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation fails
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_ac_set_aux_ref(
    ifx_record_handle_t *handle,
    const ifx_record_data_ref_t *auxiliary_data_ref,
    uint8_t auxiliary_data_ref_count)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == auxiliary_data_ref) ||
        (auxiliary_data_ref_count == 0))
    {
        status =
            IFX_ERROR(IFX_RECORD_AC, IFX_RECORD_AC_SET, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        uint8_t type[] = IFX_RECORD_AC_TYPE;
        handle->type.length = sizeof(type);

        if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
        {
            uint8_t index = UINT8_C(0);
            ifx_record_ac_t *ac_data =
                ((ifx_record_ac_t *) handle->record_data);
            ac_data->auxiliary_data_ref_count = auxiliary_data_ref_count;
            ac_data->auxiliary_data_ref = (ifx_record_data_ref_t **) malloc(
                sizeof(ifx_record_data_ref_t) * auxiliary_data_ref_count);
            if (NULL == ac_data->auxiliary_data_ref)
            {
                status = IFX_ERROR(IFX_RECORD_AC, IFX_RECORD_AC_SET,
                                   IFX_OUT_OF_MEMORY);
            }
            else
            {
                while ((index < auxiliary_data_ref_count) &&
                       (IFX_SUCCESS == status))
                {
                    ac_data->auxiliary_data_ref[index] =
                        (ifx_record_data_ref_t *) malloc(
                            sizeof(ifx_record_data_ref_t));
                    if (NULL == ac_data->auxiliary_data_ref[index])
                    {
                        IFX_FREE(ac_data->auxiliary_data_ref);
                        ac_data->auxiliary_data_ref = NULL;
                        status = IFX_ERROR(IFX_RECORD_AC, IFX_RECORD_AC_SET,
                                           IFX_OUT_OF_MEMORY);
                    }
                    else
                    {
                        ac_data->auxiliary_data_ref[index]->data_length =
                            auxiliary_data_ref[index].data_length;
                        ac_data->auxiliary_data_ref[index]->data =
                            (uint8_t *) malloc(
                                auxiliary_data_ref[index].data_length);
                        if (NULL != ac_data->auxiliary_data_ref[index]->data)
                        {
                            IFX_MEMCPY(ac_data->auxiliary_data_ref[index]->data,
                                       auxiliary_data_ref[index].data,
                                       auxiliary_data_ref[index].data_length);
                        }
                        else
                        {
                            IFX_FREE(ac_data->auxiliary_data_ref);
                            ac_data->auxiliary_data_ref = NULL;
                            status = IFX_ERROR(IFX_RECORD_AC, IFX_RECORD_AC_SET,
                                               IFX_OUT_OF_MEMORY);
                        }
                        index++;
                    }
                }
            }
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_AC, IFX_RECORD_AC_SET, IFX_RECORD_INVALID);
        }
    }

    return status;
}

/**
 * \brief Gets a carrier power state (CPS) of alternative carrier record details
 * from the given record handle.
 * \param[in] handle           Pointer to the record handle obtained while
 * creating the record.
 * \param[out] cps             Pointer to the carrier power state of alternative
 * carrier record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID, if record type is invalid
 */
ifx_status_t ifx_record_ac_get_cps(const ifx_record_handle_t *handle,
                                   ifx_record_ac_cps *cps)
{
    ifx_status_t status = IFX_SUCCESS;
    uint8_t type[] = IFX_RECORD_AC_TYPE;

    if ((NULL == handle) || (NULL == cps))
    {
        status =
            IFX_ERROR(IFX_RECORD_AC, IFX_RECORD_AC_GET, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        if (!IFX_MEMCMP(handle->type.buffer, type, sizeof(type)))
        {
            *cps = ((ifx_record_ac_t *) handle->record_data)->cps;
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_AC, IFX_RECORD_AC_GET, IFX_RECORD_INVALID);
        }
    }

    return status;
}

/**
 * \brief Gets a carrier data reference of alternative carrier record details
 * from the given record handle.
 * \param[in] handle                Pointer to the record handle obtained while
 * creating the record.
 * \param[out] carrier_data_ref     Pointer to the carrier data reference.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation fails
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_ac_get_carrier_ref(
    const ifx_record_handle_t *handle, ifx_record_data_ref_t *carrier_data_ref)
{
    ifx_status_t status = IFX_SUCCESS;
    uint8_t type[] = IFX_RECORD_AC_TYPE;
    if ((NULL == handle) || (NULL == carrier_data_ref))
    {
        status =
            IFX_ERROR(IFX_RECORD_AC, IFX_RECORD_AC_GET, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        if (!IFX_MEMCMP(handle->type.buffer, type, sizeof(type)))
        {
            carrier_data_ref->data_length =
                ((ifx_record_ac_t *) handle->record_data)
                    ->carrier_data_ref->data_length;
            carrier_data_ref->data =
                (uint8_t *) malloc(carrier_data_ref->data_length);
            if (NULL != carrier_data_ref->data)
            {
                IFX_MEMCPY(carrier_data_ref->data,
                           ((ifx_record_ac_t *) handle->record_data)
                               ->carrier_data_ref->data,
                           carrier_data_ref->data_length);
            }
            else
            {
                status = IFX_ERROR(IFX_RECORD_AC, IFX_RECORD_AC_GET,
                                   IFX_OUT_OF_MEMORY);
            }
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_AC, IFX_RECORD_AC_GET, IFX_RECORD_INVALID);
        }
    }

    return status;
}

/**
 * \brief Gets the list of auxiliary data references of alternative carrier
 * record from the given record handle.
 * \param[in] handle                        Pointer to the record handle
 * obtained while creating the record.
 * \param[out] auxiliary_data_ref           Pointer to the list of auxiliary
 * data references.
 * \param[out] auxiliary_data_ref_count     Pointer to the count of auxiliary
 * data references.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation fails
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_ac_get_aux_ref(
    const ifx_record_handle_t *handle,
    ifx_record_data_ref_t *auxiliary_data_ref,
    uint8_t *auxiliary_data_ref_count)
{
    ifx_status_t status = IFX_SUCCESS;
    uint8_t type[] = IFX_RECORD_AC_TYPE;
    if ((NULL == handle) || (NULL == auxiliary_data_ref) ||
        (auxiliary_data_ref_count == 0))
    {
        status =
            IFX_ERROR(IFX_RECORD_AC, IFX_RECORD_AC_GET, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        if (!IFX_MEMCMP(handle->type.buffer, type, sizeof(type)))
        {
            uint8_t index = UINT8_C(0);
            ifx_record_ac_t *ac_data =
                ((ifx_record_ac_t *) handle->record_data);
            *auxiliary_data_ref_count = ac_data->auxiliary_data_ref_count;

            while ((index < *auxiliary_data_ref_count) &&
                   (IFX_SUCCESS == status))
            {
                auxiliary_data_ref[index].data_length =
                    ac_data->auxiliary_data_ref[index]->data_length;
                auxiliary_data_ref[index].data = (uint8_t *) malloc(
                    ac_data->auxiliary_data_ref[index]->data_length);
                if (NULL == auxiliary_data_ref[index].data)
                {
                    status = IFX_ERROR(IFX_RECORD_AC, IFX_RECORD_AC_GET,
                                       IFX_OUT_OF_MEMORY);
                }
                else
                {
                    IFX_MEMCPY(auxiliary_data_ref[index].data,
                               ac_data->auxiliary_data_ref[index]->data,
                               ac_data->auxiliary_data_ref[index]->data_length);
                    index++;
                }
            }
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_AC, IFX_RECORD_AC_SET, IFX_RECORD_INVALID);
        }
    }

    return status;
}
