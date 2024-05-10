# SPDX-FileCopyrightText: 2024 Infineon Technologies AG
#
# SPDX-License-Identifier: MIT

# ##############################################################################
# Project configuration and settings
# ##############################################################################
cmake_minimum_required(VERSION 3.14)

# Disable in-source builds
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

# Main project configuration
project(
  hsw-nbt
  VERSION 1.1.1
  DESCRIPTION "OPTIGA Authenticate NBT Host Library for C"
  LANGUAGES C CXX)

# Compiler settings
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Build options
option(BUILD_DOCUMENTATION "Build API documentation using doxygen" ON)

# ##############################################################################
# Submodules with actual functionality
# ##############################################################################
# Build system overrides to disable downloads in submodules
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/.cmake/InfineonDownloadOverrides.cmake")
  include("${CMAKE_CURRENT_LIST_DIR}/.cmake/InfineonDownloadOverrides.cmake")
endif()

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/tests")
  include(CTest)
  add_subdirectory(tests)
endif()

# Add subdirectories to the build. Make sure the order of the add_subdirectory
# calls in CMakeLists.txt file in such a way that the dependent library is being
# built first.
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/hsw-error)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/hsw-utils)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/hsw-logger)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/hsw-protocol)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/hsw-apdu)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/hsw-apdu-protocol)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/hsw-timer)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/hsw-i2c)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/hsw-crc)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/hsw-t1prime)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/hsw-apdu-nbt)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/hsw-ndef)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/hsw-ndef-bp)

# ##############################################################################
# Documentation
# ##############################################################################
if(BUILD_DOCUMENTATION)
  find_package(Doxygen REQUIRED OPTIONAL_COMPONENTS dot)
  if(DOXYGEN_DOT_EXECUTABLE)
    set(DOXYGEN_HAVE_DOT
        YES
        CACHE INTERNAL "Flag signalizing if dot tool is available for doxygen")
  else()
    set(DOXYGEN_HAVE_DOT
        NO
        CACHE INTERNAL "Flag signalizing if dot tool is available for doxygen")
  endif()
  find_program(
    DOXYGEN_HHC
    NAMES hhc.exe
    DOC "Windows htmlhelp's hhc.exe to generate compressed HTML files")
  if("${DOXYGEN_HHC}" STREQUAL "DOXYGEN_HHC-NOTFOUND")
    set(DOXYGEN_HAVE_HHC "NO")
  else()
    set(DOXYGEN_HHC_EXECUTABLE "${DOXYGEN_HHC}")
    set(DOXYGEN_HAVE_HHC "YES")
    set(DOXYGEN_CHM_FILE "${PROJECT_NAME}.chm")
  endif()
  set(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/docs")
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/data/Doxyfile.in"
                 "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")
  add_custom_command(
    OUTPUT "${DOXYGEN_OUTPUT_DIR}/html/index.html"
    COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    DEPENDS
      "${CMAKE_CURRENT_SOURCE_DIR}/docs/userguide.md"
      "${CMAKE_CURRENT_SOURCE_DIR}/.cmake/infineon-package-management/README.md"
      "${CMAKE_CURRENT_SOURCE_DIR}/data/Doxyfile.in"
      "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
      hsw-apdu-documentation
      hsw-apdu-nbt-documentation
      hsw-apdu-protocol-documentation
      hsw-ndef-bp-documentation
      hsw-crc-documentation
      hsw-i2c-documentation
      hsw-error-documentation
      hsw-logger-documentation
      hsw-ndef-documentation
      hsw-protocol-documentation
      hsw-t1prime-documentation
      hsw-timer-documentation
      hsw-utils-documentation
    COMMENT "Generating API documentation for '${PROJECT_NAME}' using doxygen")
  add_custom_target(${PROJECT_NAME}-documentation ALL
                    DEPENDS "${DOXYGEN_OUTPUT_DIR}/html/index.html")
endif()
