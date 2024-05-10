// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file ifx-crc.c
 * \brief Reusable CRC algorithms.
 */
#include "infineon/ifx-crc.h"

/**
 * \brief Calculates 16 bit CRC according to CCITT x.25 specification.
 *
 * \param[in] data Data to calculate CRC over.
 * \param[in] data_len Number of bytes in \p data.
 * \return uint16_t CRC over data.
 */
uint16_t ifx_crc16_ccitt_x25(const uint8_t *data, size_t data_len)
{
    uint16_t crc = 0xffffU;
    for (size_t i = 0U; i < data_len; i++)
    {
        crc ^= data[i];
        for (size_t j = 0U; j < 8U; j++)
        {
            if ((crc & 1U) != 0U)
            {
                crc = (crc >> 1) ^ 0x8408U;
            }
            else
            {
                crc = crc >> 1U;
            }
        }
    }
    return crc ^ 0xffffU;
}

/**
 * \brief Calculates 16 bit CRC according to MCRF4xx specification.
 *
 * \param[in] data Data to calculate CRC over.
 * \param[in] data_len Number of bytes in \p data.
 * \return uint16_t CRC over data.
 */
uint16_t ifx_crc16_mcrf4xx(const uint8_t *data, size_t data_len)
{
    uint16_t crc = 0xffffU;
    for (size_t i = 0U; i < data_len; i++)
    {
        crc ^= data[i];
        for (size_t j = 0U; j < 8U; j++)
        {
            if ((crc & 1) != 0)
            {
                crc = (crc >> 1) ^ 0x8408U;
            }
            else
            {
                crc = crc >> 1U;
            }
        }
    }
    return crc;
}

/**
 * \brief Calculates 16 bit CRC according to G+D T=1 protocol specification.
 *
 * \param[in] data Data to calculate CRC over.
 * \param[in] data_len Number of bytes in \p data.
 * \return uint16_t CRC over data.
 */
uint16_t ifx_crc16_t1gd(const uint8_t *data, size_t data_len)
{
    uint16_t crc = 0xffffU;
    for (size_t i = 0U; i < data_len; i++)
    {
        uint8_t read_byte = data[i];
        for (size_t j = 0U; j < 8U; j++)
        {
            if ((crc ^ read_byte) & 0x01U)
            {
                crc ^= 0x10810U;
            }
            crc >>= 1U;
            read_byte >>= 1U;
        }
    }
    return crc;
}

/**
 * \brief Calculates 8 bit Longitudinal Redundancy Code (LRC).
 *
 * \param[in] data Data to calculate LRC over.
 * \param[in] data_len Number of bytes in \p data.
 * \return uint8_t LRC over data.
 */
uint8_t ifx_lrc8(const uint8_t *data, size_t data_len)
{
    uint8_t lrc = 0x00U;
    for (size_t i = 0U; i < data_len; i++)
    {
        lrc ^= data[i];
    }
    return lrc;
}
