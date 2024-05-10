// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/ifx-protocol.h
 * \brief Generic protocol API (ISO/OSI stack).
 */
#ifndef IFX_PROTOCOL_H
#define IFX_PROTOCOL_H

#include <stddef.h>
#include <stdint.h>

#include "infineon/ifx-error.h"
#include "infineon/ifx-logger.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Protocol library identifier ID.
 */
#define LIB_PROTOCOL                    UINT8_C(0x20)

/**
 * \brief IFX error encoding function identifier for ifx_protocol_activate() and
 * \ref ifx_protocol_activate_callback_t.
 */
#define IFX_PROTOCOL_ACTIVATE           UINT8_C(0x01)

/**
 * \brief IFX error encoding function identifier for
 * \ref ifx_protocol_transmit_callback_t.
 */
#define IFX_PROTOCOL_TRANSMIT           UINT8_C(0x02)

/**
 * \brief IFX error encoding function identifier for
 * \ref ifx_protocol_receive_callback_t.
 */
#define IFX_PROTOCOL_RECEIVE            UINT8_C(0x03)

/**
 * \brief Indicator for unknown length in \ref ifx_protocol_receive_callback_t.
 */
#define IFX_PROTOCOL_RECEIVE_LEN_UNKOWN SIZE_MAX

/**
 * \brief IFX error encoding function identifier for
 * ifx_protocol_layer_initialize().
 */
#define IFX_PROTOCOL_LAYER_INITIALIZE   UINT8_C(0x04)

/**
 * \brief Return code for successful calls to ifx_protocol_transceive() and
 * \ref ifx_protocol_transceive_callback_t.
 */
#define IFX_PROTOCOL_TRANSCEIVE         UINT8_C(0x05)

/**
 * \brief Function independent error reason for invalid protocol stack (missing
 * required function).
 */
#define IFX_PROTOCOL_STACK_INVALID      UINT8_C(0x01)

// Forward declaration only
typedef struct ifx_protocol ifx_protocol_t;

/**
 * \brief Protocol layer specific secure element activation function.
 *
 * \details Depending on the protocol, data needs to be exchanged with a secure
 * element to negotiate certain protocol aspects like frame sizes, waiting
 * times, etc.
 *
 * \param[in] self Protocol stack for performing necessary operations.
 * \param[out] response Buffer to store response in (e.g. ATR, ATPO, ...).
 * \param[out] response_len Buffer to store number of received bytes in (number
 * of bytes in \p response).
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
typedef ifx_status_t (*ifx_protocol_activate_callback_t)(ifx_protocol_t *self,
                                                         uint8_t **response,
                                                         size_t *response_len);

/**
 * \brief Activates secure element and performs protocol negotiation.
 *
 * \details Depending on the protocol, data needs to be exchanged with a secure
 * element to negotiate certain protocol aspects like frame sizes, waiting
 * times, etc. This function will perform the initial parameter negotiation.
 *
 * \param[in] self Protocol stack for performing necessary operations.
 * \param[out] response Buffer to store response in (e.g. ATR, ATPO, ...).
 * \param[out] response_len Buffer to store number of received bytes in (number
 * of bytes in \p response).
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 * \relates ifx_protocol
 */
ifx_status_t ifx_protocol_activate(ifx_protocol_t *self, uint8_t **response,
                                   size_t *response_len);

/**
 * \brief Protocol layer specific transceive (send + receive) function.
 *
 * \param[in] self Protocol stack for performing necessary operations.
 * \param[in] data Data to be send via protocol.
 * \param[in] data_len Number of bytes in \p data.
 * \param[out] response Buffer to store response in.
 * \param[out] response_len Buffer to store number of received bytes in (number
 * of bytes in \p response ).
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
typedef ifx_status_t (*ifx_protocol_transceive_callback_t)(
    ifx_protocol_t *self, const uint8_t *data, size_t data_len,
    uint8_t **response, size_t *response_len);

/**
 * \brief Sends data via Protocol and reads back response.
 *
 * \details Goes through ISO/OSI protocol stack and performs necessary protocol
 * operations (chaining, crc, ...).
 *
 * \param[in] self Protocol stack for performing necessary operations.
 * \param[in] data Data to be send via protocol.
 * \param[in] data_len Number of bytes in \p data.
 * \param[out] response Buffer to store response in.
 * \param[out] response_len Buffer to store number of received bytes in (number
 * of bytes in \p response ).
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 * \relates ifx_protocol
 */
ifx_status_t ifx_protocol_transceive(ifx_protocol_t *self, const uint8_t *data,
                                     size_t data_len, uint8_t **response,
                                     size_t *response_len);

/**
 * \brief Protocol layer specific transmit function.
 *
 * \param[in] self Protocol stack for performing necessary operations.
 * \param[in] data Data to be send via protocol.
 * \param[in] data_len Number of bytes in \p data.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
typedef ifx_status_t (*ifx_protocol_transmit_callback_t)(ifx_protocol_t *self,
                                                         const uint8_t *data,
                                                         size_t data_len);

/**
 * \brief Protocol layer specific receive function.
 *
 * \param[in] self Protocol stack for performing necessary operations.
 * \param[in] expected_len Expected number of bytes in response (use
 * \c IFX_PROTOCOL_RECEIVE_LEN_UNKOWN if not known before).
 * \param[out] response Buffer to store response in.
 * \param[out] response_len Buffer to store number of received bytes in (number
 * of bytes in \p response ).
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
typedef ifx_status_t (*ifx_protocol_receive_callback_t)(ifx_protocol_t *self,
                                                        size_t expected_len,
                                                        uint8_t **response,
                                                        size_t *response_len);

/**
 * \brief Protocol layer specific destructor.
 *
 * \details Different protocol (layers) need different clean-up functionality.
 * This function type gives a generic interface for performing a clean shutdown.
 *
 * \param[in] self Protocol stack for performing necessary operations.
 */
typedef void (*ifx_protocol_destroy_callback_t)(ifx_protocol_t *self);

/**
 * \brief Frees memory associated with Protocol object (but not object itself).
 *
 * \details Protocol objects can consist of several layers each of which might
 * hold dynamically allocated data that needs special clean-up functionality.
 * Users would need to manually check which members need special care and free
 * them themselves. Calling this function will ensure that all members have been
 * freed properly.
 *
 * \param[in] self Protocol object whose data shall be freed.
 * \relates ifx_protocol
 */
void ifx_protocol_destroy(ifx_protocol_t *self);

/**
 * \brief Sets Logger to be used by Protocol.
 *
 * \details Sets logger for whole protocol stack, so all layers below will also
 * have the logger set.
 *
 * \param[in] self Protocol object to set logger for.
 * \param[in] logger Logger object to be used (might be \c NULL to clear
 * logger).
 * \relates ifx_protocol
 */
void ifx_protocol_set_logger(ifx_protocol_t *self, ifx_logger_t *logger);

/**
 * \brief Initializes Protocol object by setting all members to valid values.
 *
 * \details This function is for protocol stack developers to start from a clean
 * base when initializing a layer in their custom initialization function. It
 * initializes the given Protocol struct to a clean but unusable state (no
 * members set).
 *
 * \param[in] self Protocol object to be initialized.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any
 * other value in case of error.
 */
ifx_status_t ifx_protocol_layer_initialize(ifx_protocol_t *self);

/** \struct ifx_protocol
 * \brief Generic protocol struct for building ISO/OSI layer stack.
 */
struct ifx_protocol
{
    /**
     * \brief Private base layer in ISO/OSI stack.
     *
     * \details Set by implementation's initialization function, do **NOT** set
     * manually!
     */
    ifx_protocol_t *_base;

    /**
     * \brief Private layer identification to verify that correct protocol layer
     * called member functions.
     *
     * \details Set by implementation's initialization function, do **NOT** set
     * manually!
     *
     * \details As C generics cannot differentiate on actual type, this layer ID
     * can be used to differentiate all layers in a full ISO/OSI protocol stack.
     */
    uint64_t _layer_id;

    /**
     * \brief Private protocol activation function for negotiating protocol
     * specific parameters.
     *
     * \details Set by implementation's initialization function, do **NOT** set
     * manually!
     *
     * \details Can be \c NULL if ISO/OSI layer has no specific activation.
     */
    ifx_protocol_activate_callback_t _activate;

    /**
     * \brief Private function for sending and receiving data at once.
     *
     * \details Set by implementation's initialization function, do **NOT** set
     * manually!
     *
     * \details Might be \c NULL in which case Protocol._transmit and
     * Protocol._receive must no be \c NULL.
     */
    ifx_protocol_transceive_callback_t _transceive;

    /**
     * \brief Private function for sending data.
     *
     * \details Set by implementation's initialization function, do **NOT** set
     * manually!
     *
     * \details Might be \c NULL in which case Protocol._transceive must not be
     * \c NULL.
     */
    ifx_protocol_transmit_callback_t _transmit;

    /**
     * \brief Private function for receiving data.
     *
     * \details Set by implementation's initialization function, do **NOT** set
     * manually!
     *
     * \details Might be \c NULL in which case Protocol._transceive must not be
     * \c NULL.
     */
    ifx_protocol_receive_callback_t _receive;

    /**
     * \brief Private destructor if further cleanup is necessary
     *
     * \details Set by implementation's initialization function, do **NOT** set
     * manually!
     *
     * \details ifx_protocol_destroy() will call free() for
     * Protocol._properties. If any further cleanup is necessary implement it in
     * this function, otherwise use \c NULL.
     */
    ifx_protocol_destroy_callback_t _destructor;

    /**
     * \brief Private member for optional Logger.
     *
     * \details Set by ifx_protocol_set_logger(), do **NOT** set manually!
     *
     * \details Might be \c NULL.
     */
    ifx_logger_t *_logger;

    /**
     * \brief Private member for generic properties as \c void*.
     *
     * \details Only used internally, do **NOT** set manually!
     *
     * \details Used to hold protocol specific properties / state. Might be \c
     * NULL if no state required.
     */
    void *_properties;
};

#ifdef __cplusplus
}
#endif

#endif // IFX_PROTOCOL_H
