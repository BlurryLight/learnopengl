#include "glsupport.h"

#include <array>

ShaderProgram::ShaderProgram(const char *vertexPath, const char *fragmentPath) {
  std::string vshader;
  std::string fshader;

  std::fstream ifs;

  auto readFile = [&ifs](const char *path) -> std::string {
    ifs.open(path);
    if (ifs.good()) {
      std::stringstream ss;
      ss << ifs.rdbuf();
      ifs.close();
      return ss.str();
    } else {
      ifs.close();
      throw std::runtime_error(std::string("ERROR WHEN READ") + path);
    }
  };

  vshader = readFile(vertexPath);
  fshader = readFile(fragmentPath);

  auto vshader_ptr = vshader.c_str();
  auto fshader_ptr = fshader.c_str();

  vshader_handle_ = glCreateShader(GL_VERTEX_SHADER);

  glShaderSource(vshader_handle_, 1, &vshader_ptr, NULL);
  glCompileShader(vshader_handle_);
  check_compile_error(vshader_handle_, "VSHADER");

  fshader_handle_ = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fshader_handle_, 1, &fshader_ptr, NULL);
  glCompileShader(fshader_handle_);
  check_compile_error(fshader_handle_, "FSHADER");

  //
  this->id_ = glCreateProgram();
  glAttachShader(this->id_, vshader_handle_);
  glAttachShader(this->id_, fshader_handle_);
  glLinkProgram(this->id_);
  check_compile_error(this->id_, "PROGRAM");
}

void ShaderProgram::use() { glUseProgram(id_); }

void ShaderProgram::set_int(const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(id_, name.c_str()), value);
}
void ShaderProgram::set_bool(const std::string &name, bool value) const {
  glUniform1i(glGetUniformLocation(id_, name.c_str()), static_cast<int>(value));
}

void ShaderProgram::set_float(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(id_, name.c_str()), value);
}

void ShaderProgram::check_compile_error(unsigned int shader_handle,
                                        std::string type) {
  // type = "PROGRAM" or "VSHADER" or "FSHADER"
  int success;
  std::string infolog;
  infolog.reserve(1024);

  if (type != "PROGRAM") {
    glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader_handle, 1024, NULL, &infolog[0]);
      std::cerr << "FAILED TO COMPILE SHADER" << type << '\n'
                << infolog << std::endl;
    }
  } else {
    glGetProgramiv(shader_handle, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader_handle, 1024, NULL, &infolog[0]);
      std::cerr << "FAILED TO LINK PROGRAM" << type << '\n'
                << infolog << std::endl;
    }
  }
}
