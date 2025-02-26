# SPDX-FileCopyrightText: 2024 Infineon Technologies AG
#
# SPDX-License-Identifier: MIT

# Add package only if it does not exist.
# Necessary for main project, which includes all the sub projects as submodules instead of cpm package
function(add_package_if_not_exists name path tag)
  if(NOT TARGET ${name})
    cpmaddpackage(
      NAME ${name} GIT_REPOSITORY
      ${path} GIT_TAG
      ${tag})
  endif()
endfunction()
