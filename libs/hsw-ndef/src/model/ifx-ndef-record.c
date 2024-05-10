// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file ndef-record/ifx-ndef-record.c
 * \brief Model interface to create record types and set/get record fields.
 */
#include "infineon/ifx-ndef-errors.h"
#include "infineon/ifx-ndef-lib.h"
#include "infineon/ifx-ndef-record.h"
#include "infineon/ifx-record-alt-carrier.h"
#include "infineon/ifx-record-bluetooth-le.h"
#include "infineon/ifx-record-bluetooth.h"
#include "infineon/ifx-record-error.h"
#include "infineon/ifx-record-external.h"
#include "infineon/ifx-record-handover-select.h"
#include "infineon/ifx-record-mime.h"
#include "infineon/ifx-record-uri.h"

#define MAX_LENGTH_OF_RECORD_TYPE_DATA UINT8_C(50)

/**
 * \brief Array that holds the type details of records whose type value is known
 * and fixed (not-user-defined). This array is not applicable to the
 * Multipurpose Internet Mail Extensions (MIME) or external records as their
 * type value is user-defined. \note This array is indexed as mentioned in the
 * enumeration record_type_e.
 */
uint8_t record_type_data[IFX_RECORD_MAX][MAX_LENGTH_OF_RECORD_TYPE_DATA] = {
    IFX_RECORD_URI_TYPE, IFX_RECORD_HS_TYPE,  IFX_RECORD_AC_TYPE,
    IFX_RECORD_BT_TYPE,  IFX_RECORD_BLE_TYPE, IFX_RECORD_ERROR_TYPE};

/**
 * \brief Contains the type information of records and respective get handler
 * function to retrieve the record handle. \note This array is indexed as
 * mentioned in the enumeration record_type_e.
 */
ifx_record_init_t record_handler_list[IFX_RECORD_MAX] = {
    {record_type_data[IFX_RECORD_TYPE_URI],
     sizeof(record_type_data[IFX_RECORD_TYPE_URI]), ifx_record_uri_new},
    {record_type_data[IFX_RECORD_TYPE_HANDOVER_SELECT],
     sizeof(record_type_data[IFX_RECORD_TYPE_HANDOVER_SELECT]),
     ifx_record_hs_new},
    {record_type_data[IFX_RECORD_TYPE_ALT_CARRIER],
     sizeof(record_type_data[IFX_RECORD_TYPE_ALT_CARRIER]), ifx_record_ac_new},
    {record_type_data[IFX_RECORD_TYPE_BT],
     sizeof(record_type_data[IFX_RECORD_TYPE_BT]), ifx_record_bt_new},
    {record_type_data[IFX_RECORD_TYPE_BLE],
     sizeof(record_type_data[IFX_RECORD_TYPE_BLE]), ifx_record_ble_new},
    {record_type_data[IFX_RECORD_TYPE_ERROR],
     sizeof(record_type_data[IFX_RECORD_TYPE_ERROR]), ifx_record_error_new}};

/**
 * @brief Pointer to the list of registered records.
 */
static ifx_record_init_t *registered_records_list = NULL;

/**
 * @brief Count of the registered records.
 */
static uint32_t count_of_registered_records = 0;

/**
 * \brief Initialize record list to static variable
 *
 * \param[out] registered_records_list Pointer to the list that contains
 * registered records APIs.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If registered record list is obtained successfully
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 *
 */
static ifx_status_t ifx_ndef_record_initialize_register_list(
    ifx_record_init_t **registered_records_list)
{
    if (NULL == registered_records_list)
    {
        return IFX_ERROR(IFX_NDEF_RECORD, IFX_RECORD_RETRIEVE,
                         IFX_ILLEGAL_ARGUMENT);
    }

    // Allocate memory for an array of pointers
    *registered_records_list = (ifx_record_init_t *) malloc(
        IFX_RECORD_MAX * sizeof(ifx_record_init_t));
    if (NULL == *registered_records_list)
    {
        return IFX_ERROR(IFX_NDEF_RECORD, IFX_RECORD_RETRIEVE,
                         IFX_OUT_OF_MEMORY);
    }

    for (uint32_t record_number = 0; record_number < (uint32_t) IFX_RECORD_MAX;
         record_number++)
    {
        // Allocate memory for the type field
        (*registered_records_list)[record_number].type =
            (uint8_t *) malloc(record_handler_list[record_number].type_length);
        if (NULL == (*registered_records_list)[record_number].type)
        {
            for (uint32_t count = 0; count < record_number; count++)
            {
                IFX_FREE((*registered_records_list)[count].type);
                (*registered_records_list)[count].type = NULL;
            }

            IFX_FREE(*registered_records_list);
            *registered_records_list = NULL;
            return IFX_ERROR(IFX_NDEF_RECORD, IFX_RECORD_RETRIEVE,
                             IFX_OUT_OF_MEMORY);
        }

        // Copy the type data
        IFX_MEMCPY((*registered_records_list)[record_number].type,
                   record_handler_list[record_number].type,
                   record_handler_list[record_number].type_length);

        // Assign the get_handle and type_length fields
        (*registered_records_list)[record_number].get_handle =
            record_handler_list[record_number].get_handle;
        (*registered_records_list)[record_number].type_length =
            record_handler_list[record_number].type_length;
    }

    return IFX_SUCCESS;
}

/**
 * \brief Deregister the record list.
 *
 * \return ifx_status_t
 * \retval IFX_SUCCESS If registered record list is obtained successfully
 */
static ifx_status_t ifx_ndef_record_deregister(void)
{
    if (registered_records_list != NULL)
    {
        for (uint32_t record_count = 0;
             record_count < count_of_registered_records; record_count++)
        {
            IFX_FREE(registered_records_list[record_count].type);
            registered_records_list[record_count].type = NULL;
        }

        IFX_FREE(registered_records_list);
        registered_records_list = NULL;
        count_of_registered_records = 0x00;
    }

    return IFX_SUCCESS;
}
/**
 * \brief Gets registered record list and count of registered records
 *
 * \param[out] list Pointer to the list that contains registered records
 * APIs.
 * \param[out] count  Count of the registered records
 * APIs.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If registered record list is obtained successfully
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 *
 */
static ifx_status_t ifx_ndef_record_get_record_list(ifx_record_init_t **list,
                                                    uint32_t *count)
{
    if ((NULL == list) || (NULL == count))
    {
        return IFX_ERROR(IFX_NDEF_RECORD, IFX_RECORD_RETRIEVE,
                         IFX_ILLEGAL_ARGUMENT);
    }

    if (registered_records_list == NULL)
    {
        ifx_status_t status =
            ifx_ndef_record_initialize_register_list(&registered_records_list);
        if (IFX_SUCCESS != status)
        {
            return status;
        }

        count_of_registered_records = IFX_RECORD_MAX;
    }

    *list = registered_records_list;
    *count = count_of_registered_records;

    return IFX_SUCCESS;
}

/**
 * \brief Sets the record ID to the handle of record.
 *
 * \param[out] handle       Pointer to the record handle.
 * \param[in] record_id            Pointer to the record ID.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the record ID is set to the handle successfully
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t ifx_ndef_record_set_record_id(ifx_record_handle_t *handle,
                                           const ifx_blob_t *record_id)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == record_id))
    {
        status =
            IFX_ERROR(IFX_NDEF_RECORD, IFX_RECORD_SET, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        handle->id.buffer = (uint8_t *) malloc(record_id->length);
        if (NULL != handle->id.buffer)
        {
            IFX_MEMCPY(handle->id.buffer, record_id->buffer, record_id->length);
            handle->id.length = record_id->length;
        }
        else
        {
            status =
                IFX_ERROR(IFX_NDEF_RECORD, IFX_RECORD_SET, IFX_OUT_OF_MEMORY);
        }
    }
    return status;
}

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
                                           ifx_blob_t *record_id)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == record_id))
    {
        status =
            IFX_ERROR(IFX_NDEF_RECORD, IFX_RECORD_GET, IFX_ILLEGAL_ARGUMENT);
    }
    else
    {
        record_id->buffer = (uint8_t *) malloc(handle->id.length);
        if (NULL == record_id->buffer)
        {
            status =
                IFX_ERROR(IFX_NDEF_RECORD, IFX_RECORD_GET, IFX_OUT_OF_MEMORY);
        }
        else
        {
            IFX_MEMCPY(record_id->buffer, handle->id.buffer, handle->id.length);
            record_id->length = handle->id.length;
        }
    }
    return status;
}

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
                                             ifx_record_handle_t *handle)
{
    uint8_t index = UINT8_C(0);
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == handle) || (NULL == type))
    {
        return IFX_ERROR(IFX_NDEF_RECORD, IFX_RECORD_RETRIEVE,
                         IFX_ILLEGAL_ARGUMENT);
    }

    ifx_blob_t record_type;
    IFX_MEMSET(&record_type, 0x00, sizeof(ifx_blob_t));
    record_type.length = type_length;
    record_type.buffer = (uint8_t *) malloc(type_length);
    if (NULL == record_type.buffer)
    {
        return IFX_ERROR(IFX_NDEF_RECORD, IFX_RECORD_RETRIEVE,
                         IFX_OUT_OF_MEMORY);
    }

    ifx_record_init_t *list = NULL;
    uint32_t count_of_records = 0x00;
    IFX_MEMCPY(record_type.buffer, type, type_length);
    status = ifx_ndef_record_get_record_list(&list, &count_of_records);
    if (IFX_SUCCESS == status)
    {
        bool is_record_type_found = false;
        while ((index < count_of_records) && (is_record_type_found == false))
        {
            if (!IFX_MEMCMP(record_type.buffer, list[index].type,
                            record_type.length))
            {
                status = list[index].get_handle(handle);
                is_record_type_found = true;
            }
            else
            {
                is_record_type_found = false;
                index++;
            }
        }

        if (false == is_record_type_found)
        {
            if (IFX_RECORD_TNF_TYPE_MEDIA == tnf)
            {
                status = ifx_record_mime_new(handle, &record_type);
            }
            else if (IFX_RECORD_TNF_TYPE_EXT == tnf)
            {
                status = ifx_record_ext_new(handle, &record_type);
            }
            else
            {
                status = IFX_ERROR(IFX_NDEF_RECORD, IFX_RECORD_RETRIEVE,
                                   IFX_RECORD_UNSUPPORTED);
            }
        }
    }

    IFX_FREE(record_type.buffer);
    record_type.buffer = NULL;

    return status;
}

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
 *
 */
ifx_status_t ifx_ndef_record_register_handle(ifx_record_init_t *init_handler)
{
    ifx_status_t status = IFX_SUCCESS;
    if ((NULL == init_handler) || (NULL == init_handler->type) ||
        (NULL == init_handler->get_handle))
    {
        return IFX_ERROR(IFX_NDEF_RECORD, IFX_RECORD_REGISTER,
                         IFX_ILLEGAL_ARGUMENT);
    }

    uint32_t index = UINT32_C(0);
    uint32_t count_of_records = UINT32_C(0);
    ifx_record_init_t *list = NULL;
    status = ifx_ndef_record_get_record_list(&list, &count_of_records);
    if (IFX_SUCCESS != status)
    {
        return status;
    }

    while (index < count_of_records)
    {
        if (0 == IFX_MEMCMP(list[index].type, init_handler->type,
                            init_handler->type_length))
        {
            return IFX_ERROR(IFX_NDEF_RECORD, IFX_RECORD_REGISTER,
                             IFX_RECORD_INFO_ALREADY_REGISTERED);
        }
        index++;
    }

    ifx_record_init_t *list_temp = (ifx_record_init_t *) realloc(
        list, (sizeof(ifx_record_init_t) * (count_of_records + 1)));
    if (NULL == list_temp)
    {
        return IFX_ERROR(IFX_NDEF_RECORD, IFX_RECORD_REGISTER,
                         IFX_OUT_OF_MEMORY);
    }

    list = list_temp;
    list[index].type_length = init_handler->type_length;
    list[index].get_handle = init_handler->get_handle;
    list[index].type = (uint8_t *) malloc(init_handler->type_length);
    if (NULL == list[index].type)
    {
        return IFX_ERROR(IFX_NDEF_RECORD, IFX_RECORD_REGISTER,
                         IFX_OUT_OF_MEMORY);
    }

    IFX_MEMCPY(list[index].type, init_handler->type, init_handler->type_length);
    count_of_records++;
    registered_records_list = list;
    count_of_registered_records = count_of_records;

    return status;
}

/**
 * \brief This method will free-up the internally allocated memory for ndef
 * registered records.
 *
 * \return ifx_status_t
 * \retval IFX_SUCCESS if freeing up of memory is done successfully
 */
ifx_status_t ifx_ndef_record_release_resource(void)
{
    return ifx_ndef_record_deregister();
}

/**
 * \brief This method will free-up the internally allocated memory of a record.
 *
 * \param[in] record_handle Pointer to the record handle.
 * \return ifx_status_t
 * \retval IFX_SUCCESS if releasing memory resources is done successfully
 */
ifx_status_t ifx_ndef_record_dispose(ifx_record_handle_t *record_handle)
{
    if (NULL != record_handle)
    {
        if (NULL != record_handle->record_data)
        {
            record_handle->deinit_record(record_handle->record_data);
            IFX_FREE(record_handle->record_data);
            record_handle->record_data = NULL;
        }

        if (NULL != record_handle->id.buffer)
        {
            IFX_FREE(record_handle->id.buffer);
            record_handle->id.buffer = NULL;
        }

        if (NULL != record_handle->type.buffer)
        {
            IFX_FREE(record_handle->type.buffer);
            record_handle->type.buffer = NULL;
        }
    }

    return IFX_SUCCESS;
}

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
                                          uint32_t number_of_records)
{
    ifx_status_t status = IFX_SUCCESS;
    uint32_t index = 0;
    while ((index < number_of_records) && (IFX_SUCCESS == status))
    {
        status = ifx_ndef_record_dispose(&record_handles[index]);
        index++;
    }

    return status;
}
