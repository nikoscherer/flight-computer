cmake_minimum_required(VERSION 3.22)
project(FlightSoftwareCore)

# Gets GoogleTest automatically via FetchContent
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)

# Prevents Windows/MSVC runtime mismatch issues
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_executable(unit_tests
    test_main.cpp
)

target_link_libraries(unit_tests
    PRIVATE
    GTest::gtest_main
    # flight_logic
)

# Register tests with CTest
gtest_discover_tests(unit_tests)

target_include_directories(unit_tests PRIVATE
    ../flightsoftware
)