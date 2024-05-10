// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/ifx-record-uri.h
 * \brief Model interface to create URI record types and get/set record fields.
 * \details For more details refer to technical specification document for URI
 * Record Type Definition(NFCForum-TS-RTD_URI_1.0)
 */
#ifndef IFX_RECORD_URI_H
#define IFX_RECORD_URI_H

#include <stddef.h>
#include <stdint.h>
#include "infineon/ifx-error.h"
#include "infineon/ifx-utils.h"
#include "infineon/ifx-ndef-record.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Macro definitions */
/* Definitions of function identifiers */

/**
 * \brief Identifier for model set URI record ID
 */
#define IFX_RECORD_URI_SET            UINT8_C(0x01)

/**
 * \brief Identifier for model get URI record ID
 */
#define IFX_RECORD_URI_GET            UINT8_C(0x02)

/**
 * \brief Identifier for model URI record release memory
 */
#define IFX_RECORD_URI_RELEASE_MEMORY UINT8_C(0x03)
/**
 * \brief Type information for uri record 'U'
 */
#define IFX_RECORD_URI_TYPE                                                    \
    {                                                                          \
        UINT8_C(0x55)                                                          \
    }

/**
 * \brief Identifier code maximum numbers in list
 */
#define IFX_RECORD_IDENTIFIER_CODE_MAX UINT8_C(0x24)

/* enum definitions */

/**
 * \brief URI identifiers for URI records identifier code
 * \details User can directly use the enums to identify uri identifier code and
 * pass identifier code value separately.
 */
typedef enum
{
    IFX_URI_NA = 0x00,                      /**< unabridged URI */
    IFX_URI_HTTP_WWW = 0x01,                /**< http://www. */
    IFX_URI_HTTPS_WWW = 0x02,               /**< https://www. */
    IFX_URI_HTTP = 0x03,                    /**< http:// */
    IFX_URI_HTTPS = 0x04,                   /**< https:// */
    IFX_URI_TEL = 0x05,                     /**< tel: */
    IFX_URI_MAILTO = 0x06,                  /**< mailto: */
    IFX_URI_FTP_ANONYMOUS_ANONYMOUS = 0x07, /**< ftp://anonymous:anonymous@ */
    IFX_URI_FTP_FTP = 0x08,                 /**< ftp://ftp */
    IFX_URI_FTPS = 0x09,                    /**< ftps:// */
    IFX_URI_SFTP = 0x0A,                    /**< sftp:// */
    IFX_URI_SMB = 0x0B,                     /**< smb:// */
    IFX_URI_NFS = 0x0C,                     /**< nfs:// */
    IFX_URI_FTP = 0x0D,                     /**< ftp:// */
    IFX_URI_DAV = 0x0E,                     /**< dav:// */
    IFX_URI_NEWS = 0x0F,                    /**< news: */
    IFX_URI_TELNET = 0x10,                  /**< telnet:// */
    IFX_URI_IMAP = 0x11,                    /**< imap: */
    IFX_URI_RTSP = 0x12,                    /**< rtsp:// */
    IFX_URI_URN = 0x13,                     /**< urn: */
    IFX_URI_POP = 0x14,                     /**< pop: */
    IFX_URI_SIP = 0x15,                     /**< sip: */
    IFX_URI_SIPS = 0x16,                    /**< sips:: */
    IFX_URI_TFTP = 0x17,                    /**< tftp: */
    IFX_URI_BTSPP = 0x18,                   /**< btspp:// */
    IFX_URI_BT12CAP = 0x19,                 /**< bt12cap:// */
    IFX_URI_BTGOEP = 0x1A,                  /**< btgoep:// */
    IFX_URI_TCPOBEX = 0x1B,                 /**< tcpobex:// */
    IFX_URI_IRDAOBEX = 0x1C,                /**< irdaobex:// */
    IFX_URI_FILE = 0x1D,                    /**< file:// */
    IFX_URI_URN_EPC_ID = 0x1E,              /**< urn:epc:id: */
    IFX_URI_URN_EPC_TAG = 0x1F,             /**< urn:epc:tag: */
    IFX_URI_URN_EPC_PAT = 0x20,             /**< urn:epc:pat: */
    IFX_URI_URN_EPC_RAW = 0x21,             /**< urn:epc:raw: */
    IFX_URI_URN_EPC = 0x22,                 /**< urn:epc: */
    IFX_URI_URN_NFC = 0x23                  /**< urn:nfc: */
} ifx_record_uri_identifier;

/* public functions */

/**
 * \brief Creates a new URI record and handle of the created record.
 * This handle holds the values of the record needed for encode and decode
 * operations.
 * \param handle                    Pointer to the record handle.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If model new URI record operation is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t ifx_record_uri_new(ifx_record_handle_t *handle);

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
                                           ifx_blob_t *identifier);

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
    const ifx_record_handle_t *handle, uint8_t *identifier_code);

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
                                    ifx_blob_t *uri);

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
    const ifx_record_handle_t *handle, ifx_blob_t *uri_with_identifier);

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
                                           const ifx_blob_t *identifier);

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
                                                uint8_t identifier_code);

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
                                    const ifx_blob_t *uri);

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* IFX_RECORD_URI_H */
