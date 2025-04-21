#include "Input.h"
#include <iostream>

using namespace std;

void Input::Init() {
    keyboard_states.clear();
    just_became_down_scancodes.clear();
    just_became_up_scancodes.clear();

    mouse_states.clear();
    just_became_down_buttons.clear();
    just_became_up_buttons.clear();
    mouse_position = glm::vec2(0.0f, 0.0f);
    mouse_scroll_y = 0.0f;
}

void Input::ProcessEvent(const SDL_Event& e) {
    switch (e.type) {
        SDL_Scancode scancode;
        Uint8 button;
        case SDL_QUIT:
            is_quitting = true;
            break;
        case SDL_KEYDOWN:
            scancode = e.key.keysym.scancode;
            if (keyboard_states[scancode] != INPUT_STATE_DOWN) {
                keyboard_states[scancode] = INPUT_STATE_JUST_BECAME_DOWN;
                just_became_down_scancodes.push_back(scancode);
            }
            break;
        case SDL_KEYUP:
            scancode = e.key.keysym.scancode;
            keyboard_states[scancode] = INPUT_STATE_JUST_BECAME_UP;
            just_became_up_scancodes.push_back(scancode);
            break;
        case SDL_MOUSEBUTTONDOWN: {
            button = e.button.button;
            if (mouse_states[button] != INPUT_STATE_DOWN) {
                mouse_states[button] = INPUT_STATE_JUST_BECAME_DOWN;
                just_became_down_buttons.push_back(button);
            }
            break;
        }
        case SDL_MOUSEBUTTONUP: {
            button = e.button.button;
            mouse_states[button] = INPUT_STATE_JUST_BECAME_UP;
            just_became_up_buttons.push_back(button);
            break;
        }
        case SDL_MOUSEMOTION: {
            mouse_position.x = static_cast<float>(e.motion.x);
            mouse_position.y = static_cast<float>(e.motion.y);
            break;
        }
        case SDL_MOUSEWHEEL: {
            mouse_scroll_y = static_cast<float>(e.wheel.preciseY);
            break;
        }
    }
}

void Input::LateUpdate() {
    for (int i = 0; i < just_became_down_scancodes.size(); ++i) {
        SDL_Scancode& scancode = just_became_down_scancodes[i];
        if (keyboard_states[scancode] == INPUT_STATE_JUST_BECAME_DOWN) {
            keyboard_states[scancode] = INPUT_STATE_DOWN;
        }
    }
    just_became_down_scancodes.clear();
    
    for (int i = 0; i < just_became_up_scancodes.size(); ++i) {
        SDL_Scancode& scancode = just_became_up_scancodes[i];
        if (keyboard_states[scancode] == INPUT_STATE_JUST_BECAME_UP) {
            keyboard_states[scancode] = INPUT_STATE_UP;
        }
    }
    just_became_up_scancodes.clear();

    for (int i = 0; i < just_became_down_buttons.size(); ++i) {
        Uint8& button = just_became_down_buttons[i];
        if (mouse_states[button] == INPUT_STATE_JUST_BECAME_DOWN) {
            mouse_states[button] = INPUT_STATE_DOWN;
        }
    }
    just_became_down_buttons.clear();

    for (int i = 0; i < just_became_up_buttons.size(); ++i) {
        Uint8& button = just_became_up_buttons[i];
        if (mouse_states[button] == INPUT_STATE_JUST_BECAME_UP) {
            mouse_states[button] = INPUT_STATE_UP;
        }
    }
    just_became_up_buttons.clear();

    mouse_scroll_y = 0.0f;
}

bool Input::GetKey(string keyname) {
    const auto& it = __keycode_to_scancode.find(keyname);
    if (it == __keycode_to_scancode.end()) {
        return false;
    }

    return keyboard_states[it->second] == INPUT_STATE_DOWN ||
        keyboard_states[it->second] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetKeyDown(string keyname) {
    const auto& it = __keycode_to_scancode.find(keyname);
    if (it == __keycode_to_scancode.end()) {
        return false;
    }

    return keyboard_states[it->second] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetKeyUp(string keyname) {
    const auto& it = __keycode_to_scancode.find(keyname);
    if (it == __keycode_to_scancode.end()) {
        return false;
    }

    return keyboard_states[it->second] == INPUT_STATE_JUST_BECAME_UP;
}

bool Input::GetMouseButton(Uint8 button) {
    INPUT_STATE state = mouse_states[button];
    return state == INPUT_STATE_DOWN || state == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetMouseButtonDown(Uint8 button) {
    return mouse_states[button] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetMouseButtonUp(Uint8 button) {
    return mouse_states[button] == INPUT_STATE_JUST_BECAME_UP;
}

float Input::GetMouseScrollDelta() {
    return mouse_scroll_y;
}

void Input::HideCursor() {
    SDL_ShowCursor(SDL_DISABLE);
}

void Input::ShowCursor() {
    SDL_ShowCursor(SDL_ENABLE);
}

bool Input::GetExit() {
    return is_quitting;
}