// SPDX-FileCopyrightText: 2024 Infineon Technologies AG
//
// SPDX-License-Identifier: MIT

/**
 * \file infineon/ifx-utils.h
 * \brief Provides utility functions and macros
 */
#ifndef IFX_UTILS_H
#define IFX_UTILS_H

#include <stdint.h>

#include "ifx-utils-lib.h"

#if !defined(IFX_MEMCPY) || !defined(IFX_MEMCMP) || !defined(IFX_MEMSET) ||    \
    (!defined(IFX_FREE))
#include <string.h>
#endif

#ifndef IFX_MEMCPY
/**
 * \brief Allows custom definition of memory copy API. By default, memcpy() from
 * string.h is used.
 */
#define IFX_MEMCPY(dest, src, size)                                            \
    memcpy(dest, src, size) /* flawfinder: ignore*/

#endif

#ifndef IFX_MEMCMP
/**
 * \brief Allows custom definition of memory compare API. By default, memcmp()
 * from string.h is used.
 */
#define IFX_MEMCMP(first_memory, second_memory, size)                          \
    memcmp(first_memory, second_memory, size)
#endif

#ifndef IFX_MEMSET
/**
 * \brief Allows custom definition of memory set API. By default, memset() from
 * string.h is used.
 */
#define IFX_MEMSET(buffer, value, size) memset(buffer, value, size)
#endif

#ifndef IFX_FREE
/**
 * \brief Allows custom definition of free(free pointer) API. By default, free()
 * from string.h is used.
 */
#define IFX_FREE(buffer) free(buffer)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Macro to suppress unused variable warnings.
 */
#define IFX_UNUSED_VARIABLE(var)           (void) var

/* Function identifiers */

/**
 * \brief Function identifier of utils concat for ifx_blob_t type data.
 */
#define IFX_UTILS_CONCAT                   UINT8_C(0x01)

/**
 * \brief Defines 0x01.
 */
#define IFX_ENABLE                         UINT8_C(0x01)

/**
 * \brief Defines 0x00.
 */
#define IFX_DISABLE                        UINT8_C(0x00)

/**
 * \brief Indicates Endianness of architecture.
 */
#define IFX_LITTLE_ENDIAN                  IFX_ENABLE

/**
 * \brief Validation of NULL pointer
 */
#define IFX_VALIDATE_NULL_PTR              IFX_ENABLE

/**
 * \brief Macro to check if the blob or its buffer is NULL.
 */
#define IFX_VALIDATE_NULL_PTR_BLOB(blob)   (blob == NULL || blob->buffer == NULL)

/**
 * \brief Macro to check if the memory address is pointing to NULL.
 */
#define IFX_VALIDATE_NULL_PTR_MEMORY(data) (data == NULL)

#if IFX_LITTLE_ENDIAN
/*#warning "Little Endian Compiler" */

/**
 * \brief Stores lower(little endian) byte from p2 to p1
 */
#define IFX_GET_LOWER_BYTE(p1_value, p2_value)                                 \
    (p1_value) = ((p2_value) & 0x00FF);

/**
 * \brief Stores upper(little endian) byte from p2 to p1
 */
#define IFX_GET_UPPER_BYTE(p1_value, p2_value)                                 \
    (p1_value) = (((p2_value) & 0xFF00) >> 8);

/**
 * \brief Read data from 'Buffer' Array and update 'Value' variable,
 * Array is input parameter and value is output parameter
 */
#define IFX_READ_U16(buffer, value)                                            \
    {                                                                          \
        value = (((*(uint8_t *) buffer) << 8) | (*(uint8_t *) (buffer + 1)));  \
    }

/**
 * \brief Read data from 'Value' variable and update 'Buffer' Array,
 *  Array is output parameter and value is input parameter
 */
#define IFX_UPDATE_U16(buffer, value)                                          \
    {                                                                          \
        *(uint8_t *) buffer = (uint8_t) ((value >> 8) & 0x00FF);               \
        *(uint8_t *) (buffer + 1) = (uint8_t) (value);                         \
    }

#else
/*#warning "Big Endian Compiler"*/

/**
 * \brief Stores lower(big endian) byte from p2 to p1
 */
#define IFX_GET_LOWER_BYTE(p1_value, p2_value)                                 \
    (p1_value) = (((p2_value) & 0xff00) >> 8);

/**
 * \brief Stores upper(big endian) byte from p2 to p1
 */
#define IFX_GET_UPPER_BYTE(p1_value, p2_value)                                 \
    (p1_value) = ((p2_value) & 0x00FF);
#endif

/**
 * \brief Macro for little endian/big endian read operation
 * To read array data to a word variable
 *
 * Read data from 'Buffer' Array and update 'Value' variable,
 * Array is input parameter
 * and value is output parameter
 */
#define IFX_READ_U32(buffer, u32value)                                         \
    {                                                                          \
        u32value = ((((*(uint8_t *) (buffer)) << 24) & 0XFF000000) |           \
                    (((*(uint8_t *) (buffer + 1)) << 16) & 0X00FF0000) |       \
                    (((*(uint8_t *) (buffer + 2)) << 8) & 0X0000FF00) |        \
                    (((*(uint8_t *) (buffer + 3)) & 0X000000FF)));             \
    }

/**
 * \brief Macro for little endian/big endian update operation
 * To update array data with value of word variable
 *
 * Read data from 'Value' variable and update 'Buffer' Array,
 * Array is output parameter
 * and value is input parameter
 */

#define IFX_UPDATE_U32(buffer, u32value)                                       \
    {                                                                          \
        *(uint8_t *) buffer = (uint8_t) ((u32value >> 24) & 0xFF);             \
        *(uint8_t *) (buffer + 1) = (uint8_t) ((u32value >> 16) & 0xFF);       \
        *(uint8_t *) (buffer + 2) = (uint8_t) ((u32value >> 8) & 0xFF);        \
        *(uint8_t *) (buffer + 3) = (uint8_t) (u32value);                      \
    }

/**
 * \brief Data storage for data and data length where both are required as
 * parameters.
 */
typedef struct
{
    /** Length of the byte stream */
    uint32_t length;

    /** Pointer to byte array which contains the bytes stream */
    uint8_t *buffer;
} ifx_blob_t;

/**
 * \brief Concatenates the buffers of two ifx_blob_t type data structures.
 * Buffer field of append_data will be appended at end of the buffer field of
 * result.
 *
 * \note In this API, param 'result->buffer' will be reallocated with the memory
 * needed to store the concatenated buffer value. User need to take care to free
 * the memory if required.
 *
 * \param[in] append_data Data to be appended.
 * \param[in,out] result Destination in which the data will be appended.
 * \return ifx_status_t
 * \retval IFX_ILLEGAL_ARGUMENT : If invalid/NULL parameter is passed to
 * function
 * \retval IFX_OUT_OF_MEMORY : If memory allocation fails
 */
ifx_status_t ifx_utils_concat(const ifx_blob_t *append_data,
                              ifx_blob_t *result);

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /* IFX_UTILS_H */
