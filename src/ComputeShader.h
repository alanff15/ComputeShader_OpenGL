#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

#define ASSERT(x) \
  if (!(x)) __debugbreak();

#define GLCall(x) \
  GLClearError(); \
  x;              \
  ASSERT(GLLogCall(#x, __FILE__, __LINE__))

class ComputeShader {
public:
  ComputeShader();
  ~ComputeShader();

  void addKernel(std::string prog, uint32_t ProgramIndex);

  void uploadData(float* data, size_t size, uint32_t BindingIndex);
  void compute(uint32_t ProgramIndex, uint32_t sizeX, uint32_t sizeY = 1, uint32_t sizeZ = 1);
  void downloadData(float* data, size_t size, uint32_t BindingIndex);
  void realeaseData();

private:
  void initGL(GLFWwindow*& window, int width = 1, int height = 1, const char* title = "", bool windowVisible = false);
  void GLClearError();
  bool GLLogCall(const char* function, const char* file, int line);

  GLFWwindow* window;
  std::vector<GLuint> programId;
  std::vector<GLuint> bufId;
};