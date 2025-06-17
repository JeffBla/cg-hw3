#ifndef IMGUI_RADIO_BUTTON_GETTER_SETTER_HPP
#define IMGUI_RADIO_BUTTON_GETTER_SETTER_HPP

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <glm/glm.hpp>

namespace ImGui
{

template <typename T, typename Getter, typename Callback>
void RadioButton(const char* label, T* t, Getter getter, int v_button,
                    Callback callback = nullptr)
{
    int current = (t->*getter)();
    int new_value = current;

    ImGui::RadioButton(label, &new_value, v_button);

    if (current != new_value) {
        if(callback) {
            (t->*callback)(new_value);
        }
    }
}

}

#endif // IMGUI_RADIO_BUTTON_GETTER_SETTER_HPP