#pragma once

#include <unordered_map>
#include <filesystem>
#include <thread>
#include <chrono>
#include <cstdlib>

#include "Actor.h"
#include "lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "box2d/box2d.h"
#include "SceneDB.h"
#include "ImageDB.h"
#include "Helper.h"
#include "Input.h"
#include "TextDB.h"
#include "AudioDB.h"
#include "Renderer.h"
#include "Rigidbody.h"
#include "CollisionListener.h"
#include "ParticleSystem.h"

class Actor;
class SceneDB;
class Game;

using namespace std;

class LuaManager {
public:
	static LuaManager& getInstance(){
		static LuaManager instance;
		return instance;
	}

	// init functions
	void init(lua_State* state);
	void registerClasses();
	void registerFunctions();
	
	// inheritance functions and loading from lua scripts
	void loadComponentType(const string& componentType);
	void establishInheritance(luabridge::LuaRef& instance_table, luabridge::LuaRef& base_table);
	shared_ptr<luabridge::LuaRef> createComponentInstance(const string& componentType, const string& key,
		const unordered_map<string, shared_ptr<luabridge::LuaRef>>& properties, const Actor* actor);
	void addComponentToActor(Actor* actor, const string& key, const string& type,
		const unordered_map<string, shared_ptr<luabridge::LuaRef>>& properties);
	void addComponentToActor(Actor* actor, const string& key, const string& type, 
		shared_ptr<luabridge::LuaRef> instance);
	void queueComponentAddition(const string& key, const string& type, unsigned int actorId,
		shared_ptr<luabridge::LuaRef> instance);
	void queueComponentRemoval(unsigned int actorId, const string& key);
	void queueActorForOnStart(Actor* actor);

	// running lua script functions
	void processOnStartQueue();
	void processUpdate(bool lateUpdate);
	void processAddComponent();
	void processRemoveComponent();
	
	// lua functionality functions
	static void debugLog(const string& message);
	static luabridge::LuaRef findActor(const string& name);
	static luabridge::LuaRef findAllActor(const string& name);
	static void quit();
	static void sleep(int milliseconds);
	static int getFrame();
	static void openURL(const string& url);
	static luabridge::LuaRef instantiateActor(const string& tempName);
	static void destroyActor(luabridge::LuaRef actorRef);

	// other
	lua_State* getLuaState() { return lua_state; }
	void addRuntimeComponentCounter() { runtimeComponentCounter++; }
	unsigned int getRuntimeComponentCounter() { return runtimeComponentCounter; }
	void handleCollisionEvent(Actor* actorA, Actor* actorB, 
		const CollisionListener::CollisionData& data, bool isBeginContact);
	void processOnDestroyForActor(Actor* actor);
	void processOnDestroyForComponent(Actor* actor, const string& key,
		Actor::Component& component);

private:
	struct ComponentQueueItem {
		string key;
		string type;
		unsigned int actorId;
		shared_ptr<luabridge::LuaRef> instance;
	};

	struct RemoveComponentItem {
		unsigned int actorId;
		string componentKey;
	};

	struct ComponentFunctions {
		bool hasOnStart = false;
		bool hasOnUpdate = false;
		bool hasOnLateUpdate = false;
		bool hasOnDestroy = false;
	};

	lua_State* lua_state = nullptr;
	unordered_map<string, shared_ptr<luabridge::LuaRef>> componentTypes;
	unordered_map<string, ComponentFunctions> componentFunctions;

	vector<unsigned int> actorProcessingOrder;
	unordered_map<unsigned int, vector<ComponentQueueItem>> onStartQueue;

	static inline unsigned int runtimeComponentCounter = 0;
	vector<ComponentQueueItem> componentsToAdd;
	vector<RemoveComponentItem> componentsToRemove;

	void setRigidbodyProperties(Rigidbody* newRigidBody,
		const unordered_map<string, shared_ptr<luabridge::LuaRef>>& properties);
	void setParticleSystemProperties(ParticleSystem* newParticleSystem,
		const unordered_map<string, shared_ptr<luabridge::LuaRef>>& properties);
	void setLightProperties(Light* newLight,
		const unordered_map<string, shared_ptr<luabridge::LuaRef>>& properties);

	void processCollisionForActor(Actor* actor, 
		const string& functionName, const luabridge::LuaRef& collisionData);
};