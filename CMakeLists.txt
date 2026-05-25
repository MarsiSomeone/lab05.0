cmake_minimum_required(VERSION 3.14)
project(banking VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build unit tests" ON)
option(COVERAGE "Enable code coverage" OFF)

add_library(banking SHARED
    Account.cpp
    Transaction.cpp
)

target_include_directories(banking PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

set_target_properties(banking PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

if(BUILD_TESTS)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    FetchContent_MakeAvailable(googletest)
    
    add_subdirectory(tests)
endif()


