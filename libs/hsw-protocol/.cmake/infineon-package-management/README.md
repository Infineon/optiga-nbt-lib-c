# Infineon Host Software Package Management Utility

This project offers a reusable [cmake script](InfineonPackageManagement.cmake) script for managing dependencies to Infineon host software libraries.

It is built around [CPM](https://github.com/cpm-cmake/CPM.cmake) and adds the possibility to have overrides to non-public repositories or local files.

## Usage

To use the script simply put it next to your code and use cmake's `include()` to have the functionality available.

The library offers 2 functions:

  * `require_infineon_package()` ensures that the given Infineon host software package is available for use.
  * `add_infineon_override_url()` overrides the default download URL for the given package to your local file system or private repository.

```cmake
# Ensure dependencies are available
include(InfineonPackageManagement.cmake)
require_infineon_package(NAME hsw-logger)

# Use Infineon:: targets as usual
add_executable(dummy src/dummy.c)
target_link_libraries(dummy Infineon::logger)
```

### Local Override Files

A common use-case is to add a local override file for e.g. CI systems to use local or internal versions.
You can use cmake's `if (EXIST ...)` to check if such a file is available and include it accordingly. In the override file simply include `InfineonDownloadOverrides.cmake` and call
`add_infineon_override_url()` with the desired values.

*CMakeLists.txt*:
```cmake
# Load local overrides e.g. for CI system
if(EXISTS InfineonDownloadOverrides.cmake)
  include(InfineonDownloadOverrides.cmake)
endif()

# Use rest as usual
include(InfineonPackageManagement.cmake)
require_infineon_package(NAME hsw-logger)
```

*InfineonDownloadOverrides.cmake*:
```cmake
include(InfineonPackageManagement.cmake)
add_infineon_override_url(NAME hsw-error URL /my/local/checkout LOCAL)
add_infineon_override_url(NAME hsw-logger URL https://my-private-git-server.com/hsw-logger.git)
```

### Prefer find_package()

If you prefer to use a local installation of a dependency, you can set the `CPM_USE_LOCAL_PACKAGES` cmake configuration parameter to `ON`. The script will automatically try to use local packages first by using cmake's `find_package()` mechanism.
