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

void ComputeShader::uploadData(float* data, size_t size, uint32_t Index) {
  if (Index == bufId.size()) {
    bufId.push_back(-1);
    GLCall(glGenBuffers(1, &bufId[Index]));
  }
  if (Index >= 0 && Index < bufId.size()) {
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufId[Index]));
    GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, size * sizeof(float), (GLvoid*)data, GL_DYNAMIC_COPY));
    GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, Index, bufId[Index]));
  } else {
    std::cout << "[ComputeShader Error] data upload Index=" << Index << ", next value should be: " << bufId.size() << std::endl;
  }
}

void ComputeShader::compute(uint32_t sizeX, uint32_t sizeY, uint32_t sizeZ) {
  GLCall(glUseProgram(computeProgram));
  GLCall(glDispatchCompute(sizeX, sizeY, sizeZ));  // cria workgroups
  GLCall(glMemoryBarrier(GL_ALL_BARRIER_BITS));    // sincronizar
}

void ComputeShader::downloadData(float* data, size_t size, uint32_t Index) {
  if (Index >= 0 && Index < bufId.size()) {
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufId[Index]));
    GLCall(glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size * sizeof(float), (GLvoid*)data));
  } else {
    std::cout << "[ComputeShader Error] data download Index=" << Index << ", should be between: [0, " << bufId.size() - 1 << "]" << std::endl;
  }
}

void ComputeShader::realeaseData() {
  for (int Index = 0; Index < bufId.size(); Index++) {
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufId[Index]));
    GLCall(glDeleteBuffers(1, &bufId[Index]));
  }
  bufId.clear();
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
