// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file ifx-t1prime.h
 * \brief Internal definitions for Global Platform T=1' protocol.
 */
#ifndef T1PRIME_H
#define T1PRIME_H

#include <stdint.h>
#include "infineon/ifx-protocol.h"
#include "t1prime/ifx-t1prime-datastructures.h"

/**
 * @def IFX_T1PRIME_LOG_BLOCK
 * @brief T1PRIME library block of data log macro.
 *
 * @param[in] logger_object Logger object in which logging to be done.
 * @param[in] source_information Source information from where logs are
 * originated.
 * @param[in] logger_level Log level
 * @param[in] formatter String with formatter for data passed as variable
 * argument
 * @param[in] block Block of data
 *
 * It maps to the ifx_t1prime_log_block function if IFX_T1PRIME_LOG_ENABLE macro
 * is defined other wise it defines IFX_SUCCESS.
 *
 */

#ifdef IFX_T1PRIME_LOG_ENABLE
#define IFX_T1PRIME_LOG_BLOCK(logger_object, source_information, logger_level, \
                              formatter, block)                                \
    ifx_t1prime_log_block(logger_object, source_information, logger_level,     \
                          formatter, block)
#else
#define IFX_T1PRIME_LOG_BLOCK(logger_object, source_information, logger_level, \
                              formatter, block)

#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Protocol Layer ID for Global Platform T=1' protocol.
 *
 * \details Used to verify that correct protocol layer has called member
 * functionality.
 */
#define IFX_T1PRIME_PROTOCOL_LAYER_ID 0x01u

/**
 * \brief Creates Global Platform T=1' Protocol Control Byte (PCB) of given
 * type.
 *
 * \param[in] type Type of PCB.
 * \return uint8_t PCB to be used in Block.
 */
#define IFX_T1PRIME_PCB(type)         ((type) & 0xffu)

/**
 * \brief Creates Global Platform T=1' Protocol Control Byte (PCB) for I blocks.
 *
 * \param[in] ns Send sequence counter of I block.
 * \param[in] m \c true if I block shall have \c more bit set.
 * \return uint8_t I block PCB to be used in Block.
 */
#define IFX_T1PRIME_PCB_I(ns, m)                                               \
    (IFX_T1PRIME_PCB(((ns) ? 0x40u : 0x00u) | ((m) ? 0x20u : 0x00u)))

/**
 * \brief Checks if Global Platform T=1' Protocol Control Byte (PCB) indicates
 * an I block.
 *
 * \param[in] pcb PCB to be checked.
 * \return bool \c true if PCB indicates an I block.
 */
#define IFX_T1PRIME_PCB_IS_I(pcb)     ((IFX_T1PRIME_PCB((pcb)) & 0x80u) == 0x00u)

/**
 * \brief Gets sequence counter for Global Platform T=1' Protocol Control Byte
 * (PCB) for I blocks.
 *
 * \param[in] pcb PCB to be checked.
 * \return int Sequence counter from I block.
 */
#define IFX_T1PRIME_PCB_I_GET_NS(pcb) ((IFX_T1PRIME_PCB((pcb)) & 0x40u) >> 6)

/**
 * \brief Checks if Global Platform T=1' Protocol Control Byte (PCB) for I
 * blocks indicates more data will follow.
 *
 * \param[in] pcb PCB to be checked.
 * \return bool \c true if PCB indicates more data.
 */
#define IFX_T1PRIME_PCB_I_HAS_MORE(pcb)                                        \
    ((IFX_T1PRIME_PCB((pcb)) & 0x20u) == 0x20u)

/**
 * \brief Creates Global Platform T=1' Protocol Control Byte (PCB) for R blocks.
 *
 * \param[in] ns Receive sequence counter of R block.
 * \param[in] type Type of R block to be indicated by PCB.
 * \return uint8_t R block PCB to be used in Block.
 */
#define IFX_T1PRIME_PCB_R(nr, type)                                            \
    (IFX_T1PRIME_PCB(0x80u | ((nr) ? 0x10u : 0x00u) | ((type) & 0x0fu)))

/**
 * \brief Checks if Global Platform T=1' Protocol Control Byte (PCB) indicates a
 * R block.
 *
 * \param[in] pcb PCB to be checked.
 * \return bool \c true if PCB indicates a R block.
 */
#define IFX_T1PRIME_PCB_IS_R(pcb)     ((IFX_T1PRIME_PCB((pcb)) & 0xc0u) == 0x80u)

/**
 * \brief Gets sequence counter for Global Platform T=1' Protocol Control Byte
 * (PCB) for R blocks.
 *
 * \param[in] pcb PCB to be checked.
 * \return int Sequence counter from R block.
 */
#define IFX_T1PRIME_PCB_R_GET_NR(pcb) ((IFX_T1PRIME_PCB((pcb)) & 0x10u) >> 4)

/**
 * \brief Creates Global Platform T=1' Protocol Control Byte (PCB) for
 * error-free acknowledgement R blocks.
 *
 * \param[in] nr Receive sequence counter of R block.
 * \return uint8_t R block PCB to be used by Block.
 */
#define IFX_T1PRIME_PCB_R_ACK(nr)     IFX_T1PRIME_PCB_R((nr), 0x00u)

/**
 * \brief Checks if Global Platform T=1' Protocol Control Byte (PCB) indicates
 * an error free acknowledgement R block.
 *
 * \param[in] pcb PCB to be checked.
 * \return bool \c true if PCB indicates an error free acknowledgement R block.
 */
#define IFX_T1PRIME_PCB_IS_R_ACK(pcb)                                          \
    (IFX_T1PRIME_PCB_IS_R(pcb) && (((pcb) & 0x0fu) == 0x00u))

/**
 * \brief Creates Global Platform T=1' Protocol Control Byte (PCB) for CRC error
 * R blocks.
 *
 * \param[in] nr Receive sequence counter of R block.
 * \return uint8_t R block PCB to be used by Block.
 */
#define IFX_T1PRIME_PCB_R_CRC(nr)   IFX_T1PRIME_PCB_R((nr), 0x01u)

/**
 * \brief Creates Global Platform T=1' Protocol Control Byte (PCB) for other
 * error R blocks.
 *
 * \param[in] nr Receive sequence counter of R block.
 * \return uint8_t R block PCB to be used by Block.
 */
#define IFX_T1PRIME_PCB_R_ERROR(nr) IFX_T1PRIME_PCB_R((nr), 0x02u)

/**
 * \brief Creates Global Platform T=1' Protocol Control Byte (PCB) for S blocks.
 *
 * \param[in] type Type of S block to be indicated by PCB.
 * \param[in] is_response \c true if PCB shall indicate response S block.
 * \return uint8_t S block PCB to be used in Block.
 */
#define IFX_T1PRIME_PCB_S(type, is_response)                                   \
    IFX_T1PRIME_PCB(0xc0u | ((is_response) ? 0x20u : 0x00u) | ((type) & 0x0fu))

/**
 * \brief Checks if Global Platform T=1' Protocol Control Byte (PCB) indicates a
 * S block.
 *
 * \param[in] pcb PCB to be checked.
 * \return bool \c true if PCB indicates a S block.
 */
#define IFX_T1PRIME_PCB_IS_S(pcb) ((IFX_T1PRIME_PCB((pcb)) & 0xc0u) == 0xc0u)

/**
 * \brief Checks if Global Platform T=1' Protocol Control Byte (PCB) indicates a
 * request S block.
 *
 * \param[in] pcb PCB to be checked.
 * \return bool \c true if PCB indicates a request S block.
 */
#define IFX_T1PRIME_PCB_S_IS_REQ(pcb)                                          \
    ((IFX_T1PRIME_PCB((pcb)) & 0x20u) == 0x00u)

/**
 * \brief Gets type of S(? ?) block from Global Platform T=1' Protocol Control
 * Byte (PCB).
 *
 * \param[in] pcb PCB to get S block type for.
 * \return int S block type.
 */
#define IFX_T1PRIME_PCB_S_GET_TYPE(pcb)      (IFX_T1PRIME_PCB((pcb)) & 0x1fu)

/**
 * \brief Global Platform T=1' Protocol Control Byte (PCB) for S(RESYNCH
 * request) blocks.
 */
#define IFX_T1PRIME_PCB_S_RESYNCH_REQ        IFX_T1PRIME_PCB_S(0x0u, false)

/**
 * \brief Global Platform T=1' Protocol Control Byte (PCB) for S(RESYNCH
 * response) blocks.
 */
#define IFX_T1PRIME_PCB_S_RESYNCH_RESP       IFX_T1PRIME_PCB_S(0x0u, true)

/**
 * \brief Global Platform T=1' Protocol Control Byte (PCB) for S(IFS request)
 * blocks.
 */
#define IFX_T1PRIME_PCB_S_IFS_REQ            IFX_T1PRIME_PCB_S(0x1u, false)

/**
 * \brief Global Platform T=1' Protocol Control Byte (PCB) for S(IFS response)
 * blocks.
 */
#define IFX_T1PRIME_PCB_S_IFS_RESP           IFX_T1PRIME_PCB_S(0x1u, true)

/**
 * \brief Global Platform T=1' Protocol Control Byte (PCB) for S(ABORT request)
 * blocks.
 */
#define IFX_T1PRIME_PCB_S_ABORT_REQ          IFX_T1PRIME_PCB_S(0x2u, false)

/**
 * \brief Global Platform T=1' Protocol Control Byte (PCB) for S(ABORT response)
 * blocks.
 */
#define IFX_T1PRIME_PCB_S_ABORT_RESP         IFX_T1PRIME_PCB_S(0x2u, true)

/**
 * \brief Global Platform T=1' Protocol Control Byte (PCB) for S(WTX request)
 * blocks.
 */
#define IFX_T1PRIME_PCB_S_WTX_REQ            IFX_T1PRIME_PCB_S(0x3u, false)

/**
 * \brief Global Platform T=1' Protocol Control Byte (PCB) for S(WTX response)
 * blocks.
 */
#define IFX_T1PRIME_PCB_S_WTX_RESP           IFX_T1PRIME_PCB_S(0x3u, true)

/**
 * \brief Global Platform T=1' Protocol Control Byte (PCB) for S(CIP request)
 * blocks.
 */
#define IFX_T1PRIME_PCB_S_CIP_REQ            IFX_T1PRIME_PCB_S(0x4u, false)

/**
 * \brief Global Platform T=1' Protocol Control Byte (PCB) for S(CIP response)
 * blocks.
 */
#define IFX_T1PRIME_PCB_S_CIP_RESP           IFX_T1PRIME_PCB_S(0x4u, true)

/**
 * \brief Global Platform T=1' Protocol Control Byte (PCB) for S(RELEASE
 * request) blocks.
 */
#define IFX_T1PRIME_PCB_S_RELEASE_REQ        IFX_T1PRIME_PCB_S(0x6u, false)

/**
 * \brief Global Platform T=1' Protocol Control Byte (PCB) for S(RELEASE
 * response) blocks.
 */
#define IFX_T1PRIME_PCB_S_RELEASE_RESP       IFX_T1PRIME_PCB_S(0x6u, true)

/**
 * \brief Global Platform T=1' Protocol Control Byte (PCB) for S(SWR request)
 * blocks.
 */
#define IFX_T1PRIME_PCB_S_SWR_REQ            IFX_T1PRIME_PCB_S(0xfu, false)

/**
 * \brief Global Platform T=1' Protocol Control Byte (PCB) for S(SWR response)
 * blocks.
 */
#define IFX_T1PRIME_PCB_S_SWR_RESP           IFX_T1PRIME_PCB_S(0xfu, true)

/**
 * \brief Error reason if secure element aborted transmission during
 * ifx_t1prime_transceive().
 */
#define IFX_TRANSCEIVE_ABORTED               0x60u

/**
 * \brief Error reason if invalid Block received in any secure element
 * interaction.
 */
#define IFX_T1PRIME_INVALID_BLOCK            0x61u

/**
 * \brief Node address byte (NAD) for transmission from host device to secure
 * element.
 */
#define IFX_NAD_HD_TO_SE                     0x21u

/**
 * \brief Number of read retries after which ifx_t1prime_block_transceive()
 * shall fail.
 */
#define IFX_T1PRIME_BLOCK_TRANSCEIVE_RETRIES 2u

/**
 * \brief PCB byte for POR - Request
 */
#define IFX_T1PRIME_PCB_S_POR_REQ            0xd8

#ifdef IFX_T1PRIME_INTERFACE_I2C
/**
 * \brief Default I2C clock frequency in [Hz].
 */
#define IFX_T1PRIME_DEFAULT_I2C_CLOCK_FREQUENCY_HZ 400000u

/**
 * \brief Default I2C minimum polling time in [multiple of 100us].
 */
#define IFX_T1PRIME_DEFAULT_I2C_MPOT_100US         10u

/**
 * \brief Default I2C read / write guard time in [us].
 */
#define IFX_T1PRIME_DEFAULT_I2C_RWGT_US            10u

/**
 * \brief Enable PWT delay after SWR and POR.
 */
#define IFX_T1PRIME_RESET_DELAY_PWT                1
#else
/**
 * \brief Default SPI clock frequency in [Hz].
 */
#define IFX_T1PRIME_DEFAULT_SPI_CLOCK_FREQUENCY_HZ 1000000u

/**
 * \brief Default SPI secure element guard time (SEGT) in [us].
 */
#define IFX_T1PRIME_DEFAULT_SPI_SEGT_US            200u

/**
 * \brief Default SPI secure element access length (SEAL) in [byte].
 */
#define IFX_T1PRIME_DEFAULT_SPI_SEAL               16u

/**
 * \brief Default SPI minimum polling time in [multiple of 100us].
 */
#define IFX_T1PRIME_DEFAULT_SPI_MPOT_100US         10u
#endif

/**
 * \brief Default value for power wake-up time in [ms].
 */
#define IFX_T1PRIME_DEFAULT_PWT_MS 25u

/**
 * \brief Default value for current maximum information field size (IFS).
 */
#define IFX_T1PRIME_DEFAULT_IFSC   0x08u

/**
 * \brief Default value for current block waiting time in [ms].
 */
#define IFX_T1PRIME_DEFAULT_BWT_MS ((uint16_t) 300u)

/**
 * \brief ifx_protocol_activate_callback_t for Global Platform T=1' protocol.
 *
 * \see ifx_protocol_activate_callback_t
 */
ifx_status_t ifx_t1prime_activate(ifx_protocol_t *self, uint8_t **response,
                                  size_t *response_len);

/**
 * \brief ifx_protocol_transceive_callback_t for Global Platform T=1' protocol.
 *
 * \see ifx_protocol_transceive_callback_t
 */
ifx_status_t ifx_t1prime_transceive(ifx_protocol_t *self, const uint8_t *data,
                                    size_t data_len, uint8_t **response,
                                    size_t *response_len);

/**
 * \brief ifx_protocol_destroy_callback_t for Global Platform T=1' protocol.
 *
 * \see ifx_protocol_destroy_callback_t
 */
void ifx_t1prime_destroy(ifx_protocol_t *self);

/**
 * \brief Sends Block to secure element.
 *
 * \param[in] self Protocol stack for performing necessary operations.
 * \param[in] block Block object to be send to secure element.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
ifx_status_t ifx_t1prime_block_transmit(ifx_protocol_t *self,
                                        const ifx_t1prime_block_t *block);

/**
 * \brief Reads Block from secure element.
 *
 * \param[in] self Protocol stack for performing necessary operations.
 * \param[out] block Block object to store received data in.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
ifx_status_t t1prime_block_receive(ifx_protocol_t *self,
                                   ifx_t1prime_block_t *block);

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
                                          ifx_t1prime_block_t *response_buffer);

/**
 * \brief Performs Global Platform T=1' RESYNCH operation.
 *
 * \details Sends S(RESYNCH request) and expects S(RESYNCH response).
 *
 * \param[in] self Protocol stack for performing necessary operations.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
ifx_status_t ifx_t1prime_s_resynch(ifx_protocol_t *self);

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
ifx_status_t ifx_t1prime_s_cip(ifx_protocol_t *self, ifx_cip_t *cip);

/**
 * \brief Performs Global Platform T=1' software reset (SWR).
 *
 * \details Sends S(SWR request) and expects S(SWR response).
 *
 * \param[in] self Protocol stack for performing necessary operations.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
ifx_status_t ifx_t1prime_s_swr(ifx_protocol_t *self);

/**
 * \brief Returns maximum information field size of the secure element (IFSC).
 *
 * \param[in] self T=1' protocol stack to get IFSC for.
 * \param[out] ifs_buffer Buffer to store IFSC value in.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
ifx_status_t ifx_t1prime_get_ifsc(ifx_protocol_t *self, size_t *ifsc_buffer);

/**
 * \brief Returns current protocol state for Global Platform T=1' protocol.
 *
 * \param[in] self T=1' protocol stack to get protocol state for.
 * \param[out] protocol_state_buffer Buffer to store protocol state in.
 * \return ifx_status_t \c IFX_SUCCESS if successful, any other
 * value in case of error.
 */
ifx_status_t ifx_t1prime_get_protocol_state(
    ifx_protocol_t *self, ifx_t1prime_protocol_state_t **protocol_state_buffer);

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
                                   const ifx_t1prime_block_t *block);

#ifdef __cplusplus
}
#endif

#endif // T1PRIME_H
