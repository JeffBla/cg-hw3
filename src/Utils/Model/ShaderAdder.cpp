#include "ShaderAdder.hpp"

bool ShaderAdder::compileShaders(OpenGL::OpenGLShaderProgram & program,
                                const char * vertexShaderFile,
                                const char * fragmentShaderFile,
                                const char * geometryShaderFile){
    if (!program.addShaderFromFile(OpenGL::OpenGLShader::Type::Vertex,
                                   vertexShaderFile))
    {
        return false;
    }

    if (fragmentShaderFile &&

        !program.addShaderFromFile(OpenGL::OpenGLShader::Type::Fragment,
                                   fragmentShaderFile))
    {
        return false;
    }

    if (geometryShaderFile &&
        !program.addShaderFromFile(OpenGL::OpenGLShader::Type::Geometry,
                                   geometryShaderFile))
    {
        return false;
    }

    program.link();
    if (!program.linkStatus())
    {
        return false;
    }

    return true;
}

OpenGL::OpenGLShaderProgram *ShaderAdder::addShader(const char * vertexShaderSource,
                                                    const char * fragmentShaderSource,
                                                    const char *geometryShaderSource,
                                                    std::vector<std::unique_ptr<OpenGL::OpenGLShaderProgram>>& shaders)
{
    std::unique_ptr<OpenGL::OpenGLShaderProgram> program{new OpenGL::OpenGLShaderProgram{}};
    if (!compileShaders(*program, vertexShaderSource,
                        fragmentShaderSource, nullptr))
    {
        return nullptr;
    }
    shaders.push_back(std::move(program));

    return shaders.back().get();
}

