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
  hsw-apdu-nbt
  VERSION 1.1.1
  DESCRIPTION "NBT APDU C-library"
  LANGUAGES C CXX)

# Compiler settings
set(CMAKE_C_STANDARD 99)

# Build options
option(BUILD_DOCUMENTATION "Build API documentation using doxygen" ON)

# Enables APDU NBT library logs
option(NBT_APDU_LOG_ENABLE "Enables the APDU NBT library logs" ON)
if(NBT_APDU_LOG_ENABLE)
  add_definitions(-DNBT_APDU_LOG_ENABLE=1)
else()
  message("APDU NBT logs are disabled")
endif()

# Input files
set(SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/nbt-apdu.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/nbt-build-apdu.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/nbt-build-apdu-config.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/nbt-build-apdu-perso.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/nbt-cmd.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/nbt-cmd-config.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/nbt-cmd-perso.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/nbt-errors.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/nbt-parse-response.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/include/nbt-build-apdu.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/include/nbt-build-apdu-config.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/include/nbt-build-apdu-perso.h")

set(HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/infineon/nbt-apdu.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/infineon/nbt-apdu-lib.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/infineon/nbt-cmd.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/infineon/nbt-cmd-config.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/infineon/nbt-cmd-perso.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/infineon/nbt-errors.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/infineon/nbt-parse-response.h")

# ##############################################################################
# Dependencies
# ##############################################################################
# Check local overrides for CI system.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.cmake/InfineonDownloadOverrides.cmake")
  include("${CMAKE_CURRENT_SOURCE_DIR}/.cmake/InfineonDownloadOverrides.cmake")
endif()

# Actually load dependencies
include("${CMAKE_CURRENT_SOURCE_DIR}/.cmake/infineon-package-management/\
InfineonPackageManagement.cmake")
# Note: Order of inclusion  of deps matters while using local packages. Hence
# add it in order that no packages are overrided with url from cmake/_deps
# folder
require_infineon_package(NAME hsw-error)
require_infineon_package(NAME hsw-utils)
require_infineon_package(NAME hsw-logger)
require_infineon_package(NAME hsw-protocol)
require_infineon_package(NAME hsw-apdu)
require_infineon_package(NAME hsw-apdu-protocol)

# ##############################################################################
# Library
# ##############################################################################
add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
add_library(Infineon::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC Infineon::hsw-error Infineon::hsw-apdu Infineon::hsw-logger
         Infineon::hsw-protocol Infineon::hsw-apdu-protocol Infineon::hsw-utils)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
         "$<INSTALL_INTERFACE:include>"
  PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include>")

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
      "${CMAKE_CURRENT_SOURCE_DIR}/.cmake/infineon-package-management/README.md"
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
