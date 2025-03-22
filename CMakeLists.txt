cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)

#################################################
add_library(ComputeShader
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ComputeShader/ComputeShader.cpp
)
target_include_directories(ComputeShader PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/dep/glfw-3.3.8.bin.WIN64/include
	${CMAKE_CURRENT_SOURCE_DIR}/dep/glew-2.1.0/include
)
target_link_libraries(ComputeShader PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/dep/glfw-3.3.8.bin.WIN64/lib-vc2022/glfw3.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/dep/glfw-3.3.8.bin.WIN64/lib-vc2022/glfw3_mt.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/dep/glew-2.1.0/lib/Release/x64/glew32s.lib
    OpenGL32.lib
)

find_package(OpenMP)
link_libraries(OpenMP::OpenMP_CXX)

#################################################
add_executable(teste_compShader
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)
target_link_libraries(teste_compShader PRIVATE ComputeShader)