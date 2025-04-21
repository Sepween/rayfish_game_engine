#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <unordered_set>

#include "rapidjson/document.h"
#include "glm/glm.hpp"

#include "Actor.h"
#include "EngineUtils.h"

class LuaManager;

using namespace std;

class SceneDB {
public:
	static SceneDB& getInstance() {
		static SceneDB instance;
		return instance;
	}

	SceneDB();

	~SceneDB();
	
	void init(const string& initialScene);
	bool loadScene(const string& sceneName);
	bool enterNewScene(const string& sceneName);
	vector<Actor*>& getActors() { return actors; }
	int getNumActors() { return static_cast<int>(actors.size()); }
	unordered_map<unsigned int, Actor*> getActorMap() { return actorMap; }
	Actor* getActorByID(unsigned int id);
	vector<Actor*> getActorsByName(const string& name);
	void createActorFromJson(Actor& actor, const rapidjson::Value& actorJson);
	rapidjson::Document& getTemplate(const string& tempName);

	void queueActorAddition(Actor* newActor);
	void queueActorDestroy(Actor* actorToDelete);
	void processActorAdditions();
	void processActorDestroy();
	void addDontDestroyActor(unsigned int actorId);

private:
	struct SceneDBComponent {
		string type;
		unordered_map<string, shared_ptr<luabridge::LuaRef>> properties;
	};

	vector<Actor*> actors;
	vector<Actor*> actorsToAdd;
	vector<Actor*> actorsToDestroy;
	unordered_map<unsigned int, Actor*> actorMap;
	string curScene;
	unordered_map<string, unique_ptr<rapidjson::Document>> templateCache;
	vector<Actor*> persistentActors;

	bool loadActorsFromJson(const rapidjson::Document& doc);
	void readComponents(Actor& actor, const rapidjson::Value& actorJson,
		rapidjson::Document* templateDoc);
	void collectComponentsFromJson(unordered_map<string, SceneDBComponent>& componentsMap,
		const rapidjson::Value& componentsJson);
	shared_ptr<luabridge::LuaRef> jsonValueToLuaRef(const rapidjson::Value& value);
	void removeActor(Actor* actor);
	void preloadTemplates();
};
