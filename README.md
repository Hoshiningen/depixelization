# Depixelization

This is an implementation of *Depixelizing Pixel Art* by Johannes Kopf and Dani Lischinski, and is not their real-time solution that's described in their latest paper. What their algorithm does is take some pixel-art and converts it into resolution-independent vector art.

You can find the paper my implementation is based on [here](https://johanneskopf.de/publications/pixelart/).

## Build Instructions

The project is configured in CMake, and thus requires CMake to generate the project files. You can download it from [here](https://cmake.org/download/). A version of CMake >= **3.11** is required.

To generate a project, execute the following from with in the root directory of the repository:

```bash
mkdir build && cd build
cmake ..
```

Depending on your operating system and installed IDE's, a build system will be generated in the build folder. If a specific build system is desired:

```bash
mkdir build && cd build
cmake -G "<Generator>"
```

Where you would swap the desired generator with `<Generator>`, retaining the double quotes. You can view a list of possible generators on your system with `cmake -H`.

To build and link the project, a modern C++ compiler is required, as the project is written in C++17. If cmake can find your compiler's executable, then the following will build the project (from within the build directory):

```bash
cmake --build .
```

An alternative, if you can run Visual Studio, is to generate a solution and build from within that.

### Building Tests

By default, CMake will generate project files for unit tests. This can be changed by setting the option `DEPIXELIZATION_BUILD_TESTS` to `OFF`.

```bash
cmake -DDEPIXELIZATION_BUILD_TESTS=OFF ..
```

The tests are written with google test and can be run using the following command from within the build directory:

```bash
ctest -C <configuration>
```

Where `<configuration>` is whatever configuration of the tests you want to run (Debug, Release, etc). The tests can also be run from within Visual Studio through `Test->Run->Run All Tests`.

### Boost

Boost 1.70 is a dependency of the project, and if on Windows, one may need to point CMake in the right direction to find the location where it was installed.

Set the CMake variable `BOOST_ROOT` to the the root directory of your boost installation:

```bash
cmake -DBOOST_ROOT=<boost root directory> ..
```

This will add `BOOST_ROOT` to the CMakeCache.txt. For successive project generations, CMake will pull from the cached variable and won't require you to explicitly set it from the command line.

## Usage

Depixelization is intended to be a command line interface (CLI) for converting pixel art into vector graphics, but is still a work in progress. It's not in a usable state at this time, but there are some parts that are complete. Feel free to explore the repository and play around with the different classes. You can also run the unit tests.