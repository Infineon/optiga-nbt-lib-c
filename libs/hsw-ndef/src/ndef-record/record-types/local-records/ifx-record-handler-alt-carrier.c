// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file ndef-record/record-types/local-records/ifx-record-handler-alt-carrier.c
 * \brief Alternative carrier record of encoding/decoding utility.
 * \details The alternative carrier record is used within global handover NDEF
 * records to describe a single alternative carrier. For more details refer to
 * technical specification document NFC data exchange format
 * (NFCForum-TS-NDEF_1.0)
 */
#include "ifx-record-handler-alt-carrier.h"
#include "infineon/ifx-ndef-lib.h"
#include "infineon/ifx-record-alt-carrier.h"

#define BYTE_LENGTH_OF_CARRIER_DATA_REF_DATALENGTH_FIELD   UINT8_C(1)
#define BYTE_LENGTH_OF_AUXILIARY_DATA_REF_COUNT_FIELD      UINT8_C(1)
#define BYTE_LENGTH_OF_AUXILIARY_DATA_REF_DATALENGTH_FIELD UINT8_C(1)
#define BYTE_LENGTH_OF_CPS_FIELD                           UINT8_C(1)

/* Static functions */

/**
 * \brief Calculates the size of the payload.
 * \param[in] alt_carrier_record Record data field of the alternative carrier
 * record.
 * \return uint32_t size of record
 */
static uint32_t calculate_record_detail_size(
    ifx_record_ac_t *alt_carrier_record)
{
    uint32_t size = UINT32_C(0);
    uint8_t index = UINT8_C(0);

    size += BYTE_LENGTH_OF_CPS_FIELD;
    size += alt_carrier_record->carrier_data_ref->data_length +
            BYTE_LENGTH_OF_CARRIER_DATA_REF_DATALENGTH_FIELD;
    size += BYTE_LENGTH_OF_AUXILIARY_DATA_REF_COUNT_FIELD;

    while (index < alt_carrier_record->auxiliary_data_ref_count)
    {
        size += (alt_carrier_record->auxiliary_data_ref[index++]->data_length) +
                BYTE_LENGTH_OF_AUXILIARY_DATA_REF_DATALENGTH_FIELD;
    }

    return size;
}

/* Public functions */

/**
 * \brief Encodes the alternative carrier record data to record the payload
 * bytes.
 * \param[in]   record_details      Pointer to the alternative carrier record
 *                                  data that was updated in record handle.
 * \param[out]  payload             Pointer to the payload byte array of
 * alternative carrier record.
 * \param[out]  payload_length      Pointer to the payload length of
 * alternative carrier record.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If encoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 */
ifx_status_t record_handler_ac_encode(const void *record_details,
                                      uint8_t **payload,
                                      uint32_t *payload_length)
{
    ifx_status_t status = IFX_SUCCESS;
    uint32_t index = UINT32_C(0);
    ifx_record_ac_t *alt_carrier_record = (ifx_record_ac_t *) record_details;

    if ((NULL == alt_carrier_record) || (NULL == payload) ||
        (NULL == payload_length))
    {
        return IFX_ERROR(IFX_RECORD_HANDLER_AC, IFX_RECORD_HANDLER_AC_ENCODE,
                         IFX_ILLEGAL_ARGUMENT);
    }

    uint8_t *payload_data =
        (uint8_t *) malloc(calculate_record_detail_size(alt_carrier_record));
    if (NULL == payload_data)
    {
        status = IFX_ERROR(IFX_RECORD_HANDLER_AC, IFX_RECORD_HANDLER_AC_ENCODE,
                           IFX_OUT_OF_MEMORY);
    }
    else
    {
        payload_data[index++] = alt_carrier_record->cps;
        payload_data[index++] =
            alt_carrier_record->carrier_data_ref->data_length;
        IFX_MEMCPY(&payload_data[index],
                   alt_carrier_record->carrier_data_ref->data,
                   alt_carrier_record->carrier_data_ref->data_length);
        index += alt_carrier_record->carrier_data_ref->data_length;
        payload_data[index++] = alt_carrier_record->auxiliary_data_ref_count;
        uint32_t count = UINT32_C(0);

        while (count < alt_carrier_record->auxiliary_data_ref_count)
        {
            payload_data[index++] =
                alt_carrier_record->auxiliary_data_ref[count]->data_length;
            IFX_MEMCPY(
                &payload_data[index],
                alt_carrier_record->auxiliary_data_ref[count]->data,
                alt_carrier_record->auxiliary_data_ref[count]->data_length);
            index += alt_carrier_record->auxiliary_data_ref[count]->data_length;
            count++;
        }

        *payload_length = index;
        *payload = payload_data;
    }

    return status;
}

/**
 * \brief Decodes the record payload bytes to the error record details.
 * \param[in]   payload             Pointer to the payload byte array of the
 *                                  alternative carrier record.
 * \param[in]   payload_length      Payload length of the alternative carrier
 * record.
 * \param[out]  record_details      Pointer to the alternative carrier record
 *                                  data where the decoded payload will be
 * copied.
 * \return ifx_status_t
 * \retval IFX_SUCCESS If decoding is successful
 * \retval IFX_ILLEGAL_ARGUMENT If invalid/NULL parameter is passed to function
 * \retval IFX_OUT_OF_MEMORY If memory allocation is invalid
 * \retval IFX_UNSPECIFIED_ERROR If unspecified error came
 */
ifx_status_t record_handler_ac_decode(const uint8_t *payload,
                                      uint32_t payload_length,
                                      void *record_details)
{
    ifx_status_t status = IFX_SUCCESS;
    ifx_record_ac_t *alt_carrier_record = (ifx_record_ac_t *) record_details;
    uint32_t index = UINT32_C(0);
    if ((NULL == record_details) || (NULL == payload))
    {
        return IFX_ERROR(IFX_RECORD_HANDLER_AC, IFX_RECORD_HANDLER_AC_DECODE,
                         IFX_ILLEGAL_ARGUMENT);
    }

    alt_carrier_record->cps = (ifx_record_ac_cps) payload[index++];
    alt_carrier_record->carrier_data_ref =
        (ifx_record_data_ref_t *) malloc(sizeof(ifx_record_data_ref_t));

    if (NULL != alt_carrier_record->carrier_data_ref)
    {
        alt_carrier_record->carrier_data_ref->data_length = payload[index++];
        alt_carrier_record->carrier_data_ref->data = (uint8_t *) malloc(
            alt_carrier_record->carrier_data_ref->data_length);
        if (NULL != alt_carrier_record->carrier_data_ref->data)
        {
            IFX_MEMCPY(alt_carrier_record->carrier_data_ref->data,
                       &payload[index],
                       alt_carrier_record->carrier_data_ref->data_length);
            index += alt_carrier_record->carrier_data_ref->data_length;
            alt_carrier_record->auxiliary_data_ref_count = payload[index++];

            alt_carrier_record->auxiliary_data_ref =
                (ifx_record_data_ref_t **) malloc(
                    alt_carrier_record->auxiliary_data_ref_count *
                    (sizeof(ifx_record_data_ref_t)));

            if (NULL != alt_carrier_record->auxiliary_data_ref)
            {
                uint8_t count = UINT8_C(0);
                while ((count < alt_carrier_record->auxiliary_data_ref_count) &&
                       (IFX_SUCCESS == status))
                {
                    alt_carrier_record->auxiliary_data_ref[count] =
                        (ifx_record_data_ref_t *) malloc(
                            sizeof(ifx_record_data_ref_t));
                    if (NULL != alt_carrier_record->auxiliary_data_ref[count])
                    {
                        alt_carrier_record->auxiliary_data_ref[count]
                            ->data_length = payload[index++];
                        alt_carrier_record->auxiliary_data_ref[count]->data =
                            (uint8_t *) malloc(
                                alt_carrier_record->auxiliary_data_ref[count]
                                    ->data_length);
                        if (NULL !=
                            alt_carrier_record->auxiliary_data_ref[count]->data)
                        {
                            IFX_MEMCPY(
                                alt_carrier_record->auxiliary_data_ref[count]
                                    ->data,
                                &payload[index],
                                alt_carrier_record->auxiliary_data_ref[count]
                                    ->data_length);
                            index +=
                                alt_carrier_record->auxiliary_data_ref[count]
                                    ->data_length;
                            count++;
                        }
                        else
                        {
                            status = IFX_ERROR(IFX_RECORD_HANDLER_AC,
                                               IFX_RECORD_HANDLER_AC_DECODE,
                                               IFX_OUT_OF_MEMORY);
                        }
                    }
                    else
                    {
                        status = IFX_ERROR(IFX_RECORD_HANDLER_AC,
                                           IFX_RECORD_HANDLER_AC_DECODE,
                                           IFX_OUT_OF_MEMORY);
                    }
                }
            }
            else
            {
                status =
                    IFX_ERROR(IFX_RECORD_HANDLER_AC,
                              IFX_RECORD_HANDLER_AC_DECODE, IFX_OUT_OF_MEMORY);
            }
        }
        else
        {
            status = IFX_ERROR(IFX_RECORD_HANDLER_AC,
                               IFX_RECORD_HANDLER_AC_DECODE, IFX_OUT_OF_MEMORY);
        }
    }
    else
    {
        status = IFX_ERROR(IFX_RECORD_HANDLER_AC, IFX_RECORD_HANDLER_AC_DECODE,
                           IFX_OUT_OF_MEMORY);
    }

    if ((payload_length != index) && (IFX_SUCCESS == status))
    {
        status = IFX_ERROR(IFX_RECORD_HANDLER_AC, IFX_RECORD_HANDLER_AC_DECODE,
                           IFX_UNSPECIFIED_ERROR);
    }

    return status;
}
