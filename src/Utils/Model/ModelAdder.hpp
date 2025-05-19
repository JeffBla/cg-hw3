#include "Model/Mesh.hpp"
#include "OpenGL/OpenGLShaderProgram.hpp"
#include "OpenGL/OpenGLTexture.hpp"

class ModuleAdder {
   public:
      static bool loadModel(const char *modelSource, const char *textureSource,
                          OpenGL::OpenGLShaderProgram &program, std::vector<std::shared_ptr<Model::Mesh>> &models,
                          std::vector<std::unique_ptr<OpenGL::OpenGLTexture>> &textures);
};