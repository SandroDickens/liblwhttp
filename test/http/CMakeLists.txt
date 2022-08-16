set(TEST_TARGET_NAME "HttpTests")

add_test(NAME httpTest COMMAND ${TEST_TARGET_NAME} --exe $<TARGET_FILE:${TEST_TARGET_NAME}>)

add_executable(${TEST_TARGET_NAME} HttpTests.cpp URLTests.cpp ClientTests.cpp)
target_link_libraries(${TEST_TARGET_NAME} lwhttp GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(${TEST_TARGET_NAME} DISCOVERY_TIMEOUT 30)
