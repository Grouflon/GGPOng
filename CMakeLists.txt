# seems like good resource : https://cliutils.gitlab.io/modern-cmake/

cmake_minimum_required(VERSION 3.2)

project(GGPOng)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib)

find_package(OpenGL REQUIRED)

add_subdirectory(extern/SFML)
add_subdirectory(extern/GGPO)

aux_source_directory( "./src/" GGPONG)
aux_source_directory( "./extern/imgui" IMGUI)
aux_source_directory( "./extern/imgui-sfml" IMGUI_SFML)

add_compile_definitions(IMGUI_USER_CONFIG=<imconfig-SFML.h>)

set(SOURCES
  ${GGPONG}
  ${IMGUI}
  ./extern/imgui-sfml/imgui-SFML.cpp
)

include_directories(
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
