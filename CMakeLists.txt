# seems like good resource : https://cliutils.gitlab.io/modern-cmake/

cmake_minimum_required(VERSION 3.2)

project(GGPOng)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH}
"./extern/ggpo"
)
message(${CMAKE_INSTALL_PREFIX})

#set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib)

find_package(OpenGL REQUIRED)

add_subdirectory(extern/SFML)
add_subdirectory(extern/GGPO)

file(GLOB GGPONG_SRC "./src/*.h" "./src/*.cpp")
file(GLOB IMGUI_SRC "./extern/imgui/*.h" "./extern/imgui/*.cpp")
file(GLOB IMGUI_SFML_SRC "./extern/imgui-sfml/*.h" "./extern/imgui-sfml/*.cpp")

add_compile_definitions(
	IMGUI_USER_CONFIG=<imconfig-SFML.h>
	_CRT_SECURE_NO_WARNINGS
)

set(SOURCES
  ${GGPONG_SRC}
  ${IMGUI_SRC}
  ${IMGUI_SFML_SRC}
)

include_directories(
	./src/
	./extern/imgui-sfml/
	./extern/imgui/
)

#source_group(" " FILES ${GGPONG})
#source_group("imgui" FILES ${IMGUI})

add_executable(GGPOng "./src/main" ${SOURCES})

target_link_libraries(GGPOng PRIVATE ${OPENGL_LIBRARY})
target_link_libraries(GGPOng PRIVATE sfml-graphics)
target_link_libraries(GGPOng PRIVATE sfml-window)
target_link_libraries(GGPOng PRIVATE GGPO)

add_custom_command(TARGET GGPOng POST_BUILD

	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	${CMAKE_CURRENT_BINARY_DIR}/extern/ggpo/bin/x86/$<CONFIG>/GGPO.dll
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>
)