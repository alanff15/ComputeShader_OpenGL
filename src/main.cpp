#include <iostream>
#include "ComputeShader.h"

static const std::string kernel0 = R"glsl(
///////////////////////////////////////////////////////////////////////////////
// kernel 0
#version 430 core
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(binding = 0) buffer B0 { float v0[]; };
layout(binding = 1) buffer B1 { float v1[]; };
layout(binding = 2) buffer B2 { float v2[]; };
void main() {
  const uint idx = gl_GlobalInvocationID.x;
  v2[idx] = v0[idx] + v1[idx];
}
///////////////////////////////////////////////////////////////////////////////
)glsl";

static const std::string kernel1 = R"glsl(
///////////////////////////////////////////////////////////////////////////////
// kernel 1
#version 430 core
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(binding = 0) buffer B0 { float v0[]; };
layout(binding = 1) buffer B1 { float v1[]; };
layout(binding = 2) buffer B2 { float v2[]; };
void main() {
  const uint idx = gl_GlobalInvocationID.x;
  v2[idx] = v0[idx] * v1[idx];
}
///////////////////////////////////////////////////////////////////////////////
)glsl";

int main() {
  ComputeShader cs;

  cs.addKernel(kernel0, 0);
  cs.addKernel(kernel1, 1);

  // entrada
  int size = 5;
  float v0[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
  float v1[] = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
  float v2[] = {0, 0, 0, 0, 0};
  std::cout << "v0 = " << std::ends;
  for (int i = 0; i < size; i++) std::cout << v0[i] << (i == size - 1 ? "\n" : " ") << std::ends;  // mostrar
  std::cout << "v1 = " << std::ends;
  for (int i = 0; i < size; i++) std::cout << v1[i] << (i == size - 1 ? "\n" : " ") << std::ends;  // mostrar

  // enviar dados para gpu
  cs.uploadData(v0, size, 0);  // binding 0
  cs.uploadData(v1, size, 1);  // binding 1
  cs.uploadData(v2, size, 2);  // binding 2

  // kernel 0
  cs.compute(0, size);           // executar compute shader
  cs.synchronize();              // aguardar processamento
  cs.downloadData(v2, size, 2);  // ler dados da gpu
  std::cout << "v2 = v0+v1 = " << std::ends;
  for (int i = 0; i < size; i++) std::cout << v2[i] << (i == size - 1 ? "\n" : " ") << std::ends;  // mostrar

  // kernel 1
  cs.compute(1, size);           // executar compute shader
  cs.synchronize();              // aguardar processamento
  cs.downloadData(v2, size, 2);  // ler dados da gpu
  std::cout << "v2 = v0*v1 = " << std::ends;
  for (int i = 0; i < size; i++) std::cout << v2[i] << (i == size - 1 ? "\n" : " ") << std::ends;  // mostrar

  return EXIT_SUCCESS;
}