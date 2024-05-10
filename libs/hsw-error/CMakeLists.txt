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
  hsw-error
  VERSION 0.3.2
  DESCRIPTION
    "C library for creating and parsing error information from status codes"
  LANGUAGES C CXX)

# Compiler settings
set(CMAKE_C_STANDARD 99)

# Build options
option(BUILD_DOCUMENTATION "Build API documentation using doxygen" ON)

# Input files
set(SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/ifx-error.c")
set(HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/infineon/ifx-error.h")

# ##############################################################################
# Library
# ##############################################################################
add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
add_library(Infineon::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(
  ${PROJECT_NAME}
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
         "$<INSTALL_INTERFACE:include>")

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
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
            "${CMAKE_CURRENT_SOURCE_DIR}/data/Doxyfile.in"
            "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" ${PROJECT_NAME}
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
  FILE "${PROJECT_NAME}-targets.cmake"
  NAMESPACE Infineon::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}")
export(
  EXPORT ${PROJECT_NAME}-targets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-targets.cmake"
  NAMESPACE Infineon::)
export(PACKAGE ${PROJECT_NAME})
