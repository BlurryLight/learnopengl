#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <stb_image.h>

// assimp
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

// clang-format on
class Noncopyable {
protected:
  Noncopyable() {}
  ~Noncopyable() {}

  Noncopyable(const Noncopyable&) = delete;
  Noncopyable& operator=(const Noncopyable&) = delete;
};
class ShaderProgram {
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

class GlTexture {
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

// Default camera values
constexpr float YAW = -90.0f;
constexpr float PITCH = 0.0f;
constexpr float SPEED = 0.5f;
constexpr float SENSITIVITY = 0.1f;
constexpr float ZOOM = 45.0f;

enum class CameraMovement { kFORWARD, kBACKWARD, kLEFT, kRIGHT };

class Camera {
public:
  glm::vec3 camera_position;
  glm::vec3 camera_front;
  glm::vec3 world_up;
  glm::vec3 camera_right;
  glm::vec3 camera_up;

  // Angles
  float yaw;
  float pitch;
  float roll;
  // traits
  float movement_speed;
  float mouse_sensitivity;
  float zoom;

  Camera(glm::vec3 position = glm::vec3(0.0, 0.0, 0.0),
         glm::vec3 up = glm::vec3(0.0, 0.1, 0.0), float yaw_ = YAW,
         float pitch_ = PITCH)
      : camera_front(glm::vec3(0.0, 0.0, -1.0f)),
        camera_position(position),
        world_up(up),
        pitch(pitch_),
        yaw(yaw_),
        movement_speed(SPEED),
        mouse_sensitivity(SENSITIVITY),
        zoom(ZOOM) {
    updateCameraVectors();
  }

  Camera(float Xpos, float Ypos, float Zpos, float upX, float upY, float upZ,
         float yaw_, float pitch_)
      : camera_front(glm::vec3(0.0, 0.0, -1.0f)),
        camera_position(glm::vec3(Xpos, Ypos, Zpos)),
        world_up(glm::vec3(upX, upY, upZ)),
        pitch(pitch_),
        yaw(yaw_),
        movement_speed(SPEED),
        mouse_sensitivity(SENSITIVITY),
        zoom(ZOOM) {
    updateCameraVectors();
  }

  glm::mat4 getViewMatrix() {
    return glm::lookAt(camera_position, camera_position + camera_front,
                       world_up);
  }

  void processKeyBoard(CameraMovement movement, float deltaTime) {
    float velocity = movement_speed + deltaTime;  // confusing name
    switch (movement) {
      case CameraMovement::kFORWARD: {
        this->camera_position += this->camera_front * velocity;
        break;
      }
      case CameraMovement::kBACKWARD: {
        this->camera_position -= this->camera_front * velocity;
        break;
      }
      case CameraMovement::kLEFT: {
        this->camera_position -= this->camera_right * velocity;
        break;
      }
      case CameraMovement::kRIGHT: {
        this->camera_position += this->camera_right * velocity;
        break;
      }
    }
  }

  void ProcessMouseMovement(float xoffset, float yoffset,
                            GLboolean constrainPitch = true) {
    xoffset *= mouse_sensitivity;
    yoffset *= mouse_sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
      if (pitch > 89.0f) pitch = 89.0f;
      if (pitch < -89.0f) pitch = -89.0f;
    }

    // Update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
  }

  // Processes input received from a mouse scroll-wheel event. Only requires
  // input on the vertical wheel-axis
  void ProcessMouseScroll(float yoffset) {
    if (zoom >= 1.0f && zoom <= 45.0f) zoom -= yoffset;
    if (zoom <= 1.0f) zoom = 1.0f;
    if (zoom >= 45.0f) zoom = 45.0f;
  }

 protected:
  void updateCameraVectors() {
    glm::vec3 front;
    front.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    front.y = glm::sin(glm::radians(pitch));
    front.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    this->camera_front = glm::normalize(front);

    // re-caculate camera up and camera right
    this->camera_right =
        glm::normalize(glm::cross(camera_front, this->world_up));
    this->camera_up = glm::normalize(glm::cross(camera_right, camera_front));
  }
};

struct Vertex {
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec2 TexCoords;
  glm::vec3 Tangent;   // u
  glm::vec3 Bitangent; // v
};

struct Texture {
  unsigned int id;
  std::string type;
  std::string path;
};

class Mesh {
public:
  std::vector<Vertex> vertices_;
  std::vector<unsigned int> indices_;
  std::vector<Texture> textures_;

  Mesh(std::vector<Vertex> vertics, std::vector<unsigned int> indices,
       std::vector<Texture> textures)
      : vertices_(vertics), indices_(indices), textures_(textures) {
    setupMesh();
  }
  void Draw(ShaderProgram shader);

private:
  unsigned VAO, VBO, EBO;
  void setupMesh();
};

class Model {
public:
  Model(std::string path) { loadModel(path); }
  void Draw(ShaderProgram shader);
  std::vector<Mesh> meshes_;
  std::vector<Texture> texture_loaded_;
  bool gammaCorrection; //?
  std::string directory;

private:
  void loadModel(std::string path);
  void processNode(aiNode *node, const aiScene *scene);
  Mesh processMesh(aiMesh *mesh, const aiScene *scene);
  std::vector<Texture> loadMaterailTextures(aiMaterial *mat, aiTextureType type,
                                            std::string typeName);
};

inline unsigned int loadTexture(const char *path) {

  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
  if (data) {
    GLenum format;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                    format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);
  } else {
    std::cerr << "ERROR WHEN LOADING TEXTURE" << std::endl;
    throw std::runtime_error("ERROR WHEN LOADING TEXTURE");
    stbi_image_free(data);
  }
  return textureID;
}
inline GLFWwindow *init_gl_context(int width, int height,
                                   const char *framename) {
  if (!glfwInit()) {
    std::cerr << "GLFW INIT FATAL ERROR" << std::endl;
    exit(-1);
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(width, height, framename, NULL, NULL);
  if (window == NULL) {
    std::cerr << "CREATE WINDOW FATAL ERROR" << std::endl;
    exit(-1);
  }

  glfwMakeContextCurrent(window);

  // glad loads
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "GLAD INIT FATAL ERROR" << std::endl;
    exit(-1);
  }
  return window;
}
