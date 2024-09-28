#include <iostream>
#include <algorithm>
#include <chrono>
#include "ComputeShader/ComputeShader.h"

static const std::string process = R"glsl(
///////////////////////////////////////////////////////////////////////////////
#version 430 core
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(binding = 0) buffer B0 { uint img[]; };
layout(binding = 1) buffer B1 { uint w, h; };
void main() {
  const uint j = gl_GlobalInvocationID.x;
  const uint i = gl_GlobalInvocationID.y;
  float x = float(j) / float(w - 1);
  float y = float(i) / float(h - 1);
  float fxr = 15.5f;
  float fxg = 16.0f;
  float fxb = 16.5f;
  float fyr = 8.3f;
  float fyg = 9.0f;
  float fyb = 9.7f;
  float rf = 0.5f + 0.25f * sin(2 * 3.1415f * x * fxr) + 0.25f * sin(2 * 3.1415f * y * fyr);
  float gf = 0.5f + 0.25f * sin(2 * 3.1415f * x * fxg) + 0.25f * sin(2 * 3.1415f * y * fyg);
  float bf = 0.5f + 0.25f * sin(2 * 3.1415f * x * fxb) + 0.25f * sin(2 * 3.1415f * y * fyb);
  uint A = uint(0xFF);
  uint R = uint(0xFF * clamp(rf, 0.0f, 1.0f));
  uint G = uint(0xFF * clamp(gf, 0.0f, 1.0f));
  uint B = uint(0xFF * clamp(bf, 0.0f, 1.0f));
  img[i * w + j] = (A << 24) | (R << 16) | (G << 8) | B;
}
///////////////////////////////////////////////////////////////////////////////
)glsl";

int main() {
  FILE* fp;
  std::string rawFileName;
  std::chrono::time_point<std::chrono::steady_clock> t1, t0;
  ComputeShader cs;

  cs.addKernel(process, 0);
  int w = 1920;
  int h = 1080;
  uint32_t* img = new uint32_t[w * h];

  ////////////////////////////////////////////////////////////////////////////////////
  // calculo na CPU
  t0 = std::chrono::high_resolution_clock::now();
  for (int j, i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      float x = (float)(j) / (float)(w - 1);
      float y = (float)(i) / (float)(h - 1);
      float fxr = 15.5f;
      float fxg = 16.0f;
      float fxb = 16.5f;
      float fyr = 8.3f;
      float fyg = 9.0f;
      float fyb = 9.7f;
      float rf = 0.5f + 0.25f * std::sin(2 * 3.1415f * x * fxr) + 0.25f * std::sin(2 * 3.1415f * y * fyr);
      float gf = 0.5f + 0.25f * std::sin(2 * 3.1415f * x * fxg) + 0.25f * std::sin(2 * 3.1415f * y * fyg);
      float bf = 0.5f + 0.25f * std::sin(2 * 3.1415f * x * fxb) + 0.25f * std::sin(2 * 3.1415f * y * fyb);
      uint8_t A = (uint8_t)(0xFF);
      uint8_t R = (uint8_t)(0xFF * std::clamp(rf, 0.0f, 1.0f));
      uint8_t G = (uint8_t)(0xFF * std::clamp(gf, 0.0f, 1.0f));
      uint8_t B = (uint8_t)(0xFF * std::clamp(bf, 0.0f, 1.0f));
      img[i * w + j] = (A << 24) | (R << 16) | (G << 8) | B;
    }
  }
  t1 = std::chrono::high_resolution_clock::now();
  std::cout << "tempo CPU: " << (t1 - t0).count() * 1e-6 << " ms" << std::endl;
  // salvar arquivo
  rawFileName = "../../res/img_CPU_" + std::to_string(w) + "x" + std::to_string(h) + ".raw";
  fopen_s(&fp, rawFileName.c_str(), "wb");
  fwrite(img, sizeof(uint32_t), w * h, fp);
  fclose(fp);

  ////////////////////////////////////////////////////////////////////////////////////
  // calculo na GPU
  t0 = std::chrono::high_resolution_clock::now();
  int wh[2] = {w, h};
  cs.uploadData(img, w * h * sizeof(uint32_t), 0);
  cs.uploadData(wh, 2 * sizeof(uint32_t), 1);
  cs.compute(0, w, h);
  cs.synchronize();
  cs.downloadData(img, w * h * sizeof(uint32_t), 0);
  t1 = std::chrono::high_resolution_clock::now();
  std::cout << "tempo GPU: " << (t1 - t0).count() * 1e-6 << " ms" << std::endl;
  // salvar arquivo
  rawFileName = "../../res/img_GPU_" + std::to_string(w) + "x" + std::to_string(h) + ".raw";
  fopen_s(&fp, rawFileName.c_str(), "wb");
  fwrite(img, sizeof(uint32_t), w * h, fp);
  fclose(fp);

  delete[] img;

  return EXIT_SUCCESS;
}