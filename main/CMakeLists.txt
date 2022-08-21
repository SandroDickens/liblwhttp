add_subdirectory(src/http)

target_include_directories(lwhttp
        PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/main/include>
        PRIVATE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/main>)