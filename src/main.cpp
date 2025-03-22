#include <iostream>
#include <algorithm>
#include <chrono>
#include "ComputeShader/ComputeShader.h"

#define M_PI 3.1415926535897932384626433832795f

static const std::string process = R"glsl(
///////////////////////////////////////////////////////////////////////////////
#version 430 core
layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout(binding = 0) buffer B0 { uint img[]; };
void main() {
  const uvec3 gSize = gl_WorkGroupSize * gl_NumWorkGroups;
  const uint j = gl_GlobalInvocationID.x;
  const uint i = gl_GlobalInvocationID.y;

  const float x = float(j) / float(gSize.x - 1);
  const float y = float(i) / float(gSize.y - 1);
  const float fxr = 15.5f;
  const float fxg = 16.0f;
  const float fxb = 16.5f;
  const float fyr = 8.3f;
  const float fyg = 9.0f;
  const float fyb = 9.7f;
  const float rf = 0.5f + 0.25f * sin(2 * 3.1415f * x * fxr) + 0.25f * sin(2 * 3.1415f * y * fyr);
  const float gf = 0.5f + 0.25f * sin(2 * 3.1415f * x * fxg) + 0.25f * sin(2 * 3.1415f * y * fyg);
  const float bf = 0.5f + 0.25f * sin(2 * 3.1415f * x * fxb) + 0.25f * sin(2 * 3.1415f * y * fyb);
  const uint A = uint(0xFF);
  const uint R = uint(0xFF * rf);
  const uint G = uint(0xFF * gf);
  const uint B = uint(0xFF * bf);
  img[i * gSize.x + j] = (A << 24) | (R << 16) | (G << 8) | B;
}
///////////////////////////////////////////////////////////////////////////////
)glsl";

int main() {
  FILE* fp;
  int w = 1920;
  int h = 1080;
  std::vector<uint32_t> img(w * h);

  std::chrono::time_point<std::chrono::steady_clock> t1, t0;

  // CPU
  t0 = std::chrono::high_resolution_clock::now();
#pragma omp parallel for schedule(guided)
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      const float x = float(j) / float(w - 1);
      const float y = float(i) / float(h - 1);
      constexpr float fxr = 15.5f;
      constexpr float fxg = 16.0f;
      constexpr float fxb = 16.5f;
      constexpr float fyr = 8.3f;
      constexpr float fyg = 9.0f;
      constexpr float fyb = 9.7f;
      const float rf = 0.5f + 0.25f * sin(2 * 3.1415f * x * fxr) + 0.25f * sin(2 * 3.1415f * y * fyr);
      const float gf = 0.5f + 0.25f * sin(2 * 3.1415f * x * fxg) + 0.25f * sin(2 * 3.1415f * y * fyg);
      const float bf = 0.5f + 0.25f * sin(2 * 3.1415f * x * fxb) + 0.25f * sin(2 * 3.1415f * y * fyb);
      const uint32_t A = uint32_t(0xFF);
      const uint32_t R = uint32_t(0xFF * rf);
      const uint32_t G = uint32_t(0xFF * gf);
      const uint32_t B = uint32_t(0xFF * bf);
      img[i * w + j] = (uint32_t)((A << 24) | (R << 16) | (G << 8) | B);
    }
  }
  t1 = std::chrono::high_resolution_clock::now();
  std::cout << "tempo CPU: " << (t1 - t0).count() * 1e-6 << " ms" << std::endl;

  // salvar arquivo
  std::string rawFileName = "../../res/img_CPU_" + std::to_string(w) + "x" + std::to_string(h) + ".raw";
  fopen_s(&fp, rawFileName.c_str(), "wb");
  fwrite(img.data(), sizeof(uint32_t), w * h, fp);
  fclose(fp);

  // GPU
  ComputeShader cs;
  cs.addKernel(process, 0);
  cs.uploadData(img.data(), w * h * sizeof(uint32_t), 0);
  t0 = std::chrono::high_resolution_clock::now();
  cs.uploadData(img.data(), w * h * sizeof(uint32_t), 0);
  cs.compute(0, w / 8, h / 4);
  cs.synchronize();
  cs.downloadData(img.data(), w * h * sizeof(uint32_t), 0);
  t1 = std::chrono::high_resolution_clock::now();
  std::cout << "tempo GPU: " << (t1 - t0).count() * 1e-6 << " ms" << std::endl;

  // salvar arquivo
  rawFileName = "../../res/img_GPU_" + std::to_string(w) + "x" + std::to_string(h) + ".raw";
  fopen_s(&fp, rawFileName.c_str(), "wb");
  fwrite(img.data(), sizeof(uint32_t), w * h, fp);
  fclose(fp);

  return EXIT_SUCCESS;
}