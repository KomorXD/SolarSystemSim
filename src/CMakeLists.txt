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

if(SSS_DEBUG)
	add_definitions(-DCONF_DEBUG=1)
elseif(SSS_RELEASE)
	add_definitions(-DCONF_RELEASE=1)
elseif(SSS_PROD)
	add_definitions(-DCONF_PROD=1)
endif()

if(TARGET_WINDOWS)
	add_definitions(-DTARGET_WINDOWS=1)
elseif(TARGET_LINUX)
	add_definitions(-DTARGET_LINUX=1)
endif()

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

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out/bin/${BUILD_ARCHITECTURE}-${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}/"
)
