<!--
SPDX-FileCopyrightText: Copyright (c) 2024-2026 Infineon Technologies AG
SPDX-License-Identifier: CC-BY-4.0
-->

# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.2.0] - 2026-03-12

### Changed

- Generic repo restructuring and cleanup.

### Fixed

- `hsw-apdu-nbt`: fix memory validation (change IFX_VALIDATE_NULL_PTR_ from BLOB to MEMORY in fap_encode).
- `hsw-apdu-nbt`: fix accessing pointer after free in `nbt_destroy` function.
- `hsw-apdu`: added NULL pointer checks.

## [1.1.2] - 2024-12-11

### Added

- **Enhanced Communication Robustness**: Introduced a limit on retries in the `hsw-t1prime` library during out-of-synchronization communication between the device and the host. This change aims to prevent endless retry loops and improve overall system stability when errors occur.

## [1.1.1] - 2024-05-10

### Added

- Initial release
