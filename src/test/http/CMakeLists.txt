set(MAIN_SRC_DIR "../../main")

set(BIN_NAME "http_test")

include_directories(${MAIN_SRC_DIR})

add_test(NAME httpTest COMMAND ${BIN_NAME} --exe $<TARGET_FILE:${BIN_NAME}>)

add_executable(${BIN_NAME} HttpTests.cpp URLTests.cpp ClientTests.cpp)
target_link_libraries(${BIN_NAME} lwhttp)
