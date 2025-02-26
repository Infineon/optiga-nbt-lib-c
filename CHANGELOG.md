<!--
SPDX-FileCopyrightText: Copyright (c) 2025 Infineon Technologies AG
SPDX-License-Identifier: MIT
-->

# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic
Versioning](https://semver.org/spec/v2.0.0.html).

## [1.1.2] - 2024-12-11

### Added

- **Enhanced Communication Robustness**: Introduced a limit on retries in the
  `hsw-t1prime` library during out-of-synchronization communication between the
  device and the host. This change aims to prevent endless retry loops and
  improve overall system stability when errors occur.

## [1.1.1] - 2024-05-10

### Added

- Initial release
