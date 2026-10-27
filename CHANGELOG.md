<!--
SPDX-FileCopyrightText: Copyright (c) 2024-2026 Infineon Technologies AG
SPDX-License-Identifier: CC-BY-4.0
-->

# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.2.1] - 2026-10-27

### Added

- Added the user guide to the online documentation of the OPTIGA™ Authenticate NBT host software.
- `hsw-apdu-nbt`: Added the transfer of personalization data larger than one APDU in several chunks (NDEF file and proprietary files).

### Changed

- Updated the dependent libraries to their latest versions.
- Updated the README to link to the user guide in the online documentation.

### Fixed

- `hsw-ndef`: Fixed potential use after free issues.
- `hsw-apdu-nbt`: Fixed memory handling when destroying the command set and when building and receiving APDUs, and rejected empty personalization data.

## [1.2.0] - 2026-03-12

### Changed

- Generic repo restructuring and cleanup.

### Fixed

- `hsw-apdu-nbt`: Fix memory validation (change IFX_VALIDATE_NULL_PTR_ from BLOB to MEMORY in fap_encode).
- `hsw-apdu-nbt`: Fix accessing pointer after free in `nbt_destroy` function.
- `hsw-apdu`: Added NULL pointer checks.

## [1.1.2] - 2024-12-11

### Added

- **Enhanced Communication Robustness**: Introduced a limit on retries in the `hsw-t1prime` library during out-of-synchronization communication between the device and the host. This change aims to prevent endless retry loops and improve overall system stability when errors occur.

## [1.1.1] - 2024-05-10

### Added

- Initial release
