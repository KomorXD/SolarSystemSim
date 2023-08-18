set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)

add_executable(${PROJECT_NAME})

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "*.cpp")
file(GLOB_RECURSE PROJECT_HEADERS CONFIGURE_DEPENDS "*.hpp")
file(GLOB_RECURSE VENDORS_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_SOURCE_DIR}/dependencies/glad/src/glad.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendors/*.cpp"
)
file(GLOB_RECURSE PROJECT_CONFIGS CONFIGURE_DEPENDS
    "../CMakeLists.txt"
    "../.gitignore"
    "../.gitmodules"
)
file(COPY res
	DESTINATION
		"${CMAKE_SOURCE_DIR}/out/bin/${BUILD_ARCHITECTURE}-${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}/"
)

target_sources(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
        ${PROJECT_CONFIGS}
        ${VENDORS_SOURCES}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        "${CMAKE_SOURCE_DIR}/dependencies/glfw/include/"
        "${CMAKE_SOURCE_DIR}/dependencies/glad/include/"
        "${CMAKE_SOURCE_DIR}/dependencies/glm/"
        "${CMAKE_SOURCE_DIR}/dependencies/spdlog/include/"
        "${CMAKE_CURRENT_SOURCE_DIR}/vendors/"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        glfw
        ${GLFW_LIBRARIES}
        ${GLAD_LIBRARIES}
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        $<$<CONFIG:Debug>:CONF_DEBUG=1>
        $<$<CONFIG:Release>:CONF_RELEASE=1>
        $<$<CONFIG:Prod>:CONF_PROD=1>
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out/bin/${BUILD_ARCHITECTURE}-${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}/"
)
