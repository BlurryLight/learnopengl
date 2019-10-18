#pragma once

#include "glad/glad.h"
//只需要包括glad,因为shader类里不需要接手context

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class ShaderProgram {
 public:
  // prrogram ID
  unsigned int id_;

  // read and link
  ShaderProgram(const char *vertexPath, const char *fragmentPath);

  void use();

  // utility uniform functions

  void set_bool(const std::string &name, bool value) const;
  void set_int(const std::string &name, int value) const;
  void set_float(const std::string &name, float value) const;

  operator GLuint() { return id_; }
  ~ShaderProgram() {
    glDeleteShader(vshader_handle_);
    glDeleteShader(fshader_handle_);
    glDeleteProgram(id_);
  }

 private:
  unsigned int vshader_handle_;
  unsigned int fshader_handle_;

  void check_compile_error(unsigned int shader_handle, std::string type);
};
