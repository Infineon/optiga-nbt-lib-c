// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/ifx-crc.h
 * \brief Reusable CRC algorithms.
 */
#ifndef IFX_CRC_H
#define IFX_CRC_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Calculates 16 bit CRC according to CCITT x.25 specification.
 *
 * \param[in] data Data to calculate CRC over.
 * \param[in] data_len Number of bytes in \p data.
 * \return uint16_t CRC over data.
 */
uint16_t ifx_crc16_ccitt_x25(const uint8_t *data, size_t data_len);

/**
 * \brief Calculates 16 bit CRC according to MCRF4xx specification.
 *
 * \param[in] data Data to calculate CRC over.
 * \param[in] data_len Number of bytes in \p data.
 * \return uint16_t CRC over data.
 */
uint16_t ifx_crc16_mcrf4xx(const uint8_t *data, size_t data_len);

/**
 * \brief Calculates 16 bit CRC according to G+D T=1 protocol specification.
 *
 * \param[in] data Data to calculate CRC over.
 * \param[in] data_len Number of bytes in \p data.
 * \return uint16_t CRC over data.
 */
uint16_t ifx_crc16_t1gd(const uint8_t *data, size_t data_len);

/**
 * \brief Calculates 8 bit Longitudinal Redundancy Code (LRC).
 *
 * \param[in] data Data to calculate LRC over.
 * \param[in] data_len Number of bytes in \p data.
 * \return uint8_t LRC over data.
 */
uint8_t ifx_lrc8(const uint8_t *data, size_t data_len);

#ifdef __cplusplus
}
#endif

#endif // IFX_CRC_H
