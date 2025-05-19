#include "OpenGL/OpenGLException.hpp"
#include "Avatar/Animal.hpp"
#include "Utils/Model/ModelAdder.hpp"
#include "Utils/Model/ShaderAdder.hpp"
#include "Utils/StringFormat/StringFormat.hpp"
#include "Animal.hpp"

Animal::Animal(){
    create();
}

Animal::~Animal(){
    models_.clear();
    textures_.clear();
    shaders_.clear();
}

void Animal::create(){
    ShaderAdder::addShader(vertexShader.c_str(), fragmentShader.c_str(), nullptr, shaders_);
   
    rootJoint_ = createBoneHierarchy();
}

void Animal::draw(glm::mat4 &view, glm::mat4 &projection){
    if (rootJoint_) {
        rootJoint_->draw(glm::mat4(1.0f), view, projection);
    }
}

std::shared_ptr<Joint> Animal::createBoneHierarchy() {
    // Create individual models for each body part with different sizes and textures
    
    // Create torso (root)
    auto torsoModel = createBodyPartModel("torso", glm::vec3(1.0f, 1.5f, 0.5f), texturePath);
    auto torso = std::make_unique<Joint>("torso", glm::vec3(0.0f, 0.0f, 0.0f), torsoModel);
    
    // Create head
    auto headModel = createBodyPartModel("head", glm::vec3(0.75f, 0.75f, 0.75f), texturePath);
    auto head = std::make_unique<Joint>("head", glm::vec3(0.0f, 1.0f, 0.0f), headModel);
    
    // Create left arm parts
    auto leftShoulderModel = createBodyPartModel("leftShoulder", glm::vec3(0.3f, 0.3f, 0.3f), texturePath);
    auto leftShoulder = std::make_unique<Joint>("leftShoulder", glm::vec3(-0.65f, 0.5f, 0.0f), leftShoulderModel);
    
    auto leftArmModel = createBodyPartModel("leftArm", glm::vec3(0.25f, 0.8f, 0.25f), texturePath);
    auto leftArm = std::make_unique<Joint>("leftArm", glm::vec3(0.0f, -0.5f, 0.0f), leftArmModel);
    
    auto leftHandModel = createBodyPartModel("leftHand", glm::vec3(0.3f, 0.3f, 0.3f), texturePath);
    auto leftHand = std::make_unique<Joint>("leftHand", glm::vec3(0.0f, -0.5f, 0.0f), leftHandModel);
    
    // Create right arm parts
    auto rightShoulderModel = createBodyPartModel("rightShoulder", glm::vec3(0.3f, 0.3f, 0.3f), texturePath);
    auto rightShoulder = std::make_unique<Joint>("rightShoulder", glm::vec3(0.65f, 0.5f, 0.0f), rightShoulderModel);
    
    auto rightArmModel = createBodyPartModel("rightArm", glm::vec3(0.25f, 0.8f, 0.25f), texturePath);
    auto rightArm = std::make_unique<Joint>("rightArm", glm::vec3(0.0f, -0.5f, 0.0f), rightArmModel);
    
    auto rightHandModel = createBodyPartModel("rightHand", glm::vec3(0.3f, 0.3f, 0.3f), texturePath);
    auto rightHand = std::make_unique<Joint>("rightHand", glm::vec3(0.0f, -0.5f, 0.0f), rightHandModel);
    
    // Create left leg parts
    auto leftHipModel = createBodyPartModel("leftHip", glm::vec3(0.3f, 0.3f, 0.3f), texturePath);
    auto leftHip = std::make_unique<Joint>("leftHip", glm::vec3(-0.4f, -0.9f, 0.0f), leftHipModel);
    
    auto leftLegModel = createBodyPartModel("leftLeg", glm::vec3(0.25f, 1.0f, 0.25f), texturePath);
    auto leftLeg = std::make_unique<Joint>("leftLeg", glm::vec3(0.0f, -0.6f, 0.0f), leftLegModel);
    
    auto leftFootModel = createBodyPartModel("leftFoot", glm::vec3(0.3f, 0.2f, 0.5f), texturePath);
    auto leftFoot = std::make_unique<Joint>("leftFoot", glm::vec3(0.0f, -0.6f, 0.1f), leftFootModel);
    
    // Create right leg parts
    auto rightHipModel = createBodyPartModel("rightHip", glm::vec3(0.3f, 0.3f, 0.3f), texturePath);
    auto rightHip = std::make_unique<Joint>("rightHip", glm::vec3(0.4f, -0.9f, 0.0f), rightHipModel);
    
    auto rightLegModel = createBodyPartModel("rightLeg", glm::vec3(0.25f, 1.0f, 0.25f), texturePath);
    auto rightLeg = std::make_unique<Joint>("rightLeg", glm::vec3(0.0f, -0.6f, 0.0f), rightLegModel);
    
    auto rightFootModel = createBodyPartModel("rightFoot", glm::vec3(0.3f, 0.2f, 0.5f), texturePath);
    auto rightFoot = std::make_unique<Joint>("rightFoot", glm::vec3(0.0f, -0.6f, 0.1f), rightFootModel);
    
    // Build hierarchy (bottom-up)
    leftArm->addChild(std::move(leftHand));
    leftShoulder->addChild(std::move(leftArm));
    
    rightArm->addChild(std::move(rightHand));
    rightShoulder->addChild(std::move(rightArm));
    
    leftLeg->addChild(std::move(leftFoot));
    leftHip->addChild(std::move(leftLeg));
    
    rightLeg->addChild(std::move(rightFoot));
    rightHip->addChild(std::move(rightLeg));
    
    // Add all to torso
    torso->addChild(std::move(head));
    torso->addChild(std::move(leftShoulder));
    torso->addChild(std::move(rightShoulder));
    torso->addChild(std::move(leftHip));
    torso->addChild(std::move(rightHip));
    
    // Set initial poses if desired
    if (auto leftShoulderChild = torso->getChild(1)) {
        if (auto leftArmJoint = leftShoulderChild->getChild(0)) {
            leftArmJoint->setRotation(glm::vec3(45.0f, 0.0f, 0.0f));
        }
    }
    
    if (auto rightShoulderChild = torso->getChild(2)) {
        if (auto rightArmJoint = rightShoulderChild->getChild(0)) {
            rightArmJoint->setRotation(glm::vec3(-45.0f, 0.0f, 0.0f));
        }
    }
    
    return torso;
}

std::shared_ptr<Model::Mesh> Animal::createBodyPartModel(
    const std::string& name,
    const glm::vec3& size,
    const std::string& texturePath) {
    
    std::shared_ptr<Model::Mesh> model;
    std::shared_ptr<OpenGL::OpenGLTexture> texture;
    
    if (!ModuleAdder::loadModel(cubeModelPath.c_str(), texturePath.c_str(),
                              *(shaders_.front().get()), models_, textures_)) {
        throw OpenGL::OpenGLException(
            StringFormat::StringFormat(
                "Animal: Failed to load model for body part %s",
                name.c_str())
                .c_str());
    }
    
    model = models_.back();
    
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), size);
    model->setModelMatrix(scaleMatrix);
    
    return model;
}

Joint::Joint(const std::string& name, const glm::vec3& offset, 
            std::shared_ptr<Model::Mesh> model)
    : name_(name), offset_(offset), model_(model)
{
    size_ = model->getScale();
}

Joint::~Joint()
{
    children_.clear();
}

void Joint::addChild(std::shared_ptr<Joint> child)
{
    children_.push_back(child);
}

Joint* Joint::getChild(size_t index) 
{
    if (index < children_.size()) {
        return children_[index].get();
    }
    return nullptr;
}

glm::mat4 Joint::getLocalTransform() const {
    glm::mat4 transform = glm::mat4(1.0f);
    
    transform = glm::translate(transform, offset_);
    
    transform = glm::rotate(transform, glm::radians(rotation_.x), glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotation_.y), glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotation_.z), glm::vec3(0.0f, 0.0f, 1.0f));
    
    transform = glm::scale(transform, size_);
    
    return transform;
}

void Joint::setRotation(const glm::vec3& rotation) {
    rotation_ = rotation;
}

void Joint::draw(glm::mat4 parentTransform, glm::mat4 &view, glm::mat4 &projection) {
    glm::mat4 localTransform = getLocalTransform();
    glm::mat4 worldTransform = parentTransform * localTransform;
    
    if (model_) {
        model_->setModelMatrix(worldTransform);
        model_->draw(view, projection);
    }
    
    for (auto& child : children_) {
        child->draw(worldTransform, view, projection);
    }
}