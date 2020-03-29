cmake_minimum_required(VERSION 3.5)

project(25_encoding LANGUAGES CXX)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)

set(CMAKE_BUILD_TYPE DEBUG)
include_directories(${Boost_INCLUDE_DIR})

add_library(${PROJECT_NAME} SHARED
    compression.h compression.cpp)

add_executable(${PROJECT_NAME}_test "main.cpp")

add_executable(${PROJECT_NAME}_exec "main_exec.cpp")

set_target_properties(${PROJECT_NAME} ${PROJECT_NAME}_test ${PROJECT_NAME}_exec PROPERTIES
  LINKER_LANGUAGE CXX
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  COMPILE_OPTIONS -Wpedantic -Wall -Wextra
)

add_compile_options(lboost_system)

target_link_libraries(${PROJECT_NAME}_test
        ${Boost_LIBRARIES}
        ${PROJECT_NAME}
)

target_link_libraries(${PROJECT_NAME}_exec
        ${Boost_LIBRARIES}
        ${PROJECT_NAME}
)

set_target_properties(${PROJECT_NAME}_test ${PROJECT_NAME}_exec PROPERTIES
    COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
    INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
)

enable_testing()

add_test(test_version_valid ${PROJECT_NAME}_test)
