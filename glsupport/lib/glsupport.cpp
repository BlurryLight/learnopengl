#define STB_IMAGE_IMPLEMENTATION
#include "glsupport.h"

#include <array>
unsigned int loadTextureFromFile(std::string, std::string);

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

void ShaderProgram::set_vec2(const std::string &name,
                             const GLfloat *value) const {
  glUniform2fv(glGetUniformLocation(id_, name.c_str()), 1, value);
}

void ShaderProgram::set_vec2(const std::string &name, float x, float y) const {
  glUniform2f(glGetUniformLocation(id_, name.c_str()), x, y);
}

void ShaderProgram::set_vec3(const std::string &name,
                             const GLfloat *value) const {
  glUniform3fv(glGetUniformLocation(id_, name.c_str()), 1, value);
}

void ShaderProgram::set_vec3(const std::string &name, float x, float y,
                             float z) const {
  glUniform3f(glGetUniformLocation(id_, name.c_str()), x, y, z);
}

void ShaderProgram::set_vec4(const std::string &name,
                             const GLfloat *value) const {
  glUniform4fv(glGetUniformLocation(id_, name.c_str()), 1, value);
}

void ShaderProgram::set_vec4(const std::string &name, float x, float y, float z,
                             float w) const {
  glUniform4f(glGetUniformLocation(id_, name.c_str()), x, y, z, w);
}

void ShaderProgram::set_mat2(const std::string &name, const GLfloat *value) {
  glUniformMatrix2fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE,
                     value);
}
void ShaderProgram::set_mat3(const std::string &name, const GLfloat *value) {
  glUniformMatrix3fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE,
                     value);
}
void ShaderProgram::set_mat4(const std::string &name, const GLfloat *value) {
  glUniformMatrix4fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE,
                     value);
}

void ShaderProgram::check_compile_error(unsigned int shader_handle,
                                        std::string type) {
  // type = "PROGRAM" or "VSHADER" or "FSHADER"
  int success;
  char buf[1024];

  if (type != "PROGRAM") {
    glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader_handle, 1024, NULL, buf);
      std::string infolog(buf);
      std::cerr << "FAILED TO COMPILE SHADER" << type << '\n'
                << infolog << std::endl;
    }
  } else {
    glGetProgramiv(shader_handle, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader_handle, 1024, NULL, buf);
      std::string infolog(buf);
      std::cerr << "FAILED TO LINK PROGRAM" << type << '\n'
                << infolog << std::endl;
    }
  }
}

void Mesh::Draw(ShaderProgram shader) {
  // numbers of textures
  unsigned int diffuseNum = 1;
  unsigned int specularNum = 1;
  unsigned int normalNum = 1;
  unsigned int heightNum = 1;

  for (unsigned int i = 0; i < textures_.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    std::string number;
    std::string name = textures_[i].type;
    if (name == "texture_diffuse")
      number = std::to_string(diffuseNum++);
    else if (name == "texture_specular")
      number = std::to_string(specularNum++);
    else if (name == "texture_normal")
      number = std::to_string(normalNum++);
    else if (name == "texture_height")
      number = std::to_string(heightNum++);
    else {
      std::cerr << "ERROR IN MESH DRAW: UNKNOWN TEXTURE " << name << std::endl;
      throw std::runtime_error("ERROR IN MESH DRAW: UNKNOWN TEXTURE" + name);
    }
    // bind sampler to diffrent texture
    // such as : bind texture_diffuse1 to texture0
    shader.set_int((name + number), i);
    // debug use
    //    std::cerr << name << number << std::endl;
    glBindTexture(GL_TEXTURE_2D, textures_[i].id);
  }

  // draw
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);

  // Remember OpenGL is a Status-Machine.
  // so it's a good habit to reset your setting to default after doing some work
  // to avoid weired behaviour
  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  // vbo
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex),
               &vertices_[0], GL_STATIC_DRAW);

  // ebo
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int),
               &indices_[0], GL_STATIC_DRAW);

  // set VAO

  // position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  // normal
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Normal));
  // texture
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, TexCoords));
  // tangent
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Tangent));
  // Bitangent
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Bitangent));
  // disbind VAO
  glBindVertexArray(0);
}

void Model::Draw(ShaderProgram shader) {
  for (int i = 0; i < meshes_.size(); i++)
    meshes_[i].Draw(shader);
}

void Model::loadModel(std::string path) {
  std::cerr << path << std::endl;
  Assimp::Importer importer;
  const aiScene *scene =
      importer.ReadFile(path, aiProcess_FlipUVs | aiProcess_Triangulate);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
    throw std::runtime_error("ERROR::ASSIMP::" +
                             std::string(importer.GetErrorString()));
    return;
  }
  this->directory = path.substr(0, path.find_last_of('/')); // linux only?

  processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    meshes_.push_back(processMesh(mesh, scene));
  }
  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene);
  }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;
  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;
    glm::vec3 pos;
    pos.x = mesh->mVertices[i].x;
    pos.y = mesh->mVertices[i].y;
    pos.z = mesh->mVertices[i].z;
    vertex.Position = std::move(pos);

    glm::vec3 normal;
    normal.x = mesh->mNormals[i].x;
    normal.y = mesh->mNormals[i].y;
    normal.z = mesh->mNormals[i].z;
    vertex.Normal = std::move(normal);

    if (mesh->mTextureCoords[0]) // mesh has texture coords
    {
      glm::vec2 texcoords;
      // a vertex can contain up to 8 different texture coordinates. We thus
      // make the assumption that we won't use models where a vertex can have
      // multiple texture coordinates so we always take the first set (0).

      texcoords.x = mesh->mTextureCoords[0][i].x;
      texcoords.y = mesh->mTextureCoords[0][i].y;
      vertex.TexCoords = std::move(texcoords);
    } else {
      vertex.TexCoords = glm::vec2(0.0f);
    }
    if (mesh->mTangents) {
      glm::vec3 tangent;
      tangent.x = mesh->mTangents[i].x;
      tangent.y = mesh->mTangents[i].y;
      tangent.z = mesh->mTangents[i].z;
      vertex.Tangent = std::move(tangent);
    }
    if (mesh->mBitangents) {
      glm::vec3 btangent;
      btangent.x = mesh->mBitangents[i].x;
      btangent.y = mesh->mBitangents[i].y;
      btangent.z = mesh->mBitangents[i].z;
      vertex.Bitangent = std::move(btangent);
    }
    vertices.push_back(vertex);
  }
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (unsigned j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }

  if (mesh->mMaterialIndex >= 0) {
    aiMaterial *materail = scene->mMaterials[mesh->mMaterialIndex];
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN
    std::vector<Texture> diffuseMaps = loadMaterailTextures(
        materail, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    std::vector<Texture> specularMaps = loadMaterailTextures(
        materail, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    std::vector<Texture> normalMaps = loadMaterailTextures(
        materail, aiTextureType_HEIGHT, "texture_normal"); // height?
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

    std::vector<Texture> heightMaps = loadMaterailTextures(
        materail, aiTextureType_AMBIENT, "texture_height"); // ambient?
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
  }

  return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterailTextures(aiMaterial *mat,
                                                 aiTextureType type,
                                                 std::string typeName) {
  std::vector<Texture> textures;
  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
    aiString path;
    mat->GetTexture(type, i, &path);
    bool skip = false;
    for (unsigned j = 0; j < texture_loaded_.size(); j++) {
      if (std::strcmp(texture_loaded_[j].path.c_str(), path.C_Str()) == 0) {
        textures.push_back(texture_loaded_[j]);
        skip = true;
        break;
      }
    }
    if (!skip) {
      Texture texture;
      // for debug use
      //      std::cerr << "path: " << path.C_Str() << "directory " <<
      //      this->directory
      //                << std::endl;
      texture.id = loadTextureFromFile(path.C_Str(), this->directory);
      texture.type = typeName;
      texture.path = path.C_Str();
      textures.push_back(texture);
      texture_loaded_.push_back(texture);
    }
  }
  return textures;
}

unsigned int loadTextureFromFile(std::string name, std::string directory) {

  std::string filename = directory + '/' + name;

  // debug use
  //  std::cerr << "name: " << name << "directory " << directory << "filename "
  //            << filename << std::endl;
  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  unsigned char *data =
      stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
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
    std::cerr << "ERROR WHEN LOADING TEXTURE" << std::endl;
    throw std::runtime_error("ERROR WHEN LOADING TEXTURE");
    stbi_image_free(data);
  }
  return textureID;
}

unsigned int loadHDRTexture(const char *path) {
  stbi_set_flip_vertically_on_load(true);
  int width, height, nrComponents;
  float *data = stbi_loadf(path, &width, &height, &nrComponents, 0);
  unsigned int hdrTexture;
  if (data) {
    glGenTextures(1, &hdrTexture);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB,
                 GL_FLOAT, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    std::cerr << "failed to load HDR image" << std::endl;
  }
  return hdrTexture;
}
