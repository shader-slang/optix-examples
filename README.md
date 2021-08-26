# About these Examples

These examples were derived from the 2019 Siggraph course on 
"RTX Accelerated Ray Tracing with OptiX", which can be found here: 
https://github.com/ingowald/optix7course

However, these examples have been modified to instead use Slang as the shading
language (as opposed to the traditional CUDA based workflow with OptiX).

# Building the Code

This code was intentionally written with minimal dependencies, 
requiring only CMake (as a build system), your favorite 
compiler (tested with Visual Studio 2019 under Windows, and GCC under
Linux), the OptiX 7 SDK (including CUDA 10.1 or newer and an NVIDIA driver
compatible with your downloaded OptiX SDK version), and finally a recent build of 
the Slang compiler (we use the command line slangc compiler as a drop-in 
replacement for nvcc).

## Dependencies

- a compiler
    - On Windows, tested with Visual Studio 2019 community edition
    - On Linux, tested with Ubuntu 18 and Ubuntu 19 default gcc installs
- CUDA 10.1
    - Download from developer.nvidia.com
    - on Linux, suggest to put `/usr/local/cuda/bin` into your `PATH`
- latest NVIDIA developer driver that comes with the SDK
    - download from http://developer.nvidia.com/optix and click "Get OptiX"
- OptiX 7 SDK
    - download from http://developer.nvidia.com/optix and click "Get OptiX"
    - on linux, set the environment variable `OptiX_INSTALL_DIR` to wherever you installed the SDK.
    `export OptiX_INSTALL_DIR=<wherever you installed OptiX 7 SDK>`
    - on windows, the installer should automatically put it into the right directory
- Slang
    - download prebuilt releases here: https://github.com/shader-slang/slang/releases

## Building under Linux

- Install required packages

    - on Debian/Ubuntu: `sudo apt install libglfw3-dev cmake-curses-gui`
    - on RedHat/CentOS/Fedora (tested CentOS 7.7): `sudo yum install cmake3 glfw-devel freeglut-devel`

- Clone the code
```
    git clone https://github.com/shader-slang/optix-examples.git
    cd optix7course
```

- create (and enter) a build directory
```
    mkdir build
    cd build
```

- configure with cmake
    - Ubuntu: `cmake ..`
    - CentOS 7: `cmake3 ..`

- and build
```
    make
```

## Building under Windows

- Install Required Packages
	- see above: CUDA 10.1, OptiX 7 SDK, latest driver, and cmake
- download or clone the source repository
- Open `CMake GUI` from your start menu
	- point "source directory" to the downloaded source directory
	- point "build directory" to <source directory>/build (agree to create this directory when prompted)
	- click 'configure', then specify the generator as Visual Studio 2017 or 2019, and the Optional platform as x64. If CUDA, SDK, and compiler are all properly installed this should enable the 'generate' button. If not, make sure all dependencies are properly installed, "clear cache", and re-configure.
    - look through the list of variable names for the 'SLANGC' variable. If slangc.exe is not found, set the variable to a path pointing to slangc.exe
	- click 'generate' (this creates a Visual Studio project and solutions)
	- click 'open project' (this should open the project in Visual Studio)

