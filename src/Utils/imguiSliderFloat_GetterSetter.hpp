#ifndef IMGUI_SLIDER_FLOAT_GETTER_SETTER_HPP
#define IMGUI_SLIDER_FLOAT_GETTER_SETTER_HPP

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <glm/glm.hpp>

namespace ImGui
{

template <typename T, typename Getter, typename Setter>
void SliderFloat(const char* label, T* t, Getter getter, Setter setter, float min, float max,
                    const char* format = "%.3f")
{
    float current = (t->*getter)();
    float new_value = current;

    ImGui::SliderFloat(label, &new_value, min, max, format);

    if (current != new_value) {
        (t->*setter)(new_value);
    }
}

template <typename T, typename Getter, typename Setter>
void SliderFloat3(const char* label, T* t, Getter getter, Setter setter, float min, float max,
                    const char* format = "%.3f")
{
    glm::vec3 current = (t->*getter)();
    float new_value[3] = { current.x, current.y, current.z };

    ImGui::SliderFloat3(label, new_value, min, max, format);

    if (current.x != new_value[0] || current.y != new_value[1] || current.z != new_value[2]) {
        (t->*setter)(glm::vec3(new_value[0], new_value[1], new_value[2]));
    }
}
}

#endif // IMGUI_SLIDER_FLOAT_GETTER_SETTER_HPP