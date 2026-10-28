add_executable(flight_tests
    mocks/mock_spi_bus.cpp

    tests/imu_test.cpp
    tests/bus_handler_test.cpp
    tests/spi_bus_test.cpp
)

target_include_directories(flight_tests PRIVATE mocks)

target_link_libraries(flight_tests PRIVATE
    flightlogic
    GTest::gtest_main
    GTest::gmock
)

include(GoogleTest)
gtest_discover_tests(flight_tests)