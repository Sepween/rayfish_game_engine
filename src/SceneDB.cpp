#include "SceneDB.h"
#include "LuaManager.h"
#include "EngineUtils.h"

using namespace std;

SceneDB::SceneDB() {
    preloadTemplates();
}

SceneDB::~SceneDB() {
    actorMap.clear();
    for (auto& actor : actors) {
        if (actor) delete actor;
    }
}

void SceneDB::init(const string& initialScene) {
	if (initialScene.empty()) {
		cout << "error: initial_scene unspecified";
		exit(0);
	}
	if (!loadScene(initialScene)) {
		cout << "error: scene " << initialScene << " is missing";
		exit(0);
	}
}

bool SceneDB::loadScene(const string& sceneName) {
    string scenePath = EngineUtils::getScene(sceneName);
    if (!filesystem::exists(scenePath)) {
        return false;
    }

    rapidjson::Document doc;
    EngineUtils::ReadJsonFile(scenePath, doc);

    bool success = loadActorsFromJson(doc);
    vector<Actor*> combinedActors = persistentActors;
    combinedActors.insert(combinedActors.end(), actors.begin(), actors.end());
    actors = combinedActors;
    for (Actor* actor : persistentActors) {
        actorMap[actor->id] = actor;
    }
    if (success) curScene = sceneName;

    return success;
}

bool SceneDB::enterNewScene(const string& sceneName) {
    for (auto& actor : actors) {
        if (!actor->dontDestroy) {
            LuaManager::getInstance().processOnDestroyForActor(actor);
            delete actor;
        }
    }

    actorMap.clear();
    actors.clear();

    if (!loadScene(sceneName)) {
        return false;
    }
    return true;
}

bool SceneDB::loadActorsFromJson(const rapidjson::Document& doc) {
    actors.clear();
    actorMap.clear();

    if (!doc.HasMember("actors") || !doc["actors"].IsArray()) {
        return false;
    }
    
    const rapidjson::Value& actorsArray = doc["actors"];
    actors.reserve(actorsArray.Size());
    for (const auto& actorJson : actorsArray.GetArray()) {
        Actor* new_actor = new Actor();
        createActorFromJson(*new_actor, actorJson);
        actors.push_back(new_actor);
        actorMap[new_actor->id] = new_actor;
    }
    
    return true;
}

void SceneDB::createActorFromJson(Actor& actor, const rapidjson::Value& actorJson) {
    rapidjson::Document* templateDoc = nullptr;
    if (actorJson.HasMember("template") && actorJson["template"].IsString()) {
        actor.temp = actorJson["template"].GetString();
        templateDoc = &getTemplate(actor.temp);
        rapidjson::Value& templateValue = *templateDoc;
    }
    if (!EngineUtils::loadFromDoc(actorJson, "name", actor.name) && (actor.temp != "")) {
        EngineUtils::loadFromDoc(*templateDoc, "name", actor.name);
    }
    readComponents(actor, actorJson, templateDoc);
}

rapidjson::Document& SceneDB::getTemplate(const string& tempName) {
    if (templateCache.find(tempName) == templateCache.end()) {
        cout << "error: template " << tempName << " is missing";
        exit(0);
    }
    return *templateCache[tempName];
}

void SceneDB::readComponents(Actor& actor, const rapidjson::Value& actorJson,
    rapidjson::Document* templateDoc) {
    unordered_map<string, SceneDBComponent> componentsMap;

    if (templateDoc && templateDoc->HasMember("components") && (*templateDoc)["components"].IsObject()) {
        collectComponentsFromJson(componentsMap, (*templateDoc)["components"]);
    }

    if (actorJson.HasMember("components") && actorJson["components"].IsObject()) {
        collectComponentsFromJson(componentsMap, actorJson["components"]);
    }

    for (auto& [key, data] : componentsMap) {
        if (!data.type.empty()) {
            LuaManager::getInstance().addComponentToActor(&actor, key, data.type, data.properties);
        }
    }
}

void SceneDB::collectComponentsFromJson(unordered_map<string, SceneDBComponent>& componentsMap,
    const rapidjson::Value& componentsJson){
    for (auto it = componentsJson.MemberBegin(); it != componentsJson.MemberEnd(); ++it) {
        string componentKey = it->name.GetString();
        
        if (componentsMap.find(componentKey) == componentsMap.end()) {
            // current component doesn't exist yet
            componentsMap[componentKey] = SceneDBComponent();
        }

        if (it->value.IsObject()) {
            if (it->value.HasMember("type") && it->value["type"].IsString()) {
                // current component in current json has a type
                componentsMap[componentKey].type = it->value["type"].GetString();
            }
            for (auto propIt = it->value.MemberBegin(); propIt != it->value.MemberEnd(); ++propIt) {
                string propName = propIt->name.GetString();
                if (propName != "type") {
                    shared_ptr<luabridge::LuaRef> propValue = jsonValueToLuaRef(propIt->value);
                    componentsMap[componentKey].properties[propName] = propValue;
                }
            }
        }
    }
}

shared_ptr<luabridge::LuaRef> SceneDB::jsonValueToLuaRef(const rapidjson::Value& value) {
    lua_State* lua_state = LuaManager::getInstance().getLuaState();

    if (value.IsString()) {
        return make_shared<luabridge::LuaRef>(lua_state, value.GetString());
    }
    else if (value.IsNumber()) {
        if (value.IsFloat()) {
            return make_shared<luabridge::LuaRef>(lua_state, value.GetFloat());
        }
        else {
            return make_shared<luabridge::LuaRef>(lua_state, value.GetInt());
        }
    }
    else if (value.IsBool()) {
        return make_shared<luabridge::LuaRef>(lua_state, value.GetBool());
    }
    else if (value.IsArray()) {
        luabridge::LuaRef array = luabridge::newTable(lua_state);
        for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
            array[i + 1] = jsonValueToLuaRef(value[i]);
        }
        return make_shared<luabridge::LuaRef>(array);
    }
    else if (value.IsObject()) {
        luabridge::LuaRef obj = luabridge::newTable(lua_state);
        for (auto it = value.MemberBegin(); it != value.MemberEnd(); ++it) {
            obj[it->name.GetString()] = jsonValueToLuaRef(it->value);
        }
        return make_shared<luabridge::LuaRef>(obj);
    }

    return make_shared<luabridge::LuaRef>(lua_state);
}

Actor* SceneDB::getActorByID(unsigned int id) {
    return actorMap[id];
}

vector<Actor*> SceneDB::getActorsByName(const string& name){
    vector<Actor*> matchingActors;
    matchingActors.reserve(actors.size() + actorsToAdd.size());

    for (Actor* actor : actors) {
        if (actor->name == name && !actor->toDestroy) {
            matchingActors.push_back(const_cast<Actor*>(actor));
        }
    }

    for (Actor* actor : actorsToAdd) {
        if (actor->name == name && !actor->toDestroy) {
            matchingActors.push_back(const_cast<Actor*>(actor));
        }
    }
    return matchingActors;
}

void SceneDB::queueActorAddition(Actor* newActor) {
    actorMap[newActor->id] = newActor;

    actorsToAdd.push_back(newActor);
}

void SceneDB::queueActorDestroy(Actor* actorToDelete) {
    actorsToDestroy.push_back(actorToDelete);
}

void SceneDB::processActorAdditions() {
    if (actorsToAdd.empty()) {
        return;
    }

    for (Actor* actor : actorsToAdd) {
        actors.push_back(actor);
    }

    actorsToAdd.clear();
}

void SceneDB::processActorDestroy() {
    if (actorsToDestroy.empty()) {
        return;
    }

    for (Actor* actor : actorsToDestroy) {
        removeActor(actor);
    }

    actorsToDestroy.clear();
}

void SceneDB::removeActor(Actor* actor) {
    if (!actor) return;

    actorMap.erase(actor->id);

    unordered_set<Actor*> actorSet(actors.begin(), actors.end());
    if (actorSet.count(actor)) {
        actors.erase(remove(actors.begin(), actors.end(), actor), actors.end());
    }

    delete actor;
}

void SceneDB::preloadTemplates() {
    filesystem::path templateDir = filesystem::path(EngineUtils::getTemplateFolder());

    if (filesystem::exists(templateDir) && filesystem::is_directory(templateDir)) {
        for (const auto& entry : filesystem::directory_iterator(templateDir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".template") {
                string templateName = entry.path().stem().string();
                string templatePath = entry.path().string();

                unique_ptr<rapidjson::Document> doc = make_unique<rapidjson::Document>();
                EngineUtils::ReadJsonFile(templatePath, *doc);

                templateCache[templateName] = move(doc);
            }
        }
    }
}

void SceneDB::addDontDestroyActor(unsigned int actorId) {
    persistentActors.push_back(actorMap[actorId]);
}