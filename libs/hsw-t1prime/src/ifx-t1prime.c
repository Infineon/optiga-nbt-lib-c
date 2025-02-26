// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file ifx-t1prime.c
 * \brief Global Platform T=1' protocol
 */
#include "infineon/ifx-t1prime.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ifx-t1prime.h"
#include "infineon/ifx-crc.h"
#include "infineon/ifx-error.h"
#include "infineon/ifx-logger.h"
#include "infineon/ifx-timer.h"

#ifdef IFX_T1PRIME_INTERFACE_I2C
#include "infineon/ifx-i2c.h"
#else
#include "infineon/ifx-spi.h"
#endif

/**
 * \brief String used as source information for logging.
 */
#define IFX_LOG_TAG IFX_T1PRIME_LOG_TAG

/**
 * \brief Initializes Protocol object for Global Platform T=1' protocol.
 *
 * \param[in] self Protocol object to be initialized.
 * \param[in] driver Physical (driver) layer used for communication.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any
 * other value in case of error.
 * \cond FULL_DOCUMENTATION_BUILD
 * \relates ifx_protocol
 * \endcond
 */
ifx_status_t ifx_t1prime_initialize(ifx_protocol_t *self,
                                    ifx_protocol_t *driver)
{
    // Validate parameters
    if (self == NULL)
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_LAYER_INITIALIZE,
                         IFX_ILLEGAL_ARGUMENT);
    }
    // Validate driver layer
    if ((driver == NULL) || (driver->_transmit == NULL) ||
        (driver->_receive == NULL))
    {
        IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_FATAL,
                        "Driver layer does not meet T=1' requirements");
        return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_LAYER_INITIALIZE,
                         IFX_PROTOCOL_STACK_INVALID);
    }

    // Populate object
    ifx_status_t status = ifx_protocol_layer_initialize(self);
    if (status != IFX_SUCCESS)
    {
        return status;
    }
    self->_layer_id = IFX_T1PRIME_PROTOCOL_LAYER_ID;
    self->_base = driver;
    self->_activate = ifx_t1prime_activate;
    self->_transceive = ifx_t1prime_transceive;
    self->_destructor = ifx_t1prime_destroy;

#ifndef IFX_T1PRIME_INTERFACE_I2C
    // Set clock phase and polarity
    status = ifx_spi_set_clock_polarity(self, false);
    if (status != IFX_SUCCESS)
    {
        return status;
    }
    status = ifx_spi_set_clock_phase(self, false);
    if (status != IFX_SUCCESS)
    {
        return status;
    }
#endif
    IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_DEBUG,
                    "Successfully initialized T=1' protocol stack");
    return IFX_SUCCESS;
}

/**
 * \brief ifx_protocol_activate_callback_t for Global Platform T=1' protocol.
 *
 * \see ifx_protocol_activate_callback_t
 */
ifx_status_t ifx_t1prime_activate(ifx_protocol_t *self, uint8_t **response,
                                  size_t *response_len)
{
    // Ignore unused parameters
    (void) response;
    (void) response_len;

    // Validate parameters
    if (self == NULL)
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_ACTIVATE,
                         IFX_ILLEGAL_ARGUMENT);
    }

    IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_DEBUG,
                    "Activating communication channel to secure element");

    // Set default communication values in case SE changed
    ifx_t1prime_protocol_state_t *protocol_state;
    ifx_status_t status = ifx_t1prime_get_protocol_state(self, &protocol_state);
    if (status != IFX_SUCCESS)
    {
        return status;
    }
    protocol_state->ifsc = IFX_T1PRIME_DEFAULT_IFSC;
    protocol_state->bwt = IFX_T1PRIME_DEFAULT_BWT_MS;
    protocol_state->pwt = IFX_T1PRIME_DEFAULT_PWT_MS;

#ifdef IFX_T1PRIME_INTERFACE_I2C
    status = ifx_i2c_set_clock_frequency(
        self, IFX_T1PRIME_DEFAULT_I2C_CLOCK_FREQUENCY_HZ);
    if (status != IFX_SUCCESS)
    {
        return status;
    }
    status = ifx_i2c_set_guard_time(self, IFX_T1PRIME_DEFAULT_I2C_RWGT_US);
    if (status != IFX_SUCCESS)
    {
        return status;
    }
#else
    status = ifx_spi_set_clock_frequency(
        self, T1PRIME_DEFAULT_SPI_CLOCK_FREQUENCY_HZ);
    if (status != IFX_SUCCESS)
    {
        return status;
    }
    status = ifx_spi_set_guard_time(self, T1PRIME_DEFAULT_SPI_SEGT_US);
    if (status != IFX_SUCCESS)
    {
        return status;
    }
    status = ifx_spi_set_buffer_size(self, T1PRIME_DEFAULT_SPI_SEAL);
    if (status != IFX_SUCCESS)
    {
        return status;
    }
#endif

    // Base layer should not need activation but use just in case
    if (self->_base->_activate != NULL)
    {
        uint8_t *atpo = NULL;
        size_t atpo_len;
        status = self->_base->_activate(self->_base, &atpo, &atpo_len);
        if (status != IFX_SUCCESS)
        {
            IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_ERROR,
                            "Could not activate physical layer");
            return status;
        }
        if (atpo != NULL)
        {
            free(atpo);
            atpo = NULL;
        }
    }

    // Read communication interface parameters to negotiate protocol parameters
    ifx_cip_t cip;
    status = ifx_t1prime_s_cip(self, &cip);
    if (status != IFX_SUCCESS)
    {
        return status;
    }

    // Set data-link layer parameters
    ifx_dllp_t dllp;
    status = ifx_t1prime_dllp_decode(&dllp, cip.dllp, cip.dllp_len);
    if (status != IFX_SUCCESS)
    {
        ifx_t1prime_cip_destroy(&cip);
        return status;
    }
    protocol_state->bwt = dllp.bwt;
    protocol_state->ifsc = dllp.ifsc;
    ifx_t1prime_dllp_destroy(&dllp);

    // Set physical layer parameters depending on interface
    if (cip.plid == IFX_T1PRIME_PLID_I2C)
    {
#ifndef IFX_T1PRIME_INTERFACE_I2C
        // Should not occur
        IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_ERROR,
                        "CIP contains invalid physical layer ID. Should be "
                        "IFX_T1PRIME_PLID_I2C "
                        "(%d) but is %d",
                        IFX_T1PRIME_PLID_I2C, cip.plid);
        ifx_t1prime_cip_destroy(&cip);
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_CIP_VALIDATE,
                         IFX_T1PRIME_INVALID_PLID);
#else
        ifx_plp_t plp;
        status = ifx_t1prime_plp_decode(&plp, cip.plp, cip.plp_len);
        if (status != IFX_SUCCESS)
        {
            ifx_t1prime_cip_destroy(&cip);
            return status;
        }

        // Set clock frequency
        status = ifx_i2c_set_clock_frequency(self, plp.mcf * 1000U);
        if (status != IFX_SUCCESS)
        {
            ifx_t1prime_plp_destroy(&plp);
            ifx_t1prime_cip_destroy(&cip);
            return status;
        }

        // Set guard time
        status = ifx_i2c_set_guard_time(self, plp.rwgt);
        if (status != IFX_SUCCESS)
        {
            ifx_t1prime_plp_destroy(&plp);
            ifx_t1prime_cip_destroy(&cip);
            return status;
        }

        // Set polling time
        protocol_state->mpot = plp.mpot;

        // Set power wake-up time
        protocol_state->pwt = plp.pwt;

        ifx_t1prime_plp_destroy(&plp);
#endif
    }
    else if (cip.plid == IFX_T1PRIME_PLID_SPI)
    {
#ifdef IFX_T1PRIME_INTERFACE_I2C
        // Should not occur
        IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_ERROR,
                        "CIP contains invalid physical layer ID. Should be "
                        "IFX_T1PRIME_PLID_SPI "
                        "(%d) but is %d",
                        IFX_T1PRIME_PLID_SPI, cip.plid);
        ifx_t1prime_cip_destroy(&cip);
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_CIP_VALIDATE,
                         IFX_T1PRIME_INVALID_PLID);
#else
        ifx_plp_t plp;
        status = ifx_t1prime_plp_decode(&plp, cip.plp, cip.plp_len);
        if (status != IFX_SUCCESS)
        {
            ifx_t1prime_cip_destroy(&cip);
            return status;
        }

        // Set clock frequency
        status = ifx_spi_set_clock_frequency(self, plp.mcf * 1000u);
        if (status != IFX_SUCCESS)
        {
            ifx_t1prime_plp_destroy(&plp);
            ifx_t1prime_cip_destroy(&cip);
            return status;
        }

        // Set polling time
        protocol_state->mpot = plp.mpot;

        // Set guard time
        status = ifx_spi_set_guard_time(self, plp.segt);
        if (status != IFX_SUCCESS)
        {
            ifx_t1prime_plp_destroy(&plp);
            ifx_t1prime_cip_destroy(&cip);
            return status;
        }

        // Set SPI buffer size
        status = ifx_spi_set_buffer_size(self, plp.seal);
        if (status != IFX_SUCCESS)
        {
            ifx_t1prime_plp_destroy(&plp);
            ifx_t1prime_cip_destroy(&cip);
            return status;
        }

        ifx_t1prime_plp_destroy(&plp);
#endif
    }
    else
    {
        // Should not occur
        IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_ERROR,
                        "CIP contains invalid physical layer ID (%d)",
                        cip.plid);
        ifx_t1prime_cip_destroy(&cip);
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_CIP_VALIDATE,
                         IFX_T1PRIME_INVALID_PLID);
    }
    ifx_t1prime_cip_destroy(&cip);

    // Resynchronize sequence counters
    status = ifx_t1prime_s_resynch(self);
    if (status != IFX_SUCCESS)
    {
        return status;
    }

    // Do not send pseudo ATR
    // No response is sent
    if (NULL != response_len)
    {
        *response_len = 0x00;
    }

    IFX_T1PRIME_LOG(
        self->_logger, IFX_LOG_TAG, IFX_LOG_DEBUG,
        "Successfully activated communication channel to secure element");
    return IFX_SUCCESS;
}

/**
 * \brief ifx_protocol_transceive_callback_t for Global Platform T=1' protocol.
 *
 * \see ifx_protocol_transceive_callback_t
 */
ifx_status_t ifx_t1prime_transceive(ifx_protocol_t *self, const uint8_t *data,
                                    size_t data_len, uint8_t **response,
                                    size_t *response_len)
{
    // Validate parameters
    if (self == NULL)
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_TRANSCEIVE,
                         IFX_ILLEGAL_ARGUMENT);
    }
    if ((data == NULL) || (data_len == 0U) || (response == NULL) ||
        (response_len == NULL))
    {
        IFX_T1PRIME_LOG(
            self->_logger, IFX_LOG_TAG, IFX_LOG_ERROR,
            "Illegal NULL parameter given to ifx_t1prime_transceive()");
        return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_TRANSCEIVE,
                         IFX_ILLEGAL_ARGUMENT);
    }

    // Get protocol state for communication
    ifx_t1prime_protocol_state_t *protocol_state;
    ifx_status_t status = ifx_t1prime_get_protocol_state(self, &protocol_state);
    if (status != IFX_SUCCESS)
    {
        return status;
    }

    // Prepare first block to be send
    ifx_t1prime_block_t transmission_block;
    transmission_block.information_size =
        data_len < protocol_state->ifsc ? data_len : protocol_state->ifsc;
    size_t last_information_size = transmission_block.information_size;
    size_t offset = 0U;
    size_t remaining = data_len;
    transmission_block.nad = 0x21U;
    transmission_block.pcb = IFX_T1PRIME_PCB_I(
        protocol_state->send_counter, (remaining - last_information_size) > 0U);
    transmission_block.information =
        (uint8_t *) malloc(transmission_block.information_size);
    if (transmission_block.information == NULL)
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_TRANSCEIVE,
                         IFX_OUT_OF_MEMORY);
    }
    // clang-format off
    memcpy(transmission_block.information, data, transmission_block.information_size); // Flawfinder: ignore
    // clang-format on

    // Send blocks in loop to handle state
    ifx_t1prime_block_t response_block;
    bool aborted = false;
    size_t number_of_tries = 0U;
    // Limit resynchronization of protocol to once
    bool resynchronized = false;
    // In case of error limit retry of transceiving error blocks
    while (number_of_tries <= IFX_T1PRIME_BLOCK_TRANSCEIVE_RETRIES)
    {
        status = ifx_t1prime_block_transceive(self, &transmission_block,
                                              &response_block);
        ifx_t1prime_block_destroy(&transmission_block);
        if (status != IFX_SUCCESS)
        {
            return status;
        }

        // I(N(S), M) -> SE starts sending response
        if (IFX_T1PRIME_PCB_IS_I(response_block.pcb))
        {
            // Cannot receive I block response while not all data has been sent
            if ((remaining - last_information_size) > 0U)
            {
                IFX_T1PRIME_LOG(
                    self->_logger, IFX_LOG_TAG, IFX_LOG_ERROR,
                    "Secure element started sending response before all "
                    "data has been transmitted");
                ifx_t1prime_block_destroy(&response_block);
                return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_TRANSCEIVE,
                                 IFX_T1PRIME_INVALID_BLOCK);
            }
            protocol_state->send_counter ^= 0x01U;
            break;
        }
        // R(N(R)) -> SE wants (another) block
        if (IFX_T1PRIME_PCB_IS_R(response_block.pcb))
        {
            // SE expects next block
            if ((protocol_state->send_counter ^ 0x01U) ==
                IFX_T1PRIME_PCB_R_GET_NR(response_block.pcb))
            {
                ifx_t1prime_block_destroy(&response_block);

                // Check if chain was aborted
                if (aborted)
                {
                    return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_TRANSCEIVE,
                                     IFX_TRANSCEIVE_ABORTED);
                }

                // SE has last last block
                if ((remaining - last_information_size) == 0U)
                {
                    transmission_block.pcb =
                        IFX_T1PRIME_PCB_R_CRC(protocol_state->receive_counter);
                    transmission_block.information = NULL;
                    transmission_block.information_size = 0U;
                    number_of_tries++;
                }
                else
                {
                    // Update state to move to next part of data
                    remaining -= last_information_size;
                    offset += last_information_size;
                    protocol_state->send_counter ^= 0x01U;
                    // Received valid block -> reset retries & resync state
                    number_of_tries = 0U;
                    resynchronized = false;

                    // Prepare next block
                    transmission_block.information_size =
                        remaining < protocol_state->ifsc ? remaining
                                                         : protocol_state->ifsc;
                    last_information_size = transmission_block.information_size;
                    transmission_block.pcb = IFX_T1PRIME_PCB_I(
                        protocol_state->send_counter,
                        (remaining - last_information_size) > 0U);
                    transmission_block.information =
                        (uint8_t *) malloc(transmission_block.information_size);
                    if (transmission_block.information == NULL)
                    {
                        return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_TRANSCEIVE,
                                         IFX_OUT_OF_MEMORY);
                    }
                    // clang-format off
                    memcpy(transmission_block.information, data + offset, transmission_block.information_size); // Flawfinder: ignore
                    // clang-format on
                }
            }
            // SE wants a retransmission
            else
            {
                ifx_t1prime_block_destroy(&response_block);

                // Retransmit last I block
                transmission_block.pcb =
                    IFX_T1PRIME_PCB_I(protocol_state->send_counter,
                                      (remaining - last_information_size) > 0U);
                transmission_block.information_size = last_information_size;
                transmission_block.information =
                    (uint8_t *) malloc(transmission_block.information_size);
                if (transmission_block.information == NULL)
                {
                    return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_TRANSCEIVE,
                                     IFX_OUT_OF_MEMORY);
                }
                // clang-format off
                memcpy(transmission_block.information, data + offset, transmission_block.information_size); // Flawfinder: ignore
                // clang-format on
                // Error occurred, next try is necessary
                number_of_tries++;
            }
        }
        // S(WTX REQ) -> SE needs more time
        else if (response_block.pcb == IFX_T1PRIME_PCB_S_WTX_REQ)
        {
            // Verify information field
            if ((response_block.information == NULL) ||
                (response_block.information_size != 1U))
            {
                IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_ERROR,
                                "Secure element sent invalid S(WTX request)");
                ifx_t1prime_block_destroy(&response_block);
                return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_TRANSCEIVE,
                                 IFX_T1PRIME_INVALID_BLOCK);
            }
            protocol_state->wtx =
                response_block.information[0] * protocol_state->bwt;

            // Send S(WTX RESP)
            transmission_block.pcb = IFX_T1PRIME_PCB_S_WTX_RESP;
            transmission_block.information = response_block.information;
            transmission_block.information_size =
                response_block.information_size;
            // Received valid block -> reset retries & resync state
            number_of_tries = 0U;
            resynchronized = false;
        }
        // S(IFS REQ) -> SE wants to indicate that it can send more or less data
        else if (response_block.pcb == IFX_T1PRIME_PCB_S_IFS_REQ)
        {
            // Verify IFS value
            size_t ifs;
            status = ifx_t1prime_ifs_decode(&ifs, response_block.information,
                                            response_block.information_size);
            if (status != IFX_SUCCESS)
            {
                ifx_t1prime_block_destroy(&response_block);
                return status;
            }

            // Update state in case new IFSC is smaller and SE wants a
            // retransmission
            last_information_size =
                ifs < last_information_size ? ifs : last_information_size;

            // Send S(IFS RESP)
            transmission_block.pcb = IFX_T1PRIME_PCB_S_IFS_RESP;
            transmission_block.information = response_block.information;
            transmission_block.information_size =
                response_block.information_size;
            // Received valid block -> reset retries & resync state
            number_of_tries = 0U;
            resynchronized = false;
        }
        // S(ABORT REQ) -> SE wants to stop chain request
        else if (response_block.pcb == IFX_T1PRIME_PCB_S_ABORT_REQ)
        {
            // Send S(ABORT RESP)
            ifx_t1prime_block_destroy(&response_block);
            transmission_block.pcb = IFX_T1PRIME_PCB_S_ABORT_RESP;
            transmission_block.information = NULL;
            transmission_block.information_size = 0U;
            aborted = true;
            // Received valid block -> reset retries & resync state
            number_of_tries = 0U;
            resynchronized = false;
        }
        else
        {
            ifx_t1prime_block_destroy(&response_block);
            return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_TRANSCEIVE,
                             IFX_T1PRIME_INVALID_BLOCK);
        }
        // Number of retries elapsed and not yet resynchronized
        if ((number_of_tries == IFX_T1PRIME_BLOCK_TRANSCEIVE_RETRIES + 1) && (!resynchronized)) {
            IFX_T1PRIME_LOG(
                self->_logger, IFX_LOG_TAG, IFX_LOG_WARN,
                "Giving up block exchange after %d retries. Trying to resynchronize protocol.",
                IFX_T1PRIME_BLOCK_TRANSCEIVE_RETRIES);
            status = ifx_t1prime_s_resynch(self);
            if (status != IFX_SUCCESS) {
                return status;
            }
            ifx_t1prime_block_destroy(&transmission_block);
            ifx_t1prime_block_destroy(&response_block);
            // Retransmit last I block
            transmission_block.pcb =
                IFX_T1PRIME_PCB_I(protocol_state->send_counter,
                                    (remaining - last_information_size) > 0U);
            transmission_block.information_size = last_information_size;
            transmission_block.information =
                (uint8_t *) malloc(transmission_block.information_size);
            if (transmission_block.information == NULL)
            {
                return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_TRANSCEIVE,
                                    IFX_OUT_OF_MEMORY);
            }
            // clang-format off
            memcpy(transmission_block.information, data + offset, transmission_block.information_size); // Flawfinder: ignore
            // clang-format on
            // Number of max retries reached, resynchronize.
            number_of_tries = 0U;
            resynchronized = true;
        }
        // Number of retries elapsed after it was already resynchronized -> give up
        else if ((number_of_tries == IFX_T1PRIME_BLOCK_TRANSCEIVE_RETRIES + 1) && resynchronized) {
            ifx_t1prime_block_destroy(&transmission_block);
            ifx_t1prime_block_destroy(&response_block);
            IFX_T1PRIME_LOG(
                self->_logger, IFX_LOG_TAG, IFX_LOG_WARN,
                "Giving up block exchange after protocol resynchronization.");
            return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_TRANSCEIVE, IFX_UNSPECIFIED_ERROR);
        }
    }
    // Validate response in loop to handle state
    *response = NULL;
    *response_len = 0U;
    while (true)
    {
        // I(N(S), M) -> SE sent response
        if (IFX_T1PRIME_PCB_IS_I(response_block.pcb))
        {
            // Validate sequence counter
            if (IFX_T1PRIME_PCB_I_GET_NS(response_block.pcb) !=
                protocol_state->receive_counter)
            {
                ifx_t1prime_block_destroy(&response_block);
                if ((*response) != NULL)
                {
                    free(*response);
                    *response = NULL;
                }
                *response_len = 0U;
                return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_TRANSCEIVE,
                                 IFX_T1PRIME_INVALID_BLOCK);
            }

            // Create new buffer for first response I block
            if ((*response) == NULL)
            {
                // Check that data available
                if (response_block.information_size == 0U)
                {
                    IFX_T1PRIME_LOG(
                        self->_logger, IFX_LOG_TAG, IFX_LOG_ERROR,
                        "Secure element sent invalid empty I(?, ?) block");
                    ifx_t1prime_block_destroy(&response_block);
                    return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_TRANSCEIVE,
                                     IFX_T1PRIME_INVALID_BLOCK);
                }
                *response = response_block.information;
                *response_len = response_block.information_size;
                response_block.information = NULL;
                response_block.information_size = 0U;
            }
            // Otherwise append to buffer
            else
            {
                // Check for special case of forced acknowledgement
                if (response_block.information_size > 0U)
                {
                    uint8_t *realloc_cache = *response;
                    *response = (uint8_t *) realloc(
                        *response,
                        *response_len + response_block.information_size);
                    if ((*response) == NULL)
                    {
                        free(realloc_cache);
                        realloc_cache = NULL;
                        *response_len = 0U;
                        ifx_t1prime_block_destroy(&response_block);
                        return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_TRANSCEIVE,
                                         IFX_OUT_OF_MEMORY);
                    }
                    // clang-format off
                    memcpy((*response) + *response_len, response_block.information, response_block.information_size); // Flawfinder: ignore
                    // clang-format on
                    *response_len += response_block.information_size;
                }
            }

            ifx_t1prime_block_destroy(&response_block);
            protocol_state->receive_counter ^= 0x01U;

            // Check if more data will be transmitted
            if (IFX_T1PRIME_PCB_I_HAS_MORE(response_block.pcb))
            {
                transmission_block.pcb =
                    IFX_T1PRIME_PCB_R_ACK(protocol_state->receive_counter);
                transmission_block.information = NULL;
                transmission_block.information_size = 0U;
                status = ifx_t1prime_block_transceive(self, &transmission_block,
                                                      &response_block);
                ifx_t1prime_block_destroy(&transmission_block);
                if (status != IFX_SUCCESS)
                {
                    if ((*response) != NULL)
                    {
                        free(*response);
                        *response = NULL;
                    }
                    *response_len = 0U;
                    return status;
                }
            }
            // All data received
            else
            {
                break;
            }
        }
        // R(N(R)) -> SE needs a retransmission
        else if (IFX_T1PRIME_PCB_IS_R(response_block.pcb))
        {
            // Validate that card sent correct R(N(R))
            if (IFX_T1PRIME_PCB_R_GET_NR(response_block.pcb) !=
                protocol_state->send_counter)
            {
                ifx_t1prime_block_destroy(&response_block);
                if ((*response) != NULL)
                {
                    free(*response);
                    *response = NULL;
                }
                *response_len = 0U;
            }

            // Send retransmission request
            ifx_t1prime_block_destroy(&response_block);
            transmission_block.pcb =
                IFX_T1PRIME_PCB_R_ACK(protocol_state->receive_counter);
            transmission_block.information = NULL;
            transmission_block.information_size = 0U;
            status = ifx_t1prime_block_transceive(self, &transmission_block,
                                                  &response_block);
            ifx_t1prime_block_destroy(&transmission_block);
            if (status != IFX_SUCCESS)
            {
                if ((*response) != NULL)
                {
                    free(*response);
                    *response = NULL;
                }
                *response_len = 0U;
                return status;
            }
        }
        // S(ABORT request) -> end chain
        else if (response_block.pcb == IFX_T1PRIME_PCB_S_ABORT_REQ)
        {
            // Delete response cache
            if ((*response) != NULL)
            {
                free(*response);
                *response = NULL;
            }
            *response_len = 0U;

            // Answer with S(ABORT response)
            transmission_block.pcb = IFX_T1PRIME_PCB_S_ABORT_RESP;
            transmission_block.information = NULL;
            transmission_block.information_size = 0U;
            ifx_t1prime_block_transceive(self, &transmission_block,
                                         &response_block);
            ifx_t1prime_block_destroy(&transmission_block);
            ifx_t1prime_block_destroy(&response_block);

            IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_WARN,
                            "Secure element requested to abort transmission");
            return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_TRANSCEIVE,
                             IFX_TRANSCEIVE_ABORTED);
        }
        else
        {
            IFX_T1PRIME_LOG_BLOCK(
                self->_logger, IFX_LOG_TAG, IFX_LOG_ERROR,
                "Secure element sent invalid block: ", &response_block);
            ifx_t1prime_block_destroy(&response_block);
            if ((*response) != NULL)
            {
                free(*response);
                *response = NULL;
            }
            *response_len = 0U;
            return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_TRANSCEIVE,
                             IFX_T1PRIME_INVALID_BLOCK);
        }
    }

    return IFX_SUCCESS;
}

/**
 * \brief ifx_protocol_destroy_callback_t for Global Platform T=1' protocol.
 *
 * \see ifx_protocol_destroy_callback_t
 */
void ifx_t1prime_destroy(ifx_protocol_t *self)
{
    if (self != NULL)
    {
        IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_DEBUG,
                        "Destroying T=1' protocol stack");
        if (self->_properties != NULL)
        {
            free(self->_properties);
            self->_properties = NULL;
        }
    }
}

/**
 * \brief Performs Global Platform T=1' RESYNCH operation.
 *
 * \details Sends S(RESYNCH request) and expects S(RESYNCH response).
 *
 * \param[in] self Protocol stack for performing necessary operations.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
ifx_status_t ifx_t1prime_s_resynch(ifx_protocol_t *self)
{
    // Prepare S(RESYNCH request)
    ifx_t1prime_block_t request;
    request.nad = IFX_NAD_HD_TO_SE;
    request.pcb = IFX_T1PRIME_PCB_S_RESYNCH_REQ;
    request.information_size = 0U;
    request.information = NULL;

    // Validate parameters
    if (self == NULL)
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_TRANSCEIVE,
                         IFX_ILLEGAL_ARGUMENT);
    }

    IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_DEBUG,
                    "Performing S(RESYNCH)");

    ifx_t1prime_block_t response;

    // Send S(RESYNCH request) to secure element and read back response
    ifx_status_t status =
        ifx_t1prime_block_transceive(self, &request, &response);
    ifx_t1prime_block_destroy(&request);
    if (status != IFX_SUCCESS)
    {
        return status;
    }

    // Validate response is S(RESYNCH response)
    if (response.pcb != IFX_T1PRIME_PCB_S_RESYNCH_RESP)
    {
        IFX_T1PRIME_LOG(
            self->_logger, IFX_LOG_TAG, IFX_LOG_ERROR,
            "Invalid answer to S(RESYNCH request) received (PCB: %02X)",
            response.pcb);
        ifx_t1prime_block_destroy(&response);
        return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_RECEIVE,
                         IFX_T1PRIME_INVALID_BLOCK);
    }

    ifx_t1prime_block_destroy(&response);

    // Reset protocol state
    ifx_t1prime_protocol_state_t *protocol_state;
    status = ifx_t1prime_get_protocol_state(self, &protocol_state);
    if (status != IFX_SUCCESS)
    {
        return status;
    }
    protocol_state->send_counter = 0x00U;
    protocol_state->receive_counter = 0x00U;

    IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_DEBUG,
                    "Protocol state successfully reset");
    return IFX_SUCCESS;
}

/**
 * \brief Queries Global Platform T=1' Communication Interface Parameters (CIP).
 *
 * \details Sends S(CIP request) and expects S(CIP response).
 *
 * \param[in] self Protocol stack for performing necessary operations.
 * \param[out] cip Buffer to store received CIP in.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
ifx_status_t ifx_t1prime_s_cip(ifx_protocol_t *self, ifx_cip_t *cip)
{
    // Prepare S(CIP request)
    ifx_t1prime_block_t request;
    request.nad = IFX_NAD_HD_TO_SE;
    request.pcb = IFX_T1PRIME_PCB_S_CIP_REQ;
    request.information_size = 0U;
    request.information = NULL;

    // Validate parameters
    if ((self == NULL) || (cip == NULL))
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_TRANSCEIVE,
                         IFX_ILLEGAL_ARGUMENT);
    }
    IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_DEBUG,
                    "Performing S(CIP)");
    ifx_t1prime_block_t response;

    // Send S(CIP request) to secure element and read back response
    ifx_status_t status =
        ifx_t1prime_block_transceive(self, &request, &response);
    ifx_t1prime_block_destroy(&request);
    if (status != IFX_SUCCESS)
    {
        return status;
    }

    // Validate response is S(CIP response) -> also implicitely checked by
    // ifx_t1prime_block_transceive
    if (response.pcb != IFX_T1PRIME_PCB_S_CIP_RESP)
    {
        IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_WARN,
                        "Invalid answer to S(CIP request) received (PCB: %02X)",
                        response.pcb);
        ifx_t1prime_block_destroy(&response);
        return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_RECEIVE,
                         IFX_T1PRIME_INVALID_BLOCK);
    }

    // Decode CIP information (implicitly validating data)
    status = ifx_t1prime_cip_decode(cip, response.information,
                                    response.information_size);
    ifx_t1prime_block_destroy(&response);
    if (status != IFX_SUCCESS)
    {
        return status;
    }

    IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_DEBUG,
                    "Successfully read CIP");
    return IFX_SUCCESS;
}

/**
 * \brief Performs Global Platform T=1' software reset (SWR).
 *
 * \details Sends S(SWR request) and expects S(SWR response).
 *
 * \param[in] self Protocol stack for performing necessary operations.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
ifx_status_t ifx_t1prime_s_swr(ifx_protocol_t *self)
{
    // Prepare S(SWR request)
    ifx_t1prime_block_t request;
    request.nad = IFX_NAD_HD_TO_SE;
    request.pcb = IFX_T1PRIME_PCB_S_SWR_REQ;
    request.information_size = 0U;
    request.information = NULL;

    // Validate parameters
    if (self == NULL)
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_TRANSCEIVE,
                         IFX_ILLEGAL_ARGUMENT);
    }

    IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_DEBUG,
                    "Performing S(SWR)");

    ifx_t1prime_block_t response;

    // Send S(SWR request) to secure element and read back response
    ifx_status_t status =
        ifx_t1prime_block_transceive(self, &request, &response);
    ifx_t1prime_block_destroy(&request);
    if (status != IFX_SUCCESS)
    {
        return status;
    }

    // Validate response is S(SWR response) -> also implicitly checked by
    // ifx_t1prime_block_transceive
    if (response.pcb != IFX_T1PRIME_PCB_S_SWR_RESP)
    {
        IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_WARN,
                        "Invalid answer to S(SWR request) received (PCB: %02X)",
                        response.pcb);
        ifx_t1prime_block_destroy(&response);
        return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_RECEIVE,
                         IFX_T1PRIME_INVALID_BLOCK);
    }

    ifx_t1prime_block_destroy(&response);

    ifx_t1prime_protocol_state_t *protocol_state;
    status = ifx_t1prime_get_protocol_state(self, &protocol_state);
    if (status != IFX_SUCCESS)
    {
        return status;
    }

#ifdef IFX_T1PRIME_RESET_DELAY_PWT
    ifx_timer_t swr_timer;
    // Wait for a duration of power wake-up time before initiating communication
    // with secure element
    ifx_status_t timer_status =
        ifx_timer_set(&swr_timer, (uint64_t) protocol_state->pwt * 1000U);
    if (timer_status != IFX_SUCCESS)
    {
        return timer_status;
    }

    timer_status = ifx_timer_join(&swr_timer);

    ifx_timer_destroy(&swr_timer);
    if (timer_status != IFX_SUCCESS)
    {
        return timer_status;
    }
#endif

    // Reset protocol state
    protocol_state->send_counter = 0x00U;
    protocol_state->receive_counter = 0x00U;

    IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_DEBUG,
                    "Successfully reset secure element");
    return IFX_SUCCESS;
}

/**
 * \brief Performs Global Platform T=1' power on reset (POR).
 *
 * \details Sends S(POR request).
 *
 * \param[in] self Protocol stack for performing necessary operations.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
ifx_status_t ifx_t1prime_s_por(ifx_protocol_t *self)
{
    // Prepare S(POR request)
    ifx_t1prime_block_t request;
    request.nad = IFX_NAD_HD_TO_SE;
    request.pcb = IFX_T1PRIME_PCB_S_POR_REQ;
    request.information_size = 0U;
    request.information = NULL;

    // Validate parameters
    if (self == NULL)
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_TRANSMIT,
                         IFX_ILLEGAL_ARGUMENT);
    }

    IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_DEBUG,
                    "Performing S(POR)");

    // Send S(POR request) to secure element
    ifx_status_t status = ifx_t1prime_block_transmit(self, &request);
    ifx_t1prime_block_destroy(&request);
    if (status != IFX_SUCCESS)
    {
        return status;
    }

    ifx_t1prime_protocol_state_t *protocol_state;
    status = ifx_t1prime_get_protocol_state(self, &protocol_state);
    if (status != IFX_SUCCESS)
    {
        return status;
    }

#ifdef IFX_T1PRIME_RESET_DELAY_PWT
    ifx_timer_t por_timer;
    // Wait for a duration of power wake-up time before initiating communication
    // with secure element
    ifx_status_t timer_status =
        ifx_timer_set(&por_timer, (uint64_t) protocol_state->pwt * 1000U);
    if (timer_status != IFX_SUCCESS)
    {
        return timer_status;
    }

    timer_status = ifx_timer_join(&por_timer);

    ifx_timer_destroy(&por_timer);
    if (timer_status != IFX_SUCCESS)
    {
        return timer_status;
    }
#endif

    // Reset protocol state
    protocol_state->send_counter = 0x00U;
    protocol_state->receive_counter = 0x00U;

    IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_DEBUG,
                    "Successfully reset secure element");
    return IFX_SUCCESS;
}

/**
 * \brief Sends Block to secure element.
 *
 * \param[in] self Protocol stack for performing necessary operations.
 * \param[in] block Block object to be send to secure element.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
ifx_status_t ifx_t1prime_block_transmit(ifx_protocol_t *self,
                                        const ifx_t1prime_block_t *block)
{
    // Validate parameters
    if ((self == NULL) || (block == NULL))
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_TRANSMIT,
                         IFX_ILLEGAL_ARGUMENT);
    }
    // Validate protocol stack
    if ((self->_base == NULL) || (self->_base->_transmit == NULL))
    {
        IFX_T1PRIME_LOG(
            self->_logger, IFX_LOG_TAG, IFX_LOG_FATAL,
            "ifx_t1prime_block_transmit() called with invalid protocol stack");
        return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_TRANSMIT,
                         IFX_PROTOCOL_STACK_INVALID);
    }

    // Validate data
    ifx_t1prime_protocol_state_t *protocol_state;
    ifx_status_t status = ifx_t1prime_get_protocol_state(self, &protocol_state);
    if (status != IFX_SUCCESS)
    {
        return status;
    }
    if (block->information_size > protocol_state->ifsc)
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_TRANSMIT,
                         IFX_ILLEGAL_ARGUMENT);
    }

    IFX_T1PRIME_LOG_BLOCK(self->_logger, IFX_LOG_TAG, IFX_LOG_INFO, ">> ",
                          block);

    // Encode block
    uint8_t *encoded;
    size_t encoded_len;
    status = ifx_t1prime_block_encode(block, &encoded, &encoded_len);
    if (status != IFX_SUCCESS)
    {
        return status;
    }

    // Actually transmit block
#ifdef IFX_T1PRIME_INTERFACE_I2C
    // TODO: Check what is the correct timeout to be used
    ifx_timer_t bwt_timer;
    status = ifx_timer_set(&bwt_timer, (uint64_t) protocol_state->bwt * 1000U);
    if (status != IFX_SUCCESS)
    {
        free(encoded);
        encoded = NULL;
        return status;
    }

    // Temporarily disable logging until LATE ACK is received
    ifx_logger_t *driver_logger = self->_base->_logger;
    self->_base->_logger = NULL;

    // Try sending in loop until SE acknowledges data
    do
    {
        status = self->_base->_transmit(self->_base, encoded, encoded_len);
        if (IFX_SUCCESS == status)
        {
            IFX_T1PRIME_LOG_BYTES(driver_logger, IFX_I2C_LOG_TAG, IFX_LOG_INFO,
                                  ">> ", encoded, encoded_len, " ");
            break;
        }
        // Wait for polling time and try again
        ifx_timer_t pot_timer;
        ifx_status_t timer_status =
            ifx_timer_set(&pot_timer, (uint64_t) protocol_state->mpot * 1000U);
        if (timer_status != IFX_SUCCESS)
        {
            break;
        }
        timer_status = ifx_timer_join(&pot_timer);
        ifx_timer_destroy(&pot_timer);
        if (timer_status != IFX_SUCCESS)
        {
            break;
        }
    } while (!ifx_timer_has_elapsed(&bwt_timer));
    if (status != IFX_SUCCESS)
    {
        IFX_T1PRIME_LOG(driver_logger, IFX_LOG_TAG, IFX_LOG_ERROR,
                        "could not send T=1' block via I2C");
    }
    self->_base->_logger = driver_logger;
    ifx_timer_destroy(&bwt_timer);
#else
    status = self->_base->_transmit(self->_base, encoded, encoded_len);
#endif
    free(encoded);
    encoded = NULL;
    return status;
}

/**
 * \brief Reads Block from secure element.
 *
 * \param[in] self Protocol stack for performing necessary operations.
 * \param[out] block Block object to store received data in.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
ifx_status_t t1prime_block_receive(ifx_protocol_t *self,
                                   ifx_t1prime_block_t *block)
{
    // Validate parameters
    if ((self == NULL) || (block == NULL))
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_RECEIVE,
                         IFX_ILLEGAL_ARGUMENT);
    }
    // Validate protocol stack
    if ((self->_base == NULL) || (self->_base->_receive == NULL))
    {
        IFX_T1PRIME_LOG(
            self->_logger, IFX_LOG_TAG, IFX_LOG_FATAL,
            "t1prime_block_receive() called with invalid protocol stack");
        return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_RECEIVE,
                         IFX_PROTOCOL_STACK_INVALID);
    }

    // Get protocol state for timing information
    ifx_t1prime_protocol_state_t *protocol_state;
    ifx_status_t status = ifx_t1prime_get_protocol_state(self, &protocol_state);
    if (status != IFX_SUCCESS)
    {
        return status;
    }

    // Poll for NAD
    block->nad = 0x00U;

    // Use "shortcut" read to minimize amount of I2C/SPI accesses
    block->information = NULL;
    block->information_size = 0U;
    uint8_t *binary = NULL;
    size_t binary_len = 0U;
    uint16_t crc = 0U;
    size_t information_size = 0U;

    // Use either BWT or WTX as time until response has to be ready
    uint32_t bwt = protocol_state->bwt;
    if (protocol_state->wtx > bwt)
    {
        bwt = protocol_state->wtx;
    }
    protocol_state->wtx = 0U;

    ifx_timer_t bwt_timer;
    status = ifx_timer_set(&bwt_timer, (uint32_t) (bwt * 1000U));
    if (status != IFX_SUCCESS)
    {
        return status;
    }

    // Temporarily disable logging while polling
    ifx_logger_t *driver_logger = self->_base->_logger;
    self->_base->_logger = NULL;

    do
    {
        // Use interrupt method if set
        if (protocol_state->irq_handler != NULL)
        {
            status = protocol_state->irq_handler(self, bwt * 1000U);
            switch (status)
            {
            case IFX_T1PRIME_IRQ_TRIGGERED:
                IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_INFO,
                                "T=1' data interrupt triggered");
                break;
            case IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_IRQ,
                           IFX_T1PRIME_IRQ_NOT_TRIGGERED):
                IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_WARN,
                                "T=1' data interrupt did not trigger in time");
                self->_base->_logger = driver_logger;
                ifx_timer_destroy(&bwt_timer);
                return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_RECEIVE,
                                 IFX_TOO_LITTLE_DATA);
            default:
                IFX_T1PRIME_LOG(
                    self->_logger, IFX_LOG_TAG, IFX_LOG_ERROR,
                    "Error occurred while waiting for T=1' data interrupt");
                self->_base->_logger = driver_logger;
                ifx_timer_destroy(&bwt_timer);
                return status;
            }
        }
#ifdef IFX_T1PRIME_INTERFACE_I2C
        // Try to read full block at once
        status = self->_base->_receive(
            self->_base, IFX_BLOCK_PROLOGUE_LEN + IFX_BLOCK_EPILOGUE_LEN,
            &binary, &binary_len);

        // I2C successful read signalizes start of response
        if (IFX_SUCCESS == status)
        {
            // Retry on invalid NAD
            uint8_t nad = binary[0];
            uint8_t dad = (nad >> 4) & 0x0fU;
            uint8_t sad = nad & 0x0F;
            if (dad == 0x00U || dad == 0x0fU || sad == 0x00U || sad == 0x0fU)
            {
                IFX_T1PRIME_LOG_BYTES(driver_logger, IFX_I2C_LOG_TAG,
                                      IFX_LOG_DEBUG, "<< [invalid NAD] ",
                                      binary, binary_len, " ");
                continue;
            }

            IFX_T1PRIME_LOG_BYTES(driver_logger, IFX_I2C_LOG_TAG, IFX_LOG_INFO,
                                  "<< ", binary, binary_len, " ");

            block->nad = binary[0];
            block->pcb = binary[1];
            information_size = (binary[2] << 8) | binary[3];
            crc = (binary[4] << 8) | binary[5];
            free(binary);
            binary = NULL;
            break;
        }
#else
        // Try to read full block at once
        status = self->_base->_receive(
            self->_base, 1u + IFX_BLOCK_PROLOGUE_LEN + IFX_BLOCK_EPILOGUE_LEN,
            &binary, &binary_len);

        // SPI successful read of valid NAD signalizes start of response
        if (IFX_SUCCESS == status)
        {
            // Check if valid NAD
            bool nad_valid = false;
            for (size_t offset = 0u;
                 offset < (IFX_BLOCK_PROLOGUE_LEN + IFX_BLOCK_EPILOGUE_LEN);
                 offset++)
            {
                // Iterate over data to see where response starts
                if ((binary[1 + offset] != 0x00u) &&
                    (binary[1 + offset] != 0xffu))
                {
                    // Fill up block information based on offset
                    if (offset > 0u)
                    {
                        uint8_t *block_filler = NULL;
                        size_t block_filler_len = 0u;
                        status = self->_base->_receive(self->_base, offset,
                                                       &block_filler,
                                                       &block_filler_len);
                        if (status != IFX_SUCCESS)
                        {
                            break;
                        }
                        if (block_filler == NULL)
                        {
                            break;
                        }
                        if (block_filler_len != offset)
                        {
                            free(block_filler);
                            block_filler = NULL;
                            break;
                        }
                        memmove(
                            binary + 1, binary + 1 + offset,
                            (IFX_BLOCK_PROLOGUE_LEN + IFX_BLOCK_EPILOGUE_LEN) -
                                offset);
                        // clang-format off
                        memcpy(binary + 1 + IFX_BLOCK_PROLOGUE_LEN + IFX_BLOCK_EPILOGUE_LEN - offset, block_filler, offset); // Flawfinder: ignore
                        // clang-format on
                        free(block_filler);
                        block_filler = NULL;
                    }

                    // Parse information to more suitable format
                    IFX_T1PRIME_LOG_BYTES(driver_logger, IFX_SPI_LOG_TAG,
                                          IFX_LOG_INFO, "<< ", binary + 1,
                                          binary_len - 1, " ");
                    block->nad = binary[1];
                    block->pcb = binary[2];
                    information_size = (binary[3] << 8) | binary[4];
                    crc = (binary[5] << 8) | binary[6];
                    free(binary);
                    binary = NULL;
                    nad_valid = true;
                    break;
                }
            }
            if (nad_valid)
            {
                break;
            }

            // Clean up and continue polling
            free(binary);
            binary = NULL;
        }
#endif
        // Wait for polling time and try again
        ifx_timer_t pot_timer;
        status = ifx_timer_set(&pot_timer,
                               (uint64_t) (protocol_state->mpot * 1000U));
        if (status != IFX_SUCCESS)
        {
            self->_base->_logger = driver_logger;
            ifx_timer_destroy(&bwt_timer);
            return status;
        }
        status = ifx_timer_join(&pot_timer);
        ifx_timer_destroy(&pot_timer);
        if (status != IFX_SUCCESS)
        {
            self->_base->_logger = driver_logger;
            ifx_timer_destroy(&bwt_timer);
            return status;
        }
    } while (!ifx_timer_has_elapsed(&bwt_timer));
    ifx_timer_destroy(&bwt_timer);
    self->_base->_logger = driver_logger;
    if (block->nad == 0x00U)
    {
        IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_ERROR,
                        "polling time exceeded but no data received");
        return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_RECEIVE,
                         IFX_TOO_LITTLE_DATA);
    }

    // Check if more data needs to be read
    if (information_size > 0U)
    {
        status = self->_base->_receive(self->_base, information_size,
                                       &block->information,
                                       &block->information_size);
        if (status != IFX_SUCCESS)
        {
            return status;
        }
        if (block->information_size != information_size)
        {
            IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_ERROR,
                            "too little data for T=1' block information field "
                            "received (expected %zu but was %zu)",
                            block->information_size, information_size);
            return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_RECEIVE,
                             IFX_TOO_LITTLE_DATA);
        }

        // Merge previous data with new chunk
        if (information_size == 1)
        {
            uint16_t new_crc = ((crc & 0xffU) << 8) | block->information[0];
            block->information[0] = (crc & 0xff00U) >> 8;
            crc = new_crc;
        }
        else
        {
            uint16_t new_crc = (block->information[information_size - 2] << 8) |
                               block->information[information_size - 1];
            memmove(block->information + 2, block->information,
                    information_size - 2);
            block->information[0] = (crc & 0xff00U) >> 8;
            block->information[1] = crc & 0xffU;
            crc = new_crc;
        }
    }

    // Validate CRC
    if (!ifx_t1prime_validate_crc(block, crc))
    {
        IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_WARN,
                        "T=1' block with invalid CRC received");
        ifx_t1prime_block_destroy(block);
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_BLOCK_DECODE,
                         IFX_T1PRIME_INVALID_CRC);
    }

    IFX_T1PRIME_LOG_BLOCK(self->_logger, IFX_LOG_TAG, IFX_LOG_INFO, "<< ",
                          block);

    return IFX_SUCCESS;
}

/**
 * \brief Sends Block to secure element and reads back response Block.
 *
 * \param[in] self Protocol stack for performing necessary operations.
 * \param[in] block Block object to be send to secure element.
 * \param[out] response_buffer Block object to store received data in.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
ifx_status_t ifx_t1prime_block_transceive(ifx_protocol_t *self,
                                          const ifx_t1prime_block_t *block,
                                          ifx_t1prime_block_t *response_buffer)
{
    // Prepare S(SWR request)
    ifx_t1prime_block_t swr_request;
    swr_request.nad = IFX_NAD_HD_TO_SE;
    swr_request.pcb = IFX_T1PRIME_PCB_S_SWR_REQ;
    swr_request.information_size = 0;
    swr_request.information = NULL;

    // Get protocol state with information about BWT, WTX, etc
    ifx_t1prime_protocol_state_t *protocol_state;
    ifx_status_t status = ifx_t1prime_get_protocol_state(self, &protocol_state);
    if (status != IFX_SUCCESS)
    {
        return status;
    }

    // Send and receive blocks in loop
    ifx_t1prime_block_t to_send = *block;
    uint8_t try_count = 0U;
    do
    {
        // Send block to SE
        status = ifx_t1prime_block_transmit(self, &to_send);
        if (status != IFX_SUCCESS)
        {
            return status;
        }

        // Read response from SE
        status = t1prime_block_receive(self, response_buffer);

        // Validate correct block has been received
        if (IFX_SUCCESS == status)
        {
            // Special case S(? request)
            if (IFX_T1PRIME_PCB_IS_S(block->pcb) &&
                IFX_T1PRIME_PCB_S_IS_REQ(block->pcb))
            {
                // S(? response) must match request type
                if (IFX_T1PRIME_PCB_IS_S(response_buffer->pcb) &&
                    (!IFX_T1PRIME_PCB_S_IS_REQ(response_buffer->pcb)))
                {
                    if (IFX_T1PRIME_PCB_S_GET_TYPE(block->pcb) ==
                        IFX_T1PRIME_PCB_S_GET_TYPE(response_buffer->pcb))
                    {
                        return status;
                    }
                }
                // R(N(R)) must have correct sequence counter
                else if (IFX_T1PRIME_PCB_IS_R(response_buffer->pcb))
                {
                    if (IFX_T1PRIME_PCB_R_GET_NR(response_buffer->pcb) !=
                        protocol_state->send_counter)
                    {
                        IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG,
                                        IFX_LOG_WARN,
                                        "Received R(N(R)) block with invalid "
                                        "sequence counter");
                        ifx_t1prime_block_destroy(response_buffer);
                        return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_TRANSCEIVE,
                                         IFX_T1PRIME_INVALID_BLOCK);
                    }
                }
                // I(N(S), M) invalid
                else if (IFX_T1PRIME_PCB_IS_I(response_buffer->pcb))
                {
                    IFX_T1PRIME_LOG_BLOCK(
                        self->_logger, IFX_LOG_TAG, IFX_LOG_WARN,
                        "Received unexpected I(N(S), M) block as answer to ",
                        block);
                    ifx_t1prime_block_destroy(response_buffer);
                    return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_TRANSCEIVE,
                                     IFX_T1PRIME_INVALID_BLOCK);
                }

                // Invalidate read status
                status = IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_TRANSCEIVE,
                                   IFX_T1PRIME_INVALID_BLOCK);
            }
            else
            {
                return status;
            }
        }

        // All blocks besides S(? request) trigger retransmissions by sending
        // R(N(R))
        if (!IFX_T1PRIME_PCB_IS_S(block->pcb) ||
            !IFX_T1PRIME_PCB_S_IS_REQ(block->pcb))
        {
            to_send.nad = 0x21U;
            to_send.pcb =
                IFX_T1PRIME_PCB_R_CRC(protocol_state->receive_counter);
            to_send.information = NULL;
            to_send.information_size = 0U;
        }
    } while ((++try_count) <= IFX_T1PRIME_BLOCK_TRANSCEIVE_RETRIES);
    IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_ERROR,
                    "Giving up block exchange after %d tries",
                    IFX_T1PRIME_BLOCK_TRANSCEIVE_RETRIES);

    // Reset secure element via S(SWR)
    IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_WARN,
                    "Trying to recover via S(SWR) exchange");

    // Send S(SWR request) to secure element and read back response
    if (ifx_t1prime_block_transmit(self, &swr_request) == IFX_SUCCESS)
    {
        ifx_t1prime_block_t swr_response;
        t1prime_block_receive(self, &swr_response);

        // Wait for power wake-up time to have secure element reset its state
        ifx_timer_t pwt_timer;
        if (ifx_timer_set(&pwt_timer, (uint64_t) (IFX_T1PRIME_DEFAULT_PWT_MS *
                                                  1000U)) == IFX_SUCCESS)
        {
            ifx_timer_join(&pwt_timer);
            ifx_timer_destroy(&pwt_timer);
        }
    }

    return status;
}

/**
 * \brief Checks if CRC matches for Block object.
 *
 * \param[in] block Block object to match CRC against.
 * \param[in] expected Expected CRC to be matched.
 * \return bool \c true if CRC matches, \c false otherwise.
 */
bool ifx_t1prime_validate_crc(const ifx_t1prime_block_t *block,
                              uint16_t expected)
{
    // Allocate memory for CRC calculation data (prologue + information field)
    uint8_t *binary =
        (uint8_t *) malloc(IFX_BLOCK_PROLOGUE_LEN + block->information_size);
    if (binary == NULL)
    {
        return false;
    }

    // Put fixed length prologue data
    binary[0] = block->nad;
    binary[1] = block->pcb;
    binary[2] = (block->information_size & 0xff00U) >> 8;
    binary[3] = block->information_size & 0xffU;

    // Put variable length optional information field
    if (block->information_size > 0U)
    {
        // clang-format off
        memcpy(binary + 4U, block->information, block->information_size); // Flawfinder: ignore
        // clang-format on
    }

    // Actually Validate CRC
    uint16_t actual =
        ifx_crc16_ccitt_x25(binary, (1U + 1U + 2U + block->information_size));
    free(binary);
    binary = NULL;
    return actual == expected;
}

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
                                      uint8_t **buffer, size_t *buffer_len)
{
    // Allocate memory for binary data
    *buffer_len = IFX_BLOCK_PROLOGUE_LEN + block->information_size +
                  IFX_BLOCK_EPILOGUE_LEN;
    *buffer = (uint8_t *) malloc(*buffer_len);
    if (*buffer == NULL)
    {
        *buffer_len = 0U;
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_BLOCK_ENCODE,
                         IFX_OUT_OF_MEMORY);
    }

    // Encode fixed length prologue

    // Suppress CppCheck defect #10133: False positive: objectIndex
    // cppcheck-suppress objectIndex
    (*buffer)[0] = block->nad;
    // cppcheck-suppress objectIndex
    (*buffer)[1] = block->pcb;
    // cppcheck-suppress objectIndex
    (*buffer)[2] = (block->information_size & 0xff00U) >> 8;
    // cppcheck-suppress objectIndex
    (*buffer)[3] = block->information_size & 0x00ffU;

    // Encode variable length optional information field
    if (block->information_size > 0U)
    {
        // clang-format off
        memcpy((*buffer) + 4U, block->information, block->information_size); // Flawfinder: ignore
        // clang-format on
    }

    // Encode fixed length epilogue
    uint16_t crc = ifx_crc16_ccitt_x25(*buffer, (*buffer_len) - 2U);
    (*buffer)[(*buffer_len) - 2] = (crc & 0xff00U) >> 8;
    (*buffer)[(*buffer_len) - 1] = crc & 0x00ffU;

    return IFX_SUCCESS;
}

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
                                      const uint8_t *data, size_t data_len)
{
    // Minimum length prologue + epilogue
    if (data_len < (IFX_BLOCK_PROLOGUE_LEN + IFX_BLOCK_EPILOGUE_LEN))
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_BLOCK_DECODE,
                         IFX_TOO_LITTLE_DATA);
    }

    // Clear buffers just to be sure
    block->information = NULL;

    // Parse prologue
    block->nad = data[0];
    block->pcb = data[1];
    block->information_size = (data[2] << 8) | data[3];

    // Check that indicated length matches actual length
    if (data_len != (IFX_BLOCK_PROLOGUE_LEN + (block->information_size) +
                     IFX_BLOCK_EPILOGUE_LEN))
    {
        ifx_t1prime_block_destroy(block);
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_BLOCK_DECODE,
                         IFX_INFORMATION_SIZE_MISMATCH);
    }

    // Parse variable length optional information field
    if (block->information_size > 0U)
    {
        block->information = (uint8_t *) malloc(block->information_size);
        if (block->information == NULL)
        {
            ifx_t1prime_block_destroy(block);
            return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_BLOCK_DECODE,
                             IFX_OUT_OF_MEMORY);
        }
        // clang-format off
        memcpy(block->information, data + IFX_BLOCK_PROLOGUE_LEN, block->information_size); // Flawfinder: ignore
        // clang-format on
    }
    else
    {
        block->information = NULL;
    }

    // Parse epilogue and validate CRC
    uint16_t crc = (data[data_len - 2] << 8) | data[data_len - 1];
    if (!ifx_t1prime_validate_crc(block, crc))
    {
        ifx_t1prime_block_destroy(block);
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_BLOCK_DECODE,
                         IFX_T1PRIME_INVALID_CRC);
    }
    return IFX_SUCCESS;
}

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
void ifx_t1prime_block_destroy(ifx_t1prime_block_t *block)
{
    if ((block->information_size != 0U) && (block->information != NULL))
    {
        free(block->information);
    }
    block->information = NULL;
    block->information_size = 0U;
}

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
                                    size_t data_len)
{
    // Minimum length check
    if (data_len < (1U + 1U + 1U + 1U + 1U + 1U))
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_CIP_DECODE,
                         IFX_TOO_LITTLE_DATA);
    }

    // (Re-) Initialize values for safe calls to cip_destory(CIP*)
    cip->iin = NULL;
    cip->plp_len = 0U;
    cip->plp = NULL;
    cip->dllp_len = 0U;
    cip->dllp = NULL;
    cip->hb_len = 0U;
    cip->hb = NULL;

    // Version (1 byte)
    size_t offset = 0U;
    cip->version = data[offset++];

    // Issuer identification number (variable length)
    cip->iin_len = data[offset++];
    if ((offset + cip->iin_len + 1U + 1U + 1U + 1U) > data_len)
    {
        ifx_t1prime_cip_destroy(cip);
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_CIP_DECODE,
                         IFX_TOO_LITTLE_DATA);
    }
    if (cip->iin_len > 0U)
    {
        cip->iin = (uint8_t *) malloc(cip->iin_len);
        if (cip->iin == NULL)
        {
            return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_CIP_DECODE,
                             IFX_OUT_OF_MEMORY);
        }
        // clang-format off
        memcpy(cip->iin, data + offset, cip->iin_len); // Flawfinder: ignore
        // clang-format on
        offset += cip->iin_len;
    }

    // Physical layer identifier (1 byte)
    cip->plid = data[offset++];

    // Physical layer parameters (variable length)
    cip->plp_len = data[offset++];
    if ((offset + cip->plp_len + 1U + 1U) > data_len)
    {
        ifx_t1prime_cip_destroy(cip);
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_CIP_DECODE,
                         IFX_TOO_LITTLE_DATA);
    }
    if (cip->plp_len > 0U)
    {
        cip->plp = (uint8_t *) malloc(cip->plp_len);
        if (cip->plp == NULL)
        {
            ifx_t1prime_cip_destroy(cip);
            return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_CIP_DECODE,
                             IFX_OUT_OF_MEMORY);
        }
        // clang-format off
        memcpy(cip->plp, data + offset, cip->plp_len); // Flawfinder: ignore
        // clang-format on
        offset += cip->plp_len;
    }

    // Data-link layer parameters (variable length)
    cip->dllp_len = data[offset++];
    if ((offset + cip->dllp_len + 1) > data_len)
    {
        ifx_t1prime_cip_destroy(cip);
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_CIP_DECODE,
                         IFX_TOO_LITTLE_DATA);
    }
    if (cip->dllp_len > 0U)
    {
        cip->dllp = (uint8_t *) malloc(cip->dllp_len);
        if (cip->dllp == NULL)
        {
            ifx_t1prime_cip_destroy(cip);
            return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_CIP_DECODE,
                             IFX_OUT_OF_MEMORY);
        }
        // clang-format off
        memcpy(cip->dllp, data + offset, cip->dllp_len); // Flawfinder: ignore
        // clang-format on
        offset += cip->dllp_len;
    }

    // Historical bytes (variable length)
    cip->hb_len = data[offset++];
    if ((offset + cip->hb_len) != data_len)
    {
        ifx_t1prime_cip_destroy(cip);
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_CIP_DECODE,
                         IFX_T1PRIME_INVALID_LEN);
    }
    if (cip->hb_len > 0U)
    {
        cip->hb = (uint8_t *) malloc(cip->hb_len);
        if (cip->hb == NULL)
        {
            ifx_t1prime_cip_destroy(cip);
            return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_CIP_DECODE,
                             IFX_OUT_OF_MEMORY);
        }
        // clang-format off
        memcpy(cip->hb, data + offset, cip->hb_len); // Flawfinder: ignore
        // clang-format on
    }

    // Validate CIP that it matches specification
    ifx_status_t status = ifx_t1prime_cip_validate(cip);
    if (status != IFX_SUCCESS)
    {
        ifx_t1prime_cip_destroy(cip);
        return status;
    }

    return IFX_SUCCESS;
}

/**
 * \brief Validates CIP object by checking all member values against GP
 * specification.
 *
 * \param[in] cip CIP object to be validated.
 * \return ifx_status_t \c IFX_SUCCESS if valid CIP, any other
 * value in case of error.
 */
ifx_status_t ifx_t1prime_cip_validate(const ifx_cip_t *cip)
{
    // Issuer identification number must be 3 or 4 bytes long
    if ((cip->iin_len < 3U) || (cip->iin_len > 4U))
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_CIP_VALIDATE,
                         IFX_T1PRIME_INVALID_LEN);
    }

    // SPI physical layer parameters must be at least 12 bytes long
    if (cip->plid == IFX_T1PRIME_PLID_SPI)
    {
        if (cip->plp_len < (1U + 1U + 2U + 1U + 1U + 2U + 2U + 2U))
        {
            return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_CIP_VALIDATE,
                             IFX_TOO_LITTLE_DATA);
        }
    }
    // I2C physical layer parameters must be at least 8 bytes long
    else if (cip->plid == IFX_T1PRIME_PLID_I2C)
    {
        if (cip->plp_len < (1U + 1U + 2U + 1U + 1U + 2U))
        {
            return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_CIP_VALIDATE,
                             IFX_TOO_LITTLE_DATA);
        }
    }
    // Unkown physical layer identifier
    else
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_CIP_VALIDATE,
                         IFX_T1PRIME_INVALID_PLID);
    }

    // Data-link layer parameters must be at least 4 bytes long
    if (cip->dllp_len < (2U + 2U))
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_CIP_VALIDATE,
                         IFX_TOO_LITTLE_DATA);
    }

    return IFX_SUCCESS;
}

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
void ifx_t1prime_cip_destroy(ifx_cip_t *cip)
{
    // Issuer identification number
    if ((cip->iin_len > 0U) && (cip->iin != NULL))
    {
        free(cip->iin);
    }
    cip->iin_len = 0U;
    cip->iin = NULL;

    // Physical layer parameters
    if ((cip->plp_len > 0U) && (cip->plp != NULL))
    {
        free(cip->plp);
    }
    cip->plp_len = 0U;
    cip->plp = NULL;

    // Data-link layer parameters
    if ((cip->dllp_len > 0U) && (cip->dllp != NULL))
    {
        free(cip->dllp);
    }
    cip->dllp_len = 0U;
    cip->dllp = NULL;

    // Historical bytes
    if ((cip->hb_len > 0U) && (cip->hb != NULL))
    {
        free(cip->hb);
    }
    cip->hb_len = 0U;
    cip->hb = NULL;
}

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
                                     size_t encoded_len)
{
    if (encoded_len < (2U + 2U))
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_DLLP_DECODE,
                         IFX_TOO_LITTLE_DATA);
    }
    dllp->bwt = (encoded[0] << 8) | encoded[1];
    dllp->ifsc = (encoded[2] << 8) | encoded[3];

    return IFX_SUCCESS;
}

/**
 * \brief Frees memory associated with DLLP object (but not object itself).
 *
 * \details This function does nothing at the moment but keeps the interface
 * uniform accross all datastructures.
 *
 * \param[in] dllp DLLP object whose data shall be freed.
 */
void ifx_t1prime_dllp_destroy(ifx_dllp_t *dllp)
{
    // Ignore unused parameter
    (void) dllp;
}

#ifdef IFX_T1PRIME_INTERFACE_I2C
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
                                    size_t encoded_len)
{
    if (encoded_len < (1U + 1U + 2U + 1U + 1U + 2U))
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_PLP_DECODE,
                         IFX_TOO_LITTLE_DATA);
    }
    plp->configuration = encoded[0];
    plp->pwt = encoded[1];
    plp->mcf = (encoded[2] << 8) | encoded[3];
    plp->pst = encoded[4];
    plp->mpot = encoded[5];
    plp->rwgt = (encoded[6] << 8) | encoded[7];

    return IFX_SUCCESS;
}

/**
 * \brief Frees memory associated with PLP object (but not object itself).
 *
 * \details This function does nothing at the moment but keeps the interface
 * uniform accross all datastructures.
 *
 * \param[in] plp PLP object whose data shall be freed.
 */
void ifx_t1prime_plp_destroy(ifx_plp_t *plp)
{
    // Ignore unused parameter
    (void) plp;
}
#else
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
                                    size_t encoded_len)
{
    if (encoded_len < (1u + 1u + 2u + 1u + 1u + 2u + 2u + 2u))
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_PLP_DECODE,
                         IFX_TOO_LITTLE_DATA);
    }
    plp->configuration = encoded[0];
    plp->pwt = encoded[1];
    plp->mcf = (encoded[2] << 8) | encoded[3];
    plp->pst = encoded[4];
    plp->mpot = encoded[5];
    plp->segt = (encoded[6] << 8) | encoded[7];
    plp->seal = (encoded[8] << 8) | encoded[9];
    plp->wut = (encoded[10] << 8) | encoded[11];

    return IFX_SUCCESS;
}

/**
 * \brief Frees memory associated with PLP object (but not object itself).
 *
 * \details This function does nothing at the moment but keeps the interface
 * uniform across all datastructures.
 *
 * \param[in] plp PLP object whose data shall be freed.
 */
void ifx_t1prime_plp_destroy(ifx_plp_t *plp)
{
    // Ignore unused parameter
    (void) plp;
}
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
                                    size_t data_len)
{
    // Check that length of binary data is valid
    if ((data_len < 1U) || (data_len > 2U))
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_IFS_DECODE,
                         IFX_ILLEGAL_ARGUMENT);
    }

    // Decode data
    if (data_len == 1U)
    {
        *ifs_buffer = data[0];
    }
    else
    {
        *ifs_buffer = (data[0] << 8) | data[1];
    }

    // Validate data
    if ((*ifs_buffer) > IFX_T1PRIME_MAX_IFS)
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_IFS_DECODE,
                         IFX_ILLEGAL_ARGUMENT);
    }
    return IFX_SUCCESS;
}

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
                                    size_t *buffer_len)
{
    // Check that desired IFS value is in range
    if ((ifs == 0U) || (ifs > IFX_T1PRIME_MAX_IFS))
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_IFS_ENCODE,
                         IFX_ILLEGAL_ARGUMENT);
    }

    // Allocate buffer for binary encoding
    *buffer_len = (ifs <= 0xfeU) ? 1U : 2U;
    *buffer = (uint8_t *) malloc(*buffer_len);
    if (*buffer == NULL)
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_IFS_ENCODE,
                         IFX_OUT_OF_MEMORY);
    }

    // Actually encode data
    if ((*buffer_len) == 1U)
    {
        (*buffer)[0] = ifs & 0xffU;
    }
    else
    {
        (*buffer)[0] = (ifs & 0x0f00U) >> 8;
        (*buffer)[1] = ifs & 0x00ffU;
    }

    return IFX_SUCCESS;
}

/**
 * \brief Returns maximum information field size of the secure element (IFSC).
 *
 * \param[in] self T=1' protocol stack to get IFSC for.
 * \param[out] ifs_buffer Buffer to store IFSC value in.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
ifx_status_t ifx_t1prime_get_ifsc(ifx_protocol_t *self, size_t *ifsc_buffer)
{
    // Validate parameters
    if ((self == NULL) || (ifsc_buffer == NULL))
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_GET_IFSC,
                         IFX_ILLEGAL_ARGUMENT);
    }

    ifx_t1prime_protocol_state_t *protocol_state;
    ifx_status_t status = ifx_t1prime_get_protocol_state(self, &protocol_state);
    if (status != IFX_SUCCESS)
    {
        return status;
    }
    *ifsc_buffer = protocol_state->ifsc;
    return IFX_SUCCESS;
}

/**
 * \brief Sets maximum information field size of the host device (IFSD).
 *
 * \param[in] self T=1' protocol stack to set IFSD for.
 * \param[in] ifsd IFS value to be used.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
ifx_status_t ifx_t1prime_set_ifsd(ifx_protocol_t *self, size_t ifsd)
{
    // Prepare IFS information
    ifx_t1prime_block_t request;
    request.nad = 0x21U;
    request.pcb = IFX_T1PRIME_PCB_S_IFS_REQ;
    request.information_size = 0U;
    request.information = NULL;

    // Validate parameters
    if (self == NULL)
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_SET_IFSD,
                         IFX_ILLEGAL_ARGUMENT);
    }
    // Check that desired IFS value is in range
    if ((ifsd == 0U) || (ifsd > IFX_T1PRIME_MAX_IFS))
    {
        IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_ERROR,
                        "Requested to set IFSD to invalid value %xd", ifsd);
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_SET_IFSD,
                         IFX_ILLEGAL_ARGUMENT);
    }

    IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_DEBUG,
                    "Setting IFSD to %xd", ifsd);

    ifx_status_t status = ifx_t1prime_ifs_encode(ifsd, &(request.information),
                                                 &(request.information_size));
    if (status != IFX_SUCCESS)
    {
        return status;
    }

    // Send S(IFS request) to secure element and read back response
    ifx_t1prime_block_t response;
    status = ifx_t1prime_block_transceive(self, &request, &response);
    ifx_t1prime_block_destroy(&request);
    if (status != IFX_SUCCESS)
    {
        return status;
    }

    // Validate response is S(IFS response) -> also implicitly checked by
    // ifx_t1prime_block_transceive
    if (response.pcb != IFX_T1PRIME_PCB_S_IFS_RESP)
    {
        IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_WARN,
                        "Invalid answer to S(IFS request) received (PCB: %02X)",
                        response.pcb);
        ifx_t1prime_block_destroy(&response);
        return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_RECEIVE,
                         IFX_T1PRIME_INVALID_BLOCK);
    }

    // Decode IFS response
    size_t response_ifs;
    status = ifx_t1prime_ifs_decode(&response_ifs, response.information,
                                    response.information_size);
    ifx_t1prime_block_destroy(&response);
    if (status != IFX_SUCCESS)
    {
        IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_WARN,
                        "Invalid S(IFS response) received");
        return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_RECEIVE,
                         IFX_T1PRIME_INVALID_BLOCK);
    }

    // Check that negotiated value matches
    if (response_ifs != ifsd)
    {
        IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_WARN,
                        "Invalid S(IFS response) received");
        return IFX_ERROR(LIB_T1PRIME, IFX_PROTOCOL_RECEIVE,
                         IFX_T1PRIME_INVALID_BLOCK);
    }

    IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_DEBUG,
                    "Successfully set IFSD to %xd", ifsd);
    return IFX_SUCCESS;
}

/**
 * \brief Returns current block waiting time (BWT) in [ms].
 *
 * \param[in] self T=1' protocol stack to get BWT for.
 * \param[out] bwt_ms_buffer Buffer to store BWT value in.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 * \cond FULL_DOCUMENTATION_BUILD
 * \relates ifx_protocol
 * \endcond
 */
ifx_status_t ifx_t1prime_get_bwt(ifx_protocol_t *self, uint16_t *bwt_ms_buffer)
{
    // Validate parameters
    if ((self == NULL) || (bwt_ms_buffer == NULL))
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_GET_BWT,
                         IFX_ILLEGAL_ARGUMENT);
    }

    ifx_t1prime_protocol_state_t *protocol_state;
    ifx_status_t status = ifx_t1prime_get_protocol_state(self, &protocol_state);
    if (status != IFX_SUCCESS)
    {
        return status;
    }
    *bwt_ms_buffer = protocol_state->bwt;
    return IFX_SUCCESS;
}

/**
 * \brief Sets block waiting time (BWT) in [ms].
 *
 * \param[in] self T=1' protocol stack to set BWT for.
 * \param[in] bwt_ms BWT value to be used.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 * \cond FULL_DOCUMENTATION_BUILD
 * \relates ifx_protocol
 * \endcond
 */
ifx_status_t ifx_t1prime_set_bwt(ifx_protocol_t *self, uint16_t bwt_ms)
{
    // Validate parameters
    if (self == NULL)
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_SET_BWT,
                         IFX_ILLEGAL_ARGUMENT);
    }

    ifx_t1prime_protocol_state_t *protocol_state;
    ifx_status_t status = ifx_t1prime_get_protocol_state(self, &protocol_state);
    if (status != IFX_SUCCESS)
    {
        return status;
    }
    protocol_state->bwt = bwt_ms;
    IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_DEBUG,
                    "Successfully set BWT to %d", bwt_ms);
    return IFX_SUCCESS;
}

/**
 * \brief Sets T=1' interrupt handler function.
 *
 * \details If value is not set (\c NULL - default) then polling mode will be
 * used.
 *
 * \param[in] self T=1' protocol stack to set interrupt handler for.
 * \param[in] irq Interrupt handler to be used.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
ifx_status_t ifx_t1prime_set_irq_handler(ifx_protocol_t *self,
                                         ifx_t1prime_irq_handler_t irq)
{
    // Validate parameters
    if (self == NULL)
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_SET_IRQ_HANDLER,
                         IFX_ILLEGAL_ARGUMENT);
    }

    // Cache interrupt handler for later use
    ifx_t1prime_protocol_state_t *protocol_state;
    ifx_status_t status = ifx_t1prime_get_protocol_state(self, &protocol_state);
    if (status != IFX_SUCCESS)
    {
        return status;
    }
    protocol_state->irq_handler = irq;
    if (irq == NULL)
    {
        IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_DEBUG,
                        "Successfully cleared T=1' interrupt handler");
    }
    else
    {
        IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_DEBUG,
                        "Successfully set T=1' interrupt handler");
    }
    return IFX_SUCCESS;
}

/**
 * \brief Getter for T=1' interrupt handler function.
 *
 * \details If value is not set then polling mode is used and \c NULL is
 * returned.
 *
 * \param[in] self T=1' protocol stack to get interrupt handler for.
 * \param[in] irq_buffer Buffer to store interrupt handler in.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error. \cond FULL_DOCUMENTATION_BUILD \relates Protocol
 * \endcond
 */
ifx_status_t ifx_t1prime_get_irq_handler(ifx_protocol_t *self,
                                         ifx_t1prime_irq_handler_t *irq_buffer)
{
    // Validate parameters
    if ((self == NULL) || (irq_buffer == NULL))
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_GET_IRQ_HANDLER,
                         IFX_ILLEGAL_ARGUMENT);
    }

    ifx_t1prime_protocol_state_t *protocol_state;
    ifx_status_t status = ifx_t1prime_get_protocol_state(self, &protocol_state);
    if (status != IFX_SUCCESS)
    {
        return status;
    }
    *irq_buffer = protocol_state->irq_handler;
    return IFX_SUCCESS;
}

/**
 * \brief Returns current protocol state for Global Platform T=1' protocol.
 *
 * \param[in] self T=1' protocol stack to get protocol state for.
 * \param[out] protocol_state_buffer Buffer to store protocol state in.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
ifx_status_t ifx_t1prime_get_protocol_state(
    ifx_protocol_t *self, ifx_t1prime_protocol_state_t **protocol_state_buffer)
{
    // Validate parameters
    if (self == NULL)
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_GET_PROTOCOL_STATE,
                         IFX_ILLEGAL_ARGUMENT);
    }
    if (protocol_state_buffer == NULL)
    {
        IFX_T1PRIME_LOG(self->_logger, IFX_LOG_TAG, IFX_LOG_ERROR,
                        "ifx_t1prime_get_protocol_state() called with illegal "
                        "NULL argument");
        return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_GET_PROTOCOL_STATE,
                         IFX_ILLEGAL_ARGUMENT);
    }

    // Verify that correct protocol layer called this function
    if (self->_layer_id != IFX_T1PRIME_PROTOCOL_LAYER_ID)
    {
        if (self->_base == NULL)
        {
            return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_GET_PROTOCOL_STATE,
                             IFX_PROTOCOL_STACK_INVALID);
        }
        return ifx_t1prime_get_protocol_state(self->_base,
                                              protocol_state_buffer);
    }

    // Check if protocol state has been initialized
    if (self->_properties == NULL)
    {
        // Lazy initialize properties
        self->_properties = (ifx_t1prime_protocol_state_t *) malloc(
            sizeof(ifx_t1prime_protocol_state_t));
        if (self->_properties == NULL)
        {
            return IFX_ERROR(LIB_T1PRIME, IFX_T1PRIME_GET_PROTOCOL_STATE,
                             IFX_OUT_OF_MEMORY);
        }
        ifx_t1prime_protocol_state_t *properties =
            (ifx_t1prime_protocol_state_t *) self->_properties;
        properties->bwt = IFX_T1PRIME_DEFAULT_BWT_MS;
        properties->ifsc = IFX_T1PRIME_MAX_IFS;
        properties->send_counter = 0x00U;
        properties->receive_counter = 0x00U;
        properties->wtx = 0x00U;
        properties->irq_handler = NULL;
        properties->pwt = IFX_T1PRIME_DEFAULT_PWT_MS;
#ifdef IFX_T1PRIME_INTERFACE_I2C
        properties->mpot = IFX_T1PRIME_DEFAULT_I2C_MPOT_100US;
#else
        properties->mpot = IFX_T1PRIME_DEFAULT_SPI_MPOT_100US;
#endif
    }

    *protocol_state_buffer = (ifx_t1prime_protocol_state_t *) self->_properties;
    return IFX_SUCCESS;
}

/**
 * \brief Extension of ifx_logger_log() for logging T=1' blocks.
 *
 * \param[in] logger ifx_logger_t  object holding concrete implementation.
 * \param[in] source String with information where the log originated from-
 * \param[in] level ifx_log_level of message (used for filtering).
 * \param[in] msg Optional message to be prepended to block (might be \c NULL).
 * \param[in] block Block to be logged.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other value in
 * case of error.
 */
ifx_status_t ifx_t1prime_log_block(const ifx_logger_t *logger,
                                   const char *source, ifx_log_level level,
                                   const char *msg,
                                   const ifx_t1prime_block_t *block)
{
// Logging disabled at compile time
#ifndef IFX_DISABLE_LOGGING
    // Validate parameters
    if ((logger == NULL) || (logger->_log == NULL) || (block == NULL))
    {
        return IFX_ERROR(LIB_T1PRIME, IFX_LOGGER_LOG, IFX_ILLEGAL_ARGUMENT);
    }

    // Pre-check level
    if (level < logger->_level)
    {
        return IFX_SUCCESS;
    }

    // Get string representation of block
    char *representation;
    switch (block->pcb)
    {
    case IFX_T1PRIME_PCB_I(0U, 0U):
        representation = (char *) "I(0, 0)";
        break;
    case IFX_T1PRIME_PCB_I(1U, 0U):
        representation = (char *) "I(1, 0)";
        break;
    case IFX_T1PRIME_PCB_I(0U, 1U):
        representation = (char *) "I(0, 1)";
        break;
    case IFX_T1PRIME_PCB_I(1U, 1U):
        representation = (char *) "I(1, 1)";
        break;
    case IFX_T1PRIME_PCB_R_ACK(0U):
        representation = (char *) "R(0)";
        break;
    case IFX_T1PRIME_PCB_R_ACK(1U):
        representation = (char *) "R(1)";
        break;
    case IFX_T1PRIME_PCB_R_CRC(0U):
        representation = (char *) "R(0) CRC";
        break;
    case IFX_T1PRIME_PCB_R_CRC(1U):
        representation = (char *) "R(1) CRC";
        break;
    case IFX_T1PRIME_PCB_R_ERROR(0U):
        representation = (char *) "R(0) Error";
        break;
    case IFX_T1PRIME_PCB_R_ERROR(1U):
        representation = (char *) "R(1) Error";
        break;
    case IFX_T1PRIME_PCB_S_RESYNCH_REQ:
        representation = (char *) "S(RESYNCH request)";
        break;
    case IFX_T1PRIME_PCB_S_RESYNCH_RESP:
        representation = (char *) "S(RESYNCH response)";
        break;
    case IFX_T1PRIME_PCB_S_IFS_REQ:
        representation = (char *) "S(IFS request)";
        break;
    case IFX_T1PRIME_PCB_S_IFS_RESP:
        representation = (char *) "S(IFS response)";
        break;
    case IFX_T1PRIME_PCB_S_ABORT_REQ:
        representation = (char *) "S(ABORT request)";
        break;
    case IFX_T1PRIME_PCB_S_ABORT_RESP:
        representation = (char *) "S(ABORT response)";
        break;
    case IFX_T1PRIME_PCB_S_WTX_REQ:
        representation = (char *) "S(WTX request)";
        break;
    case IFX_T1PRIME_PCB_S_WTX_RESP:
        representation = (char *) "S(WTX response)";
        break;
    case IFX_T1PRIME_PCB_S_CIP_REQ:
        representation = (char *) "S(CIP request)";
        break;
    case IFX_T1PRIME_PCB_S_CIP_RESP:
        representation = (char *) "S(CIP response)";
        break;
    case IFX_T1PRIME_PCB_S_RELEASE_REQ:
        representation = (char *) "S(RELEASE request)";
        break;
    case IFX_T1PRIME_PCB_S_RELEASE_RESP:
        representation = (char *) "S(RELEASE response)";
        break;
    case IFX_T1PRIME_PCB_S_SWR_REQ:
        representation = (char *) "S(SWR request)";
        break;
    case IFX_T1PRIME_PCB_S_SWR_RESP:
        representation = (char *) "S(SWR response)";
        break;
    case IFX_T1PRIME_PCB_S_POR_REQ:
        representation = (char *) "S(POR request)";
        break;
    default:
        representation = (char *) "Unknown block";
        break;
    }

    // Either use block representation or format output string
    char *formatted;
    if ((msg == NULL) || (strlen(msg) == 0U)) // Flawfinder: ignore
    {
        formatted = representation;
    }
    else
    {
        size_t formatted_length =
            snprintf(NULL, 0U, "%s%s", msg, representation);
        formatted = (char *) malloc(formatted_length + 1U);
        if (formatted == NULL)
        {
            return IFX_ERROR(LIB_T1PRIME, IFX_LOGGER_LOG, IFX_OUT_OF_MEMORY);
        }
// Disable sprintf warning because msbuild warning not valid in C99
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996)
#endif
        snprintf(formatted, formatted_length + 1U, "%s%s", msg, representation);
#ifdef _MSC_VER
#pragma warning(pop)
#endif
    }

    // Actually log message
    ifx_status_t status = logger->_log(logger, source, level, formatted);

    // Free memory if dynamically allocated
    if ((msg != NULL) && (strlen(msg) > 0U)) // Flawfinder: ignore
    {
        free(formatted);
        formatted = NULL;
    }

    return status;
#else // IFX_DISABLE_LOGGING
    return IFX_SUCCESS;
#endif
}
