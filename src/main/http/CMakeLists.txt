set(TARGET_NAME "lwhttp")

file(GLOB LIB_SRC_FILES "*.cpp")
if (CMAKE_HOST_WIN32)
add_library(${TARGET_NAME} STATIC ${LIB_SRC_FILES})
elseif (CMAKE_HOST_UNIX)
    add_library(${TARGET_NAME} SHARED ${LIB_SRC_FILES})
endif ()
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_definitions(-D_DEBUG)
endif ()

target_link_libraries(${TARGET_NAME} crypto ssl)
