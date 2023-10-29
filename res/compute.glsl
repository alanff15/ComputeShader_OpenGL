static const std::string shaderProgStr = R"glsl(
#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(binding = 0) buffer readwrite { float buf[]; };

void main() {
  const uint idx = gl_GlobalInvocationID.x;
  buf[idx] *= 2.0;
}

)glsl";