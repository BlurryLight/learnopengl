

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

void processInput(GLFWwindow* window) {
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

void framebuffer_size_callback(GLFWwindow*, int width, int height) {
  glViewport(0, 0, width, height);
}

GLFWwindow* init_gl() {
  if (!glfwInit()) {
    std::cerr << "GLFW INIT FATAL ERROR" << std::endl;
    exit(-1);
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window =
      glfwCreateWindow(SRC_WIDTH, SRC_HEIGHT, "LEARN OPENGL", NULL, NULL);
  if (window == NULL) {
    std::cerr << "CREATE WINDOW FATAL ERROR" << std::endl;
    exit(-1);
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // glad loads
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "GLAD INIT FATAL ERROR" << std::endl;
    exit(-1);
  }
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);
  return window;
}

int main() {
  auto window = init_gl();
  ShaderProgram linkedShader("tri.vert", "simple.frag");
  linkedShader.use();

  ShaderProgram lameShader("tri.vert", "lame.frag");
  lameShader.use();

  glEnable(GL_DEPTH_TEST);

  float vertices[] = {
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

  unsigned int VBO, cubeVAO;
  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(cubeVAO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  unsigned int lightVAO;
  glGenVertexArrays(1, &lightVAO);
  glBindVertexArray(lightVAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw CUBE
    linkedShader.use();
    linkedShader.set_vec3("lightColor",
                          glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
    linkedShader.set_vec3("objectColor",
                          glm::value_ptr(glm::vec3(1.0, 0.5, 0.31)));

    glm::mat4 projection =
        glm::perspective(glm::radians(camera.zoom),
                         (float)SRC_WIDTH / (float)SRC_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.getViewMatrix();
    linkedShader.set_mat4("p", glm::value_ptr(projection));
    linkedShader.set_mat4("v", glm::value_ptr(view));
    glm::mat4 model = glm::mat4(1.0f);
    linkedShader.set_mat4("m", glm::value_ptr(model));

    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // draw lame
    lameShader.use();

    lameShader.set_mat4("p", glm::value_ptr(projection));
    lameShader.set_mat4("v", glm::value_ptr(view));
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    glm::mat4 model_lame = glm::mat4(1.0f);
    model_lame = glm::translate(model_lame, lightPos);
    model_lame = glm::scale(model_lame, glm::vec3(0.2f));
    lameShader.set_mat4("m", glm::value_ptr(model_lame));

    glBindVertexArray(lightVAO);
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
