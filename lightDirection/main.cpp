

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

bool blinn = true;
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

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
    blinn = !blinn;
    std::cerr << (blinn ? "blinn" : "non-blinn") << std::endl;
  }
}

void framebuffer_size_callback(GLFWwindow *, int width, int height) {
  glViewport(0, 0, width, height);
}

int main() {
  auto window = init_gl_context(SRC_WIDTH, SRC_HEIGHT, "phoning");
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetKeyCallback(window, key_callback);
  ShaderProgram linkedShader("tri.vert", "simple.frag");
  //  ShaderProgram linkedShader("lame.vert", "lame.frag");
  linkedShader.use();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // clang-format off
 float planeVertices[] = {
  // positions            // normals         // texcoords
  10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
      -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
      -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
      
      10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
      -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
      10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
};
  // clang-format on

  unsigned int planeVBO, planeVAO;
  glGenBuffers(1, &planeVBO);
  glGenVertexArrays(1, &planeVAO);
  glBindVertexArray(planeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  glm::vec3 lightPos = glm::vec3(0.0f);
  unsigned int floorTexture = loadTexture("wood.png");

  linkedShader.use();
  linkedShader.set_int("floorTexture", 0);

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    linkedShader.use();

    glm::mat4 projection =
        glm::perspective(glm::radians(camera.zoom),
                         (float)SRC_WIDTH / (float)SRC_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.getViewMatrix();
    linkedShader.set_mat4("p", glm::value_ptr(projection));
    linkedShader.set_mat4("v", glm::value_ptr(view));

    linkedShader.set_vec3("lightPos", glm::value_ptr(lightPos));
    linkedShader.set_vec3("viewPos", glm::value_ptr(camera.camera_position));
    linkedShader.set_int("blinn", blinn);

    glBindVertexArray(planeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);

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
