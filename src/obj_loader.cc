// Heavily adapted from
// https://github.com/assimp/assimp/blob/master/samples/SimpleTexturedOpenGL/SimpleTexturedOpenGL/src/model_loading.cpp

#include "obj_loader.hpp"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image/stb_image.h"
#endif

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../lib/stb_image/stb_image_write.h"
#endif

#include <fstream>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

static std::string modelpath = "./test/models/tree/Tree.obj";

GLfloat LightAmbient[] = {0.5f, 0.5f, 0.5f, 1.0f};
GLfloat LightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat LightPosition[] = {0.0f, 0.0f, 15.0f, 1.0f};

// the global Assimp scene object
const aiScene *g_scene = nullptr;
GLuint scene_list = 0;
aiVector3D scene_min, scene_max, scene_center;

// images / texture

std::map<std::string, GLuint *>
    diffuseTextureIdMap; // map image filenames to textureIds
std::map<std::string, GLuint *> specularTextureIdMap;
GLuint *textureIds; // pointer to texture Array

// Create an instance of the Importer class
Assimp::Importer importer;

GLfloat xrot;
GLfloat yrot;
GLfloat zrot;

std::string vertexShaderCode;
std::string fragmentShaderCode;
GLuint shaderProgram;

GLint modelMatrixLoc;
GLint viewMatrixLo;
GLint projectionMatrixLoc;
GLint diffuseLoc;
GLint specularLoc;
GLint ambientLoc;
GLint shininessLoc;
GLint textureLoc;

std::string getBasePath(const std::string &path) {
  size_t pos = path.find_last_of("\\/");
  return (std::string::npos == pos) ? "" : path.substr(0, pos + 1);
}

void freeTextureIds() {
  // no need to delete pointers in it manually here. (Pointers point to
  // textureIds deleted in next step)
  diffuseTextureIdMap.clear();

  if (textureIds) {
    delete[] textureIds;
    textureIds = nullptr;
  }
}

int LoadGLTextures(const aiScene *scene) {
  freeTextureIds();

  if (scene->HasTextures())
    return 1;

  /* getTexture Filenames and Numb of Textures */
  for (unsigned int m = 0; m < scene->mNumMaterials; m++) {
    int texIndex = 0;
    aiReturn texFound = AI_SUCCESS;

    aiString path; // filename

    while (texFound == AI_SUCCESS) {
      texFound = scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE,
                                                  texIndex, &path);
      diffuseTextureIdMap[path.data] =
          nullptr; // fill map with textures, pointers still NULL yet
      texIndex++;
    }

    texFound = AI_SUCCESS;
    texIndex = 0;
    while (texFound == AI_SUCCESS) {
      texFound = scene->mMaterials[m]->GetTexture(aiTextureType_SPECULAR,
                                                  texIndex, &path);
      specularTextureIdMap[path.data] =
          nullptr; // fill map with textures, pointers still NULL yet
      std::cout << "Got specular texture: " << path.data << "\n";
      texIndex++;
    }
  }

  size_t numTextures = diffuseTextureIdMap.size();
  std::cout << "Got diffuse texture count: " << numTextures << "\n";

  /* create and fill array with GL texture ids */
  textureIds = new GLuint[numTextures];
  glGenTextures(static_cast<GLsizei>(numTextures),
                textureIds); /* Texture name generation */

  /* get iterator */
  std::map<std::string, GLuint *>::iterator itr = diffuseTextureIdMap.begin();

  std::string basepath = getBasePath(modelpath);
  for (size_t i = 0; i < numTextures; i++) {
    std::string filename = (*itr).first; // get filename
    (*itr).second = &textureIds[i];      // save texture id for filename in map
    itr++;                               // next texture

    std::string fileloc = basepath + filename; /* Loading of image */
    int x, y, n;
    unsigned char *data =
        stbi_load(fileloc.c_str(), &x, &y, &n, STBI_rgb_alpha);

    if (nullptr != data) {
      std::cout << "Loaded texture file " << fileloc << " \n";
      // Binding of texture name
      glBindTexture(GL_TEXTURE_2D, textureIds[i]);
      // redefine standard texture values
      // We will use linear interpolation for magnification filter
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      // We will use linear interpolation for minifying filter
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      // Texture specification
      glTexImage2D(GL_TEXTURE_2D, 0, n, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                   data); // Texture specification.

      // we also want to be able to deal with odd texture dimensions
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
      glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
      glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
      stbi_image_free(data);

      std::cout << "texture count: " << numTextures << "\n";

    } else {
      std::cerr << "Couldnt load image " << fileloc << "\n";
      return -1;
    }
  }

  // do same, but for specular textures

  numTextures = specularTextureIdMap.size();
  std::cout << "Got specular texture count: " << numTextures << "\n";

  /* create and fill array with GL texture ids */
  textureIds = new GLuint[numTextures];
  glGenTextures(static_cast<GLsizei>(numTextures),
                textureIds); /* Texture name generation */

  itr = specularTextureIdMap.begin();

  for (size_t i = 0; i < numTextures; i++) {
    std::string filename = (*itr).first; // get filename
    (*itr).second = &textureIds[i];      // save texture id for filename in map
    itr++;                               // next texture

    std::string fileloc = basepath + filename; /* Loading of image */
    int x, y, n;
    unsigned char *data =
        stbi_load(fileloc.c_str(), &x, &y, &n, STBI_rgb_alpha);

    if (nullptr != data) {
      std::cout << "Loaded texture file " << fileloc << " \n";
      // Binding of texture name
      glBindTexture(GL_TEXTURE_2D, textureIds[i]);
      // redefine standard texture values
      // We will use linear interpolation for magnification filter
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      // We will use linear interpolation for minifying filter
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      // Texture specification
      glTexImage2D(GL_TEXTURE_2D, 0, n, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                   data); // Texture specification.

      // we also want to be able to deal with odd texture dimensions
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
      glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
      glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
      stbi_image_free(data);
    } else {
      std::cerr << "Couldnt load image " << fileloc << "\n";
      return -1;
    }
  }

  return true;
}

void Color4f(const aiColor4D *color) {
  glColor4f(color->r, color->g, color->b, color->a);
}

void set_float4(float f[4], float a, float b, float c, float d) {
  f[0] = a;
  f[1] = b;
  f[2] = c;
  f[3] = d;
}

void color4_to_float4(const aiColor4D *c, float f[4]) {
  f[0] = c->r;
  f[1] = c->g;
  f[2] = c->b;
  f[3] = c->a;
}

void apply_material(const aiMaterial *mtl) {
  float c[4];

  GLenum fill_mode;
  int ret1, ret2;
  aiColor4D diffuse;
  aiColor4D specular;
  aiColor4D ambient;
  aiColor4D emission;
  ai_real shininess, strength;
  int two_sided;
  int wireframe;
  unsigned int max; // changed: to unsigned

  int texIndex = 0;
  aiString texPath; // contains filename of texture

  if (AI_SUCCESS ==
      mtl->GetTexture(aiTextureType_DIFFUSE, texIndex, &texPath)) {

    // glUniform3fv(diffuseLoc, 1, glm::value_ptr(diffuse));
    // glUniform3fv(specularLoc, 1, glm::value_ptr(specular));
    // glUniform3fv(ambientLoc, 1, glm::value_ptr(ambient));
    glUniform1f(shininessLoc, shininess);

    glActiveTexture(GL_TEXTURE0);
    // bind texture
    unsigned int texId = *diffuseTextureIdMap[texPath.data];
    glBindTexture(GL_TEXTURE_2D, texId);
  }
  // sepcular support is not enough from here
  // this segfaults
  if (AI_SUCCESS ==
      mtl->GetTexture(aiTextureType_SPECULAR, texIndex, &texPath)) {
    // bind texture
    // std::cout << "got texture \n";
    unsigned int texId = *specularTextureIdMap[texPath.data];
    glBindTexture(GL_TEXTURE_2D, texId);
  }

  set_float4(c, 0.8f, 0.8f, 0.8f, 1.0f);
  if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
    color4_to_float4(&diffuse, c);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c);

  set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
  if (AI_SUCCESS ==
      aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular))
    color4_to_float4(&specular, c);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);

  // this is the part that makes the textures black
  // set_float4(c, 0.2f, 0.2f, 0.2f, 1.0f);
  // if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT,
  // &ambient))
  //     color4_to_float4(&ambient, c);
  // glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, c);

  set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
  if (AI_SUCCESS ==
      aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission))
    color4_to_float4(&emission, c);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, c);

  max = 1;
  ret1 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max);
  max = 1;
  ret2 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS_STRENGTH, &strength,
                                 &max);
  if ((ret1 == AI_SUCCESS) && (ret2 == AI_SUCCESS))
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess * strength);
  else {
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
    set_float4(c, 0.0f, 0.0f, 0.0f, 0.0f);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);
  }

  max = 1;
  if (AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_ENABLE_WIREFRAME,
                                              &wireframe, &max))
    fill_mode = wireframe ? GL_LINE : GL_FILL;
  else
    fill_mode = GL_FILL;
  glPolygonMode(GL_FRONT_AND_BACK, fill_mode);

  max = 1;
  if ((AI_SUCCESS ==
       aiGetMaterialIntegerArray(mtl, AI_MATKEY_TWOSIDED, &two_sided, &max)) &&
      two_sided)
    glEnable(GL_CULL_FACE);
  else
    glDisable(GL_CULL_FACE);
}

void recursive_render(const struct aiScene *sc, const struct aiNode *nd,
                      float scale) {
  unsigned int i;
  unsigned int n = 0, t;
  aiMatrix4x4 m = nd->mTransformation;

  aiMatrix4x4 m2;
  aiMatrix4x4::Scaling(aiVector3D(scale, scale, scale), m2);
  m = m * m2;

  // update transform
  m.Transpose();
  glPushMatrix();
  glMultMatrixf((float *)&m);

  // draw all meshes assigned to this node

  // std::cout << "Mesh count: " << nd->mNumMeshes << " \n";
  // std::cout << "Meshese: " << sc->mMeshes[0] << " \n";

  for (; n < nd->mNumMeshes; n++) {
    const struct aiMesh *mesh = sc->mMeshes[nd->mMeshes[n]];

    apply_material(sc->mMaterials[mesh->mMaterialIndex]);

    glEnable(GL_LIGHTING);
    // if (mesh->mNormals == nullptr)
    // {
    //     glDisable(GL_LIGHTING);
    // }
    // else
    // {
    //     glEnable(GL_LIGHTING);
    // }

    glEnable(GL_COLOR_MATERIAL);
    // if (mesh->mColors[0] != nullptr)
    // {
    //     glEnable(GL_COLOR_MATERIAL);
    // }
    // else
    // {
    //     glDisable(GL_COLOR_MATERIAL);
    // }

    // std::cout << "Face count: " << mesh->mNumFaces << " \n";
    for (t = 0; t < mesh->mNumFaces; ++t) {
      const struct aiFace *face = &mesh->mFaces[t];
      GLenum face_mode;

      switch (face->mNumIndices) {
      case 1:
        face_mode = GL_POINTS;
        break;
      case 2:
        face_mode = GL_LINES;
        break;
      case 3:
        face_mode = GL_TRIANGLES;
        break;
      default:
        face_mode = GL_POLYGON;
        break;
      }

      glBegin(face_mode);

      for (i = 0; i < face->mNumIndices; i++) // go through all vertices in face
      {
        int vertexIndex =
            face->mIndices[i]; // get group index for current index
        if (mesh->mColors[0] != nullptr)
          Color4f(&mesh->mColors[0][vertexIndex]);
        if (mesh->mNormals != nullptr)

          if (mesh->HasTextureCoords(
                  0)) // HasTextureCoords(texture_coordinates_set)
          {
            glTexCoord2f(mesh->mTextureCoords[0][vertexIndex].x,
                         1 - mesh->mTextureCoords[0][vertexIndex]
                                 .y); // mTextureCoords[channel][vertex]
          }

        glNormal3fv(&mesh->mNormals[vertexIndex].x);
        glVertex3fv(&mesh->mVertices[vertexIndex].x);
      }
      glEnd();
    }
  }

  // draw all children
  for (n = 0; n < nd->mNumChildren; ++n) {
    recursive_render(sc, nd->mChildren[n], scale);
  }

  glPopMatrix();
}

bool load_scene(const std::string &filename) {
  g_scene =
      importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs |
                                      aiProcess_ValidateDataStructure |
                                      aiProcessPreset_TargetRealtime_Quality);

  if (g_scene == nullptr) {
    std::cerr << "Error: " << importer.GetErrorString() << std::endl;
    return false;
  }

  return true;
}

GLuint createShaderProgram(const std::string &vertexShaderCode,
                           const std::string &fragmentShaderCode) {
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  const char *vertexShaderCodePtr = vertexShaderCode.c_str();
  glShaderSource(vertexShader, 1, &vertexShaderCodePtr, nullptr);
  glCompileShader(vertexShader);

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  const char *fragmentShaderCodePtr = fragmentShaderCode.c_str();
  glShaderSource(fragmentShader, 1, &fragmentShaderCodePtr, nullptr);
  glCompileShader(fragmentShader);

  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  // glDeleteShader(vertexShader);
  // glDeleteShader(fragmentShader);

  return shaderProgram;
}

std::string readShaderFromFile(const std::string &filename) {
  std::ifstream file(filename);
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

int loader_init() {
  if (!load_scene(modelpath)) {
    std::cerr << "Failed to load object file" << std::endl;
    return -1;
  }

  glEnable(GL_TEXTURE_2D);

  if (!LoadGLTextures(g_scene)) {
    std::cerr << "Failed to load textures" << std::endl;
    return -1;
  }

  std::string vertexShaderCode = readShaderFromFile("vertex_shader.glsl");
  std::string fragmentShaderCode = readShaderFromFile("fragment_shader.glsl");
  GLuint shaderProgram =
      createShaderProgram(vertexShaderCode, fragmentShaderCode);
  glUseProgram(shaderProgram);

  GLint modelMatrixLoc = glGetUniformLocation(shaderProgram, "modelMatrix");
  GLint viewMatrixLoc = glGetUniformLocation(shaderProgram, "viewMatrix");
  GLint projectionMatrixLoc =
      glGetUniformLocation(shaderProgram, "projectionMatrix");
  GLint diffuseLoc = glGetUniformLocation(shaderProgram, "material.diffuse");
  GLint specularLoc = glGetUniformLocation(shaderProgram, "material.specular");
  GLint ambientLoc = glGetUniformLocation(shaderProgram, "material.ambient");
  GLint shininessLoc =
      glGetUniformLocation(shaderProgram, "material.shininess");
  GLint textureLoc = glGetUniformLocation(shaderProgram, "textureMap");

  glShadeModel(GL_SMOOTH); // Enables Smooth Shading
  glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
  glClearDepth(1.0f);      // Depth Buffer Setup
  glEnable(GL_DEPTH_TEST); // Enables Depth Testing
  glDepthFunc(GL_LEQUAL);  // The Type Of Depth Test To Do
  glHint(GL_PERSPECTIVE_CORRECTION_HINT,
         GL_NICEST); // Really Nice Perspective Calculation

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0); // Uses default lighting parameters
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  glEnable(GL_NORMALIZE);

  glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
  glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
  glEnable(GL_LIGHT1);

  glEnable(GL_TEXTURE_2D);

  return 0;
}

int loader_loop() {

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT |
          GL_DEPTH_BUFFER_BIT); // Clear The Screen And The Depth Buffer

  // Set up projection matrix
  float aspectRatio =
      (float)1000 / (float)800; // Assuming window size of 800x600
  float fov = 90.0f;
  float nearPlane = 0.1f;
  float farPlane = 100.0f;

  float projectionMatrix[16];
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(fov, aspectRatio, nearPlane, farPlane);
  glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);
  // glUniformMatrix4fv(projectionMatrixLoc, 1, GL_FALSE, projectionMatrix);

  // Set up view matrix
  float viewMatrix[16];
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0.0f, 0.0f, 0.0f,  // Camera position
            0.0f, 0.0f, 0.0f,  // Look-at point
            0.0f, 1.0f, 0.0f); // Up vector
  // gluLookAt(10.0f, -400.0f, -40.0f, // Camera position
  //           0.0f, 0.0f, 0.0f,       // Look-at point
  //           0.0f, 1.0f, 0.0f);      // Up vector
  glGetFloatv(GL_MODELVIEW_MATRIX, viewMatrix);
  // glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, viewMatrix);

  // Set up model matrix
  float modelMatrix[16];
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0f, 0.0f, -1.0f); // Move the object along the -z axis
  glRotatef(45.0f, 0.0f, 1.0f,
            0.0f); // Rotate the object by 45 degrees around the y-axis
  glGetFloatv(GL_MODELVIEW_MATRIX, modelMatrix);
  // glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, modelMatrix);

  // glTranslatef(0.0f, 5.0f, 5.0f); // Move 40 Units And Into The Scree
  // glRotatef(xrot, 1.0f, 0.0f, 0.0f);
  // glRotatef(yrot, 0.0f, 1.0f, 0.0f);
  // glRotatef(zrot, 0.0f, 0.0f, 1.0f);

  recursive_render(g_scene, g_scene->mRootNode, 0.5);
}

// int maino() {
//   if (!glfwInit()) {
//     std::cerr << "Failed to initialize GLFW" << std::endl;
//     return -1;
//   }

//   GLFWwindow *window =
//       glfwCreateWindow(1000, 800, "OBJ Viewer", nullptr, nullptr);
//   if (!window) {
//     std::cerr << "Failed to create GLFW window" << std::endl;
//     glfwTerminate();
//     return -1;
//   }

//   glfwMakeContextCurrent(window);

//   if (glewInit() != GLEW_OK) {
//     std::cerr << "Failed to initialize GLEW" << std::endl;
//     glfwTerminate();
//     return -1;
//   }

//   if (!load_scene(modelpath)) {
//     std::cerr << "Failed to load object file" << std::endl;
//     return -1;
//   }

//   // loader_init() ;

//   while (!glfwWindowShouldClose(window)) {

//     // loader_loop();

//     // yrot += 0.2f;

//     glfwSwapBuffers(window);
//     glfwPollEvents();
//   }
//   // glDeleteProgram(shaderProgram);

//   glfwTerminate();
//   return 0;
// }
