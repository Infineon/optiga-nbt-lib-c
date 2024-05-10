// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file ifx-i2c-mock.c
 * \brief Mock implementation of I2C interface.
 */
#include <stddef.h>

#include "infineon/ifx-i2c.h"

/**
 * \brief Mock implementation of interface function.
 */
ifx_status_t ifx_i2c_get_clock_frequency(ifx_protocol_t *self,
                                         uint32_t *frequency_hz_buffer)
{
    if ((self == NULL) || (frequency_hz_buffer == NULL))
    {
        return IFX_ERROR(LIB_I2C, IFX_I2C_GET_CLOCK_FREQUENCY,
                         IFX_ILLEGAL_ARGUMENT);
    }
    // Update with mock value
    *frequency_hz_buffer = 0x00;
    return IFX_SUCCESS;
}

/**
 * \brief Mock implementation of interface function.
 */
ifx_status_t ifx_i2c_set_clock_frequency(ifx_protocol_t *self,
                                         uint32_t frequency_hz)
{
    // Ignore unused parameter
    (void) frequency_hz;

    if (self == NULL)
    {
        return IFX_ERROR(LIB_I2C, IFX_I2C_SET_CLOCK_FREQUENCY,
                         IFX_ILLEGAL_ARGUMENT);
    }
    return IFX_SUCCESS;
}

/**
 * \brief Mock implementation of interface function.
 */
ifx_status_t ifx_i2c_get_slave_address(ifx_protocol_t *self,
                                       uint16_t *address_buffer)
{
    if ((self == NULL) || (address_buffer == NULL))
    {
        return IFX_ERROR(LIB_I2C, IFX_I2C_GET_SLAVE_ADDR, IFX_ILLEGAL_ARGUMENT);
    }
    // Update with mock value
    *address_buffer = 0x00;
    return IFX_SUCCESS;
}

/**
 * \brief Mock implementation of interface function.
 */
ifx_status_t ifx_i2c_set_slave_address(ifx_protocol_t *self, uint16_t address)
{
    // Ignore unused parameter
    (void) address;

    if (self == NULL)
    {
        return IFX_ERROR(LIB_I2C, IFX_I2C_SET_SLAVE_ADDR, IFX_ILLEGAL_ARGUMENT);
    }
    return IFX_SUCCESS;
}

/**
 * \brief Mock implementation of interface function.
 */
ifx_status_t ifx_i2c_get_guard_time(ifx_protocol_t *self,
                                    uint32_t *guard_time_us_buffer)
{
    if ((self == NULL) || (guard_time_us_buffer == NULL))
    {
        return IFX_ERROR(LIB_I2C, IFX_I2C_GET_GUARD_TIME, IFX_ILLEGAL_ARGUMENT);
    }
    // Update with mock value
    *guard_time_us_buffer = 0x00;
    return IFX_SUCCESS;
}

/**
 * \brief Mock implementation of interface function.
 */
ifx_status_t ifx_i2c_set_guard_time(ifx_protocol_t *self,
                                    uint32_t guard_time_us)
{
    // Ignore unused parameter
    (void) guard_time_us;

    if (self == NULL)
    {
        return IFX_ERROR(LIB_I2C, IFX_I2C_SET_GUARD_TIME, IFX_ILLEGAL_ARGUMENT);
    }
    return IFX_SUCCESS;
}
