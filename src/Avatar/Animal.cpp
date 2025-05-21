#include "OpenGL/OpenGLException.hpp"
#include "Avatar/Animal.hpp"
#include "Utils/Model/ModelAdder.hpp"
#include "Utils/Model/ShaderAdder.hpp"
#include "Utils/StringFormat/StringFormat.hpp"
#include "Animal.hpp"

Animal::Animal(){
    isHumanForm_ = true;
    transformationProgress_ = 0.0f;
    create();
}

Animal::~Animal(){
    models_.clear();
    textures_.clear();
    shaders_.clear();
}

void Animal::create(){
    ShaderAdder::addShader(vertexShader.c_str(), fragmentShader.c_str(), nullptr, shaders_);
   
    humanRootJoint_ = createBoneHierarchy();
    pigRootJoint_ = createPigBoneHierarchy();

    rootJoint_ = humanRootJoint_;
}

void Animal::toggleForm() {
    isHumanForm_ = !isHumanForm_;
    transformationProgress_ = 0.0001f;  // Set to a small non-zero value to trigger isTransforming()
}

void Animal::draw(glm::mat4 &view, glm::mat4 &projection){
    if (transformationProgress_ > 0.0f && transformationProgress_ < 1.0f) {
        drawTransformation(view, projection);
    } else if (rootJoint_) {
        rootJoint_->draw(glm::mat4(1.0f), view, projection);
    }
}

void Animal::drawTransformation(glm::mat4 &view, glm::mat4 &projection) {
    // Get the root joints for both forms
    std::shared_ptr<Joint> sourceJoint = isHumanForm_ ? pigRootJoint_ : humanRootJoint_;
    std::shared_ptr<Joint> targetJoint = isHumanForm_ ? humanRootJoint_ : pigRootJoint_;
    
    // Draw the transforming model using interpolation
    drawTransformingJoint(sourceJoint, targetJoint, glm::mat4(1.0f), view, projection, transformationProgress_);
}

void Animal::drawTransformingJoint(
    std::shared_ptr<Joint> sourceJoint, 
    std::shared_ptr<Joint> targetJoint,
    glm::mat4 parentTransform, 
    glm::mat4 &view, 
    glm::mat4 &projection,
    float progress) {
    
    if (!sourceJoint || !targetJoint) {
        return;
    }
    
    glm::vec3 interpolatedOffset = glm::mix(sourceJoint->getOffset(), targetJoint->getOffset(), progress);
    glm::vec3 interpolatedRotation = glm::mix(sourceJoint->getRotation(), targetJoint->getRotation(), progress);
    glm::vec3 interpolatedSize = glm::mix(sourceJoint->getSize(), targetJoint->getSize(), progress);
    
    glm::mat4 localTransform = glm::mat4(1.0f);
    localTransform = glm::translate(localTransform, interpolatedOffset);
    
    const float EPSILON = 0.0001f;
    if (std::abs(interpolatedRotation.x) > EPSILON) {
        localTransform = glm::rotate(localTransform, glm::radians(interpolatedRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    }
    if (std::abs(interpolatedRotation.y) > EPSILON) {
        localTransform = glm::rotate(localTransform, glm::radians(interpolatedRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    if (std::abs(interpolatedRotation.z) > EPSILON) {
        localTransform = glm::rotate(localTransform, glm::radians(interpolatedRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    }
    
    glm::mat4 worldTransformNoScale = parentTransform * localTransform;
    glm::mat4 worldTransform = glm::scale(worldTransformNoScale, interpolatedSize);
    
    // Draw the interpolated model
    if (sourceJoint->getModel() && targetJoint->getModel()) {
        auto model = progress < 0.5f ? sourceJoint->getModel() : targetJoint->getModel();
        model->setModelMatrix(worldTransform);
        model->draw(view, projection);
    }
    
    // Recursively draw children
    // We assume both hierarchies have the same structure
    size_t minChildCount = std::min(sourceJoint->getChildCount(), targetJoint->getChildCount());
    
    for (size_t i = 0; i < minChildCount; i++) {
        auto sourceChild = sourceJoint->getChild(i);
        auto targetChild = targetJoint->getChild(i);
        
        if (sourceChild && targetChild) {
            drawTransformingJoint(
                sourceJoint->getChildPtr(i),
                targetJoint->getChildPtr(i),
                worldTransformNoScale,
                view,
                projection,
                progress
            );
        }
    }
}

void Animal::updateTransformation(float deltaTime) {
    // Update transformation progress (assume complete transform takes 2 seconds)
    const float transformationSpeed = 0.5f; // 2 seconds for complete transformation
    
    if (transformationProgress_ < 1.0f) {
        transformationProgress_ += deltaTime * transformationSpeed;
        
        // Clamp to 1.0
        if (transformationProgress_ >= 1.0f) {
            transformationProgress_ = 1.0f;
            rootJoint_ = isHumanForm_ ? humanRootJoint_ : pigRootJoint_;
        }
    }
}

std::shared_ptr<Joint> Animal::createBoneHierarchy() {
    // Create individual models for each body part with different sizes and textures
    
    // Create torso (root)
    auto torsoModel = createBodyPartModel("torso", glm::vec3(1.0f, 1.5f, 0.5f), texturePath);
    auto torso = std::make_unique<Joint>("torso", glm::vec3(0.0f, 0.0f, 0.0f), torsoModel);
    
    // Create head
    auto headModel = createBodyPartModel("head", glm::vec3(0.75f, 0.75f, 0.75f), texturePath);
    auto head = std::make_unique<Joint>("head", glm::vec3(0.0f, 2.2f, 0.0f), headModel);
    
    // Create left arm parts
    auto leftShoulderModel = createBodyPartModel("leftShoulder", glm::vec3(0.3f, 0.3f, 0.3f), texturePath);
    auto leftShoulder = std::make_unique<Joint>("leftShoulder", glm::vec3(-1.2f, 1.0f, 0.0f), leftShoulderModel);
    
    auto leftArmModel = createBodyPartModel("leftArm", glm::vec3(0.25f, 0.8f, 0.25f), texturePath);
    auto leftArm = std::make_unique<Joint>("leftArm", glm::vec3(0.0f, -0.8f, 0.0f), leftArmModel);
    
    auto leftHandModel = createBodyPartModel("leftHand", glm::vec3(0.3f, 0.3f, 0.3f), texturePath);
    auto leftHand = std::make_unique<Joint>("leftHand", glm::vec3(0.0f, -1.0f, 0.0f), leftHandModel);
    
    // Create right arm parts
    auto rightShoulderModel = createBodyPartModel("rightShoulder", glm::vec3(0.3f, 0.3f, 0.3f), texturePath);
    auto rightShoulder = std::make_unique<Joint>("rightShoulder", glm::vec3(1.2f, 1.0f, 0.0f), rightShoulderModel);
    
    auto rightArmModel = createBodyPartModel("rightArm", glm::vec3(0.25f, 0.8f, 0.25f), texturePath);
    auto rightArm = std::make_unique<Joint>("rightArm", glm::vec3(0.0f, -0.8f, 0.0f), rightArmModel);
    
    auto rightHandModel = createBodyPartModel("rightHand", glm::vec3(0.3f, 0.3f, 0.3f), texturePath);
    auto rightHand = std::make_unique<Joint>("rightHand", glm::vec3(0.0f, -1.0f, 0.0f), rightHandModel);
    
    // Create left leg parts
    auto leftHipModel = createBodyPartModel("leftHip", glm::vec3(0.3f, 0.3f, 0.3f), texturePath);
    auto leftHip = std::make_unique<Joint>("leftHip", glm::vec3(-0.4f, -1.65f, 0.0f), leftHipModel);
    
    auto leftLegModel = createBodyPartModel("leftLeg", glm::vec3(0.25f, 1.0f, 0.25f), texturePath);
    auto leftLeg = std::make_unique<Joint>("leftLeg", glm::vec3(0.0f, -1.2f, 0.0f), leftLegModel);
    
    auto leftFootModel = createBodyPartModel("leftFoot", glm::vec3(0.3f, 0.2f, 0.5f), texturePath);
    auto leftFoot = std::make_unique<Joint>("leftFoot", glm::vec3(0.0f, -1.0f, 0.1f), leftFootModel);
    
    // Create right leg parts
    auto rightHipModel = createBodyPartModel("rightHip", glm::vec3(0.3f, 0.3f, 0.3f), texturePath);
    auto rightHip = std::make_unique<Joint>("rightHip", glm::vec3(0.4f, -1.65f, 0.0f), rightHipModel);
    
    auto rightLegModel = createBodyPartModel("rightLeg", glm::vec3(0.25f, 1.0f, 0.25f), texturePath);
    auto rightLeg = std::make_unique<Joint>("rightLeg", glm::vec3(0.0f, -1.2f, 0.0f), rightLegModel);
    
    auto rightFootModel = createBodyPartModel("rightFoot", glm::vec3(0.3f, 0.2f, 0.5f), texturePath);
    auto rightFoot = std::make_unique<Joint>("rightFoot", glm::vec3(0.0f, -1.0f, 0.1f), rightFootModel);
    
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
    
    return torso;
}


std::shared_ptr<Joint> Animal::createPigBoneHierarchy() {
    // Pig model will be on all fours with distinctive pig features
    
    // Create torso (root) - larger and longer for pig body
    auto torsoModel = createBodyPartModel("pigTorso", glm::vec3(1.2f, 0.8f, 1.8f), pigTexturePath);
    auto torso = std::make_shared<Joint>("pigTorso", glm::vec3(0.0f, 0.0f, 0.0f), torsoModel);
    
    // Create head - more elongated for pig snout
    auto headModel = createBodyPartModel("pigHead", glm::vec3(0.9f, 0.6f, 1.0f), pigTexturePath);
    auto head = std::make_shared<Joint>("pigHead", glm::vec3(0.0f, 0.6f, 1.4f), headModel);
    head->setRotation(glm::vec3(0.0f, 0.0f, 0.0f)); // Looking forward
    
    // Create snout
    auto snoutModel = createBodyPartModel("pigSnout", glm::vec3(0.5f, 0.3f, 0.6f), pigTexturePath);
    auto snout = std::make_shared<Joint>("pigSnout", glm::vec3(0.0f, -0.1f, 0.7f), snoutModel);
    
    // Create ears
    auto leftEarModel = createBodyPartModel("leftEar", glm::vec3(0.3f, 0.1f, 0.3f), pigTexturePath);
    auto leftEar = std::make_shared<Joint>("leftEar", glm::vec3(-0.4f, 0.4f, 0.0f), leftEarModel);
    leftEar->setRotation(glm::vec3(0.0f, 0.0f, -30.0f)); // Tilt outward
    
    auto rightEarModel = createBodyPartModel("rightEar", glm::vec3(0.3f, 0.1f, 0.3f), pigTexturePath);
    auto rightEar = std::make_shared<Joint>("rightEar", glm::vec3(0.4f, 0.4f, 0.0f), rightEarModel);
    rightEar->setRotation(glm::vec3(0.0f, 0.0f, 30.0f)); // Tilt outward
    
    // Create front legs
    auto frontLeftLegModel = createBodyPartModel("frontLeftLeg", glm::vec3(0.25f, 0.6f, 0.25f), pigTexturePath);
    auto frontLeftLeg = std::make_shared<Joint>("frontLeftLeg", glm::vec3(-0.5f, -0.7f, 0.8f), frontLeftLegModel);
    
    auto frontLeftFootModel = createBodyPartModel("frontLeftFoot", glm::vec3(0.3f, 0.2f, 0.3f), pigTexturePath);
    auto frontLeftFoot = std::make_shared<Joint>("frontLeftFoot", glm::vec3(0.0f, -0.7f, 0.0f), frontLeftFootModel);
    
    auto frontRightLegModel = createBodyPartModel("frontRightLeg", glm::vec3(0.25f, 0.6f, 0.25f), pigTexturePath);
    auto frontRightLeg = std::make_shared<Joint>("frontRightLeg", glm::vec3(0.5f, -0.7f, 0.8f), frontRightLegModel);
    
    auto frontRightFootModel = createBodyPartModel("frontRightFoot", glm::vec3(0.3f, 0.2f, 0.3f), pigTexturePath);
    auto frontRightFoot = std::make_shared<Joint>("frontRightFoot", glm::vec3(0.0f, -0.7f, 0.0f), frontRightFootModel);
    
    // Create hind legs
    auto hindLeftLegModel = createBodyPartModel("hindLeftLeg", glm::vec3(0.25f, 0.6f, 0.25f), pigTexturePath);
    auto hindLeftLeg = std::make_shared<Joint>("hindLeftLeg", glm::vec3(-0.5f, -0.7f, -0.8f), hindLeftLegModel);
    
    auto hindLeftFootModel = createBodyPartModel("hindLeftFoot", glm::vec3(0.3f, 0.2f, 0.3f), pigTexturePath);
    auto hindLeftFoot = std::make_shared<Joint>("hindLeftFoot", glm::vec3(0.0f, -0.7f, 0.0f), hindLeftFootModel);
    
    auto hindRightLegModel = createBodyPartModel("hindRightLeg", glm::vec3(0.25f, 0.6f, 0.25f), pigTexturePath);
    auto hindRightLeg = std::make_shared<Joint>("hindRightLeg", glm::vec3(0.5f, -0.7f, -0.8f), hindRightLegModel);
    
    auto hindRightFootModel = createBodyPartModel("hindRightFoot", glm::vec3(0.3f, 0.2f, 0.3f), pigTexturePath);
    auto hindRightFoot = std::make_shared<Joint>("hindRightFoot", glm::vec3(0.0f, -0.7f, 0.0f), hindRightFootModel);
    
    // Create tail - curly!
    auto tailBaseModel = createBodyPartModel("tailBase", glm::vec3(0.15f, 0.15f, 0.15f), pigTexturePath);
    auto tailBase = std::make_shared<Joint>("tailBase", glm::vec3(0.0f, 0.2f, -1.0f), tailBaseModel);
    tailBase->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    
    auto tailMidModel = createBodyPartModel("tailMid", glm::vec3(0.12f, 0.12f, 0.12f), pigTexturePath);
    auto tailMid = std::make_shared<Joint>("tailMid", glm::vec3(0.0f, 0.2f, -0.1f), tailMidModel);
    tailMid->setRotation(glm::vec3(0.0f, 0.0f, 45.0f));
    
    auto tailEndModel = createBodyPartModel("tailEnd", glm::vec3(0.1f, 0.1f, 0.1f), pigTexturePath);
    auto tailEnd = std::make_shared<Joint>("tailEnd", glm::vec3(0.1f, 0.1f, 0.0f), tailEndModel);
    tailEnd->setRotation(glm::vec3(0.0f, 0.0f, 45.0f));
    
    // Build hierarchy
    // Head parts
    head->addChild(snout);
    head->addChild(leftEar);
    head->addChild(rightEar);
    
    // Legs
    frontLeftLeg->addChild(frontLeftFoot);
    frontRightLeg->addChild(frontRightFoot);
    hindLeftLeg->addChild(hindLeftFoot);
    hindRightLeg->addChild(hindRightFoot);
    
    // Tail
    tailMid->addChild(tailEnd);
    tailBase->addChild(tailMid);
    
    // Add all to torso
    torso->addChild(head);
    torso->addChild(frontLeftLeg);
    torso->addChild(frontRightLeg);
    torso->addChild(hindLeftLeg);
    torso->addChild(hindRightLeg);
    torso->addChild(tailBase);
    
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
    rotation_ = glm::vec3(0.0f, 0.0f, 0.0f);
    children_.clear();
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

std::shared_ptr<Joint> Joint::getChildPtr(size_t index)
{
    if (index < children_.size()) {
        return children_[index];
    }
    return nullptr;
}

size_t Joint::getChildCount() const
{
    return children_.size();
}

glm::mat4 Joint::getLocalTransform() const {
    glm::mat4 transform = glm::mat4(1.0f);
    
    transform = glm::translate(transform, offset_);
    
    const float EPSILON = 0.0001f; // Small threshold value for rotation checks

    if (std::abs(rotation_.x) > EPSILON) {
        transform = glm::rotate(transform, glm::radians(rotation_.x), glm::vec3(1.0f, 0.0f, 0.0f));
    }
    if (std::abs(rotation_.y) > EPSILON) {
        transform = glm::rotate(transform, glm::radians(rotation_.y), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    if (std::abs(rotation_.z) > EPSILON) {
        transform = glm::rotate(transform, glm::radians(rotation_.z), glm::vec3(0.0f, 0.0f, 1.0f));
    }
    return transform;
}

void Joint::setRotation(const glm::vec3& rotation) {
    rotation_ = rotation;
}

void Joint::draw(glm::mat4 parentTransform, glm::mat4 &view, glm::mat4 &projection) {
    glm::mat4 localTransform = getLocalTransform();
    glm::mat4 worldTransformNoScale = parentTransform * localTransform;
    glm::mat4 worldTransform = glm::scale(worldTransformNoScale, size_);

    if (model_) {
        model_->setModelMatrix(worldTransform);
        model_->draw(view, projection);
    }
    
    for (auto& child : children_) {
        child->draw(worldTransformNoScale, view, projection);
    }
}