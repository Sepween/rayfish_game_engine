#include "Light.h"
#include "LightSystem.h"

void Light::OnStart() {
	LightSystem::getInstance().RegisterLight(this);
}

void Light::OnUpdate() {
	if (actor) {
		luabridge::LuaRef rb = actor->GetComponent("Rigidbody");
		if (!rb.isNil() && rb["x"].isNumber() && rb["y"].isNumber()) {
			position.x = rb["x"].cast<float>();
			position.y = rb["y"].cast<float>();
		}
		else {
			luabridge::LuaRef transform = actor->GetComponent("Transform");
			if (!transform.isNil() && transform["x"].isNumber() && transform["y"].isNumber()) {
				position.x = transform["x"].cast<float>();
				position.y = transform["y"].cast<float>();
			}
		}
	}
}

void Light::OnDestroy() {
	LightSystem::getInstance().UnregisterLight(this);
}