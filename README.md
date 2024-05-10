<!--
SPDX-FileCopyrightText: Copyright (c) 2024 Infineon Technologies AG
SPDX-License-Identifier: MIT
-->

# OPTIGA&trade; Authenticate NBT Host Library for C

[![Contributor Covenant](https://img.shields.io/badge/Contributor%20Covenant-2.1-4baaaa.svg)](CODE_OF_CONDUCT.md)
[![REUSE Compliance Check](https://github.com/Infineon/optiga-nbt-lib-c/actions/workflows/linting-test.yml/badge.svg?branch=main)](https://github.com/Infineon/optiga-nbt-lib-c/actions/workflows/linting-test.yml)
[![CMake Build](https://github.com/Infineon/optiga-nbt-lib-c/actions/workflows/build-test.yml/badge.svg?branch=main)](https://github.com/Infineon/optiga-nbt-lib-c/actions/workflows/cmake-single-platform.yml)

This is the OPTIGA&trade; Authenticate NBT Host Library for C, which supports the utilization of the [OPTIGA&trade; Authenticate NBT](https://www.infineon.com/OPTIGA-Authenticate-NBT) in embedded applications.

## Overview

The host library offers an extensive API to interact with the OPTIGA&trade; Authenticate NBT and to utilize its full functionality. The library is mainly intended to be used in embedded C/C++ applications interfacing to the OPTIGA&trade; Authenticate NBT via I2C.

Refer to the [OPTIGA&trade; Authenticate NBT - GitHub overview](https://github.com/Infineon/optiga-nbt) repository for an overview of the available host software for the OPTIGA&trade; Authenticate NBT.

### Features

* NFC-to-I2C bridge commands for interaction with the OPTIGA&trade; Authenticate NBT
* Sends command APDUs (C-APDU) and receives response APDUs (R-APDU) from the OPTIGA&trade; Authenticate NBT
* Configuration of the OPTIGA&trade; Authenticate NBT via its configurator application
* Personalization and operational commands to support simplified interaction with the OPTIGA&trade; Authenticate NBT
* NDEF host library that enables the host application to build, encode and decode NDEF records and messages
* NDEF host library that supports "NFC Forum well known type" and "NFC Forum external type"
* NDEF host library that provides brand-protection supported NDEF records
* Supports GlobalPlatform T=1' protocol for I2C communication between host MCU and OPTIGA&trade; Authenticate NBT

### User guide

This host library's [user guide](./docs/userguide.md) is available in the [`docs`](./docs) folder.

## Getting started

This section contains information on how to setup the OPTIGA&trade; Authenticate NBT Host Library for C and integrate it into custom embedded applications.

### Setup and requirements

Minimum required versions:

- Doxygen 1.9.8
- CMake 3.14

### Project layout

```text
├── .cmake/          # Includes sources for dependency management
├── .github/         # GitHub-related resources (e.g., workflows)
├── .reuse/          # Configuration for the this project's license information 
├── LICENSES/        # Licenses used in this project
├── data/            # Includes Doxygen, Cppcheck configuration files
├── docs/            # Includes documentation sources and images
└── libs/            # Includes dependent modules of the host library
```

### Build

To build the host library as a static library, configure `cmake` and use `cmake --build` to perform compilation.

Execute the following commands from the root folder of the host library, where `CMakeLists.txt` exists:

```sh
cmake -S . -B build -DBUILD_DOCUMENTATION=OFF
cmake --build build
```

To build the documentation use the following commands

```sh
cmake -S . -B build -DBUILD_DOCUMENTATION=ON 
cmake --build build
```

This creates a subfolder docs in the build directory which contains the generated documentation as static HTML pages.

### Usage

See this host library's [user guide](./docs/userguide.md) and the included sub-libraries' `README.md` files for more information.

## Additional information

### Related resources

- [OPTIGA&trade; Authenticate NBT - product page](https://www.infineon.com/OPTIGA-Authenticate-NBT)
- [OPTIGA&trade; Authenticate NBT - GitHub overview](https://github.com/Infineon/optiga-nbt)

### Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for detailed contribution instructions and refer to our [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md).

### Contact

In case of questions regarding this repository and its contents, refer to [MAINTAINERS.md](MAINTAINERS.md) for the contact details of this project's maintainers.

### Licensing

Please see our [LICENSE](LICENSE) for copyright and license information.

This project follows the [REUSE](https://reuse.software/) approach, so copyright and licensing information is available for every file (including third party components) either in the file header, an individual *.license file or the .reuse/dep5 file. All licenses can be found in the [LICENSES](LICENSES) folder.
