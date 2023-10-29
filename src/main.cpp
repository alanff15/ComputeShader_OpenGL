#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer/Renderer.h"

#include "../../res/compute.glsl"

#include <fstream>
#include <string>
std::string parseShader(const std::string& filepath) {
  std::ifstream stream(filepath);
  std::string line, ret = "";
  while (getline(stream, line)) ret += line + "\n";
  return ret;
}

void initGL(GLFWwindow*& window, int width = 1, int height = 1, const char* title = "", bool windowVisible = false) {
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

int main() {
  GLFWwindow* window;
  initGL(window);

  // compilar shader
  // std::string shaderProgStr = parseShader("../../res/compute.glsl");
  const char* src = shaderProgStr.c_str();
  GLCall(uint32_t cs = glCreateShader(GL_COMPUTE_SHADER));
  GLCall(glShaderSource(cs, 1, &src, NULL));
  GLCall(glCompileShader(cs));
  // linkar programa
  GLCall(uint32_t computeProgram = glCreateProgram());
  GLCall(glAttachShader(computeProgram, cs));
  GLCall(glLinkProgram(computeProgram));
  GLCall(glValidateProgram(computeProgram));
  // limpar memoria
  GLCall(glDeleteShader(cs));

  // entrada
  constexpr int size = 10;
  float data[size] = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
  for (int i = 0; i < size; i++) std::cout << data[i] << (i == size - 1 ? "\n" : " ") << std::ends;  // mostrar
  // enviar dados para gpu
  GLuint bufId;
  GLCall(glGenBuffers(1, &bufId));
  GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufId));
  GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, size * sizeof(float), (GLvoid*)data, GL_DYNAMIC_COPY));
  GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, bufId));

  // executar shader
  GLCall(glUseProgram(computeProgram));
  GLCall(glDispatchCompute(size, 1, 1));         // cria workgroups apenas em x
  GLCall(glMemoryBarrier(GL_ALL_BARRIER_BITS));  // sincronizar

  // ler dados da gpu
  GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufId));
  GLCall(glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size * sizeof(float), (GLvoid*)data));
  // saida
  for (int i = 0; i < size; i++) std::cout << data[i] << (i == size - 1 ? "\n" : " ") << std::ends;  // mostrar

  GLCall(glDeleteProgram(computeProgram));
  glfwTerminate();
  return EXIT_SUCCESS;
}