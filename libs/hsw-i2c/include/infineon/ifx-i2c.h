// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/ifx-i2c.h
 * \brief Generic API for I2C drivers
 */
#ifndef IFX_I2C_H
#define IFX_I2C_H

#include <stdint.h>

#include "infineon/ifx-protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief IFX error code library identifier.
 */
#define LIB_I2C                     UINT8_C(0x23)

/**
 * \brief String used as source information for logging.
 */
#define IFX_I2C_LOG_TAG             "IFX_I2C"

/**
 * \brief IFX error encoding function identifier for
 * ifx_i2c_get_clock_frequency().
 */
#define IFX_I2C_GET_CLOCK_FREQUENCY 0x01

/**
 * \brief IFX error encoding function identifier for
 * ifx_ifx_i2c_set_clock_frequency().
 */
#define IFX_I2C_SET_CLOCK_FREQUENCY 0x02

/**
 * \brief IFX error encoding function identifier for
 * ifx_i2c_get_slave_address().
 */
#define IFX_I2C_GET_SLAVE_ADDR      0x03

/**
 * \brief IFX error encoding function identifier for
 * ifx_i2c_set_slave_address().
 */
#define IFX_I2C_SET_SLAVE_ADDR      0x04

/**
 * \brief IFX error encoding function identifier for ifx_i2c_get_guard_time().
 */
#define IFX_I2C_GET_GUARD_TIME      0x05

/**
 * \brief IFX error encoding function identifier for
 * ifx_ifx_i2c_set_guard_time().
 */
#define IFX_I2C_SET_GUARD_TIME      0x06

/**
 * \brief Getter for I2C clock frequency in [Hz].
 *
 * \param[in] self Protocol object to get clock frequency for.
 * \param[out] frequency_hz_buffer Buffer to store clock frequency in.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 * \cond FULL_DOCUMENTATION_BUILD
 * \relates ifx_protocol
 * \endcond
 */
ifx_status_t ifx_i2c_get_clock_frequency(ifx_protocol_t *self,
                                         uint32_t *frequency_hz_buffer);

/**
 * \brief Sets I2C clock frequency in [Hz].
 *
 * \param[in] self Protocol object to set clock frequency for.
 * \param[in] frequency_hz Desired clock frequency in [Hz].
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 * \cond FULL_DOCUMENTATION_BUILD
 * \relates ifx_protocol
 * \endcond
 */
ifx_status_t ifx_i2c_set_clock_frequency(ifx_protocol_t *self,
                                         uint32_t frequency_hz);

/**
 * \brief Getter for I2C slave address.
 *
 * \param[in] self Protocol object to get I2C slave address for.
 * \param[out] address_buffer Buffer to store I2C address in.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 * \cond FULL_DOCUMENTATION_BUILD
 * \relates ifx_protocol
 * \endcond
 */
ifx_status_t ifx_i2c_get_slave_address(ifx_protocol_t *self,
                                       uint16_t *address_buffer);

/**
 * \brief Sets I2C slave address.
 *
 * \param[in] self Protocol object to set I2C slave address for.
 * \param[out] address Desired I2C slave address.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 * \cond FULL_DOCUMENTATION_BUILD
 * \relates ifx_protocol
 * \endcond
 */
ifx_status_t ifx_i2c_set_slave_address(ifx_protocol_t *self, uint16_t address);

/**
 * \brief Getter for I2C guard time in [us].
 *
 * \details Some peripherals have a guard time that needs to be waited
 * between consecutive I2C requests. Setting this guard time will ensure that
 * said time is awaited between requests.
 *
 * \param[in] self Protocol object to get I2C guard time for.
 * \param[out] guard_time_us_buffer Buffer to store I2C guard time in.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 * \cond FULL_DOCUMENTATION_BUILD
 * \relates ifx_protocol
 * \endcond
 */
ifx_status_t ifx_i2c_get_guard_time(ifx_protocol_t *self,
                                    uint32_t *guard_time_us_buffer);

/**
 * \brief Sets guard time to be waited between I2C transmissions.
 *
 * \param[in] self Protocol object to set I2C guard time for.
 * \param[in] guard_time_us Desired I2C guard time in [us].
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 * \see ifx_i2c_get_guard_time()
 * \cond FULL_DOCUMENTATION_BUILD
 * \relates ifx_protocol
 * \endcond
 */
ifx_status_t ifx_i2c_set_guard_time(ifx_protocol_t *self,
                                    uint32_t guard_time_us);

#ifdef __cplusplus
}
#endif

#endif // IFX_I2C_H
