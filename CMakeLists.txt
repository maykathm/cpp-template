###################################################################################################################
##                                                                                                              ###
##                                            C++/CMake template                                                ###
##                                                                                                              ###
###################################################################################################################

cmake_minimum_required(VERSION 3.16)

project(cpptemplate)
set(CMAKE_CXX_STANDARD 11)

# These are options the uesr can set from cmake
option(BUILD_SHARED_LIBS "Build library as shared" ON)
option(CMAKE_UNITY_BUILD OFF)
option(copy_dependencies_during_install "During install, copy privately linked dependencies to install folder" ON)

# Set a default build type if none was specified
set(default_build_type "Release")
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  set(default_build_type "Debug")
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# By default, Linux makes all symbols public. Change that to hide all symbols by default
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

# Add a new library with the project name (cpptemplate) as its name
# Also create an alias so users can link to it with cpptemplate::cpptemplate
add_library(${PROJECT_NAME})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Set the library version 
set_target_properties(${PROJECT_NAME}
    PROPERTIES SOVERSION 0
    VERSION 0.0.0
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDE_DIR}>
)

if (BUILD_SHARED_LIBS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE "BUILDING_DLL")
endif ()

# Add the cmake folder to the cmake module path so it can find the 
# copy_dependencies.cmake file during the installation phase
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Set that the output compiled targets (library and executables) are placed in the binary directory directly.
# This can be especially helpful in Windows since the compiled dlls and exes won't have the r-path like in Linux.
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

###################################################################################################################
##                                                                                                              ###
##                                                  Dependencies                                                ###
##                                                                                                              ###
###################################################################################################################

function (link_dependencies_to_target target)
    find_package(BLAS REQUIRED)
    find_package(MPI REQUIRED)

    target_link_libraries(${target} PRIVATE BLAS::BLAS)
    target_link_libraries(${target} PUBLIC MPI::MPI_CXX)
endfunction()

link_dependencies_to_target(${PROJECT_NAME})

###################################################################################################################
##                                                                                                              ###
##                                            Add source files and tests                                        ###
##                                                                                                              ###
###################################################################################################################

add_subdirectory(src)
add_subdirectory(test)

###################################################################################################################
##                                                                                                              ###
##                                                    Install                                                   ###
##                                                                                                              ###
###################################################################################################################

set(dependencies_folder "cpptemplate-dependencies")

# If copying dependencies, set the rpath of the library to that folder so it can find them later.
if ("${copy_dependencies_during_install}")
    set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN/${dependencies_folder}")
endif()

include(GNUInstallDirs)

set(CMAKE_INSTALL_CMAKEDIR
    ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    CACHE STRING "Path to cpptemplate cmake files"
)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} 
        COMPONENT "${PROJECT_NAME}_Runtime"
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} 
        COMPONENT "${PROJECT_NAME}_Runtime"
        NAMELINK_COMPONENT "${PROJECT_NAME}_Development"
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT "${PROJECT_NAME}_Development"
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY
    include/${PROJECT_NAME}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

set(configFile ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake)
set(versionFile ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake)

include(CMakePackageConfigHelpers)

if ("${copy_dependencies_during_install}")
    set(config_cmake_in "${PROJECT_NAME}_only_public_dependenciesConfig.cmake.in")
else()
    set(config_cmake_in "${PROJECT_NAME}_all_dependenciesConfig.cmake.in")
endif()

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${config_cmake_in}
    ${configFile}
    INSTALL_DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
)

get_target_property(version ${PROJECT_NAME} VERSION)

write_basic_package_version_file(
    ${versionFile}
    VERSION ${version}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${configFile}
    ${versionFile}
    DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
)

install(EXPORT ${PROJECT_NAME}Targets
    DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
    NAMESPACE ${PROJECT_NAME}::
    FILE ${PROJECT_NAME}Targets.cmake
    COMPONENT ${PROJECT_NAME}_Development
)

function(get_privately_linked_targets target)

    get_target_property(linked_library_list ${target} LINK_LIBRARIES)
    get_target_property(link_interface_list ${target} INTERFACE_LINK_LIBRARIES)
    foreach(linked_library ${linked_library_list})
        if (NOT ";${linked_library};" MATCHES ${link_interface_list})
            list(APPEND private_targets ${linked_library})
        endif()
    endforeach()
    set(private_targets ${private_targets} PARENT_SCOPE)

endfunction()


if ("${copy_dependencies_during_install}")
    get_privately_linked_targets(${PROJECT_NAME})
    string(REPLACE ":" ";" include_list "${private_targets}")
    string(TOLOWER "${include_list}" include_list)
    foreach(include_lib ${include_list})
        list(APPEND include_libraries_list ".*${include_list}.*")
    endforeach()
    string(REPLACE ";" "$<SEMICOLON>" include_libraries_list "${include_libraries_list}")

    add_custom_target(copy_dependencies_during_install COMMAND
        ${CMAKE_COMMAND}
        -D library_target_list="$<TARGET_FILE:${PROJECT_NAME}>"
        -D include_libraries_list="${include_libraries_list}"
        -D copy_to_dir="${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/${dependencies_folder}"
        -P ${PROJECT_SOURCE_DIR}/cmake/copy_dependencies.cmake
    )

    add_dependencies(copy_dependencies_during_install ${PROJECT_NAME})

    install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} --build . --config $<CONFIG> --target copy_dependencies_during_install)")
endif()