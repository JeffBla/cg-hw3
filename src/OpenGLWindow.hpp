#ifndef HOMEWORK01_WINDOW_HPP_
#define HOMEWORK01_WINDOW_HPP_

#include "Model/Mesh.hpp"
#include "Avatar/Animal.hpp"

#include "glad/glad.h"

#include "GLFW/glfw3.h"

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

#include <memory>
#include <string>

class OpenGLWindow
{
private:
    enum RenderMode
    {
        Line = 0,
        Fill = 1
    };

public:
    explicit OpenGLWindow(glm::ivec2 windowSize, std::string title,
                          glm::ivec2 openglVersion);
    ~OpenGLWindow();

    OpenGLWindow(OpenGLWindow &&other) = delete;
    OpenGLWindow &operator=(OpenGLWindow &&other) = delete;
    OpenGLWindow(const OpenGLWindow &other) = delete;
    OpenGLWindow &operator=(const OpenGLWindow &other) = delete;

    void create();
    void startRender();

    bool addModel(const char *modelSource, const char *textureSource,
                  OpenGL::OpenGLShaderProgram &program);

    void setMouseLastPosition(float x, float y) noexcept;

    void frameBufferSizeCallbackImpl(GLFWwindow *window, int width, int height);

    void firstEnterMouseCallback(GLFWwindow *window, double xpos, double ypos,
                                 bool &control_flag);
    void cameraDirectionLoop(GLFWwindow *window, double xpos, double ypos);
    void mouseCallbackImpl(GLFWwindow *window, double xpos, double ypos);

    bool wasFreeCamera_ = true;
    bool isFirstFreeCamera_ = true;

private:
    bool createWindow();
    bool initializeGLAD();
    void initializeImgui();
    bool initializeOpenGL();

    void destroy();
    void destroyGLAD();
    void destroyImgui();
    void destroyOpenGL();
    void destroyModel();

    void windowRenderLoop();
    void windowRenderUpdate();
    void windowRenderLateUpdate();
    void windowRenderImguiUpdate();

    void windowImguiMain();
    void windowImguiGeneralSetting();
    void _windowImguiModelSetting(std::shared_ptr<Joint> &joint);
    void windowImguiModelSetting();

    void clearColor();

    void processInput();
    void captureMouse();
    void cameraMovement();
    void shouldExit();
    void shouldShowPolygonMode();

    float aspectRatio() const noexcept;
    int height() const noexcept;
    int width() const noexcept;

    bool ModelCreate();

    GLFWwindow *window_;

    glm::ivec2 size_;
    std::string title_;
    glm::ivec2 version_;

    RenderMode renderMode_;

    glm::vec4 backgroundColor_;

    bool isFreeCamera_ = false;
    glm::vec3 lookAt_;
    glm::vec3 cameraPosition_ = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraFront_ = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp_ = glm::vec3(0.0f, 1.0f, 0.0f);
    float cameraSpeedFactor_ = 2.5f;

    float deltaTime_ = 0.0f; // time between current frame and last frame
    float lastFrame_ = 0.0f;

    static bool mouseCaptured_;
    float mouse_lastX_ = 400, mouse_lastY_ = 300;
    float mouse_yaw_ = -90.0f;
    float mouse_pitch_ = 0.0f;
    float mouse_fov_ = 45.0f;
    float sensitivity_ = 1.0f;

    std::unique_ptr<Animal> animal_;
};

#endif // HOMEWORK01_WINDOW_HPP_
