add_test([=[IMUTests.InitTest]=]  C:/Bulldog-Rocketry/flight-computer/build/tests/test/flight_tests.exe [==[--gtest_filter=IMUTests.InitTest]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[IMUTests.InitTest]=]  PROPERTIES DEF_SOURCE_LINE [==[C:\Bulldog-Rocketry\flight-computer\test\tests\imu_test.cpp:10]==] WORKING_DIRECTORY C:/Bulldog-Rocketry/flight-computer/build/tests/test SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set(  flight_tests_TESTS IMUTests.InitTest)
