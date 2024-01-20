###################################################################################################################
##                                                                                                              ###
##                                              Project Settings                                                ###
##                                                User Options                                                  ###
##                                             Export no symbols                                                ###
##                                                                                                              ###
###################################################################################################################

cmake_minimum_required(VERSION 3.21)

project(example)
set(CMAKE_CXX_STANDARD 11)

option(BUILD_SHARED_LIBS "Build library as shared" ON)

find_package(cpptemplate REQUIRED)

# Add a new library with the project name (cpptemplate) as its name
# Also create an alias so users can link to it with cpptemplate::cpptemplate
add_executable(greeter_user GreeterUser.cpp)

target_link_libraries(greeter_user PRIVATE cpptemplate::cpptemplate)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
