
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

#include <chrono>
#include <filesystem>

constexpr static GLuint SRC_WIDTH = 800;
constexpr static GLuint SRC_HEIGHT = 600;

void mouse_callback(GLFWwindow *window, double xpos, double ypos);
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

void framebuffer_size_callback(GLFWwindow *, int width, int height) {
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

// render a sphere
unsigned int sphereVAO = 0;
unsigned int indexCount;
void renderSphere();

int main() {
  auto window = local_init_gl_context(SRC_WIDTH, SRC_HEIGHT, "brdf");
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  glEnable(GL_DEPTH_TEST);
  ShaderProgram shader("brdf.vert", "brdf.frag");
  shader.use();
  shader.set_vec3("albedo", 0.5f, 0.3f, 0.1f);
  shader.set_float("ao", 1.0f);

  shader.set_int("albedoMap", 0);
  shader.set_int("metalllicMap", 1);
  shader.set_int("roughnessMap", 2);
  // texture
  unsigned int albedo = loadTexture("rustediron2_basecolor.png");
  unsigned int metallic = loadTexture("rustediron2_metallic.png");
  unsigned int roughness = loadTexture("rustediron2_roughness.png");
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, albedo);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, metallic);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, roughness);

  // lights
  glm::vec3 lightPosition[] = {
      glm::vec3(-10.0f, 10.0f, 10.0f), glm::vec3(10.0f, 10.0f, 10.0f),
      glm::vec3(-10.0f, -10.0f, 10.0f), glm::vec3(10.0f, -10.0f, 10.0f)
      // z = 10, xy plane

  };
  glm::vec3 lightColors[] = {
      glm::vec3(300.0f, 300.0f, 300.0f), glm::vec3(300.0f, 300.0f, 300.0f),
      glm::vec3(300.0f, 300.0f, 300.0f), glm::vec3(300.0f, 300.0f, 300.0f)};
  int fps_count = 0;

  int nrRows = 7;
  int nrCols = 7;
  float spacing = 2.5;

  // loop
  std::chrono::high_resolution_clock::time_point start =
      std::chrono::high_resolution_clock::now();
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection =
        glm::perspective(glm::radians(camera.zoom),
                         (float)SRC_WIDTH / (float)SRC_HEIGHT, 0.1f, 100.0f);
    auto view = camera.getViewMatrix();
    shader.set_mat4("projection", glm::value_ptr(projection));
    shader.set_mat4("view", glm::value_ptr(view));
    shader.set_vec3("camPos", glm::value_ptr(camera.camera_position));
    auto model = glm::mat4(1.0f);
    for (int row = 0; row < nrRows; ++row) {
      shader.set_float("metallic", (float)row / (float)nrRows);
      for (int col = 0; col < nrCols; ++col) {
        shader.set_float("roughness",
                         glm::clamp((float)col / (float)nrCols, 0.05f, 0.95f));
        model = glm::mat4(1.0f);
        model = glm::translate(model,
                               glm::vec3(col - (nrCols / 2) * spacing,
                                         row - (nrRows / 2) * spacing, 0.0f));
        model = glm::scale(model, glm::vec3(0.25));
        shader.set_mat4("model", glm::value_ptr(model));
        renderSphere();
      }
    }
    // render light
    for (int i = 0; i < 4; i++) {
      // moving in X direction
      // glm::vec3 newPos = lightPosition[i] + glm::vec3(sin(glfwGetTime()
      // * 5.0) * 5.0,0.0,0.0);
      glm::vec3 newPos = lightPosition[i];
      shader.set_vec3("lightPosition[" + std::to_string(i) + "]",
                      glm::value_ptr(newPos));
      shader.set_vec3("lightColors[" + std::to_string(i) + "]",
                      glm::value_ptr(lightColors[i]));
      model = glm::mat4(1.0f);
      model = glm::translate(model, newPos);
      model = glm::scale(model, glm::vec3(0.1f));
      shader.set_mat4("model", glm::value_ptr(model));
      renderSphere();
    }
    glfwSwapBuffers(window);
    glfwPollEvents();
    fps_count++;
    if (fps_count == 100) {
      std::chrono::high_resolution_clock::time_point end =
          std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> time_span = end - start;
      start = std::chrono::high_resolution_clock::now();
      fps_count = 0;
      std::cout << "100 Frames costs " << time_span.count() << " seconds"
                << "\nFps is " << 100 / time_span.count() << std::endl;
    }
  }
  glfwTerminate();
  return 0;
}
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset =
      lastY - ypos; // reversed since y-coordinates go from bottom to top

  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}
void renderSphere() {
  if (sphereVAO == 0) {
    glGenVertexArrays(1, &sphereVAO);
    unsigned int vbo, ebo;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    std::vector<glm::vec3> positions; // x y z
    std::vector<glm::vec2> uv;        // textures
    std::vector<glm::vec3> normals;   // nx ny nz
    std::vector<unsigned int> indices;
    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;
    const float PI = 3.14159265359;
    for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
      for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
        float xSegment = (float)x / (float)X_SEGMENTS;
        float ySegment = (float)y / (float)Y_SEGMENTS;

        float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
        float yPos = std::cos(ySegment * PI);
        float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
        positions.emplace_back(xPos, yPos, zPos);
        uv.emplace_back(xSegment, ySegment);
        normals.emplace_back(xPos, yPos, zPos);
      }
    }
    bool oddRow = false;
    for (int y = 0; y < Y_SEGMENTS; ++y) {
      if (!oddRow) // even rows: y == 0, y == 2; and so on
      {
        for (int x = 0; x <= X_SEGMENTS; ++x) {
          indices.push_back(y * (X_SEGMENTS + 1) + x);
          indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
        }
      } else {
        for (int x = X_SEGMENTS; x >= 0; --x) {
          indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
          indices.push_back(y * (X_SEGMENTS + 1) + x);
        }
      }
      oddRow = !oddRow;
    }
    indexCount = indices.size();

    std::vector<float> data;
    for (int i = 0; i < positions.size(); ++i) {
      data.push_back(positions[i].x);
      data.push_back(positions[i].y);
      data.push_back(positions[i].z);
      if (uv.size() > 0) {
        data.push_back(uv[i].x);
        data.push_back(uv[i].y);
      }
      if (normals.size() > 0) {
        data.push_back(normals[i].x);
        data.push_back(normals[i].y);
        data.push_back(normals[i].z);
      }
    }

    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(),
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);
    float stride = (3 + 2 + 3) * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
                          (void *)(5 * sizeof(float)));
  }
  glBindVertexArray(sphereVAO);
  glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
