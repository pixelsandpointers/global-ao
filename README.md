# Global Ambient Occlusion

This is the submission to the coursework of the Visual Computing practical in Summer 2023 at Technical University of Darmstadt.  


## Global ambient occlusion theory
Global Ambient Occlusion is a method used when rendering 3D objects. 
It is used to increase the realism of an object by shading it independently of light sources in the rendered scene.
To implement this method, the self-obscuration, i.e. the concealment of certain surface areas of an object by its own geometry, is exploited and calculated.
The self-obscuration by concrete light sources of a scene can be calculated by algorithms such as Shadow Mapping, although these usually only affect diffuse and specular components of the exposure.
Global Ambient Occlusion, on the other hand, calculates an ambient term that is intended to simulate shading in ambient light.
This ultimately enables the enhancement of shading on both illuminated and non-illuminated surfaces of the object.


## Dependencies
We have added the following dependencies to the project which are automatically pulled during project build:
- `Assimp`: for different 3d file formats as .obj
- `Glad`: for handling OpenGL setup
- `GLFW`: for window context creation
- `GLM`: for general math utilities /w OpenGL and Vulkan
- `ImGui`: to spawn a UI within the GLFW context to adjust number of samples and a debug toggle
- `STB`: to read and write commonly used image formats

The following dependencies must be available on the host system and will not be automatically fetched by CMake:
- `OpenGL`
- `Vulkan`
- `jinja` (for building)

## Branches
We implemented different versions of global ambient occlusion which are all to be found on a separate branch:
- main (OpenGL implementation)
- cpu (CPU implementation)
- compute (Compute Shader implementation)
- vk (Vulkan implementation)


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
- Shader files are within the `shader` subdirectory within the `global-ao` source dir.
- Unit tests are also placed in the `global-ao` subdirectory and have `.test.cxx` as extension.
- Only Integration tests are placed in the `tests` subdirectory
- We provide some models within the `global-ao` directory which can be found in the `resources` subdirectory.

## Include headers
Include headers using `<>` and include the project name as a directory prefix.   
For example, `#include <global-ao/GAOGenerator.hxx>`

## Tests
Tests are written using the Catch2 framework.   
To add a test use the provided `global_ao_add_test` function:

```cmake
global_ao_add_test(my-test my-test.cxx [LINK_LIBRARIES lib1 [lib2 ...]]])
```
