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
include(CTest)

add_executable(unit_tests
    test_main.cpp
)

target_link_libraries(unit_tests
    PRIVATE
    GTest::gtest_main
    flight_logic
)

target_include_directories(unit_tests PRIVATE
    ../flightsoftware
    ../flightsoftware/bus/inc
    ../flightsoftware/containers/inc
)

# Register tests with CTest
gtest_discover_tests(unit_tests)