// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file model/ifx-record-uri.c
 * \brief Model interface to create URI record types and get/set record fields.
 * \details For more details refer to technical specification document for URI
 * Record Type Definition(NFCForum-TS-RTD_URI_1.0)
 */
#include "ifx-record-handler-uri.h"
#include "infineon/ifx-ndef-errors.h"
#include "infineon/ifx-ndef-lib.h"
#include "infineon/ifx-ndef-record.h"
#include "infineon/ifx-record-uri.h"

/**
 * \brief Identifier lists type structure
 */
typedef struct
{
    uint8_t identifier_code;
    uint8_t *identifier;
    uint8_t identifier_length;
} ifx_identifier_list;

/**
 * \brief List of identifiers and identifier codes.
 */
static const ifx_identifier_list id_list[IFX_RECORD_IDENTIFIER_CODE_MAX] = {
    {IFX_URI_NA, (uint8_t *) "", 0},
    {IFX_URI_HTTP_WWW, (uint8_t *) "http://www.", 11},
    {IFX_URI_HTTPS_WWW, (uint8_t *) "https://www.", 12},
    {IFX_URI_HTTP, (uint8_t *) "http://", 7},
    {IFX_URI_HTTPS, (uint8_t *) "https://", 8},
    {IFX_URI_TEL, (uint8_t *) "tel:", 4},
    {IFX_URI_MAILTO, (uint8_t *) "mailto:", 7},
    {IFX_URI_FTP_ANONYMOUS_ANONYMOUS, (uint8_t *) "ftp://anonymous:anonymous@",
     26},
    {IFX_URI_FTP_FTP, (uint8_t *) "ftp://ftp", 9},
    {IFX_URI_FTPS, (uint8_t *) "ftps://", 7},
    {IFX_URI_SFTP, (uint8_t *) "sftp://", 7},
    {IFX_URI_SMB, (uint8_t *) "smb://", 6},
    {IFX_URI_NFS, (uint8_t *) "nfs://", 6},
    {IFX_URI_FTP, (uint8_t *) "ftp://", 6},
    {IFX_URI_DAV, (uint8_t *) "dav://", 6},
    {IFX_URI_NEWS, (uint8_t *) "news:", 5},
    {IFX_URI_TELNET, (uint8_t *) "telnet://", 9},
    {IFX_URI_IMAP, (uint8_t *) "imap:", 5},
    {IFX_URI_RTSP, (uint8_t *) "rtsp://", 7},
    {IFX_URI_URN, (uint8_t *) "urn:", 4},
    {IFX_URI_POP, (uint8_t *) "pop:", 4},
    {IFX_URI_SIP, (uint8_t *) "sip:", 4},
    {IFX_URI_SIPS, (uint8_t *) "sips:", 5},
    {IFX_URI_TFTP, (uint8_t *) "tftp:", 5},
    {IFX_URI_BTSPP, (uint8_t *) "btspp://", 8},
    {IFX_URI_BT12CAP, (uint8_t *) "bt12cap://", 10},
    {IFX_URI_BTGOEP, (uint8_t *) "btgoep://", 9},
    {IFX_URI_TCPOBEX, (uint8_t *) "tcpobex://", 10},
    {IFX_URI_IRDAOBEX, (uint8_t *) "irdaobex://", 11},
    {IFX_URI_FILE, (uint8_t *) "file://", 7},
    {IFX_URI_URN_EPC_ID, (uint8_t *) "urn:epc:id:", 11},
    {IFX_URI_URN_EPC_TAG, (uint8_t *) "urn:epc:tag:", 12},
    {IFX_URI_URN_EPC_PAT, (uint8_t *) "urn:epc:pat:", 12},
    {IFX_URI_URN_EPC_RAW, (uint8_t *) "urn:epc:raw:", 12},
    {IFX_URI_URN_EPC, (uint8_t *) "urn:epc:", 8},
    {IFX_URI_URN_NFC, (uint8_t *) "urn:nfc:", 8}};

/**
 * \brief Converts the identifier bytes to the identifier code.
 * \param[in] identifier_bytes Pointer to the binary large object (BLOB) type
 *                             data excluded NULL ('\0') termination character.
 * \param[out] identifier_code Pointer to the identifier code.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_RECORD_URI_IDENTIFIER_INVALID if identifier is not matching with
 * existing list
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 */
static ifx_status_t get_identifier_code(const ifx_blob_t *identifier_bytes,
                                        uint8_t *identifier_code)
{
    ifx_status_t status = IFX_SUCCESS;
    bool is_valid_identifier = false;
    if ((NULL == identifier_bytes) || (NULL == identifier_code))
    {
        return IFX_ERROR(IFX_RECORD_URI, IFX_RECORD_URI_GET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    /* Compare the identifier within existing identifier list. */
    for (uint8_t index = 0; index < IFX_RECORD_IDENTIFIER_CODE_MAX; index++)
    {
        /* Compare the identifier and the identifier_bytes->length with existing
         * identifier length. */
        if (!IFX_MEMCMP(id_list[index].identifier, identifier_bytes->buffer,
                        identifier_bytes->length) &&
            (id_list[index].identifier_length == identifier_bytes->length))
        {
            is_valid_identifier = true;
            *identifier_code = id_list[index].identifier_code;
            break;
        }
    }

    if (is_valid_identifier == false)
    {
        status = IFX_ERROR(IFX_RECORD_URI, IFX_RECORD_URI_GET,
                           IFX_RECORD_URI_IDENTIFIER_INVALID);
    }

    return status;
}

/**
 * \brief Converts the identifier code to the identifier bytes.
 * \param[out] identifier_bytes     Pointer to the BLOB type data excluded NULL
 *                                  ('\0') termination character.
 * \param[in] identifier_code       Pointer to the identifier code.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_RECORD_URI_IDENTIFIER_CODE_INVALID if the identifier code is not
 * matching with the existing list
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
static ifx_status_t get_identifier(ifx_blob_t *identifier_bytes,
                                   uint8_t identifier_code)
{
    ifx_status_t status = IFX_SUCCESS;
    bool is_valid_identifier_code = false;

    if ((NULL == identifier_bytes))
    {
        return IFX_ERROR(IFX_RECORD_URI, IFX_RECORD_URI_GET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    for (uint8_t index = 0; index < IFX_RECORD_IDENTIFIER_CODE_MAX; index++)
    {
        /* Compare the identifier and identifier_bytes->length with existing
         * identifier list. */
        if (id_list[index].identifier_code == identifier_code)
        {
            is_valid_identifier_code = true;
            identifier_bytes->buffer =
                (uint8_t *) malloc(id_list[index].identifier_length);
            if (identifier_bytes->buffer == NULL)
            {
                return IFX_ERROR(IFX_RECORD_URI, IFX_RECORD_URI_GET,
                                 IFX_OUT_OF_MEMORY);
            }
            IFX_MEMCPY(identifier_bytes->buffer, id_list[index].identifier,
                       id_list[index].identifier_length);
            identifier_bytes->length = id_list[index].identifier_length;
            break;
        }
    }

    if (is_valid_identifier_code == false)
    {
        status = IFX_ERROR(IFX_RECORD_URI, IFX_RECORD_URI_GET,
                           IFX_RECORD_URI_IDENTIFIER_CODE_INVALID);
    }

    return status;
}

/**
 * @brief Release all the allocated memory for the created uri record data
 *
 * @param[in] record_data    data of the uri record
 * \retval IFX_SUCCESS If memory release operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 */
static ifx_status_t record_uri_deinit(void *record_data)
{
    if (NULL == record_data)
    {
        return IFX_ERROR(IFX_RECORD_URI, IFX_RECORD_URI_RELEASE_MEMORY,
                         IFX_ILLEGAL_ARGUMENT);
    }
    ifx_record_uri_t *uri_record = (ifx_record_uri_t *) (record_data);

    if (uri_record->uri->buffer != NULL)
    {
        IFX_FREE(uri_record->uri->buffer);
        uri_record->uri->buffer = NULL;
    }

    if (uri_record->uri != NULL)
    {
        IFX_FREE(uri_record->uri);
        uri_record->uri = NULL;
    }
    return IFX_SUCCESS;
}

/**
 * \brief Creates a new URI record and handle of the created record.
 * This handle holds the values of the record needed for encode and decode
 * operations.
 * \param[out] handle                    Pointer to the record handle.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If model new URI record operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t ifx_record_uri_new(ifx_record_handle_t *handle)
{
    if (NULL == handle)
    {
        return IFX_ERROR(IFX_RECORD_URI, IFX_RECORD_URI_SET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    uint8_t type[] = IFX_RECORD_URI_TYPE;
    handle->tnf = IFX_RECORD_TNF_TYPE_KNOWN;
    handle->id.buffer = NULL;
    handle->id.length = IFX_NDEF_ID_LEN_FIELD_NONE;

    ifx_record_uri_t *uri_rec =
        (ifx_record_uri_t *) malloc(sizeof(ifx_record_uri_t));
    if (NULL == uri_rec)
    {
        return IFX_ERROR(IFX_RECORD_URI, IFX_RECORD_URI_SET, IFX_OUT_OF_MEMORY);
    }

    handle->type.buffer = (uint8_t *) malloc(sizeof(type));
    if (NULL == handle->type.buffer)
    {
        IFX_FREE(uri_rec);
        uri_rec = NULL;
        return IFX_ERROR(IFX_RECORD_URI, IFX_RECORD_URI_SET, IFX_OUT_OF_MEMORY);
    }

    IFX_MEMCPY(handle->type.buffer, type, sizeof(type));
    handle->type.length = sizeof(type);
    handle->encode_record = record_handler_uri_encode;
    handle->decode_record = record_handler_uri_decode;
    handle->deinit_record = record_uri_deinit;
    handle->record_data = (void *) uri_rec;

    return IFX_SUCCESS;
}

/**
 * \brief Gets the identifier bytes from the URI record.
 * \param[in] handle                Pointer to the record handle.
 * \param[out] identifier           Pointer to the BLOB type data field and
 * length excluded NULL termination character.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 * \retval IFX_RECORD_URI_IDENTIFIER_INVALID CODE if identifier code does not
 * match with existing list
 */
ifx_status_t ifx_record_uri_get_identifier(const ifx_record_handle_t *handle,
                                           ifx_blob_t *identifier)
{
    ifx_status_t status = IFX_SUCCESS;
    uint8_t type[] = IFX_RECORD_URI_TYPE;

    if ((NULL == handle) || (NULL == identifier))
    {
        return IFX_ERROR(IFX_RECORD_URI, IFX_RECORD_URI_GET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    if (!IFX_MEMCMP(handle->type.buffer, type, sizeof(type)))
    {
        status = get_identifier(
            identifier,
            ((ifx_record_uri_t *) handle->record_data)->identifier_code);
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_URI, IFX_RECORD_URI_GET, IFX_RECORD_INVALID);
    }

    return status;
}

/**
 * \brief Gets the identifier code from the URI record details
 *
 * \param[in] handle                Pointer to the record handle.
 * \param[out] identifier_code      Pointer to the URI identifier code.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_uri_get_identifier_code(
    const ifx_record_handle_t *handle, uint8_t *identifier_code)
{
    ifx_status_t status = IFX_SUCCESS;
    uint8_t type[] = IFX_RECORD_URI_TYPE;

    if ((NULL == handle) || (NULL == identifier_code))
    {
        return IFX_ERROR(IFX_RECORD_URI, IFX_RECORD_URI_GET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    if (!IFX_MEMCMP(handle->type.buffer, type, sizeof(type)))
    {
        *identifier_code =
            ((ifx_record_uri_t *) handle->record_data)->identifier_code;
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_URI, IFX_RECORD_URI_GET, IFX_RECORD_INVALID);
    }

    return status;
}

/**
 * \brief Gets the URI value field from the URI record.
 * \param[in] handle                Pointer to the record handle.
 * \param[out] uri                  Pointer to the BLOB type data, data field
 * and length excluded NULL ('\0') termination character.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If get operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_uri_get_uri(const ifx_record_handle_t *handle,
                                    ifx_blob_t *uri)
{
    ifx_status_t status = IFX_SUCCESS;
    uint8_t type[] = IFX_RECORD_URI_TYPE;

    if ((NULL == handle) || (NULL == uri))
    {
        return IFX_ERROR(IFX_RECORD_URI, IFX_RECORD_URI_GET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    if (!IFX_MEMCMP(handle->type.buffer, type, sizeof(type)))
    {
        ifx_record_uri_t *uri_data = (ifx_record_uri_t *) handle->record_data;
        uri->buffer = (uint8_t *) malloc(uri_data->uri->length);
        if (NULL == uri->buffer)
        {
            status = IFX_ERROR(IFX_RECORD_URI, IFX_RECORD_URI_GET,
                               IFX_OUT_OF_MEMORY);
        }
        else
        {
            IFX_MEMCPY(uri->buffer, uri_data->uri->buffer,
                       uri_data->uri->length);
            uri->length = uri_data->uri->length;
        }
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_URI, IFX_RECORD_URI_GET, IFX_RECORD_INVALID);
    }

    return status;
}

/**
 * \brief Gets the URI value along with the identifier from the URI record.
 * \param[in] handle               Pointer to the URI record handle.
 * \param[out] uri_with_identifier Pointer to the BLOB type data, if URI
 * identifier code is not supported then return URI bytes without identifier
 * bytes data field and length excluded NULL termination character.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If identifier code matches with existing list, populates
 * uri_value + identifier
 * \retval IFX_RECORD_URI_IDENTIFIER_INVALID CODE If identifier code does not
 * match, still it populates uri_value
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_uri_get_uri_with_identifier(
    const ifx_record_handle_t *handle, ifx_blob_t *uri_with_identifier)
{
    ifx_status_t status = IFX_SUCCESS;
    uint8_t type[] = IFX_RECORD_URI_TYPE;

    if ((NULL == handle) || (NULL == uri_with_identifier))
    {
        return IFX_ERROR(IFX_RECORD_URI, IFX_RECORD_URI_GET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    if (!IFX_MEMCMP(handle->type.buffer, type, sizeof(type)))
    {
        ifx_record_uri_t *uri_data = (ifx_record_uri_t *) handle->record_data;
        status = get_identifier(uri_with_identifier, uri_data->identifier_code);

        /* If error occurred in fetching the URI identifier, fill the identifier
        as blank and still continue to populate the URI value field.
        This error status will be returned as it is from this method. */
        if (IFX_SUCCESS != status)
        {
            /* Set the data length to 0. */
            uri_with_identifier->length = UINT8_C(0);
            uri_with_identifier->buffer =
                (uint8_t *) malloc(uri_data->uri->length);
            if (NULL == uri_with_identifier->buffer)
            {
                return IFX_ERROR(IFX_RECORD_URI, IFX_RECORD_URI_GET,
                                 IFX_OUT_OF_MEMORY);
            }
        }
        /* If fetching the URI identifier is successful, memory is allocated
         * already.*/
        uint8_t *buffer_temp = (uint8_t *) realloc(
            uri_with_identifier->buffer,
            (uri_data->uri->length + uri_with_identifier->length));
        if (NULL == buffer_temp)
        {
            IFX_FREE(uri_with_identifier->buffer);
            uri_with_identifier->buffer = NULL;
            return IFX_ERROR(IFX_RECORD_URI, IFX_RECORD_URI_GET,
                             IFX_OUT_OF_MEMORY);
        }

        uri_with_identifier->buffer = buffer_temp;
        IFX_MEMCPY(&uri_with_identifier->buffer[uri_with_identifier->length],
                   uri_data->uri->buffer, uri_data->uri->length);
        uri_with_identifier->length =
            uri_data->uri->length + uri_with_identifier->length;
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_URI, IFX_RECORD_URI_GET, IFX_RECORD_INVALID);
    }

    return status;
}

/**
 * \brief Sets an identifier code value of the record handle from the identifier
 * bytes.
 * \param[out] handle               Pointer to the record type handle.
 * \param[in] identifier            Pointer to BLOB type identifier bytes.
 *                                  BLOB data field and length excluded NULL
 *                                  termination character
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_RECORD_URI_IDENTIFIER_CODE_INVALID If identifier code does not
 * match with existing list
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_URI_IDENTIFIER_INVALID If identifier bytes is not matching
 * with existing list
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_uri_set_identifier(ifx_record_handle_t *handle,
                                           const ifx_blob_t *identifier)
{
    ifx_status_t status = IFX_SUCCESS;
    uint8_t type[] = IFX_RECORD_URI_TYPE;

    if ((NULL == handle) || (NULL == identifier))
    {
        return IFX_ERROR(IFX_RECORD_URI, IFX_RECORD_URI_SET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    handle->type.length = sizeof(type);

    if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
    {
        status = get_identifier_code(
            identifier,
            &((ifx_record_uri_t *) handle->record_data)->identifier_code);
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_URI, IFX_RECORD_URI_SET, IFX_RECORD_INVALID);
    }

    return status;
}

/**
 * \brief Sets the identifier code in the URI record details.
 * \param[out] handle          Pointer to the record handle.
 * \param[in] identifier_code  Identifier code data
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_uri_set_identifier_code(ifx_record_handle_t *handle,
                                                uint8_t identifier_code)
{
    ifx_status_t status = IFX_SUCCESS;
    uint8_t type[] = IFX_RECORD_URI_TYPE;

    if (NULL == handle)
    {
        return IFX_ERROR(IFX_RECORD_URI, IFX_RECORD_URI_SET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    handle->type.length = sizeof(type);
    if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
    {
        ((ifx_record_uri_t *) handle->record_data)->identifier_code =
            identifier_code;
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_URI, IFX_RECORD_URI_SET, IFX_RECORD_INVALID);
    }

    return status;
}

/**
 * \brief Sets the URI value in the URI record details.
 * \param[out] handle   Pointer to the record handle.
 * \param[in] uri       Pointer to the BLOB type data, data field, and
 *                      length excluded NULL termination character.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If the set operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 * \retval IFX_RECORD_INVALID If record type is invalid
 */
ifx_status_t ifx_record_uri_set_uri(ifx_record_handle_t *handle,
                                    const ifx_blob_t *uri)
{
    ifx_status_t status = IFX_SUCCESS;
    uint8_t type[] = IFX_RECORD_URI_TYPE;

    if ((NULL == handle) || (NULL == uri))
    {
        return IFX_ERROR(IFX_RECORD_URI, IFX_RECORD_URI_SET,
                         IFX_ILLEGAL_ARGUMENT);
    }

    handle->type.length = sizeof(type);
    if (!IFX_MEMCMP(handle->type.buffer, type, handle->type.length))
    {
        ((ifx_record_uri_t *) handle->record_data)->uri =
            (ifx_blob_t *) malloc(sizeof(ifx_blob_t));
        if (NULL != ((ifx_record_uri_t *) handle->record_data)->uri)
        {
            ((ifx_record_uri_t *) handle->record_data)->uri->length =
                uri->length;
            ((ifx_record_uri_t *) handle->record_data)->uri->buffer =
                (uint8_t *) malloc(uri->length);
            if (NULL != ((ifx_record_uri_t *) handle->record_data)->uri->buffer)
            {
                IFX_MEMCPY(
                    ((ifx_record_uri_t *) handle->record_data)->uri->buffer,
                    uri->buffer, uri->length);
            }
            else
            {
                IFX_FREE(((ifx_record_uri_t *) handle->record_data)->uri);
                ((ifx_record_uri_t *) handle->record_data)->uri = NULL;
                status = IFX_ERROR(IFX_RECORD_URI, IFX_RECORD_URI_SET,
                                   IFX_OUT_OF_MEMORY);
            }
        }
        else
        {
            status = IFX_ERROR(IFX_RECORD_URI, IFX_RECORD_URI_SET,
                               IFX_OUT_OF_MEMORY);
        }
    }
    else
    {
        status =
            IFX_ERROR(IFX_RECORD_URI, IFX_RECORD_URI_SET, IFX_RECORD_INVALID);
    }

    return status;
}
