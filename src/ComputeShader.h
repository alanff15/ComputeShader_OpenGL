#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

class ComputeShader {
public:
  ComputeShader(GLFWwindow* extWindow = NULL);
  ~ComputeShader();

  /// @brief Add kernel to the compute shader, 'ProgramIndex' must start with 0 and increment
  /// if you select the same 'ProgramIndex' the later program will replace the previous
  /// @param prog string containing the shader code
  /// @param ProgramIndex index of the actual kernel to receive the shader
  void addKernel(std::string prog, uint32_t ProgramIndex);

  /// @brief Upload data from local memory to the GPU, 'BindingIndex' must start with 0 and
  /// increment if you select the same 'BindingIndex' the later data will replace the previous
  /// @param data data array to be uploaded, type 'float'
  /// @param size size of the float array (number of floats, eg.: float data[5] -> size=5)
  /// @param BindingIndex index of the actual binding to be selected inside the kernel layout
  void uploadData(float* data, size_t size, uint32_t BindingIndex);

  /// @brief Runs the compute shader selected by 'ProgramIndex' with as many workgroups per axis
  /// as indicated by 'sizeX', 'sizeY' and 'sizeZ'
  /// @param ProgramIndex index of the kernel to run
  /// @param sizeX size of the workgroup in X
  /// @param sizeY size of the workgroup in Y, default value is 1
  /// @param sizeZ size of the workgroup in Z, default value is 1
  void compute(uint32_t ProgramIndex, uint32_t sizeX, uint32_t sizeY = 1, uint32_t sizeZ = 1);

  /// @brief Download data from the GPU to the local memory, 'BindingIndex' indicates from which
  /// buffer the data will be taken
  /// @param data data array to be downloaded, type 'float'
  /// @param size size of the float array (number of floats, eg.: float data[5] -> size=5)
  /// @param BindingIndex index of the buffer
  void downloadData(float* data, size_t size, uint32_t BindingIndex);

  /// @brief Releases all buffers in the GPU memory, resests the binding index to 0
  void realeaseData();

  /// @brief Remove all kernels in the GPU memory, resests the program index to 0
  void realeaseKernels();

  /// @brief Waits for openGL processes to finish, may assume one of the following values:
  /// GL_ALL_BARRIER_BITS, GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT, GL_ELEMENT_ARRAY_BARRIER_BIT,
  /// GL_UNIFORM_BARRIER_BIT, GL_TEXTURE_FETCH_BARRIER_BIT, GL_SHADER_IMAGE_ACCESS_BARRIER_BIT,
  /// GL_COMMAND_BARRIER_BIT, GL_PIXEL_BUFFER_BARRIER_BIT,GL_TEXTURE_UPDATE_BARRIER_BIT,
  /// GL_BUFFER_UPDATE_BARRIER_BIT, GL_FRAMEBUFFER_BARRIER_BIT, GL_TRANSFORM_FEEDBACK_BARRIER_BIT,
  /// GL_ATOMIC_COUNTER_BARRIER_BIT, GL_SHADER_STORAGE_BARRIER_BIT
  /// @param barriers default value is GL_ALL_BARRIER_BITS
  void synchronize(GLuint barriers = GL_ALL_BARRIER_BITS);

private:
  void initGL(GLFWwindow*& window, int width = 1, int height = 1, const char* title = "", bool windowVisible = false);
  void GLClearError();
  bool GLLogCall(const char* function, const char* file, int line);

  GLFWwindow* window;
  std::vector<GLuint> programId;
  std::vector<GLuint> bufId;
};