#ifndef INPUT_H
#define INPUT_H

#include "SDL2/SDL.h"
#include "glm/glm.hpp"

#include <unordered_map>
#include <vector>
#include <string>

#include "SDL_SCANCODE_MAP.cpp"

using namespace std;

enum INPUT_STATE { INPUT_STATE_UP, INPUT_STATE_JUST_BECAME_DOWN, INPUT_STATE_DOWN, INPUT_STATE_JUST_BECAME_UP };

class Input
{
public:
	void Init();
	void ProcessEvent(const SDL_Event& e);
	void LateUpdate();

	static bool GetKey(string keyname);
	static bool GetKeyDown(string keyname);
	static bool GetKeyUp(string keyname);

	static glm::vec2 GetMousePosition() { return mouse_position; }
	static bool GetMouseButton(Uint8 button);
	static bool GetMouseButtonDown(Uint8 button);
	static bool GetMouseButtonUp(Uint8 button);
	static float GetMouseScrollDelta();
	static void HideCursor();
	static void ShowCursor();

	bool GetExit();

	static Input& getInstance() {
		static Input instance;
		return instance;
	}

private:
	static inline std::unordered_map<SDL_Scancode, INPUT_STATE> keyboard_states;
	static inline std::vector<SDL_Scancode> just_became_down_scancodes;
	static inline std::vector<SDL_Scancode> just_became_up_scancodes;

	static inline std::unordered_map<Uint8, INPUT_STATE> mouse_states;
	static inline std::vector<Uint8> just_became_down_buttons;
	static inline std::vector<Uint8> just_became_up_buttons;
	static inline glm::vec2 mouse_position{ 0.0f, 0.0f };
	static inline float mouse_scroll_y = 0.0f;

	bool is_quitting = false;
};

#endif