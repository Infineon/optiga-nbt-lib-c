# SPDX-FileCopyrightText: 2024 Infineon Technologies AG
#
# SPDX-License-Identifier: MIT

# Local download URL overrides for CI system
include("${CMAKE_CURRENT_LIST_DIR}/infineon-package-management/\
InfineonPackageManagement.cmake")

add_infineon_override_url(NAME hsw-error URL
                          "${CMAKE_CURRENT_SOURCE_DIR}/libs/hsw-error" LOCAL)
add_infineon_override_url(NAME hsw-apdu URL
                          "${CMAKE_CURRENT_SOURCE_DIR}/libs/hsw-apdu" LOCAL)
add_infineon_override_url(NAME hsw-logger URL
                          "${CMAKE_CURRENT_SOURCE_DIR}/libs/hsw-logger" LOCAL)
add_infineon_override_url(
  NAME hsw-apdu-protocol URL
  "${CMAKE_CURRENT_SOURCE_DIR}/libs/hsw-apdu-protocol" LOCAL)
add_infineon_override_url(NAME hsw-t1prime URL
                          "${CMAKE_CURRENT_SOURCE_DIR}/libs/hsw-t1prime" LOCAL)
add_infineon_override_url(NAME hsw-timer URL
                          "${CMAKE_CURRENT_SOURCE_DIR}/libs/hsw-timer" LOCAL)
add_infineon_override_url(NAME hsw-utils URL
                          "${CMAKE_CURRENT_SOURCE_DIR}/libs/hsw-utils" LOCAL)
add_infineon_override_url(NAME hsw-protocol URL
                          "${CMAKE_CURRENT_SOURCE_DIR}/libs/hsw-protocol" LOCAL)
add_infineon_override_url(NAME hsw-ndef URL
                          "${CMAKE_CURRENT_SOURCE_DIR}/libs/hsw-ndef" LOCAL)
add_infineon_override_url(NAME hsw-ndef-bp URL
                          "${CMAKE_CURRENT_SOURCE_DIR}/libs/hsw-ndef-bp" LOCAL)
add_infineon_override_url(NAME hsw-crc URL
                          "${CMAKE_CURRENT_SOURCE_DIR}/libs/hsw-crc" LOCAL)
add_infineon_override_url(NAME hsw-i2c URL
                          "${CMAKE_CURRENT_SOURCE_DIR}/libs/hsw-i2c" LOCAL)
