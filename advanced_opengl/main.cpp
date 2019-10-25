

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

int main() {
  auto window = init_gl_context(SRC_WIDTH, SRC_HEIGHT, "advanced_gl");
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);
  ShaderProgram linkedShader("tri.vert", "simple.frag");
  ShaderProgram outerShader("tri.vert", "lame.frag");
  linkedShader.use();
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

  // clang-format off
   float cubeVertices[] = {
        // positions          // texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
       float planeVertices[] = {
        // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f								
    };
    float transparentVertices[] = {
        // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
        1.0f,  0.5f,  0.0f,  1.0f,  0.0f
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
       glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                             (void *)0);
       glEnableVertexAttribArray(1);
       glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                             (void *)(3 * sizeof(float)));
       glBindVertexArray(0);
       // plane
       unsigned int planeVAO, planeVBO;
       glGenVertexArrays(1, &planeVAO);
       glGenBuffers(1, &planeVBO);
       glBindVertexArray(planeVAO);
       glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
       glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices,
                    GL_STATIC_DRAW);
       glEnableVertexAttribArray(0);
       glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                             (void *)0);
       glEnableVertexAttribArray(1);
       glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                             (void *)(3 * sizeof(float)));
       glBindVertexArray(0);
       // transparent
       unsigned int transparentVAO, transparentVBO;
       glGenVertexArrays(1, &transparentVAO);
       glGenBuffers(1, &transparentVBO);
       glBindVertexArray(transparentVAO);
       glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
       glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices),
                    transparentVertices, GL_STATIC_DRAW);
       glEnableVertexAttribArray(0);
       glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                             (void *)0);
       glEnableVertexAttribArray(1);
       glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                             (void *)(3 * sizeof(float)));
       glBindVertexArray(0);

       // texture

       unsigned int cubeTexture = loadTexture("marble.jpg");
       unsigned int floorTexture = loadTexture("metal.png");
       unsigned int windowTexture = loadTexture("transparent_window.png");

       linkedShader.use();
       linkedShader.set_int("texture1", 0);

       // transparent window locations
       // --------------------------------
       std::vector<glm::vec3> windows{
           glm::vec3(-1.5f, 0.0f, -0.48f), glm::vec3(1.5f, 0.0f, 0.51f),
           glm::vec3(0.0f, 0.0f, 0.7f), glm::vec3(-0.3f, 0.0f, -2.3f),
           glm::vec3(0.5f, 0.0f, -0.6f)};

       glEnable(GL_BLEND);
       glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
       while (!glfwWindowShouldClose(window)) {
         float currentFrame = glfwGetTime();
         deltaTime = currentFrame - lastFrame;
         lastFrame = currentFrame;
         std::map<float, glm::vec3> sorted;
         for (unsigned int i = 0; i < windows.size(); i++) {
           float distance = glm::length(camera.camera_position - windows[i]);
           sorted[distance] = windows[i];
         }

         processInput(window);

         glClearColor(0.1, 0.1, 0.1, 1.0);
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
                 GL_STENCIL_BUFFER_BIT);

         // draw CUBE
         linkedShader.use();

         glm::mat4 projection = glm::perspective(
             glm::radians(camera.zoom), (float)SRC_WIDTH / (float)SRC_HEIGHT,
             0.1f, 100.0f);
         glm::mat4 view = camera.getViewMatrix();
         linkedShader.set_mat4("p", glm::value_ptr(projection));
         linkedShader.set_mat4("v", glm::value_ptr(view));
         outerShader.use();
         outerShader.set_mat4("p", glm::value_ptr(projection));
         outerShader.set_mat4("v", glm::value_ptr(view));
         // cube
         linkedShader.use();
         glStencilFunc(GL_ALWAYS, 1, 0xFF);
         glStencilMask(0xFF);

         glBindVertexArray(cubeVAO);
         glActiveTexture(GL_TEXTURE0);
         glBindTexture(GL_TEXTURE_2D, cubeTexture);
         glm::mat4 model = glm::mat4(1.0f);
         model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
         linkedShader.set_mat4("m", glm::value_ptr(model));

         glDrawArrays(GL_TRIANGLES, 0, 36);
         model = glm::mat4(1.0f);

         model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
         linkedShader.set_mat4("m", glm::value_ptr(model));
         glDrawArrays(GL_TRIANGLES, 0, 36);

         // floor
         glStencilMask(0x00);
         glBindVertexArray(planeVAO);
         glBindTexture(GL_TEXTURE_2D, floorTexture);
         linkedShader.set_mat4("m", glm::value_ptr(glm::mat4(1.0f)));
         glDrawArrays(GL_TRIANGLES, 0, 6);
         glBindVertexArray(0);

         // outer ring
         glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
         glStencilMask(0x00);
         glDisable(GL_DEPTH_TEST);
         outerShader.use();
         float scale = 1.1f;
         glBindVertexArray(cubeVAO);
         glActiveTexture(GL_TEXTURE0);
         glBindTexture(GL_TEXTURE_2D, cubeTexture);
         model = glm::mat4(1.0f);
         model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
         model = glm::scale(model, glm::vec3(scale));
         outerShader.set_mat4("m", glm::value_ptr(model));
         glDrawArrays(GL_TRIANGLES, 0, 36);
         model = glm::mat4(1.0f);
         model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
         model = glm::scale(model, glm::vec3(scale));
         outerShader.set_mat4("m", glm::value_ptr(model));
         glDrawArrays(GL_TRIANGLES, 0, 36);
         glStencilMask(0xFF);
         glBindVertexArray(0);
         glEnable(GL_DEPTH_TEST);

         // transparent
         linkedShader.use();
         glBindVertexArray(transparentVAO);
         glBindTexture(GL_TEXTURE_2D, windowTexture);
         for (auto it = sorted.rbegin(); it != sorted.rend(); ++it) {
           model = glm::mat4(1.0f);
           model = glm::translate(model, it->second);
           linkedShader.set_mat4("m", glm::value_ptr(model));
           glDrawArrays(GL_TRIANGLES, 0, 6);
         }

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
