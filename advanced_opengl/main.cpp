

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <sstream>

// My dear math
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// user-defined
#include <glsupport.h>
#include <map>

#include <filesystem>

constexpr static GLuint SRC_WIDTH = 800;
constexpr static GLuint SRC_HEIGHT = 600;

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SRC_WIDTH / 2.0f;
float lastY = SRC_HEIGHT / 2.0f;
bool firstMouse = true;

float fov = 45.0f;
float lastFrame = 0.0;
float deltaTime = 0.0;

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.processKeyBoard(CameraMovement::kFORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.processKeyBoard(CameraMovement::kBACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.processKeyBoard(CameraMovement::kLEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.processKeyBoard(CameraMovement::kRIGHT, deltaTime);
}
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.ProcessMouseScroll(yoffset);
}

void framebuffer_size_callback(GLFWwindow*, int width, int height) {
  glViewport(0, 0, width, height);
}

inline GLFWwindow *local_init_gl_context(int width, int height,
                                         const char *framename) {
  if (!glfwInit()) {
    std::cerr << "GLFW INIT FATAL ERROR" << std::endl;
    exit(-1);
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);

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

int main() {
  auto window = local_init_gl_context(SRC_WIDTH, SRC_HEIGHT, "advanced_gl");
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);
  ShaderProgram redShader("tri.vert", "red.fs");
  ShaderProgram yellowShader("tri.vert", "yellow.fs");
  ShaderProgram blueShader("tri.vert", "blue.fs");
  ShaderProgram greenShader("tri.vert", "green.fs");
  glEnable(GL_MULTISAMPLE);

  glEnable(GL_DEPTH_TEST);
  // clang-format off
  float cubeVertices[] = {
      // positions
      -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f,
      0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,

      -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,
      0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,

      -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,
      -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,

      0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f,
      0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,

      -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,
      0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f,

      -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,
      0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f,
  };

  // clang-format on
  unsigned int cubeVAO, cubeVBO;
  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &cubeVBO);
  glBindVertexArray(cubeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glBindVertexArray(0);

  // texture

  unsigned int uniformBlockIndexRed =
      glGetUniformBlockIndex(redShader.id_, "Matrices");
  unsigned int uniformBlockIndexGreen =
      glGetUniformBlockIndex(greenShader.id_, "Matrices");
  unsigned int uniformBlockIndexBlue =
      glGetUniformBlockIndex(blueShader.id_, "Matrices");
  unsigned int uniformBlockIndexYellow =
      glGetUniformBlockIndex(yellowShader.id_, "Matrices");
  glUniformBlockBinding(redShader.id_, uniformBlockIndexRed, 0);
  glUniformBlockBinding(yellowShader.id_, uniformBlockIndexYellow, 0);
  glUniformBlockBinding(blueShader.id_, uniformBlockIndexBlue, 0);
  glUniformBlockBinding(greenShader.id_, uniformBlockIndexGreen, 0);

  unsigned int uboMatrices;
  glGenBuffers(1, &uboMatrices);
  // way 1
  glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
  glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  // way 2
  glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0,
                    2 * sizeof(glm::mat4));

  glm::mat4 projection =
      glm::perspective(glm::radians(camera.zoom),
                       (float)SRC_WIDTH / (float)SRC_HEIGHT, 0.1f, 100.0f);
  glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
                  glm::value_ptr(projection));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // draw CUBE

    glm::mat4 view = camera.getViewMatrix();
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
                    glm::value_ptr(view));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // cube

    glBindVertexArray(cubeVAO);
    redShader.use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-0.75f, 0.75f, 0.0f));
    redShader.set_mat4("m", glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    greenShader.use();
    model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(-0.75f, -0.75f, 0.0f));
    greenShader.set_mat4("m", glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    blueShader.use();
    model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(0.75f, -0.75f, 0.0f));
    blueShader.set_mat4("m", glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    yellowShader.use();
    model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(0.75f, 0.75f, 0.0f));
    yellowShader.set_mat4("m", glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset =
      lastY - ypos;  // reversed since y-coordinates go from bottom to top

  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}

unsigned int load_Texture(char const *path) {
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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }

  return textureID;
}
