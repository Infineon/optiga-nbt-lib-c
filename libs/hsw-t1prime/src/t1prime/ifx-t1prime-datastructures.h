// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file t1prime/ifx-t1prime-datastructures.h
 * \brief Global Platform T=1' data structure definitions.
 */
#ifndef IFX_T1PRIME_DATASTRUCTURES_H
#define IFX_T1PRIME_DATASTRUCTURES_H

#include <stdbool.h>
#include <stdint.h>
#include "infineon/ifx-error.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Fixed number of bytes in Block prologue.
 */
#define IFX_BLOCK_PROLOGUE_LEN         (1u + 1u + 2u)

/**
 * \brief Fixed number of bytes in Block epilogue.
 */
#define IFX_BLOCK_EPILOGUE_LEN         2u

/**
 * \brief IFX error encoding function identifier for ifx_t1prime_block_decode().
 */
#define IFX_T1PRIME_BLOCK_DECODE       0x01u

/**
 * \brief Error reason if information size does not match length of data in
 * ifx_t1prime_block_decode().
 *
 * \details Used in combination with \ref LIB_T1PRIME and \ref
 * IFX_T1PRIME_BLOCK_DECODE so the full result code will always be \c
 * IFX_ERROR(LIB_T1PRIME,IFX_T1PRIME_BLOCK_DECODE,IFX_INFORMATION_SIZE_MISMATCH).
 */
#define IFX_INFORMATION_SIZE_MISMATCH  0x01u

/**
 * \brief Error reason if CRC does not match data in ifx_t1prime_block_decode().
 *
 * \details Used in combination with \ref LIB_T1PRIME and \ref
 * IFX_T1PRIME_BLOCK_DECODE so the full result code will always be \c
 * IFX_ERROR(LIB_T1PRIME,IFX_T1PRIME_BLOCK_DECODE,IFX_T1PRIME_INVALID_CRC).
 */
#define IFX_T1PRIME_INVALID_CRC        0x02u

/**
 * \brief IFX error encoding function identifier for ifx_t1prime_block_encode().
 */
#define IFX_T1PRIME_BLOCK_ENCODE       0x02u
/**
 * \brief Physical layer identifier for SPI in CIP.
 */
#define IFX_T1PRIME_PLID_SPI           0x01u

/**
 * \brief Physical layer identifier for SPI in CIP.
 */
#define IFX_T1PRIME_PLID_I2C           0x02u

/**
 * \brief Error reason if any length information does not match during
 * ifx_t1prime_cip_decode() or ifx_t1prime_cip_validate().
 */
#define IFX_T1PRIME_INVALID_LEN        0x01u

/**
 * \brief Error reason if invalid physical layer identifier detected during
 * ifx_t1prime_cip_decode() or ifx_t1prime_cip_validate().
 */
#define IFX_T1PRIME_INVALID_PLID       0x02u

/**
 * \brief IFX error encoding function identifier for ifx_t1prime_cip_decode().
 */
#define IFX_T1PRIME_CIP_DECODE         0x03u

/**
 * \brief IFX error encoding function identifier for ifx_t1prime_cip_validate().
 */
#define IFX_T1PRIME_CIP_VALIDATE       0x04u

/**
 * \brief IFX error encoding function identifier for ifx_t1prime_dllp_decode().
 */
#define IFX_T1PRIME_DLLP_DECODE        0x05u

/**
 * \brief IFX error encoding function identifier for ifx_t1prime_plp_decode().
 */
#define IFX_T1PRIME_PLP_DECODE         0x06u

/**
 * \brief Maximum allowed information field size.
 */
#define IFX_T1PRIME_MAX_IFS            0xff9u

/**
 * \brief Return code for successful calls to ifx_t1prime_ifs_decode().
 */
#define IFX_T1PRIME_IFS_DECODE         0x07u

/**
 * \brief IFX error encoding function identifier for ifx_t1prime_ifs_encode().
 */
#define IFX_T1PRIME_IFS_ENCODE         0x08u

/**
 * \brief Return code for successful calls to ifx_t1prime_get_ifsc().
 */
#define IFX_T1PRIME_GET_IFSC           0x09u

/**
 * \brief IFX error code function identifier for ifx_t1prime_set_ifsd().
 */
#define IFX_T1PRIME_SET_IFSD           0x0Au

/**
 * \brief IFX error encoding function identifier for ifx_t1prime_set_bwt().
 */
#define IFX_T1PRIME_SET_BWT            0x0Bu

/**
 * \brief Return code for successful calls to ifx_t1prime_get_bwt().
 */
#define IFX_T1PRIME_GET_BWT            0x0Cu

/**
 * \brief IFX error encoding function identifier for
 * ifx_t1prime_set_irq_handler().
 */
#define IFX_T1PRIME_SET_IRQ_HANDLER    0x0Du

/**
 * \brief Return code for successful calls to ifx_t1prime_get_irq_handler().
 */
#define IFX_T1PRIME_GET_IRQ_HANDLER    0x0Eu

/**
 * \brief IFX error encoding function identifier for
 * ifx_t1prime_get_protocol_state().
 */
#define IFX_T1PRIME_GET_PROTOCOL_STATE 0x0Fu

/** \struct ifx_t1prime_block_t
 * \brief Data storage for a Global Platform T=1' block.
 */
typedef struct
{
    /**
     * \brief Node address byte (NAD) with routing information.
     */
    uint8_t nad;

    /**
     * \brief Protocol control byte (PCB).
     */
    uint8_t pcb;

    /**
     * \brief Number of bytes in Block.information.
     */
    size_t information_size;

    /**
     * \brief Block data in information field.
     */
    uint8_t *information;
} ifx_t1prime_block_t;

/**
 * \brief Decodes binary data to its member representation in Block object.
 *
 * \param[out] block Block object to store values in.
 * \param[in] data Binary data to be decoded.
 * \param[in] data_len Number of bytes in \p data.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
ifx_status_t ifx_t1prime_block_decode(ifx_t1prime_block_t *block,
                                      const uint8_t *data, size_t data_len);

/**
 * \brief Encodes Block to its binary representation.
 *
 * \param[in] block Block to be encoded.
 * \param[out] buffer Buffer to store encoded data in.
 * \param[out] buffer_len Pointer for storing number of bytes in \p buffer.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
ifx_status_t ifx_t1prime_block_encode(const ifx_t1prime_block_t *block,
                                      uint8_t **buffer, size_t *buffer_len);

/**
 * \brief Frees memory associated with Block object (but not object itself).
 *
 * \details Block objects may contain dynamically allocated data (e.g. by
 * ifx_t1prime_block_decode() ). Users would need to manually check which
 * members have been dynamically allocated and free them themselves. Calling
 * this function will ensure that all dynamically allocated members have been
 * freed.
 *
 * \param[in] block Block object whose data shall be freed.
 */
void ifx_t1prime_block_destroy(ifx_t1prime_block_t *block);

/**
 * \brief Checks if CRC matches for Block object.
 *
 * \param[in] block Block object to match CRC against.
 * \param[in] expected Expected CRC to be matched.
 * \return bool \c true if CRC matches, \c false otherwise.
 */
bool ifx_t1prime_validate_crc(const ifx_t1prime_block_t *block,
                              uint16_t expected);

/** \struct ifx_cip_t
 * \brief Data storage for a Global Platform T=1' Communication Interface
 * Parameters (CIP).
 */
typedef struct
{
    /**
     * \brief Protocol version.
     */
    uint8_t version;

    /**
     * \brief Number of bytes in CIP.iin.
     */
    uint8_t iin_len;

    /**
     * \brief Issuer identification number.
     */
    uint8_t *iin;

    /**
     * \brief Physical layer identifier.
     */
    uint8_t plid;

    /**
     * \brief Number of bytes in CIP.plp.
     */
    uint8_t plp_len;

    /**
     * \brief Physical layer parameters.
     */
    uint8_t *plp;

    /**
     * \brief Number of bytes in CIP.dllp.
     */
    uint8_t dllp_len;

    /**
     * \brief Data-link layer parameters.
     */
    uint8_t *dllp;

    /**
     * \brief Number of bytes in CIP.hb.
     */
    uint8_t hb_len;

    /**
     * \brief Optional historical bytes.
     */
    uint8_t *hb;
} ifx_cip_t;

/**
 * \brief Decodes binary data to its member representation in CIP object.
 *
 * \param[out] cip CIP object to store values in.
 * \param[in] data Binary data to be decoded.
 * \param[in] data_len Number of bytes in \p data.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
ifx_status_t ifx_t1prime_cip_decode(ifx_cip_t *cip, const uint8_t *data,
                                    size_t data_len);

/**
 * \brief Validates CIP object by checking all member values against GP
 * specification.
 *
 * \param[in] cip CIP object to be validated.
 * \return ifx_status_t \c IFX_SUCCESS if valid CIP, any other
 * value in case of error.
 */
ifx_status_t ifx_t1prime_cip_validate(const ifx_cip_t *cip);

/**
 * \brief Frees memory associated with CIP object (but not object itself)
 *
 * \details CIP objects will most likely be populated by
 * ifx_t1prime_cip_decode() . Users would need to manually check which members
 * have been dynamically allocated and free them themselves. Calling this
 * function will ensure that all dynamically allocated members have been freed.
 *
 * \code
 *      uint8_t encoded[] = {...};
 *      size_t encoded_len = sizeof(encoded);
 *      ifx_cip_t cip;
 *      // Might allocate several buffers
 *      ifx_t1prime_cip_decode(&cip, encoded, encoded_len);
 *      ifx_t1prime_cip_destroy(&cip);
 * \endcode
 *
 * \param[in] cip CIP object whose data shall be freed.
 */
void ifx_t1prime_cip_destroy(ifx_cip_t *cip);

/** \struct ifx_dllp_t
 * \brief Data storage for a Global Platform T=1' data-link layer parameters
 * (ifx_dllp_t).
 */
typedef struct
{
    /**
     * \brief Block waiting time in [ms].
     */
    uint16_t bwt;

    /**
     * \brief Maximum information field size of secure element.
     */
    uint16_t ifsc;
} ifx_dllp_t;

/**
 * \brief Decodes binary data to its member representation in DLLP object.
 *
 * \param[out] dllp DLLP object to store values in.
 * \param[in] data Binary data to be decoded.
 * \param[in] data_len Number of bytes in \p data.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
ifx_status_t ifx_t1prime_dllp_decode(ifx_dllp_t *dllp, const uint8_t *encoded,
                                     size_t encoded_len);

/**
 * \brief Frees memory associated with DLLP object (but not object itself).
 *
 * \details This function does nothing at the moment but keeps the interface
 * uniform across all datastructures.
 *
 * \param[in] dllp DLLP object whose data shall be freed.
 */
void ifx_t1prime_dllp_destroy(ifx_dllp_t *dllp);

#ifdef IFX_T1PRIME_INTERFACE_I2C
/** \struct ifx_plp_t
 * \brief Data storage for a Global Platform T=1' I2C phyiscal layer parameters.
 */
typedef struct
{
    /**
     * \brief RFU.
     */
    uint8_t configuration;

    /**
     * \brief Power wake-up time in [ms].
     */
    uint8_t pwt;

    /**
     * \brief Maximum clock frequency in [kHz].
     */
    uint16_t mcf;

    /**
     * \brief Power saving timeout in [ms].
     */
    uint8_t pst;

    /**
     * \brief Minimum polling time in [multiple of 100us].
     */
    uint8_t mpot;

    /**
     * \brief Read / write guard time in [us].
     */
    uint16_t rwgt;
} ifx_plp_t;

/**
 * \brief Decodes binary data to its member representation in PLP object.
 *
 * \param[out] plp PLP object to store values in.
 * \param[in] data Binary data to be decoded.
 * \param[in] data_len Number of bytes in \p data.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
ifx_status_t ifx_t1prime_plp_decode(ifx_plp_t *plp, const uint8_t *encoded,
                                    size_t encoded_len);

/**
 * \brief Frees memory associated with PLP object (but not object itself).
 *
 * \details This function does nothing at the moment but keeps the interface
 * uniform across all datastructures.
 *
 * \param[in] plp PLP object whose data shall be freed.
 */
void ifx_t1prime_plp_destroy(ifx_plp_t *plp);

#else
/** \struct ifx_plp_t
 * \brief Data storage for a Global Platform T=1' SPI phyiscal layer parameters.
 */
typedef struct
{
    /**
     * \brief RFU.
     */
    uint8_t configuration;

    /**
     * \brief Power wake-up time in [ms].
     */
    uint8_t pwt;

    /**
     * \brief Maximum clock frequency in [kHz].
     */
    uint16_t mcf;

    /**
     * \brief Power saving timeout in [ms].
     */
    uint8_t pst;

    /**
     * \brief Minimum polling time in [multiple of 100us].
     */
    uint8_t mpot;

    /**
     * \brief Secure element guard time in [ms].
     */
    uint16_t segt;

    /**
     * \brief Maximum secure element access length in [byte].
     */
    uint16_t seal;

    /**
     * \brief Wake-up time in [ms].
     */
    uint16_t wut;
} ifx_plp_t;

/**
 * \brief Decodes binary data to its member representation in PLP object.
 *
 * \param[out] plp PLP object to store values in.
 * \param[in] data Binary data to be decoded.
 * \param[in] data_len Number of bytes in \p data.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
ifx_status_t ifx_t1prime_plp_decode(ifx_plp_t *plp, const uint8_t *encoded,
                                    size_t encoded_len);

/**
 * \brief Frees memory associated with PLP object (but not object itself).
 *
 * \details This function does nothing at the moment but keeps the interface
 * uniform accross all datastructures.
 *
 * \param[in] plp PLP object whose data shall be freed.
 */
void ifx_t1prime_plp_destroy(ifx_plp_t *plp);
#endif

/**
 * \brief Decodes binary information field size (IFS).
 *
 * \param[out] ifs_buffer Buffer to store IFS value in.
 * \param[in] data Binary IFS data.
 * \param[in] data_len Number of bytes in \p data.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
ifx_status_t ifx_t1prime_ifs_decode(size_t *ifs_buffer, const uint8_t *data,
                                    size_t data_len);

/**
 * \brief Encodes information field size (IFS) to its binary representation.
 *
 * \param[in] ifs IFS value to be encoded.
 * \param[out] buffer Buffer to store encoded data in.
 * \param[out] buffer_len Number of bytes in \p buffer.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
ifx_status_t ifx_t1prime_ifs_encode(size_t ifs, uint8_t **buffer,
                                    size_t *buffer_len);

/** \struct ifx_t1prime_protocol_state_t
 * \brief State of T=1' protocol keeping track of sequence counters, information
 * field sizes, etc.
 */
typedef struct
{
    /**
     * \brief Current block waiting time in [ms].
     */
    uint16_t bwt;

    /**
     * \brief Power wake-up time in [ms].
     */
    uint8_t pwt;

    /**
     * \brief Minimum polling time in [multiple of 100us].
     */
    uint8_t mpot;

    /**
     * \brief Maximum size of secure element information field in [byte].
     */
    size_t ifsc;

    /**
     * \brief Sequence counter of transmitted I(N(S), M) blocks.
     */
    uint8_t send_counter;

    /**
     * \brief Sequence counter of received I(N(S), M) blocks.
     */
    uint8_t receive_counter;

    /**
     * \brief Waiting time extension if secure element cannot respond in time.
     */
    uint32_t wtx;

    /**
     * \brief Interrupt handler used to detect data ready events.
     *
     * \details Set to \c NULL to use polling mode.
     */
    ifx_t1prime_irq_handler_t irq_handler;
} ifx_t1prime_protocol_state_t;

#ifdef __cplusplus
}
#endif

#endif // IFX_T1PRIME_DATASTRUCTURES_H
