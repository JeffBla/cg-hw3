#include "OpenGLWindow.hpp"

#include "glm/vec2.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

int main(int argc, char *argv[])
{
    std::unique_ptr<OpenGLWindow> window{nullptr};

    try
    {
        window.reset(new OpenGLWindow{glm::ivec2{800, 600}, "Homework01",
                                      glm::ivec2{3, 3}});
    }
    catch (std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    window->startRender();

    return 0;
}
