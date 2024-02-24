#include <iostream>
#include "ComputeShader.h"

ComputeShader::ComputeShader(std::string prog) {
  // abrir contexto opengl
  initGL(window);
  // compilar shader
  const char* src = prog.c_str();
  GLCall(uint32_t cs = glCreateShader(GL_COMPUTE_SHADER));
  GLCall(glShaderSource(cs, 1, &src, NULL));
  GLCall(glCompileShader(cs));
  // linkar programa
  GLCall(computeProgram = glCreateProgram());
  GLCall(glAttachShader(computeProgram, cs));
  GLCall(glLinkProgram(computeProgram));
  GLCall(glValidateProgram(computeProgram));
  // limpar memoria
  GLCall(glDeleteShader(cs));
}

ComputeShader::~ComputeShader() {
  // liberar memoria de dados
  realeaseData();
  // apagar programa da gpu
  GLCall(glDeleteProgram(computeProgram));
  // fechar contexto opengl
  glfwTerminate();
}

void ComputeShader::uploadData(float* data, size_t size) {
  GLCall(glGenBuffers(1, &bufId));
  GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufId));
  GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, size * sizeof(float), (GLvoid*)data, GL_DYNAMIC_COPY));
  GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, bufId));
}

void ComputeShader::compute(uint32_t sizeX, uint32_t sizeY, uint32_t sizeZ) {
  GLCall(glUseProgram(computeProgram));
  GLCall(glDispatchCompute(sizeX, sizeY, sizeZ));  // cria workgroups
  GLCall(glMemoryBarrier(GL_ALL_BARRIER_BITS));    // sincronizar
}

void ComputeShader::downloadData(float* data, size_t size) {
  GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufId));
  GLCall(glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size * sizeof(float), (GLvoid*)data));
}

void ComputeShader::realeaseData() {
  GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufId));
  GLCall(glDeleteBuffers(1, &bufId));
}

void ComputeShader::initGL(GLFWwindow*& window, int width, int height, const char* title, bool windowVisible) {
  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  if (!windowVisible) {
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  }

  window = glfwCreateWindow(width, height, title, nullptr, nullptr);

  if (!window) {
    glfwTerminate();
    std::cout << "Failed to create glfw window\n";
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);

  if (glewInit() != GLEW_OK) {
    std::cout << "Failed to initalize OpenGL\n";
    exit(EXIT_FAILURE);
  }
}

void ComputeShader::GLClearError() {
  while (glGetError() != GL_NO_ERROR)
    ;
}

bool ComputeShader::GLLogCall(const char* function, const char* file, int line) {
  while (GLenum error = glGetError()) {
    std::cout << "[OpenGL Error] (" << error << "): " << function << " " << file << ":" << line << std::endl;
    return false;
  }
  return true;
}
