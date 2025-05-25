#include <iostream>
#include "ComputeShader.h"

#define ASSERT(x) \
  if (!(x)) __debugbreak();

#define GLCall(x) \
  GLClearError(); \
  x;              \
  ASSERT(GLLogCall(#x, __FILE__, __LINE__))

ComputeShader::ComputeShader(GLFWwindow* extWindow) : makeCurrentEnable(false) {
  // abrir contexto opengl
  window = extWindow;
  if (window == NULL) initGL(window);
}

ComputeShader::~ComputeShader() {
  // liberar memoria de dados
  realeaseData();
  // apagar programas da gpu
  realeaseKernels();
  // fechar contexto opengl
  glfwTerminate();
}

void ComputeShader::makeCurrent(bool enable) {
  makeCurrentEnable = enable;
}

void ComputeShader::addKernel(std::string Program, uint32_t ProgramIndex) {
  if (makeCurrentEnable) glfwMakeContextCurrent(window);
  // compilar shader
  const char* src = Program.c_str();
  GLCall(uint32_t cs = glCreateShader(GL_COMPUTE_SHADER));
  GLCall(glShaderSource(cs, 1, &src, NULL));
  GLCall(glCompileShader(cs));

  // checar warning/erro de compialcao
  GLint status;
  GLCall(glGetShaderiv(cs, GL_COMPILE_STATUS, &status));
  GLint len;
  GLCall(glGetShaderiv(cs, GL_INFO_LOG_LENGTH, &len));
  if (len > 0) {
    std::vector<char> log(len);
    GLCall(glGetShaderInfoLog(cs, len, nullptr, log.data()));
    std::cerr << "[ComputeShader Compile]: program Index = " << ProgramIndex << "\n" << log.data() << std::endl;
  }
  if (status == GL_FALSE) {
    glDeleteShader(cs);
    return;
  }

  if (ProgramIndex == programId.size()) {
    programId.push_back(-1);
  } else {
    GLCall(glDeleteProgram(programId[ProgramIndex]));
  }
  if (ProgramIndex >= 0 && ProgramIndex < programId.size()) {
    // linkar programa
    GLCall(programId[ProgramIndex] = glCreateProgram());
    GLCall(glAttachShader(programId[ProgramIndex], cs));
    GLCall(glLinkProgram(programId[ProgramIndex]));

    // checar warning/erro de linkagem
    GLint status;
    glGetProgramiv(programId[ProgramIndex], GL_LINK_STATUS, &status);
    GLint len;
    glGetProgramiv(programId[ProgramIndex], GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
      std::vector<char> log(len);
      glGetProgramInfoLog(programId[ProgramIndex], len, nullptr, log.data());
      std::cerr << "[ComputeShader Link]: program Index = " << ProgramIndex << "\n" << log.data() << std::endl;
    }
    if (status == GL_FALSE) {
      glDeleteShader(cs);
      glDeleteProgram(programId[ProgramIndex]);
      return;
    }

    GLCall(glValidateProgram(programId[ProgramIndex]));
  } else {
    std::cout << "[ComputeShader Error] data program Index=" << ProgramIndex << ", next value should be: " << programId.size() << std::endl;
  }
  // limpar memoria
  GLCall(glDeleteShader(cs));
}

void ComputeShader::reserveMemory(size_t size, uint32_t BindingIndex) {
  if (makeCurrentEnable) glfwMakeContextCurrent(window);
  if (BindingIndex == bufId.size()) {
    bufId.push_back(-1);
    GLCall(glGenBuffers(1, &bufId[BindingIndex]));
  }
  if (BindingIndex >= 0 && BindingIndex < bufId.size()) {
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufId[BindingIndex]));
    GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_COPY));
    GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BindingIndex, bufId[BindingIndex]));
  } else {
    std::cout << "[ComputeShader Error] data upload Index=" << BindingIndex << ", next value should be: " << bufId.size() << std::endl;
  }
}

void ComputeShader::uploadData(void* data, size_t offset, size_t size, uint32_t BindingIndex) {
  if (makeCurrentEnable) glfwMakeContextCurrent(window);
  if (BindingIndex == bufId.size()) {
    bufId.push_back(-1);
    GLCall(glGenBuffers(1, &bufId[BindingIndex]));
  }
  if (BindingIndex >= 0 && BindingIndex < bufId.size()) {
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufId[BindingIndex]));
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);  // GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_COPY));
    GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BindingIndex, bufId[BindingIndex]));
  } else {
    std::cout << "[ComputeShader Error] data upload Index=" << BindingIndex << ", next value should be: " << bufId.size() << std::endl;
  }
}

void ComputeShader::compute(uint32_t ProgramIndex, uint32_t sizeX, uint32_t sizeY, uint32_t sizeZ) {
  if (makeCurrentEnable) glfwMakeContextCurrent(window);
  if (ProgramIndex >= 0 && ProgramIndex < programId.size()) {
    GLCall(glUseProgram(programId[ProgramIndex]));
    GLCall(glDispatchCompute(sizeX, sizeY, sizeZ));  // criar workgroups
  } else {
    std::cout << "[ComputeShader Error] data download Index=" << ProgramIndex << ", should be between: [0, " << programId.size() - 1 << "]" << std::endl;
  }
}

void ComputeShader::downloadData(void* data, size_t offset, size_t size, uint32_t BindingIndex) {
  if (makeCurrentEnable) glfwMakeContextCurrent(window);
  if (BindingIndex >= 0 && BindingIndex < bufId.size()) {
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufId[BindingIndex]));
    GLCall(glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, (GLvoid*)data));
  } else {
    std::cout << "[ComputeShader Error] data download Index=" << BindingIndex << ", should be between: [0, " << bufId.size() - 1 << "]" << std::endl;
  }
}

void ComputeShader::realeaseData() {
  if (makeCurrentEnable) glfwMakeContextCurrent(window);
  for (int BindingIndex = 0; BindingIndex < bufId.size(); BindingIndex++) {
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufId[BindingIndex]));
    GLCall(glDeleteBuffers(1, &bufId[BindingIndex]));
  }
  bufId.clear();
}

void ComputeShader::realeaseKernels() {
  if (makeCurrentEnable) glfwMakeContextCurrent(window);
  for (int ProgramIndex = 0; ProgramIndex < programId.size(); ProgramIndex++) {
    GLCall(glDeleteProgram(programId[ProgramIndex]));
  }
  programId.clear();
}

void ComputeShader::synchronize(GLuint barriers) {
  if (makeCurrentEnable) glfwMakeContextCurrent(window);
  GLCall(glMemoryBarrier(barriers));
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
  while (glGetError() != GL_NO_ERROR);
}

bool ComputeShader::GLLogCall(const char* function, const char* file, int line) {
  while (GLenum error = glGetError()) {
    std::cout << "[OpenGL Error] (" << error << "): " << function << " " << file << ":" << line << std::endl;
    return false;
  }
  return true;
}
