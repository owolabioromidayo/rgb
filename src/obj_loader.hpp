#ifndef OBJ_LOADER_HPP_
#define OBJ_LOADER_HPP_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

std::string getBasePath(const std::string &path);

void freeTextureIds();
int LoadGLTextures(const aiScene *scene);
void recursive_render(const struct aiScene *sc, const struct aiNode *nd,
                      float scale);

int loader_init();
int loader_loop();

#endif