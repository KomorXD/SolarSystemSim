option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)

include_directories(
	"../dependencies/glfw/include/"
	"../dependencies/glad/include/"
	"../dependencies/glm/"
	"../dependencies/spdlog/include/"
	"vendors/"
)

file(GLOB PROJECT_HEADERS 
	"*.hpp" 
	"scenes/*.hpp"
	"renderer/*.hpp"
)

file(GLOB PROJECT_SOURCES 
	"*.cpp" 
	"scenes/*.cpp"
	"renderer/*.cpp"
)

file(GLOB VENDORS_SOURCES 
	"../dependencies/glad/src/glad.c"
	"vendors/imgui/*.cpp"
	"vendors/stb_image/*.cpp"
)

file(GLOB PROJECT_CONFIGS
	"../CMakeLists.txt"
	"../.gitignore"
	"../.gitmodules"
)

file(COPY res DESTINATION "${CMAKE_BINARY_DIR}/${PROJECT_NAME}")

source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Sources" FILES ${PROJECT_SOURCES})
source_group("Vendors" FILES ${VENDORS_SOURCES})

if(SSS_DEBUG)
	add_definitions(-DCONF_DEBUG=1)
elseif(SSS_RELEASE)
	add_definitions(-DCONF_RELEASE=1)
elseif(SSS_PROD)
	add_definitions(-DCONF_PROD=1)
endif()

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${PROJECT_CONFIGS} ${VENDORS_SOURCES})

target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES} ${GLAD_LIBRARIES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()