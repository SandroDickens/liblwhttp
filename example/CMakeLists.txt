set(DEMO_TARGET_NAME "demo")

add_executable(${DEMO_TARGET_NAME} main.cpp)
target_link_directories(${DEMO_TARGET_NAME} PUBLIC ${CMAKE_BINARY_DIR}/lib)
target_link_libraries(${DEMO_TARGET_NAME} lwhttp)
