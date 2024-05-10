// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file model/ifx-record-handover-select.c
 * \brief Model interface to create the handover select record types and set/get
 * record fields.
 */
#include "ifx-record-handler-handover-select.h"
#include "infineon/ifx-ndef-errors.h"
#include "infineon/ifx-ndef-lib.h"
#include "infineon/ifx-ndef-message.h"
#include "infineon/ifx-ndef-record.h"
#include "infineon/ifx-record-alt-carrier.h"
#include "infineon/ifx-record-error.h"
#include "infineon/ifx-record-handler.h"
#include "infineon/ifx-record-handover-select.h"

/**
 * \brief Default major version of the handover select specification
 */
#define DEFAULT_MAJOR_VERSION UINT8_C(0x01)

/**
 * \brief Default minor version of the handover select specification
 */
#define DEFAULT_MINOR_VERSION UINT8_C(0x05)

/**
 * @brief Release all the allocated memory for the created handover select
 * record data
 *
 * @param[in] record_data    data of the handover select record
 * \retval IFX_SUCCESS If memory release operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 */
static ifx_status_t record_hs_deinit(void *record_data)
{
    if (NULL == record_data)
    {
        return IFX_ERROR(IFX_RECORD_HS, IFX_RECORD_HS_RELEASE_MEMORY,
                         IFX_ILLEGAL_ARGUMENT);
    }
    ifx_record_hs_t *hs_record = (ifx_record_hs_t *) (record_data);
    uint32_t index = 0;
    while (index < hs_record->count_of_local_records)
    {
        if (hs_record->local_record_list[index] != NULL)
        {
            ifx_ndef_record_dispose(hs_record->local_record_list[index]);
            IFX_FREE(hs_record->local_record_list[index]);
            hs_record->local_record_list[index] = NULL;
        }
        index++;
    }
    if (hs_record->local_record_list != NULL)
    {
        IFX_FREE(hs_record->local_record_list);
        hs_record->local_record_list = NULL;
    }
    return IFX_SUCCESS;
}

/**
 * \brief Creates a new handover select record and handle of the created record.
 * This handle holds the values of the record needed for encode and decode
 * operations.
 * \param[out] handle    Handle of the created select record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the record creation operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t ifx_record_hs_new(ifx_record_handle_t *handle)
{
    ifx_status_t status = IFX_SUCCESS;
    if (NULL != handle)
    {
        uint8_t type[] = IFX_RECORD_HS_TYPE;
        handle->tnf = IFX_RECORD_TNF_TYPE_KNOWN;
        handle->type.length = sizeof(type);
        handle->type.buffer = (uint8_t *) malloc(handle->type.length);
        if (handle->type.buffer != NULL)
        {
            handle->id.buffer = NULL;
            handle->id.length = IFX_NDEF_ID_LEN_FIELD_NONE;
            IFX_MEMCPY(handle->type.buffer, type, handle->type.length);
            handle->encode_record = record_handler_hs_encode;
            handle->decode_record = record_handler_hs_decode;
            handle->deinit_record = record_hs_deinit;
            handle->record_data = (void *) malloc(sizeof(ifx_record_hs_t));
            if (NULL != handle->record_data)
            {
                ((ifx_record_hs_t *) handle->record_data)->major_version =
                    DEFAULT_MAJOR_VERSION;
                ((ifx_record_hs_t *) handle->record_data)->minor_version =
                    DEFAULT_MINOR_VERSION;
                ((ifx_record_hs_t *) handle->record_data)->local_record_decode =
                    ifx_ndef_message_decode;
                ((ifx_record_hs_t *) handle->record_data)->local_record_encode =
                    ifx_ndef_message_encode;
            }
            else
            {
                IFX_FREE(handle->type.buffer);
                handle->type.buffer = NULL;
                status = IFX_ERROR(IFX_RECORD_HS, IFX_RECORD_HS_SET,
                                   IFX_OUT_OF_MEMORY);
            }
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_HS, IFX_RECORD_HS_SET, IFX_OUT_OF_MEMORY);
        }
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_HS, IFX_RECORD_HS_SET, IFX_ILLEGAL_ARGUMENT);
    }

    return status;
}

/**
 * \brief Sets the major version in the handover select record details for the
 * given record handle.
 * \param[out] handle        Pointer to the record handle obtained while
 *                           creating the record.
 * \param[in] major_version  Major version field of the handover select record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_hs_set_major_ver(ifx_record_handle_t *handle,
                                         uint8_t major_version)
{
    ifx_status_t status = IFX_SUCCESS;

    if (NULL == handle)
    {
        status =
            IFX_ERROR(IFX_RECORD_HS, IFX_RECORD_HS_SET, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        const uint8_t type[] = IFX_RECORD_HS_TYPE;
        if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
        {
            ((ifx_record_hs_t *) handle->record_data)->major_version =
                major_version;
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_HS, IFX_RECORD_HS_SET, IFX_RECORD_INVALID);
        }
    }

    return status;
}

/**
 * \brief Sets the minor version in handover select record details for the given
 * record handle.
 * \param[out] handle        Pointer to the record handle obtained while
 *                           creating the record.
 * \param[in] minor_version  Minor version field of the handover select record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_hs_set_minor_ver(ifx_record_handle_t *handle,
                                         uint8_t minor_version)
{
    ifx_status_t status = IFX_SUCCESS;

    if (NULL == handle)
    {
        status =
            IFX_ERROR(IFX_RECORD_HS, IFX_RECORD_HS_SET, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        const uint8_t type[] = IFX_RECORD_HS_TYPE;

        if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
        {
            ((ifx_record_hs_t *) handle->record_data)->minor_version =
                minor_version;
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_HS, IFX_RECORD_HS_SET, IFX_RECORD_INVALID);
        }
    }

    return status;
}

/**
 * \brief Sets the list of local records (alternative carrier and error records)
 * to the handover select record for the given record handle.
 * \param[out] handle                   Pointer to the record handle obtained
 *                                      while creating the record.
 * \param[in] local_record_list         Pointer to the list of local records
 *                                      such as alternative carrier, error
 * records.
 * \param[in] count_of_local_records    Number of the local records.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t ifx_record_hs_set_local_records(
    ifx_record_handle_t *handle,
    const ifx_local_record_handles_t *local_record_list,
    uint32_t count_of_local_records)
{
    ifx_status_t status = IFX_SUCCESS;

    if ((NULL == handle) || (NULL == local_record_list))
    {
        status =
            IFX_ERROR(IFX_RECORD_HS, IFX_RECORD_HS_SET, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        const uint8_t type[] = IFX_RECORD_HS_TYPE;
        if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
        {
            uint32_t index = UINT32_C(0);
            uint32_t count = UINT32_C(0);
            ifx_record_hs_t *hs_record =
                ((ifx_record_hs_t *) handle->record_data);
            hs_record->count_of_local_records = count_of_local_records;
            hs_record->local_record_list = (ifx_record_handle_t **) malloc(
                sizeof(ifx_record_handle_t) * count_of_local_records);
            if (NULL != hs_record->local_record_list)
            {
                uint32_t alt_carr_rec_count = count_of_local_records;
                if (NULL != local_record_list->error_record)
                {
                    alt_carr_rec_count = count_of_local_records - 1;
                }

                while ((index < alt_carr_rec_count) && (IFX_SUCCESS == status))
                {
                    hs_record->local_record_list[index] =
                        (ifx_record_handle_t *) malloc(
                            sizeof(ifx_record_handle_t));
                    if (NULL != hs_record->local_record_list[index])
                    {
                        IFX_MEMSET(hs_record->local_record_list[index], 0,
                                   sizeof(ifx_record_handle_t));
                        IFX_MEMCPY(
                            hs_record->local_record_list[index],
                            local_record_list->alt_carrier_rec_list[count],
                            sizeof(ifx_record_handle_t));
                        index++;
                        count++;
                    }
                    else
                    {
                        IFX_FREE(hs_record->local_record_list);
                        hs_record->local_record_list = NULL;
                        status = IFX_ERROR(IFX_RECORD_HS, IFX_RECORD_HS_SET,
                                           IFX_OUT_OF_MEMORY);
                    }
                }

                if (NULL != local_record_list->error_record)
                {
                    hs_record->local_record_list[index] =
                        (ifx_record_handle_t *) malloc(
                            sizeof(ifx_record_handle_t));
                    if (NULL != hs_record->local_record_list[index])
                    {
                        IFX_MEMSET(hs_record->local_record_list[index], 0,
                                   sizeof(ifx_record_handle_t));
                        IFX_MEMCPY(hs_record->local_record_list[index],
                                   local_record_list->error_record,
                                   sizeof(ifx_record_handle_t));
                    }
                    else
                    {
                        IFX_FREE(hs_record->local_record_list);
                        hs_record->local_record_list = NULL;
                        status = IFX_ERROR(IFX_RECORD_HS, IFX_RECORD_HS_SET,
                                           IFX_OUT_OF_MEMORY);
                    }
                }
            }
            else
            {
                status = IFX_ERROR(IFX_RECORD_HS, IFX_RECORD_HS_SET,
                                   IFX_OUT_OF_MEMORY);
            }
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_HS, IFX_RECORD_HS_SET, IFX_RECORD_INVALID);
        }
    }
    return status;
}

/**
 * \brief Gets the major version of the handover select record from the given
 * record handle.
 * \param[in] handle            Pointer to the record handle obtained while
 *                              creating the record.
 * \param[out] major_version    Major version field of the handover select
 * record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_hs_get_major_ver(const ifx_record_handle_t *handle,
                                         uint8_t *major_version)
{
    ifx_status_t status = IFX_SUCCESS;

    if ((NULL == handle) || (NULL == major_version))
    {
        status =
            IFX_ERROR(IFX_RECORD_HS, IFX_RECORD_HS_GET, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        const uint8_t type[] = IFX_RECORD_HS_TYPE;

        if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
        {
            *major_version =
                ((ifx_record_hs_t *) handle->record_data)->major_version;
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_HS, IFX_RECORD_HS_GET, IFX_RECORD_INVALID);
        }
    }

    return status;
}

/**
 * \brief Gets the minor version of handover select record from the given record
 * handle.
 * \param[in] handle            Pointer to the record handle obtained while
 *                              creating the record.
 * \param[out] minor_version    Minor version field of the handover select
 * record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_hs_get_minor_ver(const ifx_record_handle_t *handle,
                                         uint8_t *minor_version)
{
    ifx_status_t status = IFX_SUCCESS;

    if ((NULL == handle) || (NULL == minor_version))
    {
        status =
            IFX_ERROR(IFX_RECORD_HS, IFX_RECORD_HS_GET, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        const uint8_t type[] = IFX_RECORD_HS_TYPE;

        if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
        {
            *minor_version =
                ((ifx_record_hs_t *) handle->record_data)->minor_version;
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_HS, IFX_RECORD_HS_GET, IFX_RECORD_INVALID);
        }
    }

    return status;
}

/**
 * \brief Gets the list of local records from the given record handle.
 * \param[in] handle             Pointer to the record handle obtained while
 *                               creating the record.
 * \param[out] local_record_list Pointer to the list of local records such as
 *                               alternative carrier, error records.
 * \param[out] count_of_local_records   Number of the local records.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t ifx_record_hs_get_local_records(
    const ifx_record_handle_t *handle,
    ifx_local_record_handles_t *local_record_list,
    uint32_t *count_of_local_records)
{
    ifx_status_t status = IFX_SUCCESS;

    if ((NULL == handle) || (NULL == local_record_list))
    {
        status =
            IFX_ERROR(IFX_RECORD_HS, IFX_RECORD_HS_GET, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        const uint8_t type[] = IFX_RECORD_HS_TYPE;
        const uint8_t alt_carr_type[] = IFX_RECORD_AC_TYPE;
        const uint8_t error_type[] = IFX_RECORD_ERROR_TYPE;
        if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
        {
            uint32_t index = UINT32_C(0);
            uint32_t count = UINT32_C(0);
            *count_of_local_records = ((ifx_record_hs_t *) handle->record_data)
                                          ->count_of_local_records;
            local_record_list->alt_carrier_rec_list =
                (ifx_record_handle_t **) malloc(sizeof(ifx_record_handle_t) *
                                                (*count_of_local_records));
            if (NULL != local_record_list->alt_carrier_rec_list)
            {
                while ((index < *count_of_local_records) &&
                       (IFX_SUCCESS == status))
                {
                    if (!IFX_MEMCMP(((ifx_record_hs_t *) handle->record_data)
                                        ->local_record_list[index]
                                        ->type.buffer,
                                    alt_carr_type,
                                    ((ifx_record_hs_t *) handle->record_data)
                                        ->local_record_list[index]
                                        ->type.length))
                    {
                        local_record_list->alt_carrier_rec_list[count] =
                            (ifx_record_handle_t *) malloc(
                                sizeof(ifx_record_handle_t));
                        if (NULL !=
                            local_record_list->alt_carrier_rec_list[count])
                        {
                            IFX_MEMCPY(
                                local_record_list->alt_carrier_rec_list[count],
                                ((ifx_record_hs_t *) handle->record_data)
                                    ->local_record_list[index],
                                sizeof(
                                    *(((ifx_record_hs_t *) handle->record_data)
                                          ->local_record_list[index])));
                            index++;
                            count++;
                        }
                        else
                        {
                            IFX_FREE(local_record_list->alt_carrier_rec_list);
                            local_record_list->alt_carrier_rec_list = NULL;
                            status = IFX_ERROR(IFX_RECORD_HS, IFX_RECORD_HS_GET,
                                               IFX_OUT_OF_MEMORY);
                        }
                    }
                    else if (!IFX_MEMCMP(
                                 ((ifx_record_hs_t *) handle->record_data)
                                     ->local_record_list[index]
                                     ->type.buffer,
                                 error_type,
                                 ((ifx_record_hs_t *) handle->record_data)
                                     ->local_record_list[index]
                                     ->type.length))
                    {
                        local_record_list->error_record =
                            (ifx_record_handle_t *) malloc(sizeof(
                                *(((ifx_record_hs_t *) handle->record_data)
                                      ->local_record_list[index])));
                        if (NULL != local_record_list->error_record)
                        {
                            IFX_MEMCPY(
                                local_record_list->error_record,
                                ((ifx_record_hs_t *) handle->record_data)
                                    ->local_record_list[index],
                                sizeof(
                                    *(((ifx_record_hs_t *) handle->record_data)
                                          ->local_record_list[index])));
                            index++;
                        }
                        else
                        {
                            status = IFX_ERROR(IFX_RECORD_HS, IFX_RECORD_HS_GET,
                                               IFX_OUT_OF_MEMORY);
                        }
                    }
                    else
                    {
                        status = IFX_ERROR(IFX_RECORD_HS, IFX_RECORD_HS_GET,
                                           IFX_RECORD_INVALID);
                    }
                }
            }
            else
            {
                status = IFX_ERROR(IFX_RECORD_HS, IFX_RECORD_HS_GET,
                                   IFX_OUT_OF_MEMORY);
            }
        }
        else
        {
            status =
                IFX_ERROR(IFX_RECORD_HS, IFX_RECORD_HS_GET, IFX_RECORD_INVALID);
        }
    }

    return status;
}
