cmake_minimum_required(VERSION 3.0)
project(demo)

set(CMAKE_C_STANDARD 14)
set(CMAKE_CXX_STANDARD 17)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "The default is the Release version" FORCE)
endif ()

if (CMAKE_HOST_UNIX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -march=native -pipe")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wold-style-cast -Wextra -Wshadow -Wpointer-arith -Wwrite-strings \
	-Woverloaded-virtual \
	-Wno-pessimizing-move \
	-Wno-uninitialized -Wno-unused-parameter -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable \
	-Wno-unused-value -march=native -pipe")
endif ()

enable_testing()

add_subdirectory(src/main)
add_subdirectory(src/test)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    message(STATUS "Debug version...")
    if (CMAKE_HOST_UNIX)
        set(ASAN_FLAGS "-fsanitize=address -fsanitize=undefined -fno-sanitize-recover=all -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fno-sanitize=null -fno-sanitize=alignment")
    else ()
        set(ASAN_FLAGS "")
    endif ()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g ${ASAN_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g ${ASAN_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
    add_definitions(-D_DEBUG)
else ()
    message(STATUS "Release version...")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2")
endif ()

include_directories(src/main)
add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} lwhttp)