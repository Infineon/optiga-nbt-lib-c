// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/ifx-apdu.h
 * \brief APDU (response) en-/decoding utility.
 */
#ifndef IFX_APDU_H
#define IFX_APDU_H

#include <stddef.h>
#include <stdint.h>
#include "infineon/ifx-error.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief IFX error encoding library identifier.
 */
#define LIB_APDU                     UINT8_C(0x10)

/**
 * \brief Check APDU response status word(SW) is success(0x9000)
 *  \param[in] response_sw response status word(SW).
 */
#define IFX_CHECK_SW_OK(response_sw) (response_sw == 0x9000)

/**
 * \brief LE value for expecting any number of bytes <= 256.
 *
 * \details The short length APDU LE encoding according to ISO7816-3 Case 2S or
 * Case 4S is a single byte `0x00` meaning 256 bytes.
 */
#define IFX_APDU_LE_ANY              0x100u

/**
 * \brief LE value for expecting any number of bytes <= 65536.
 *
 * \details The extended length APDU LE encoding according to ISO7816-3 Case 2E
 * or Case 4E is a 2 bytes `{0x00, 0x00}` meaning 65536 bytes.
 */
#define IFX_APDU_LE_ANY_EXTENDED     0x10000u

/**
 * \brief IFX error encoding function identifier for ifx_apdu_decode().
 */
#define IFX_APDU_DECODE              0x01u

/**
 * \brief Error reason if LC does not match length of data in ifx_apdu_decode().
 *
 * \details Used in combination with \ref LIB_APDU and \ref IFX_APDU_DECODE so
 * the full result code will always be \c
 * IFX_ERROR(LIB_APDU,IFX_APDU_DECODE,IFX_LC_MISMATCH)
 */
#define IFX_LC_MISMATCH              0x01u

/**
 * \brief Error reason if LC and LE do not use same form (short / extended)
 * length in ifx_apdu_decode().
 *
 * \details Used in combination with \ref LIB_APDU and \ref IFX_APDU_DECODE so
 * the full result code will always be \c
 * IFX_ERROR(LIB_APDU,IFX_APDU_DECODE,IFX_EXTENDED_LEN_MISMATCH)
 */
#define IFX_EXTENDED_LEN_MISMATCH    0x02u

/**
 * \brief IFX error encoding function identifier for ifx_apdu_encode().
 */
#define IFX_APDU_ENCODE              0x02u

/**
 * \brief IFX error encoding function identifier for ifx_apdu_response_decode().
 */
#define IFX_APDU_RESPONSE_DECODE     0x03u

/**
 * \brief IFX error encoding function identifier for ifx_apdu_response_encode().
 */
#define IFX_APDU_RESPONSE_ENCODE     0x04u

/** \struct ifx_apdu_t
 * \brief Data storage for APDU fields.
 */
typedef struct
{
    /**
     * \brief APDU instruction class.
     */
    uint8_t cla;

    /**
     * \brief APDU instruction code.
     */
    uint8_t ins;

    /**
     * \brief First APDU instruction parameter byte.
     */
    uint8_t p1;

    /**
     * \brief Second APDU instruction parameter byte.
     */
    uint8_t p2;

    /**
     * \brief Length of APDU.data.
     */
    size_t lc;

    /**
     * \brief Actual APDU content data (might be \c NULL ).
     */
    uint8_t *data;

    /**
     * \brief Expected number of bytes in response.
     */
    size_t le;
} ifx_apdu_t;

/** \struct ifx_apdu_response_t
 * \brief Data storage struct for APDU response information.
 */
typedef struct
{
    /**
     * \brief Actual response data (might be \c NULL ).
     */
    uint8_t *data;

    /**
     * \brief Number of bytes in ifx_apdu_response_t.data.
     */
    size_t len;

    /**
     * \brief APDU response status word.
     */
    uint16_t sw;
} ifx_apdu_response_t;

/**
 * \brief Encodes APDU to its binary representation.
 *
 * \param[in] apdu APDU to be encoded.
 * \param[out] buffer Buffer to store encoded data in.
 * \param[out] buffer_len Pointer for storing number of bytes in \p buffer.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other value in
 * case of error.
 * \relates ifx_apdu_t
 */
ifx_status_t ifx_apdu_encode(const ifx_apdu_t *apdu, uint8_t **buffer,
                             size_t *buffer_len);

/**
 * \brief Decodes binary data to its member representation in APDU object.
 *
 * \param[out] apdu APDU object to store values in.
 * \param[in] data Binary data to be decoded.
 * \param[in] data_len Number of bytes in \p data.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other value in
 * case of error.
 * \relates ifx_apdu_t
 */
ifx_status_t ifx_apdu_decode(ifx_apdu_t *apdu, const uint8_t *data,
                             size_t data_len);

/**
 * \brief Frees memory associated with APDU object (but not object itself).
 *
 * \details APDU objects may contain dynamically allocated data (e.g.
 * APDU.data). Users would need to manually check which members have been
 * dynamically allocated and free them themselves. Calling this function will
 * ensure that all dynamically allocated members have been freed.
 *
 * \param[in] apdu APDU object whose data shall be freed.
 * \relates ifx_apdu_t
 */
void ifx_apdu_destroy(ifx_apdu_t *apdu);

/**
 * \brief Decodes binary data to its member representation in
 * ifx_apdu_response_t object.
 *
 * \param[out] response ifx_apdu_response_t object to store values in.
 * \param[in] data Binary data to be decoded.
 * \param[in] data_len Number of bytes in \p data.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 * \relates ifx_apdu_response_t
 */
ifx_status_t ifx_apdu_response_decode(ifx_apdu_response_t *response,
                                      const uint8_t *data, size_t data_len);

/**
 * \brief Encodes ifx_apdu_response_t to its binary representation.
 *
 * \param[in] response ifx_apdu_response_t to be encoded.
 * \param[out] buffer Buffer to store encoded data in.
 * \param[out] buffer_len Pointer for storing number of bytes in \p buffer.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 * \relates ifx_apdu_response_t
 */
ifx_status_t ifx_apdu_response_encode(const ifx_apdu_response_t *response,
                                      uint8_t **buffer, size_t *buffer_len);

/**
 * \brief Frees memory associated with ifx_apdu_response_t object (but not
 * object itself).
 *
 * \details ifx_apdu_response_t objects will most likely be populated by
 * ifx_apdu_response_decode(). Users would need to manually check which members
 * have been dynamically allocated and free them themselves. Calling this
 * function will ensure that all dynamically allocated members have been freed.
 *
 * \param[in] response ifx_apdu_response_t object whose data shall be freed.
 * \relates ifx_apdu_response_t
 */
void ifx_apdu_response_destroy(ifx_apdu_response_t *response);

#ifdef __cplusplus
}
#endif

#endif // IFX_APDU_H
