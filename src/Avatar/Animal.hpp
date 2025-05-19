#ifndef ANIMAL_HPP
#define ANIMAL_HPP

#include "OpenGL/OpenGLShaderProgram.hpp"
#include "OpenGL/OpenGLTexture.hpp"
#include "Model/Mesh.hpp"

#include <vector>
#include <string>
#include <memory>

class Joint;

class Animal
{
    private:
        std::vector<std::shared_ptr<Model::Mesh>> models_;
        std::vector<std::unique_ptr<OpenGL::OpenGLTexture>> textures_;
        std::vector<std::unique_ptr<OpenGL::OpenGLShaderProgram>> shaders_;

        std::string vertexShader = "Shader/BasicVertexShader.vs.glsl";
        std::string fragmentShader = "Shader/BasicFragmentShader.fs.glsl";
        std::string texturePath = "resources/texture/uv.png";

        std::string cubeModelPath = "resources/model/cube.obj";

        // Root joint of the skeleton
        std::shared_ptr<Joint> rootJoint_;  
        
        void create();
        std::shared_ptr<Model::Mesh> createBodyPartModel(
            const std::string& name,
            const glm::vec3& size,
            const std::string& texturePath);
        std::shared_ptr<Joint> createBoneHierarchy();
    public:
        Animal();
        ~Animal();

        void draw(glm::mat4 &view, glm::mat4 &projection);
        void setPosition(const glm::vec3 &position);
        void setRotation(const glm::vec3 &rotation);
        void setScale(const glm::vec3 &scale);

};

class Joint {
public:
    Joint(const std::string& name, const glm::vec3& offset, 
          std::shared_ptr<Model::Mesh> model);
    ~Joint();
    
    void addChild(std::shared_ptr<Joint> child);
    
    Joint* getChild(size_t index);

    glm::mat4 getLocalTransform() const;

    // Set the local rotation of this joint
    void setRotation(const glm::vec3& rotation);
    
    void draw(glm::mat4 parentTransform, glm::mat4 &view, glm::mat4 &projection);

private:
    std::string name_;                  // Name of the joint
    glm::vec3 offset_;                  // Offset from parent joint
    glm::vec3 size_;                    // Size of the cube for this joint
    glm::vec3 rotation_;                // Local rotation
    std::vector<std::shared_ptr<Joint>> children_; // Child joints
    std::shared_ptr<Model::Mesh> model_;   // Visual model
};

#endif // ANIMAL_HPP