#include "OpenGL/OpenGLException.hpp"
#include "Avatar/Animal.hpp"
#include "Utils/Model/ModelAdder.hpp"
#include "Utils/Model/ShaderAdder.hpp"
#include "Utils/StringFormat/StringFormat.hpp"
#include "Animal.hpp"

Animal::Animal(){
    currentFrame = TransformFrame::FRAME1;
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
   
    rootJoint_ = createBoneHierarchy();

    setupTransformFrames();
}

void Animal::TransformToFrame(int frame)
{
    if(currentFrame == frame) {
        return;
    }
    currentFrame = static_cast<TransformFrame>(frame);
    transformationProgress_ = 0.0001f;
}

void Animal::Animate()
{
    isAnimating_ = !isAnimating_;
    transformationProgress_ = isAnimating_ ? 0.0001f : 0.0f;
}

void Animal::draw(glm::mat4 &view, glm::mat4 &projection){
    if (transformationProgress_ > 0.0f && transformationProgress_ < 1.0f) {
        Transformation();
    }
    else if(isAnimating_ && transformationProgress_ >= 1.0f) {
        currentFrame = static_cast<TransformFrame>((currentFrame + 1) % transformFrameSize);
        transformationProgress_ = 0.0001f;
    }
    rootJoint_->draw(glm::mat4(1.0f), view, projection);
}

void Animal::setupTransformFrames()
{
    transformFrames_.resize(4);
    
    // Set up the transformation frame1 for each joint
    transformFrames_[TransformFrame::FRAME1] = 
    {
        {"torso", std::make_shared<TransformFrameData>(
            glm::vec3(0.0f, 0.0f, 0.0f), 
            glm::vec3(0.0f),
            glm::vec3(1.0f, 1.5f, 0.5f)
        )},
        {"abdomen", std::make_shared<TransformFrameData>(
            glm::vec3(0.0f, -0.5f, 0.5f),
            glm::vec3(0.0f),
            glm::vec3(0.5f, 0.5f, 0.2f)
        )},
        {"abdomenLeft", std::make_shared<TransformFrameData>(
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f),
            glm::vec3(0.5f, 0.5f, 0.2f)
        )},
        {"abdomenRight", std::make_shared<TransformFrameData>(
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f),
            glm::vec3(0.5f, 0.5f, 0.2f)
        )},
        {"tail", std::make_shared<TransformFrameData>(
            glm::vec3(0.0f),
            glm::vec3(0.0f),
            glm::vec3(0.1f, 0.1f, 0.1f)
        )},

        {"head", std::make_shared<TransformFrameData>(
            glm::vec3(0.0f, 2.2f, 0.0f),
            glm::vec3(0.0f),
            glm::vec3(0.75f, 0.75f, 0.75f)
        )},
        {"nose", std::make_shared<TransformFrameData>(
            glm::vec3(0.0f, -0.3f, 0.75f),
            glm::vec3(0.0f),
            glm::vec3(0.2f, 0.2f, 0.2f)
        )},

        {"leftShoulder", std::make_shared<TransformFrameData>(
            glm::vec3(-1.2f, 1.0f, 0.0f),
            glm::vec3(0.0f),
            glm::vec3(0.25f, 0.4f, 0.25f)
        )},
        {"leftArm", std::make_shared<TransformFrameData>(
            glm::vec3(0.0f, -0.8f, 0.0f),
            glm::vec3(0.0f),
            glm::vec3(0.25f, 0.4f, 0.25f)
        )},
        {"leftHand", std::make_shared<TransformFrameData>(
            glm::vec3(0.0f, -0.7f, 0.0f),
            glm::vec3(0.0f),
            glm::vec3(0.3f, 0.3f, 0.3f)
        )},

        {"rightShoulder", std::make_shared<TransformFrameData>(
            glm::vec3(1.2f, 1.0f, 0.0f),
            glm::vec3(0.0f),
            glm::vec3(0.25f, 0.4f, 0.25f)
        )},
        {"rightArm", std::make_shared<TransformFrameData>(
            glm::vec3(0.0f, -0.8f, 0.0f),
            glm::vec3(0.0f),
            glm::vec3(0.25f, 0.4f, 0.25f)
        )},
        {"rightHand", std::make_shared<TransformFrameData>(
            glm::vec3(0.0f, -0.7f, 0.0f),
            glm::vec3(0.0f),
            glm::vec3(0.3f, 0.3f, 0.3f)
        )},

        {"leftHip", std::make_shared<TransformFrameData>(
            glm::vec3(-0.4f, -2.0f, 0.0f),
            glm::vec3(0.0f),
            glm::vec3(0.25f, 0.5f, 0.25f)
        )},
        {"leftLeg", std::make_shared<TransformFrameData>(
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(0.0f),
            glm::vec3(0.25f, 0.5f, 0.25f)
        )},
        {"leftFoot", std::make_shared<TransformFrameData>(
            glm::vec3(0.0f, -0.7f, 0.1f),
            glm::vec3(0.0f),
            glm::vec3(0.3f, 0.2f, 0.5f)
        )},

        {"rightHip", std::make_shared<TransformFrameData>(
           glm::vec3(0.4f, -2.0f, 0.0f),
            glm::vec3(0.0f),
            glm::vec3(0.25f, 0.5f, 0.25f)
        )},
        {"rightLeg", std::make_shared<TransformFrameData>(
           glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(0.0f),
            glm::vec3(0.25f, 0.5f, 0.25f)
        )},
        {"rightFoot", std::make_shared<TransformFrameData>(
            glm::vec3(0.0f, -0.7f, 0.1f),
            glm::vec3(0.0f),
            glm::vec3(0.3f, 0.2f, 0.5f)
        )}
    };

    // Set up the transformation frame2 for each joint
    transformFrames_[TransformFrame::FRAME2] = 
    {
        {"torso", std::make_shared<TransformFrameData>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(40.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.5f, 0.5f))},
        {"abdomen", std::make_shared<TransformFrameData>(glm::vec3(0.0f, -0.1f, 0.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 1.0f, 0.4f))},
        {"abdomenRight", std::make_shared<TransformFrameData>(glm::vec3(-0.6f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 1.0f, 0.4f))},
        {"abdomenLeft", std::make_shared<TransformFrameData>(glm::vec3(0.6f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 1.0f, 0.4f))},
        {"tail", std::make_shared<TransformFrameData>(glm::vec3(0.0f, -1.5f, -0.3f), glm::vec3(40.0f, 0.0f, 0.0f), glm::vec3(0.1f, 0.5f, 0.1f))},
        {"head", std::make_shared<TransformFrameData>(glm::vec3(0.0f, 2.2f, 0.0f), glm::vec3(-35.0f, 0.0f, 0.0f), glm::vec3(0.75f, 0.75f, 0.75f))},
        {"nose", std::make_shared<TransformFrameData>(glm::vec3(0.0f, -0.3f, 0.75f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.4f, 0.4f, 0.2f))},
        {"leftShoulder", std::make_shared<TransformFrameData>(glm::vec3(-1.2f, 1.0f, 0.0f), glm::vec3(-65.0f, 0.0f, 0.0f), glm::vec3(0.25f, 0.4f, 0.25f))},
        {"leftArm", std::make_shared<TransformFrameData>(glm::vec3(0.0f, -0.8f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.25f, 0.4f, 0.25f))},
        {"leftHand", std::make_shared<TransformFrameData>(glm::vec3(0.0f, -0.7f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.3f, 0.3f, 0.3f))},
        {"rightShoulder", std::make_shared<TransformFrameData>(glm::vec3(1.2f, 1.0f, 0.0f), glm::vec3(-65.0f, 0.0f, 0.0f), glm::vec3(0.25f, 0.4f, 0.25f))},
        {"rightArm", std::make_shared<TransformFrameData>(glm::vec3(0.0f, -0.8f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.25f, 0.4f, 0.25f))},
        {"rightHand", std::make_shared<TransformFrameData>(glm::vec3(0.0f, -0.7f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.3f, 0.3f, 0.3f))},
        {"leftHip", std::make_shared<TransformFrameData>(glm::vec3(-0.4f, -2.0f, 0.3f), glm::vec3(-30.0f, 0.0f, 0.0f), glm::vec3(0.25f, 0.5f, 0.25f))},
        {"leftLeg", std::make_shared<TransformFrameData>(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(15.0f, 0.0f, 0.0f), glm::vec3(0.25f, 0.5f, 0.25f))},
        {"leftFoot", std::make_shared<TransformFrameData>(glm::vec3(0.0f, -0.7f, 0.1f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.3f, 0.2f, 0.5f))},
        {"rightHip", std::make_shared<TransformFrameData>(glm::vec3(0.4f, -1.0f, 0.7f), glm::vec3(-90.f, 0.0f, 0.0f), glm::vec3(0.25f, 0.5f, 0.25f))},
        {"rightLeg", std::make_shared<TransformFrameData>(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(15.0f, 0.0f, 0.0f), glm::vec3(0.25f, 0.5f, 0.25f))},
        {"rightFoot", std::make_shared<TransformFrameData>(glm::vec3(0.0f, -0.7f, 0.1f), glm::vec3(25.0f, 0.0f, 0.0f), glm::vec3(0.3f, 0.2f, 0.5f))}
    };

    // Set up the transformation frame3 for each joint
    transformFrames_[TransformFrame::FRAME3] = 
    {
        {"torso", std::make_shared<TransformFrameData>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(100.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.5f, 0.5f))},
        {"abdomen", std::make_shared<TransformFrameData>(glm::vec3(0.0f, -0.1f, 0.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 1.0f, 0.4f))},
        {"abdomenRight", std::make_shared<TransformFrameData>(glm::vec3(-0.9f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 1.0f, 0.4f))},
        {"abdomenLeft", std::make_shared<TransformFrameData>(glm::vec3(0.9f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 1.0f, 0.4f))},
        {"tail", std::make_shared<TransformFrameData>(glm::vec3(0.0f, -1.5f, -0.3f), glm::vec3(40.0f, 0.0f, 0.0f), glm::vec3(0.1f, 0.5f, 0.1f))},
        {"head", std::make_shared<TransformFrameData>(glm::vec3(0.0f, 2.2f, 0.0f), glm::vec3(-105.0f, 0.0f, 0.0f), glm::vec3(0.75f, 0.75f, 0.75f))},
        {"nose", std::make_shared<TransformFrameData>(glm::vec3(0.0f, -0.3f, 0.75f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.4f, 0.4f, 0.2f))},
        {"leftShoulder", std::make_shared<TransformFrameData>(glm::vec3(-1.2f, 1.0f, 0.0f), glm::vec3(-85.0f, 0.0f, 0.0f), glm::vec3(0.25f, 0.4f, 0.25f))},
        {"leftArm", std::make_shared<TransformFrameData>(glm::vec3(0.0f, -0.8f, 0.0f), glm::vec3(-10.0f, 0.0f, 0.0f), glm::vec3(0.25f, 0.4f, 0.25f))},
        {"leftHand", std::make_shared<TransformFrameData>(glm::vec3(0.0f, -0.7f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.3f, 0.2f, 0.5f))},
        {"rightShoulder", std::make_shared<TransformFrameData>(glm::vec3(1.2f, 1.0f, 0.0f), glm::vec3(-95.0f, 0.0f, 0.0f), glm::vec3(0.25f, 0.4f, 0.25f))},
        {"rightArm", std::make_shared<TransformFrameData>(glm::vec3(0.0f, -0.8f, 0.0f), glm::vec3(-10.0f, 0.0f, 0.0f), glm::vec3(0.25f, 0.4f, 0.25f))},
        {"rightHand", std::make_shared<TransformFrameData>(glm::vec3(0.0f, -0.7f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.3f, 0.2f, 0.5f))},
        {"leftHip", std::make_shared<TransformFrameData>(glm::vec3(-0.4f, -1.0f, 0.5f), glm::vec3(-85.0f, 0.0f, 0.0f), glm::vec3(0.25f, 0.5f, 0.25f))},
        {"leftLeg", std::make_shared<TransformFrameData>(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(15.0f, 0.0f, 0.0f), glm::vec3(0.25f, 0.5f, 0.25f))},
        {"leftFoot", std::make_shared<TransformFrameData>(glm::vec3(0.0f, -0.7f, 0.1f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.3f, 0.2f, 0.5f))},
        {"rightHip", std::make_shared<TransformFrameData>(glm::vec3(0.4f, -1.0f, 0.7f), glm::vec3(-72.0f, 0.0f, 0.0f), glm::vec3(0.25f, 0.5f, 0.25f))},
        {"rightLeg", std::make_shared<TransformFrameData>(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(20.0f, 0.0f, 0.0f), glm::vec3(0.25f, 0.5f, 0.25f))},
        {"rightFoot", std::make_shared<TransformFrameData>(glm::vec3(0.0f, -0.5f, 0.3f), glm::vec3(-30.0f, 0.0f, 0.0f), glm::vec3(0.3f, 0.2f, 0.5f))}
    };
}

void Animal::Transformation() {
    TransformingJoint(rootJoint_, transformationProgress_);
}

void Animal::TransformingJoint(std::shared_ptr<Joint> joint, float progress) {
    auto prevFrameData = transformFrames_[(currentFrame-1 + transformFrameSize)
                                             % transformFrameSize][joint->getName()];
    auto FrameData = transformFrames_[currentFrame][joint->getName()];


    glm::vec3 interpolatedOffset = glm::mix(prevFrameData->offset, FrameData->offset, progress);
    glm::vec3 interpolatedRotation = glm::mix(prevFrameData->rotation, FrameData->rotation, progress);
    glm::vec3 interpolatedSize = glm::mix(prevFrameData->size, FrameData->size, progress);
    
    joint->setOffset(interpolatedOffset);
    joint->setRotation(interpolatedRotation);
    joint->setSize(interpolatedSize);
    
    // Recursively draw children
    for (size_t i = 0; i < joint->getChildCount(); i++) {
        TransformingJoint(joint->getChildPtr(i), progress);
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
        }
    }
}

std::shared_ptr<Joint> Animal::createBoneHierarchy() {
    // Create individual models for each body part with different sizes and textures
    
    // Create torso (root)
    auto torsoModel = createBodyPartModel("torso", glm::vec3(1.0f, 1.5f, 0.5f), texturePath);
    auto torso = std::make_unique<Joint>("torso", glm::vec3(0.0f, 0.0f, 0.0f), torsoModel);
    
    auto abdomenModel = createBodyPartModel("abdomen", glm::vec3(0.5f, 0.5f, 0.2f), texturePath);
    auto abdomen = std::make_unique<Joint>("abdomen", glm::vec3(0.0f, -0.5f, 0.5f), abdomenModel);

    auto abdomenLeftModel = createBodyPartModel("abdomenLeft", glm::vec3(0.5f, 0.5f, 0.2f), texturePath);
    auto abdomenLeft = std::make_unique<Joint>("abdomenLeft", glm::vec3(0.0f, 0.0f, 0.0f), abdomenLeftModel);

    auto abdomenRightModel = createBodyPartModel("abdomenRight", glm::vec3(0.5f, 0.5f, 0.2f), texturePath);
    auto abdomenRight = std::make_unique<Joint>("abdomenRight", glm::vec3(0.0f, 0.0f, 0.0f), abdomenRightModel);

    auto tailModel = createBodyPartModel("tail", glm::vec3(0.1f, 0.1f, 0.1f), texturePath);
    auto tail = std::make_unique<Joint>("tail", glm::vec3(0.0f, 0.0f, 0.0f), tailModel);

    // Create head
    auto headModel = createBodyPartModel("head", glm::vec3(0.75f, 0.75f, 0.75f), texturePath);
    auto head = std::make_unique<Joint>("head", glm::vec3(0.0f, 2.2f, 0.0f), headModel);
    
    // Create noise
    auto noseModel = createBodyPartModel("nose", glm::vec3(0.2f, 0.2f, 0.2f), texturePath);
    auto nose = std::make_unique<Joint>("nose", glm::vec3(0.0f, -0.3f, 0.75f), noseModel);

    // Create left arm parts
    auto leftShoulderModel = createBodyPartModel("leftShoulder", glm::vec3(0.25f, 0.4f, 0.25f), texturePath);
    auto leftShoulder = std::make_unique<Joint>("leftShoulder", glm::vec3(-1.2f, 1.0f, 0.0f), leftShoulderModel);
    
    auto leftArmModel = createBodyPartModel("leftArm", glm::vec3(0.25f, 0.4f, 0.25f), texturePath);
    auto leftArm = std::make_unique<Joint>("leftArm", glm::vec3(0.0f, -0.8f, 0.0f), leftArmModel);
    
    auto leftHandModel = createBodyPartModel("leftHand", glm::vec3(0.3f, 0.3f, 0.3f), texturePath);
    auto leftHand = std::make_unique<Joint>("leftHand", glm::vec3(0.0f, -0.7f, 0.0f), leftHandModel);
    
    // Create right arm parts
    auto rightShoulderModel = createBodyPartModel("rightShoulder", glm::vec3(0.25f, 0.4f, 0.25f), texturePath);
    auto rightShoulder = std::make_unique<Joint>("rightShoulder", glm::vec3(1.2f, 1.0f, 0.0f), rightShoulderModel);
    
    auto rightArmModel = createBodyPartModel("rightArm", glm::vec3(0.25f, 0.4f, 0.25f), texturePath);
    auto rightArm = std::make_unique<Joint>("rightArm", glm::vec3(0.0f, -0.8f, 0.0f), rightArmModel);
    
    auto rightHandModel = createBodyPartModel("rightHand", glm::vec3(0.3f, 0.3f, 0.3f), texturePath);
    auto rightHand = std::make_unique<Joint>("rightHand", glm::vec3(0.0f, -0.7f, 0.0f), rightHandModel);
    
    // Create left leg parts
    auto leftHipModel = createBodyPartModel("leftHip", glm::vec3(0.25f, 0.5f, 0.25f), texturePath);
    auto leftHip = std::make_unique<Joint>("leftHip", glm::vec3(-0.4f, -2.0f, 0.0f), leftHipModel);
    
    auto leftLegModel = createBodyPartModel("leftLeg", glm::vec3(0.25f, 0.5f, 0.25f), texturePath);
    auto leftLeg = std::make_unique<Joint>("leftLeg", glm::vec3(0.0f, -1.0f, 0.0f), leftLegModel);
    
    auto leftFootModel = createBodyPartModel("leftFoot", glm::vec3(0.3f, 0.2f, 0.5f), texturePath);
    auto leftFoot = std::make_unique<Joint>("leftFoot", glm::vec3(0.0f, -0.7f, 0.1f), leftFootModel);
    
    // Create right leg parts
    auto rightHipModel = createBodyPartModel("rightHip", glm::vec3(0.25f, 0.5f, 0.25f), texturePath);
    auto rightHip = std::make_unique<Joint>("rightHip", glm::vec3(0.4f, -2.0f, 0.0f), rightHipModel);
    
    auto rightLegModel = createBodyPartModel("rightLeg", glm::vec3(0.25f, 0.5f, 0.25f), texturePath);
    auto rightLeg = std::make_unique<Joint>("rightLeg", glm::vec3(0.0f, -1.0f, 0.0f), rightLegModel);
    
    auto rightFootModel = createBodyPartModel("rightFoot", glm::vec3(0.3f, 0.2f, 0.5f), texturePath);
    auto rightFoot = std::make_unique<Joint>("rightFoot", glm::vec3(0.0f, -0.7f, 0.1f), rightFootModel);
    
    // Build hierarchy (bottom-up)
    leftArm->addChild(std::move(leftHand));
    leftShoulder->addChild(std::move(leftArm));
    
    rightArm->addChild(std::move(rightHand));
    rightShoulder->addChild(std::move(rightArm));
    
    leftLeg->addChild(std::move(leftFoot));
    leftHip->addChild(std::move(leftLeg));
    
    rightLeg->addChild(std::move(rightFoot));
    rightHip->addChild(std::move(rightLeg));
    
    head->addChild(std::move(nose));

    // Add all to torso
    torso->addChild(std::move(abdomen));
    torso->addChild(std::move(abdomenLeft));
    torso->addChild(std::move(abdomenRight));
    torso->addChild(std::move(tail));
    torso->addChild(std::move(head));
    torso->addChild(std::move(leftShoulder));
    torso->addChild(std::move(rightShoulder));
    torso->addChild(std::move(leftHip));
    torso->addChild(std::move(rightHip));
    
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