cmake_minimum_required(VERSION 3.11)
project(demo)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/main/include)

option(ENABLE_DEMO "Enable demo" OFF)
option(ENABLE_TESTS "Enable test" OFF)
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)

set(CMAKE_C_STANDARD 14)
set(CMAKE_CXX_STANDARD 17)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

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

# AddressSanitizer
if (ENABLE_ASAN)
    set(ASAN_FLAGS "-fsanitize=address -fsanitize=undefined -fno-sanitize-recover=all -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fno-sanitize=null -fno-sanitize=alignment")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ASAN_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ASAN_FLAGS}")
endif ()

# Debug or Release
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    message(STATUS "Debug version...")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_DEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
    add_definitions(-D_DEBUG)
else ()
    message(STATUS "Release version...")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
endif ()

if (CMAKE_HOST_UNIX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif ()

include_directories(main)
add_subdirectory(main)

if (ENABLE_DEMO)
    add_subdirectory(example)
endif ()

if (ENABLE_TESTS)
    enable_testing()
    add_subdirectory(test)
endif ()
