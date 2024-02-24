cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/dep/glfw-3.3.8.bin.WIN64/include
	${CMAKE_CURRENT_SOURCE_DIR}/dep/glew-2.1.0/include
)

link_libraries(
    ${CMAKE_CURRENT_SOURCE_DIR}/dep/glfw-3.3.8.bin.WIN64/lib-vc2022/glfw3.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/dep/glfw-3.3.8.bin.WIN64/lib-vc2022/glfw3_mt.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/dep/glew-2.1.0/lib/Release/x64/glew32s.lib
    OpenGL32.lib
)

#################################################
add_executable(computeshader
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ComputeShader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)
#################################################