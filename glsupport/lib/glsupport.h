#pragma once

#include <glad/glad.h>
//只需要包括glad,因为shader类里不需要接手context

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Noncopyable {
 protected:
  Noncopyable() {}
  ~Noncopyable() {}

  Noncopyable(const Noncopyable&) = delete;
  Noncopyable& operator=(const Noncopyable&) = delete;
};
class ShaderProgram : public Noncopyable {
 public:
  // prrogram ID
  unsigned int id_;

  // read and link
  ShaderProgram(const char* vertexPath, const char* fragmentPath);
  // move and copy

  void use();

  // utility uniform functions

  void set_bool(const std::string& name, bool value) const;
  void set_int(const std::string& name, int value) const;
  void set_float(const std::string& name, float value) const;
  // transformation
  void set_vec2(const std::string& name, const GLfloat* value) const;
  void set_vec2(const std::string& name, float x, float y) const;

  void set_vec3(const std::string& name, const GLfloat* value) const;
  void set_vec3(const std::string& name, float x, float y, float z) const;

  void set_vec4(const std::string& name, const GLfloat* value) const;
  void set_vec4(const std::string& name, float x, float y, float z,
                float w) const;

  void set_mat2(const std::string& name, const GLfloat* value);
  void set_mat3(const std::string& name, const GLfloat* value);
  void set_mat4(const std::string& name, const GLfloat* value);

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

class GlTexture : public Noncopyable {
 public:
  // Texture ID
  unsigned int id_;
  GlTexture() { glGenTextures(1, &id_); }
  ~GlTexture() { glDeleteTextures(1, &id_); }
  operator GLuint() { return id_; }
};

class GlBufferObject : public Noncopyable {
 public:
  // BufferObject ID
  unsigned int id_;
  GlBufferObject() { glGenBuffers(1, &id_); }
  ~GlBufferObject() { glDeleteBuffers(1, &id_); }
  operator GLuint() { return id_; }
};
