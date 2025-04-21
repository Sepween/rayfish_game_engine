#pragma once

#include <string>
#include <iostream>
#include <algorithm>
#include <filesystem>

#include "glm/glm.hpp"
#include "rapidjson/document.h"

#include "SceneDB.h"
#include "EngineUtils.h"
#include "Actor.h"
#include "Renderer.h"
#include "AudioDB.h"
#include "Input.h"
#include "ImageDB.h"
#include "lua/lua.hpp"
#include "Rigidbody.h"

class LuaManager;

using namespace std;

class Game {
public:
	Game();

	static Game& getInstance() {
		static Game instance;
		return instance;
	}

	void run();

	static void Load(const string& scene_name);
	static string GetCurrent();
	static void DontDestroy(luabridge::LuaRef actor);

private:
	bool new_scene;
	bool exiting;
	string cur_scene_name;
	string new_scene_name;
	SceneDB& sceneDB;
	Renderer& renderer;
	AudioDB& audioDB;
	LuaManager& luaManager;
	Input& inputManager;
	lua_State* lua_state;

	void checkLoadResources();
	void enterNewScene();
};