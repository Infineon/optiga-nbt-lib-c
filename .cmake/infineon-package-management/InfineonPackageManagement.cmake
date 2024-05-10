# SPDX-FileCopyrightText: 2024 Infineon Technologies AG
#
# SPDX-License-Identifier: MIT

# ##############################################################################
# Wrapper around CPM (https://github.com/TheLartians/CPM.cmake) for searching
# and downloading infineon libraries and cmake packages.
#
# Can be used with different repository structures to have consistent download
# behaviour by providing override URLS and simply downloading required packages
# on the fly.
# ##############################################################################

# CPM as base manager for downloading dependencies
set(CPM "${CMAKE_CURRENT_LIST_DIR}/CPM.cmake")
if(NOT (EXISTS "${CPM}"))
  message(STATUS "Downloading CPM.cmake")
  file(
    DOWNLOAD
    https://github.com/TheLartians/CPM.cmake/releases/download/v0.34.0/CPM.cmake
    ${CPM})
endif()
include(${CPM})

# Query base URL for downloading remote dependencies
function(_get_git_remote_url)
  # Ignore if already set
  if(DEFINED INFINEON_GIT_PACKAGES_BASE_URL)
    return()
  endif()

  # Try to do lookup based on current git repository
  find_package(Git)
  if(GIT_FOUND)
    execute_process(COMMAND "${GIT_EXECUTABLE}" config --get remote.origin.url
                    OUTPUT_VARIABLE GIT_REMOTE_URL)
    if(GIT_REMOTE_URL)
      string(REGEX REPLACE "\n" "" GIT_REMOTE_URL "${GIT_REMOTE_URL}")
      set(INFINEON_GIT_PACKAGES_BASE_URL
          "${GIT_REMOTE_URL}/.."
          CACHE INTERNAL "Base URL for all Infineon git repositories")
    endif()
  endif()
endfunction()
_get_git_remote_url()

# ##############################################################################
# Makes sure that target with given name is available by checking local targets
# or otherwise downloading from git repository.
#
# @param NAME=<TARGET> Name of Infineon target to be searched.
# ##############################################################################
function(require_infineon_package)
  # Parse and validate parameters
  cmake_parse_arguments(REQUIRE_INFINEON_PACKAGE "" "NAME" "" ${ARGN})
  if(NOT DEFINED REQUIRE_INFINEON_PACKAGE_NAME)
    message(FATAL_ERROR "Missing required parameter 'NAME'")
  endif()

  # Check if override URL has been defined
  if(DEFINED _${REQUIRE_INFINEON_PACKAGE_NAME}_OVERRIDE_URL)
    set(DOWNLOAD_URL "${_${REQUIRE_INFINEON_PACKAGE_NAME}_OVERRIDE_URL}")

    # Check if local checkout has been defined
  elseif(DEFINED CPM_${REQUIRE_INFINEON_PACKAGE_NAME}_SOURCE)
    # Ignore as already handled by CPM

    # Otherwise use URL based on root git repository
  else()
    if((NOT DEFINED INFINEON_GIT_PACKAGES_BASE_URL)
       OR ("${INFINEON_GIT_PACKAGE_BASE_URL}" STREQUAL ""))
      message(FATAL_ERROR "Could not determine base URL for git repositories")
    endif()
    set(DOWNLOAD_URL
        "${INFINEON_GIT_PACKAGES_BASE_URL}/${REQUIRE_INFINEON_PACKAGE_NAME}")
  endif()

  # Check if target already available
  if(TARGET Infineon::${REQUIRE_INFINEON_PACKAGE_NAME})
    message(
      STATUS
        "Using previously added version of '${REQUIRE_INFINEON_PACKAGE_NAME}'")
    return()
  endif()

  # Otherwise download via CPM
  if(DEFINED CPM_${REQUIRE_INFINEON_PACKAGE_NAME}_SOURCE)
    message(
      STATUS "Using local version of '${REQUIRE_INFINEON_PACKAGE_NAME}' from \
'${CPM_${REQUIRE_INFINEON_PACKAGE_NAME}_SOURCE}'")
  else()
    message(
      STATUS
        "Downloading '${REQUIRE_INFINEON_PACKAGE_NAME}' from '${DOWNLOAD_URL}'")
  endif()
  CPMAddPackage(
    NAME
    ${REQUIRE_INFINEON_PACKAGE_NAME}
    GIT_TAG
    master
    GIT_REPOSITORY
    "${DOWNLOAD_URL}"
    OPTIONS
    "BUILD_TESTING OFF"
    "BUILD_DOCUMENTATION OFF")
endfunction()

# ##############################################################################
# Adds an override URL for the given package that will be used by
# require_infineon_package() to download said package.
#
# This can be used if submodules are on a different repository structure than
# the default to override the default behaviour without changing the actual
# CMakeLists.txt files.
#
# @param NAME=<TARGET> Name of Infineon target to override default URL for.
# @param URL=<DOWNLOAD_URL> Override URL for target. @param LOCAL Optional flag
# if URL points to a local checkout (ignores all further override URLs).
# ##############################################################################
function(add_infineon_override_url)
  # Parse parameters
  cmake_parse_arguments(ADD_INFINEON_OVERRIDE_URL "LOCAL" "NAME;URL" "" ${ARGN})

  # Validate parameters
  if(NOT DEFINED ADD_INFINEON_OVERRIDE_URL_NAME)
    message(FATAL_ERROR "Missing required parameter 'NAME'")
  endif()
  if(NOT DEFINED ADD_INFINEON_OVERRIDE_URL_URL)
    message(FATAL_ERROR "Missing required parameter 'URL'")
  endif()

  # Check if local flag is set
  if(${ADD_INFINEON_OVERRIDE_URL_LOCAL})
    # Double check that different local directories cannot be added
    if(DEFINED CPM_${ADD_INFINEON_OVERRIDE_URL_NAME}_SOURCE)
      get_filename_component(
        EXISTING_SOURCE "${CPM_${ADD_INFINEON_OVERRIDE_URL_NAME}_SOURCE}"
        ABSOLUTE)
      get_filename_component(REQUESTED_SOURCE
                             "${ADD_INFINEON_OVERRIDE_URL_URL}" ABSOLUTE)
      if(EXISTING_SOURCE STREQUAL REQUESTED_SOURCE)
        return()
      else()
        message(
          FATAL_ERROR
            "Local directory override for '${ADD_INFINEON_OVERRIDE_URL_NAME}' \
cannot be set to '${REQUESTED_SOURCE}' as it is already set to \
'${EXISTING_SOURCE}'")
      endif()
    endif()

    # Set corresponding CPM variable
    set("CPM_${ADD_INFINEON_OVERRIDE_URL_NAME}_SOURCE"
        "${ADD_INFINEON_OVERRIDE_URL_URL}"
        CACHE INTERNAL "Local checkout of '${ADD_INFINEON_OVERRIDE_URL_NAME}'")
    return()
  endif()

  # Check if local directory has been added before
  if(DEFINED CPM_${ADD_INFINEON_OVERRIDE_URL_NAME}_SOURCE)
    message("Override URL for package '${ADD_INFINEON_OVERRIDE_URL_NAME}' has \
previously been set to local directory, ignoring URL \
'${ADD_INFINEON_OVERRIDE_URL_URL}'")
    return()
  endif()

  # Check if override already defined
  if(DEFINED _${ADD_INFINEON_OVERRIDE_URL_NAME}_OVERRIDE_URL)
    if(NOT ("${_${ADD_INFINEON_OVERRIDE_URL_NAME}_OVERRIDE_URL}" STREQUAL
            "${ADD_INFINEON_OVERRIDE_URL_URL}"))
      message(
        FATAL_ERROR
          "Override for package '${ADD_INFINEON_OVERRIDE_URL_NAME}' with URL \
'${ADD_INFINEON_OVERRIDE_URL_URL}' already set to other URL \
'${_${ADD_INFINEON_OVERRIDE_URL_NAME}_OVERRIDE_URL}'")
    endif()
  else()
    set("_${ADD_INFINEON_OVERRIDE_URL_NAME}_OVERRIDE_URL"
        "${ADD_INFINEON_OVERRIDE_URL_URL}"
        CACHE INTERNAL
              "URL download override for '${ADD_INFINEON_OVERRIDE_URL_NAME}'")
  endif()
endfunction()
