// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/ifx-timer.h
 * \brief Generic C API for joinable timers.
 */
#ifndef IFX_TIMER_H
#define IFX_TIMER_H

#include <stdbool.h>
#include <stdint.h>

#include "infineon/ifx-error.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief IFX error encoding library identifier.
 */
#define LIB_TIMER         UINT8_C(0x02)

/**
 * \brief IFX error encoding function identifier for ifx_timer_set().
 */
#define IFX_TIMER_SET     UINT8_C(0x01)

/**
 * \brief IFX error encoding function identifier for ifx_timer_join().
 */
#define IFX_TIMER_JOIN    UINT8_C(0x02)

/**
 * \brief Error reason if timer has not been set before calling timer_join().
 *
 * \details Used in combination with \ref LIB_TIMER and \ref IFX_TIMER_JOIN so
 * the full result code will always be \c IFX_ERROR(LIB_TIMER, IFX_TIMER_JOIN,
 * IFX_TIMER_NOT_SET).
 */
#define IFX_TIMER_NOT_SET 0x01u

/** \struct ifx_timer_t
 * \brief Generic struct for joinable timers.
 */
typedef struct
{
    /**
     * \brief Private member for start of timer as \c void* to be as generic as
     * possible.
     *
     * \details Set by ifx_timer_set(), do **NOT** set manually!
     */
    void *_start;

    /**
     * \brief Private member for duration of timer in [us].
     *
     * \details Set by ifx_timer_set(), do **NOT** set manually!
     */
    uint64_t _duration;
} ifx_timer_t;

/**
 * \brief Sets Timer for given amount of [us].
 *
 * \param[in] timer Timer object to be set.
 * \param[in] time_us us Timer duration in [us].
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other value in
 * case of error.
 * \relates ifx_timer_t
 */
ifx_status_t ifx_timer_set(ifx_timer_t *timer, uint64_t time_us);

/**
 * \brief Checks if Timer has elapsed.
 *
 * \details Per definition timers that have not previously been set are
 * considered elapsed.
 *
 * \param[in] timer Timer object to be checked.
 * \return bool \c true if timer has elapsed.
 * \relates ifx_timer_t
 */
bool ifx_timer_has_elapsed(const ifx_timer_t *timer);

/**
 * \brief Waits for Timer to finish.
 *
 * \param[in] timer Timer to be joined (wait until finished).
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other value in
 * case of error.
 * \relates ifx_timer_t
 */
ifx_status_t ifx_timer_join(const ifx_timer_t *timer);

/**
 * \brief Frees memory associated with Timer object (but not object itself).
 *
 * \details Timer objects may contain dynamically allocated data that needs
 * special functionality to be freed. Users would need to know the concrete type
 * based on the linked implementation. This would negate all benefits of having
 * a generic interface. Calling this function will ensure that all dynamically
 * allocated members have been freed.
 *
 * \param[in] timer Timer object whose data shall be freed.
 * \relates ifx_timer_t
 */
void ifx_timer_destroy(ifx_timer_t *timer);

#ifdef __cplusplus
}
#endif

#endif // IFX_TIMER_H
