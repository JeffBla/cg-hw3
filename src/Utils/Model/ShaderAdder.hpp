#include "Model/Mesh.hpp"
#include "OpenGL/OpenGLShaderProgram.hpp"
#include "OpenGL/OpenGLTexture.hpp"

#include <vector>
#include <memory>

class ShaderAdder {
    private:
        static bool compileShaders(OpenGL::OpenGLShaderProgram &program,
                                    const char *vertexShaderFile,
                                    const char *fragmentShaderFile = nullptr,
                                    const char *geometryShaderFile = nullptr);

   public:
      static OpenGL::OpenGLShaderProgram *addShader(const char *vertexShaderSource,
                                                    const char *fragmentShaderSource,
                                                    const char *geometryShaderSource,
                                                    std::vector<std::unique_ptr<OpenGL::OpenGLShaderProgram>> &shaders);
};