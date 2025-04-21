#include "Actor.h"
#include "LuaManager.h"

luabridge::LuaRef Actor::GetComponentByKey(const string& key) {
	auto it = components.find(key);
	if (it != components.end() && (*it->second.instance)["enabled"]) {
		return *(it->second.instance);
	}
	return luabridge::LuaRef(LuaManager::getInstance().getLuaState());
}

luabridge::LuaRef Actor::GetComponent(const string& typeName) {
	vector<pair<string, Component*>> matchingComponents;
	for (auto& [key, component] : components) {
		if (component.type == typeName && (*component.instance)["enabled"]) {
			matchingComponents.push_back(make_pair(key, &component));
		}
	}

	if (!matchingComponents.empty()) {
		sort(matchingComponents.begin(), matchingComponents.end(),
			[](const auto& a, const auto& b) {
				return a.first < b.first;
			});

		return *(matchingComponents[0].second->instance);
	}

	return luabridge::LuaRef(LuaManager::getInstance().getLuaState());
}

luabridge::LuaRef Actor::GetComponents(const string& typeName) {
	vector<pair<string, Component*>> matchingComponents;
	for (auto& [key, component] : components) {
		if (component.type == typeName && (*component.instance)["enabled"]) {
			matchingComponents.push_back(make_pair(key, &component));
		}
	}

	sort(matchingComponents.begin(), matchingComponents.end(),
		[](const auto& a, const auto& b) {
			return a.first < b.first;
		});

	lua_State* state = LuaManager::getInstance().getLuaState();
	luabridge::LuaRef table = luabridge::newTable(state);
	for (int i = 0; i < matchingComponents.size(); ++i) {
		table[i + 1] = *(matchingComponents[i].second->instance);
	}

	return table;
}

luabridge::LuaRef Actor::AddComponent(const string& typeName) {
	unsigned int counter = LuaManager::getInstance().getRuntimeComponentCounter();
	LuaManager::getInstance().addRuntimeComponentCounter();

	string key = "r" + to_string(counter);

	unordered_map<string, shared_ptr<luabridge::LuaRef>> emptyProperties;

	shared_ptr<luabridge::LuaRef> instance = LuaManager::getInstance().
		createComponentInstance(typeName, key, emptyProperties, this);

	LuaManager::getInstance().queueComponentAddition(key, typeName, id, instance);

	return *instance;
}

void Actor::RemoveComponent(luabridge::LuaRef component) {
	string componentKey = "";

	if (component.isTable()) {
		if (component["enabled"].isBool()) {
			component["enabled"] = false;
		}
		if (component["key"].isString()) {
			componentKey = component["key"].cast<string>();
		}
	}
	else if (component.isUserdata()) {
		if (component.isInstance<Rigidbody>()) {
			Rigidbody* rb = component.cast<Rigidbody*>();
			if (rb) {
				componentKey = rb->GetKey();
				rb->SetEnabled(false);
			}
		}
	}

	if (componentKey != "") {
		auto it = components.find(componentKey);
		if (it != components.end()) {
			LuaManager::getInstance().processOnDestroyForComponent(this, componentKey, it->second);
			LuaManager::getInstance().queueComponentRemoval(id, componentKey);
		}
	}
}