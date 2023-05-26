# global-ao

Global ambient occlusion implementation.   

## CMake

This project uses CMake.   
To build the project, run the following commands:   

```bash
mkdir build
cd build
cmake ..
cmake --build .
```
   
### Adding targets
To avoid conflicts with dependencies or when this project is used as a dependency 
new targets should be added using the following naming convention:

```cmake
add_executable(${CMAKE_PROJECT_NAME}-my-executable my-executable.cxx)
add_library(${CMAKE_PROJECT_NAME}-my-library my-library.cxx)
```

## Project structure
Project structure should follow the
[Canonical Project Structure guidelines](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1204r0.html).   
In particular, the following rules should be followed:
- Source files should be placed in the `global-ao` subdirectory
- Header files are not placed in a separate directory like `include` or `inc` but are 
placed in the `global-ao` subdirectory.
- Unit tests are also placed in the `global-ao` subdirectory and have `.test.cxx` as extension.
- Only Integration tests are placed in the `tests` subdirectory

## Include headers
Include headers using `<>` and include the project name as a directory prefix.   
For example, `#include <global-ao/ambient_occlusion.hpp>`.   

## Tests
Tests are written using the Catch2 framework.   
To add a test use the provided `global_ao_add_test` function:

```cmake
global_ao_add_test(my-test my-test.cxx [LINK_LIBRARIES lib1 [lib2 ...]]])
```

## Versioning
Use semantic versioning (https://semver.org/).
