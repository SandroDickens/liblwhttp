set(MAIN_SRC_DIR "../../main")

set(BIN_NAME "http_test")

include_directories(${MAIN_SRC_DIR})

add_test(NAME httpTest COMMAND ${BIN_NAME} --exe $<TARGET_FILE:${BIN_NAME}>)

#file(GLOB MAIN_SRC_FILES "${MAIN_SRC_DIR}/http/*.cpp")
#message(STATUS "HttpTests.cpp srcs: ${MAIN_SRC_FILES}")
add_executable(${BIN_NAME} HttpTests.cpp)
target_link_libraries(${BIN_NAME} lwhttp)
