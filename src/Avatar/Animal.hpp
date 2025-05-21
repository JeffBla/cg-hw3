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
    public:
        Animal();
        ~Animal();

        void toggleForm(); // Start transformation between human and pig
        void updateTransformation(float deltaTime); 
        bool isTransforming() const { return transformationProgress_ > 0.0f && transformationProgress_ < 1.0f; }

        void draw(glm::mat4 &view, glm::mat4 &projection);
        void setPosition(const glm::vec3 &position);
        void setRotation(const glm::vec3 &rotation);
        void setScale(const glm::vec3 &scale);

        std::shared_ptr<Joint> getRootJoint() const {
            return rootJoint_;
        }
    private:
        std::vector<std::shared_ptr<Model::Mesh>> models_;
        std::vector<std::unique_ptr<OpenGL::OpenGLTexture>> textures_;
        std::vector<std::unique_ptr<OpenGL::OpenGLShaderProgram>> shaders_;

        std::string vertexShader = "Shader/BasicVertexShader.vs.glsl";
        std::string fragmentShader = "Shader/BasicFragmentShader.fs.glsl";
        std::string texturePath = "resources/texture/uv.png";
        std::string pigTexturePath = "resources/texture/uv.png";

        std::string cubeModelPath = "resources/model/cube.obj";

        // Root joint of the skeleton
        std::shared_ptr<Joint> rootJoint_;  

        std::shared_ptr<Joint> humanRootJoint_;  
        std::shared_ptr<Joint> pigRootJoint_;  
        
        bool isHumanForm_;
        float transformationProgress_; // 0.0f = source form, 1.0f = target form

        void create();
        std::shared_ptr<Model::Mesh> createBodyPartModel(
            const std::string& name,
            const glm::vec3& size,
            const std::string& texturePath);
        std::shared_ptr<Joint> createBoneHierarchy();
        std::shared_ptr<Joint> createPigBoneHierarchy();
    
        void drawTransformation(glm::mat4 &view, glm::mat4 &projection);
        void drawTransformingJoint(std::shared_ptr<Joint> sourceJoint, 
                                    std::shared_ptr<Joint> targetJoint,
                                    glm::mat4 parentTransform, 
                                    glm::mat4 &view, 
                                    glm::mat4 &projection,
                                    float progress);
};

class Joint 
{
    public:
        Joint(const std::string& name, const glm::vec3& offset, 
            std::shared_ptr<Model::Mesh> model);
        ~Joint();
        
        void addChild(std::shared_ptr<Joint> child);
        Joint* getChild(size_t index);
        std::shared_ptr<Joint> getChildPtr(size_t index);
        size_t getChildCount() const;

        std::vector<std::shared_ptr<Joint>> getChildren() const {
            return children_;
        }

        glm::mat4 getLocalTransform() const;
        
        void draw(glm::mat4 parentTransform, glm::mat4 &view, glm::mat4 &projection);

        // Set the local rotation of this joint
        void setRotation(const glm::vec3& rotation);
        inline glm::vec3 &getRotation() {
            return rotation_;
        }
        inline glm::vec3 &getOffset() {
            return offset_;
        }
        inline void setSize(glm::vec3 size) {
            size_ = size;
        }
        inline glm::vec3 getSize() const{
            return size_;
        }
        inline std::shared_ptr<Model::Mesh> getModel() const{
            return model_;
        }
        inline std::string getName() const {
            return name_;
        }
    private:
        std::string name_;               
        glm::vec3 offset_;                  // Offset from parent joint
        glm::vec3 size_;                    
        glm::vec3 rotation_;                // Local rotation
        std::vector<std::shared_ptr<Joint>> children_;
        std::shared_ptr<Model::Mesh> model_;  
};

#endif // ANIMAL_HPP