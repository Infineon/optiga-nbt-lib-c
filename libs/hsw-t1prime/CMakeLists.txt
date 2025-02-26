# SPDX-FileCopyrightText: 2024 Infineon Technologies AG
#
# SPDX-License-Identifier: MIT

# ##############################################################################
# Project configuration and settings
# ##############################################################################
cmake_minimum_required(VERSION 3.14)

# Disable in-source builds
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

# Main project configuration
project(
  hsw-t1prime
  VERSION 0.2.2
  DESCRIPTION "Global Platform T=1' Protocol"
  LANGUAGES C CXX)

# Compiler settings
set(CMAKE_C_STANDARD 99)

# Build options
option(BUILD_DOCUMENTATION "Build API documentation using doxygen" ON)
option(IFX_T1PRIME_USE_I2C
       "Build T=1' for I2C interface (For SPI interface, set it OFF)" ON)

# Enables T1PRIME library logs
option(IFX_T1PRIME_LOG_ENABLE "Enables the T1PRIME library logs" ON)
if(IFX_T1PRIME_LOG_ENABLE)
  add_definitions(-DIFX_T1PRIME_LOG_ENABLE=1)
else()
  message("T1PRIME library logs are disabled")
endif()

# Input files
set(SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ifx-t1prime.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ifx-t1prime.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/t1prime/ifx-t1prime-datastructures.h")
set(HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/infineon/ifx-t1prime.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/infineon/ifx-t1prime-lib.h")

# ##############################################################################
# Dependencies
# ##############################################################################
# Actually load dependencies
include("${CMAKE_CURRENT_SOURCE_DIR}/.cmake/CPM.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/.cmake/utils.cmake")

add_package_if_not_exists(hsw-error "../hsw-error.git" "6712781fc3fff696e993cfc53d2ab8b656056cfd")
add_package_if_not_exists(hsw-crc "../hsw-crc.git" "c44c8d7ac4d0929041881c784cd18048bea21b82")
add_package_if_not_exists(hsw-logger "../hsw-logger.git" "304d0f74af02e4e91e8ad3d4e977fdc2616e81ad")
add_package_if_not_exists(hsw-timer "../hsw-timer.git" "efdc79f81e639836b2ac3f797ba78abd403af2fb")
add_package_if_not_exists(hsw-protocol "../hsw-protocol.git" "73e49383d203d7cc6f353fa35a3f856e1cbf78bb")

if(${IFX_T1PRIME_USE_I2C})
add_package_if_not_exists(hsw-i2c "../hsw-i2c.git" "bb244c9c6292bff14b9f1b004bb72b5a6a08637e")
else()
  message("Not supported, only I2C supported.")
endif()

# ##############################################################################
# Library
# ##############################################################################
add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
add_library(Infineon::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC Infineon::hsw-error Infineon::hsw-crc Infineon::hsw-timer
         Infineon::hsw-protocol Infineon::hsw-logger)
if(${IFX_T1PRIME_USE_I2C})
  target_compile_definitions(${PROJECT_NAME} PRIVATE IFX_T1PRIME_INTERFACE_I2C)
  target_link_libraries(${PROJECT_NAME} PUBLIC Infineon::hsw-i2c)
else()
  target_link_libraries(${PROJECT_NAME} PUBLIC Infineon::hsw-spi)
endif()
target_include_directories(
  ${PROJECT_NAME}
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
         "$<INSTALL_INTERFACE:include>"
  PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>")

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
      "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
      "${CMAKE_CURRENT_SOURCE_DIR}/data/Doxyfile.in"
      "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
      ${PROJECT_NAME}
    COMMENT "Generating API documentation for '${PROJECT_NAME}' using doxygen")
  add_custom_target(${PROJECT_NAME}-documentation ALL
                    DEPENDS "${DOXYGEN_OUTPUT_DIR}/html/index.html")
endif()

# ##############################################################################
# Sub-Directories
# ##############################################################################
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests")
  if(IFX_ENABLE_TEST)
    include(CTest)
    add_subdirectory(tests)
  endif()
endif()

# ##############################################################################
# Installation
# ##############################################################################
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# Config file to be findable by cmake
configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/data/${PROJECT_NAME}-config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}")

# Version file for semantic version checks
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

# Main library
install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(DIRECTORY include/infineon DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

# CMake files for find_package()
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
              "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}")

# CMake targets for stuff like target_link_libraries()
install(
  EXPORT ${PROJECT_NAME}-targets
  FILE ${PROJECT_NAME}-targets.cmake
  NAMESPACE Infineon::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}")
export(
  EXPORT ${PROJECT_NAME}-targets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-targets.cmake"
  NAMESPACE Infineon::)
export(PACKAGE ${PROJECT_NAME})
