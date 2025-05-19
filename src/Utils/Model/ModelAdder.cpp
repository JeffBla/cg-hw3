#include "ModelAdder.hpp"

#include "Model/TextureFactory.hpp"
#include "OpenGL/OpenGLException.hpp"
#include "Utils/Compilers.hpp"
#include "Utils/Global.hpp"
#include "Utils/StringFormat/StringFormat.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/mat4x4.hpp"

#include "tiny_obj_loader.h"

#include <vector>

bool ModuleAdder::loadModel(const char * modelSource, const char * textureSource,
                            OpenGL::OpenGLShaderProgram & program, 
                            std::vector<std::shared_ptr<Model::Mesh>>& models, 
                            std::vector<std::unique_ptr<OpenGL::OpenGLTexture>>& textures)
{
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string errorMessage;

    auto success =
        tinyobj::LoadObj(shapes, materials, errorMessage, modelSource);

    if (!success) {
        std::cerr << "[Error]" << errorMessage.c_str();

        return false;
    }

    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> textureCoordinates;
    std::vector<unsigned int> indices;

    for (auto& shape : shapes) {
        positions.insert(positions.end(), shape.mesh.positions.begin(),
                         shape.mesh.positions.end());
        if (!shape.mesh.normals.empty()) {
            normals.insert(normals.end(), shape.mesh.normals.begin(),
                           shape.mesh.normals.end());
        }

        if (!shape.mesh.texcoords.empty()) {
            textureCoordinates.insert(textureCoordinates.end(),
                                      shape.mesh.texcoords.begin(),
                                      shape.mesh.texcoords.end());
        }

        indices.insert(indices.end(), shape.mesh.indices.begin(),
                       shape.mesh.indices.end());
    }

    std::unique_ptr<OpenGL::OpenGLTexture> texture;
    std::unique_ptr<Model::Mesh> mesh;

    if (textureSource) {
        texture = Model::TextureFactory::loadFromFile(textureSource);
        mesh.reset(new Model::Mesh{positions, normals, textureCoordinates,
                                   indices, program, texture.get()});

        textures.push_back(std::move(texture));
    } else {
        mesh.reset(new Model::Mesh{positions, normals, textureCoordinates,
                                   indices, program});
    }

    models.push_back(std::move(mesh));
    return true;
}
