#pragma once

#include <string>
#include <optional>
#include <vector>
#include <unordered_map>
#include <memory>

#include "SDL2/SDL.h"
#include "glm/glm.hpp"
#include "lua.hpp"
#include "LuaBridge/LuaBridge.h"

class LuaManager;

using namespace std;

class Actor {
public:
	Actor() :
		id(next_id++),
		name(""),
		temp(""),
		toDestroy(false),
		dontDestroy(false),
		processedOnStart(false){
	}

	struct Component {
		string type;
		unsigned int actorId;
		shared_ptr<luabridge::LuaRef> instance;
	};

	unsigned int id;
	string name;
	string temp;
	bool toDestroy;
	bool dontDestroy;
	bool processedOnStart;
	unordered_map<string, Component> components;

	string getName() { return name; }
	unsigned int getID() { return id; }
	luabridge::LuaRef GetComponentByKey(const string& key);
	luabridge::LuaRef GetComponent(const string& typeName);
	luabridge::LuaRef GetComponents(const string& typeName);
	luabridge::LuaRef AddComponent(const string& typeName);
	void RemoveComponent(luabridge::LuaRef component);

	static inline unsigned int next_id = 0;
};