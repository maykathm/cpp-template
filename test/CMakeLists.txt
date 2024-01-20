
if (BUILD_SHARED_LIBS)
    set(Boost_USE_STATIC_LIBS OFF)
endif()
find_package(Boost COMPONENTS unit_test_framework)

set(test_exe ${PROJECT_NAME}_test)
add_executable(${test_exe})

# Because the library only exports certain symbols, to unit test non-exported
# functions/methods, instead of linking directly to the library, we have to 
# link to the target object. Because then we are linking to the target object
# instead of the library, we have to also explicitly link to the library's 
# dependencies by calling the function link_dependencies_to_target
target_link_libraries(${test_exe} PRIVATE "$<TARGET_OBJECTS:${PROJECT_NAME}>")
target_include_directories(${test_exe} PRIVATE "../include")
target_include_directories(${test_exe} PRIVATE "../src")
link_dependencies_to_target(${test_exe})

# Link to boost library for unit testing.
target_link_libraries(${test_exe} PRIVATE ${Boost_LIBRARIES})

target_sources(${test_exe} PRIVATE
  BoostMain.cpp
  GreeterTest.cpp
)