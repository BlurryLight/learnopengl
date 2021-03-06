

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
static const GLuint SCR_WIDTH = 800;
static const GLuint SCR_HEIGHT = 600;
void framebuffer_size_callback(GLFWwindow*, int width, int height);
int main()
{
  if(!glfwInit())
  {
    std::cerr << "FATAL INIT FAILED"<<std::endl;
    return -1;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
  glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH,SCR_HEIGHT,"LEARNOPENGL",NULL,NULL);

  if(window == NULL)
  {
    std::cerr << "GLFW INIT FAILED"<<std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);

  //glad loads
  if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cerr << "FAILED TO INIT GLAD"<<std::endl;
    return -1;
  }

  //v shader
  int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  std::fstream fs;
  std::string vshader;
  fs.open("tri.vshader");
  if(fs.good())
  {
    std::stringstream ss;
    ss << fs.rdbuf();
    vshader = ss.str();
  }
  else
  {
    std::cerr << "FAILED TO INIT VSHADER"<<std::endl;
    return -1;
  }
  fs.close();

  std::cout<<vshader<<std::endl;
  const char* vshader_source = vshader.c_str();
  glShaderSource(vertexShader,1,&vshader_source,NULL);
  glCompileShader(vertexShader);

  int success = 0;
  char infoLog[512];
  glGetShaderiv(vertexShader,GL_COMPILE_STATUS,&success);
  if(!success)
  {
    glGetShaderInfoLog(vertexShader,512,NULL,infoLog);
    std::cerr << "FAILED TO COMPILE VSHADER"<<infoLog<<std::endl;
    return -1;
  }

  GLuint fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  std::string fshader;
  fs.open("simple.fshader");
  if(fs.good())
  {
    std::stringstream ss;
    ss << fs.rdbuf();
    fshader = ss.str();
  }
  else
  {
    std::cerr << "FAILED TO INIT fSHADER"<<fs.is_open()<<std::endl;
    return -1;
  }

  fs.close();

  std::cout<<fshader<<std::endl;
  std::cout<<fshader<<std::endl;
  const char* fshader_source = fshader.c_str();
  glShaderSource(fragmentShader,1,&fshader_source,NULL);
  glCompileShader(fragmentShader);

  GLint succ;
  glGetShaderiv(fragmentShader,GL_COMPILE_STATUS,&succ);
  if(!succ)
  {
    glGetShaderInfoLog(fragmentShader,512,NULL,infoLog);
     printf("OpenGL version is (%s)\n", glGetString(GL_VERSION));
    std::cerr << "FAILED TO COMPILE FSHADER"<<infoLog<<std::endl;
    std::cout<<infoLog<<std::endl;
    return -1;
  }

  int linkedShader;
  linkedShader = glCreateProgram();
  glAttachShader(linkedShader,vertexShader);
  glAttachShader(linkedShader,fragmentShader);
  glLinkProgram(linkedShader);

  success = 0;
  glGetProgramiv(linkedShader,GL_LINK_STATUS,&success);
  if(!success)
  {
    glGetProgramInfoLog(linkedShader,512,NULL,infoLog);
    std::cerr << "FAILED TO LINK SHADERS"<<infoLog<<std::endl;
    return -1;
  }

  glUseProgram(linkedShader);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);


// data
  float vertices[] = {
    -0.5f,-0.5f,0.0f,
    0.5f,-0.5f,0.0f,
    0.0f,0.5f,0.0f
  };

  unsigned int VBO,VAO;

  glGenVertexArrays(1,&VAO);
  glGenBuffers(1,&VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER,VBO);
  glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER,0);

  glBindVertexArray(0);

  while(!glfwWindowShouldClose(window))
  {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    glClearColor(0.2f,0.3f,0.3f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(linkedShader);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES,0,3);

    glfwSwapBuffers(window);
    glfwPollEvents();

  }

  glfwTerminate();
  return 0;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}