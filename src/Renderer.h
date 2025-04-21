#pragma once

#include <string>
#include <algorithm>

#include "glm/glm.hpp"

#include "Helper.h"
#include "AudioHelper.h"
#include "EngineUtils.h"
#include "TextDB.h"
#include "AudioDB.h"
#include "ImageDB.h"
#include "SceneDB.h"
#include "Input.h"
#include "LightSystem.h"

using namespace std;

class Renderer {
public:
	Renderer() :
		window_title(""),
		window(nullptr),
		sdl_renderer(nullptr),
		clear_color({ 255,255,255,255 }),
		window_dimensions({ 640,360 }),
		window_position({ 100,100 }),
		textDB(TextDB::getInstance()),
		audioDB(AudioDB::getInstance()),
		sceneDB(SceneDB::getInstance()),
		inputManager(Input::getInstance()),
		lightSystem(LightSystem::getInstance()),
		camera_pos({ 0.0f, 0.0f }),
		zoom_factor(1.0f){
	}

	void init();
	void renderGame();
	void renderImageAndUI(bool render_ui);
	void renderUIText();
	void renderPixel();
	void renderLights();

	static void SetPosition(float x, float y);
	static float GetPositionX();
	static float GetPositionY();
	static void SetZoom(float zoom_factor);
	static float GetZoom();
	SDL_Renderer* GetSDLRenderer();

	static Renderer& getInstance() {
		static Renderer instance;
		return instance;
	}

private:
	string window_title;
	SDL_Window* window;
	SDL_Renderer* sdl_renderer;
	SDL_Color clear_color;
	glm::ivec2 window_dimensions;
	glm::ivec2 window_position;
	TextDB& textDB;
	AudioDB& audioDB;
	SceneDB& sceneDB;
	Input& inputManager;
	LightSystem& lightSystem;
	glm::vec2 camera_pos;
	float zoom_factor;

	void checkLoadResources();
};