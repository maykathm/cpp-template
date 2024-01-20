# cpp-template: Starting template for a C++ CMake project

This template serves as a starting point for C++/Cmake projects and contains the following:

- Boost unit tests
- include directory with headers for publicly facing apis
- both public and private dependencies
- dependencies copy CMake script to copy dependencies during the install step
- Dockerfile for both testing the install in different environments

## Template Overview

The purpose of the template is to serve as a starting point for creating a C++/CMake application. It uses MPI and Blas for demonstrative purposes. Also for demonstrative purposes, this project offers the possibility of copying private dependencies to a subdirectory of the install folder. In general, it is probably not advisable to bring a Blas dependency with the application, but rather better to use a version optimized for the target machine. Other private dependencies, however, might be better to package with the application.

### `cmake` Folder

Contains cmake scripts for the application. To ensure the application can see the `copy_dependencies.cmake`, I included the line in the main `CMakeLists.txt`:
```cmake
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
```

The `copy_dependencies.cmake` is a script to help with copying dependencies during the install phase.

The `cpptemplate_*_Configure.cmake.in` are files used during the install phase and become part of the installed application to correctly configure it for use in other applications.

### `environment` Folder

Contains files for building the application environment

### `example` Folder

Contains an example application that uses this installed template. It's purpose is both to demonstrate how the application is used as well as a way to test in a clean testing environment that the installed application works correctly. One can see its use in the `cpptemplate_test` stage in the Dockerfile.

### `include` Folder

Contains publically facing headers for the application. The include folder gets copied as-is during install.

### `src` Folder

Contains source files and private headers.

### `test` Folder

Contains all test files for the application. Note that since the library does not export its symbols by default, the unit testing requires a different approach from directly linking to the library and testing it.

## Developing using VS Code dev containers

### Create the development environment image and container
```bash
docker build -f environment/Dockerfile -t cpptemplate_env --target build_env .
```
When creating the container, I like to mount the directory with my source code into the container to simplify development.
```bash
docker create --name cpptemplate_env -v <path to cpp-template directory>:/mnt/cpptemplate -it cpptemplate_env
```

### Develop

To work in dev containers:
- start the previously built container `docker start cpptemplate_env`
- install the VS Code dev containers extension if not installed
- click on the blue box in the lower-left corner of VS Code and select the `cpptemplate_env` container.
- open up the folder at `/mnt/cpptemplate`

One can either use VS Code extensions to configure and run the project or use the command line. Since the container has `cmake-curses-gui`, one can use that UI for configuring by going into the build directory and executing the command:
```bash
root@da7a384n8c97:/mnt/cpptemplate/build# ccmake ../
```
Then one should see a nice UI something like the following:
```bash
BUILD_SHARED_LIBS               *ON
 Boost_INCLUDE_DIR               */usr/include
 Boost_UNIT_TEST_FRAMEWORK_LIBR  */usr/lib/x86_64-linux-gnu/libboost_unit_test_framework.so.1.74.0
 CMAKE_BUILD_TYPE                *
 CMAKE_INSTALL_CMAKEDIR          *lib/cmake/cpptemplate
 CMAKE_INSTALL_PREFIX            */usr/local
 CMAKE_UNITY_BUILD               *OFF
 copy_dependencies_during_insta  *ON



BUILD_SHARED_LIBS: Build library as shared                                                                                                                                                    
Keys: [enter] Edit an entry [d] Delete an entry                                                                                                                           CMake Version 3.22.1
      [l] Show log output   [c] Configure
      [h] Help              [q] Quit without generating
      [t] Toggle advanced mode (currently off)
```

Once configured, a `make -j8` inside the build folder will compile.

## Testing

Besides running the Boost unit tests in the `build_env`:
```bash
root@da7a384n8c97:/mnt/cpptemplate/build# ./cpptemplate_test
Running 2 test cases...

*** No errors detected
root@da7a384n8c97:/mnt/cpptemplate/build#
```

one can make use of the Dockerfile to test the project in different environments. The Dockerfile is set up that if you choose to compile with copying the private dependencies, then the example-running test environment will not have the private dependencies installed. If, instead, one chooses to not copy any dependencies, the example-test environment will have the minimal amount of required packages installed.

### Run examples in test environment without copying private dependencies

```bash
docker build  -f environment/Dockerfile -t tmp --build-arg=copy_dependencies_during_install=off .
```

### Run examples in test environment with copying private dependencies

```bash
docker build  -f environment/Dockerfile -t tmp --build-arg=copy_dependencies_during_install=on .
```
Since the default is to copy dependencies, one can also leave the build argument off of the command.

```bash
docker build  -f environment/Dockerfile -t tmp .
```
