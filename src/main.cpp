#include <iostream>
#include "ComputeShader.h"

static const std::string progStr = R"glsl(
///////////////////////////////////////////////////////////////////////////////
// compute shader
#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(binding = 0) buffer readwrite { float buf[]; };

void main() {
  const uint idx = gl_GlobalInvocationID.x;
  buf[idx] *= 2.0;
}
///////////////////////////////////////////////////////////////////////////////
)glsl";

int main() {
  ComputeShader cs(progStr);

  // entrada
  constexpr int size = 10;
  float data[size] = {10.1f, 11.1f, 12.1f, 13.1f, 14.1f, 15.1f, 16.1f, 17.1f, 18.1f, 19.1f};
  for (int i = 0; i < size; i++) std::cout << data[i] << (i == size - 1 ? "\n" : " ") << std::ends;  // mostrar

  cs.uploadData(data, size);    // enviar dados para gpu
  cs.compute(size);             // executar compute shader
  cs.downloadData(data, size);  // ler dados da gpu

  // saida
  for (int i = 0; i < size; i++) std::cout << data[i] << (i == size - 1 ? "\n" : " ") << std::ends;  // mostrar

  return EXIT_SUCCESS;
}