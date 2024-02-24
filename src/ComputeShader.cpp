#include <iostream>
#include "ComputeShader.h"

ComputeShader::ComputeShader() {
  // abrir contexto opengl
  initGL(window);
}

ComputeShader::~ComputeShader() {
  // liberar memoria de dados
  realeaseData();
  // apagar programas da gpu
  for (int ProgramIndex = 0; ProgramIndex < programId.size(); ProgramIndex++) {
    GLCall(glDeleteProgram(programId[ProgramIndex]));
  }
  programId.clear();
  // fechar contexto opengl
  glfwTerminate();
}

void ComputeShader::addKernel(std::string Program, uint32_t ProgramIndex) {
  // compilar shader
  const char* src = Program.c_str();
  GLCall(uint32_t cs = glCreateShader(GL_COMPUTE_SHADER));
  GLCall(glShaderSource(cs, 1, &src, NULL));
  GLCall(glCompileShader(cs));
  if (ProgramIndex == programId.size()) {
    programId.push_back(-1);
    // linkar programa
    GLCall(programId[ProgramIndex] = glCreateProgram());
    GLCall(glAttachShader(programId[ProgramIndex], cs));
    GLCall(glLinkProgram(programId[ProgramIndex]));
    GLCall(glValidateProgram(programId[ProgramIndex]));
  } else {
    std::cout << "[ComputeShader Error] data program Index=" << ProgramIndex << ", next value should be: " << programId.size() << std::endl;
  }
  // limpar memoria
  GLCall(glDeleteShader(cs));
}

void ComputeShader::uploadData(float* data, size_t size, uint32_t BindingIndex) {
  if (BindingIndex == bufId.size()) {
    bufId.push_back(-1);
    GLCall(glGenBuffers(1, &bufId[BindingIndex]));
  }
  if (BindingIndex >= 0 && BindingIndex < bufId.size()) {
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufId[BindingIndex]));
    GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, size * sizeof(float), (GLvoid*)data, GL_DYNAMIC_COPY));
    GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BindingIndex, bufId[BindingIndex]));
  } else {
    std::cout << "[ComputeShader Error] data upload Index=" << BindingIndex << ", next value should be: " << bufId.size() << std::endl;
  }
}

void ComputeShader::compute(uint32_t ProgramIndex, uint32_t sizeX, uint32_t sizeY, uint32_t sizeZ) {
  if (ProgramIndex >= 0 && ProgramIndex < programId.size()) {
    GLCall(glUseProgram(programId[ProgramIndex]));
    GLCall(glDispatchCompute(sizeX, sizeY, sizeZ));  // criar workgroups
  } else {
    std::cout << "[ComputeShader Error] data download Index=" << ProgramIndex << ", should be between: [0, " << programId.size() - 1 << "]" << std::endl;
  }
}

void ComputeShader::downloadData(float* data, size_t size, uint32_t BindingIndex) {
  if (BindingIndex >= 0 && BindingIndex < bufId.size()) {
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufId[BindingIndex]));
    GLCall(glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size * sizeof(float), (GLvoid*)data));
  } else {
    std::cout << "[ComputeShader Error] data download Index=" << BindingIndex << ", should be between: [0, " << bufId.size() - 1 << "]" << std::endl;
  }
}

void ComputeShader::realeaseData() {
  for (int BindingIndex = 0; BindingIndex < bufId.size(); BindingIndex++) {
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufId[BindingIndex]));
    GLCall(glDeleteBuffers(1, &bufId[BindingIndex]));
  }
  bufId.clear();
}

void ComputeShader::synchronize() {
  GLCall(glMemoryBarrier(GL_ALL_BARRIER_BITS));
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
