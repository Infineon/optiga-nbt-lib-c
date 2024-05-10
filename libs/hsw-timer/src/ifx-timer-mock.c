// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file ifx-timer-mock.c
 * \brief Mock implementation of Timer interface.
 */
#include <stddef.h>

#include "infineon/ifx-timer.h"

/**
 * \brief Mock implementation of interface function.
 */
ifx_status_t ifx_timer_set(ifx_timer_t *timer, uint64_t time_us)
{
    // Ignore unused parameter
    (void) time_us;

    if (timer == NULL)
    {
        return IFX_ERROR(LIB_TIMER, IFX_TIMER_SET, IFX_ILLEGAL_ARGUMENT);
    }
    return IFX_SUCCESS;
}

/**
 * \brief Mock implementation of interface function.
 */
bool ifx_timer_has_elapsed(const ifx_timer_t *timer)
{
    // Ignore unused parameter
    (void) timer;

    return true;
}

/**
 * \brief Mock implementation of interface function.
 */
ifx_status_t ifx_timer_join(const ifx_timer_t *timer)
{
    if (timer == NULL)
    {
        return IFX_ERROR(LIB_TIMER, IFX_TIMER_JOIN, IFX_ILLEGAL_ARGUMENT);
    }
    return IFX_SUCCESS;
}

/**
 * \brief Mock implementation of interface function.
 */
void ifx_timer_destroy(ifx_timer_t *timer)
{
    // Ignore unused parameter
    (void) timer;
}
