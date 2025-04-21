#include "LuaManager.h"
#include "SceneDB.h"
#include "Game.h"
#include "EngineUtils.h"
#include "Physics.h"
#include "EventBus.h"

using namespace std;

void LuaManager::init(lua_State* state) {
	lua_state = state;
	registerClasses();
	registerFunctions();
}

void LuaManager::registerClasses() {
	luabridge::getGlobalNamespace(lua_state)
		.beginClass<Actor>("Actor")
			.addFunction("GetName", &Actor::getName)
			.addFunction("GetID", &Actor::getID)
			.addFunction("GetComponentByKey", &Actor::GetComponentByKey)
			.addFunction("GetComponent", &Actor::GetComponent)
			.addFunction("GetComponents", &Actor::GetComponents)
			.addFunction("AddComponent", &Actor::AddComponent)
			.addFunction("RemoveComponent", &Actor::RemoveComponent)
		.endClass()
		.beginClass<glm::vec2>("vec2")
			.addProperty("x", &glm::vec2::x)
			.addProperty("y", &glm::vec2::y)
		.endClass()
		.beginClass<b2Vec2>("Vector2")
			.addConstructor<void(*) (float, float)>()
			.addProperty("x", &b2Vec2::x)
			.addProperty("y", &b2Vec2::y)
			.addFunction("Normalize", &b2Vec2::Normalize)
			.addFunction("Length", &b2Vec2::Length)
			.addFunction("__add", &b2Vec2::operator_add)
			.addFunction("__sub", &b2Vec2::operator_sub)
			.addFunction("__mul", &b2Vec2::operator_mul)
			.addStaticFunction("Distance", &b2Distance)
			.addStaticFunction("Dot", static_cast<float (*)(const b2Vec2&, const b2Vec2&)>(&b2Dot))
		.endClass()
		.beginClass<Rigidbody>("Rigidbody")
			.addConstructor<void(*)()>()
			.addFunction("GetPosition", &Rigidbody::GetPosition)
			.addFunction("GetRotation", &Rigidbody::GetRotation)
			.addFunction("OnStart", &Rigidbody::OnStart)
			.addFunction("OnDestroy", &Rigidbody::OnDestroy)
			.addStaticFunction("StepWorld", &Rigidbody::StepWorld)
			.addProperty("key", &Rigidbody::GetKey, &Rigidbody::SetKey)
			.addProperty("enabled", &Rigidbody::GetEnabled, &Rigidbody::SetEnabled)
			.addProperty("actor", &Rigidbody::GetActor, &Rigidbody::SetActor)
			.addProperty("x", &Rigidbody::GetX, &Rigidbody::SetX)
			.addProperty("y", &Rigidbody::GetY, &Rigidbody::SetY)
			.addProperty("velocity", &Rigidbody::GetVelocity, &Rigidbody::SetVelocity)
			.addProperty("angular_velocity", &Rigidbody::GetAngularVelocity, &Rigidbody::SetAngularVelocity)
			.addProperty("gravity_scale", &Rigidbody::GetGravityScale, &Rigidbody::SetGravityScale)
			.addProperty("up_direction", &Rigidbody::GetUpDirection, &Rigidbody::SetUpDirection)
			.addProperty("right_direction", &Rigidbody::GetRightDirection, &Rigidbody::SetRightDirection)
			.addProperty("rotation", &Rigidbody::GetRotation, &Rigidbody::SetRotation)
			.addFunction("AddForce", &Rigidbody::AddForce)
			.addFunction("SetVelocity", &Rigidbody::SetVelocity)
			.addFunction("SetPosition", &Rigidbody::SetPosition)
			.addFunction("SetRotation", &Rigidbody::SetRotation)
			.addFunction("SetAngularVelocity", &Rigidbody::SetAngularVelocity)
			.addFunction("SetGravityScale", &Rigidbody::SetGravityScale)
			.addFunction("SetUpDirection", &Rigidbody::SetUpDirection)
			.addFunction("SetRightDirection", &Rigidbody::SetRightDirection)
			.addFunction("GetVelocity", &Rigidbody::GetVelocity)
			.addFunction("GetAngularVelocity", &Rigidbody::GetAngularVelocity)
			.addFunction("GetGravityScale", &Rigidbody::GetGravityScale)
			.addFunction("GetUpDirection", &Rigidbody::GetUpDirection)
			.addFunction("GetRightDirection", &Rigidbody::GetRightDirection)
		.endClass()
		.beginClass<ParticleSystem>("ParticleSystem")
			.addConstructor<void(*)()>()
			.addFunction("OnStart", &ParticleSystem::onStart)
			.addFunction("OnUpdate", &ParticleSystem::onUpdate)
			.addProperty("key", &ParticleSystem::GetKey, &ParticleSystem::SetKey)
			.addProperty("enabled", &ParticleSystem::GetEnabled, &ParticleSystem::SetEnabled)
			.addProperty("actor", &ParticleSystem::GetActor, &ParticleSystem::SetActor)
			.addProperty("x", &ParticleSystem::GetX, &ParticleSystem::SetX)
			.addProperty("y", &ParticleSystem::GetY, &ParticleSystem::SetY)
			.addProperty("frames_between_bursts", &ParticleSystem::GetBurstInterval, &ParticleSystem::SetBurstInterval)
			.addProperty("burst_quantity", &ParticleSystem::GetPixelPerBurst, &ParticleSystem::SetPixelPerBurst)
			.addProperty("start_scale_min", &ParticleSystem::GetScaleMin, &ParticleSystem::SetScaleMin)
			.addProperty("start_scale_max", &ParticleSystem::GetScaleMax, &ParticleSystem::SetScaleMax)
			.addProperty("rotation_min", &ParticleSystem::GetRotationMin, &ParticleSystem::SetRotationMin)
			.addProperty("rotation_max", &ParticleSystem::GetRotationMax, &ParticleSystem::SetRotationMax)
			.addProperty("emit_radius_min", &ParticleSystem::GetRadiusMin, &ParticleSystem::SetRadiusMin)
			.addProperty("emit_radius_max", &ParticleSystem::GetRadiusMax, &ParticleSystem::SetRadiusMax)
			.addProperty("emit_angle_min", &ParticleSystem::GetAngleMin, &ParticleSystem::SetAngleMin)
			.addProperty("emit_angle_max", &ParticleSystem::GetAngleMax, &ParticleSystem::SetAngleMax)
			.addProperty("start_speed_min", &ParticleSystem::GetSpeedMin, &ParticleSystem::SetSpeedMin)
			.addProperty("start_speed_max", &ParticleSystem::GetSpeedMax, &ParticleSystem::SetSpeedMax)
			.addProperty("rotation_speed_min", &ParticleSystem::GetRotationSpeedMin, &ParticleSystem::SetRotationSpeedMin)
			.addProperty("rotation_speed_max", &ParticleSystem::GetRotationSpeedMax, &ParticleSystem::SetRotationSpeedMax)
			.addProperty("start_color_r", &ParticleSystem::GetR, &ParticleSystem::SetR)
			.addProperty("start_color_g", &ParticleSystem::GetG, &ParticleSystem::SetG)
			.addProperty("start_color_b", &ParticleSystem::GetB, &ParticleSystem::SetB)
			.addProperty("start_color_a", &ParticleSystem::GetA, &ParticleSystem::SetA)
			.addProperty("image", &ParticleSystem::GetImage, &ParticleSystem::SetImage)
			.addProperty("sorting_order", &ParticleSystem::GetSortingOrder, &ParticleSystem::SetSortingOrder)
			.addProperty("duration_frames", &ParticleSystem::GetDurationFrames, &ParticleSystem::SetDurationFrames)
			.addProperty("gravity_scale_x", &ParticleSystem::GetGravityScaleX, &ParticleSystem::SetGravityScaleX)
			.addProperty("gravity_scale_y", &ParticleSystem::GetGravityScaleY, &ParticleSystem::SetGravityScaleY)
			.addProperty("drag_factor", &ParticleSystem::GetDrag, &ParticleSystem::SetDrag)
			.addProperty("angular_drag_factor", &ParticleSystem::GetAngularDrag, &ParticleSystem::SetAngularDrag)
			.addProperty("end_scale", &ParticleSystem::GetEndScale, &ParticleSystem::SetEndScale)
			.addProperty("end_color_r", &ParticleSystem::GetEndColorR, &ParticleSystem::SetEndColorR)
			.addProperty("end_color_g", &ParticleSystem::GetEndColorG, &ParticleSystem::SetEndColorG)
			.addProperty("end_color_b", &ParticleSystem::GetEndColorB, &ParticleSystem::SetEndColorB)
			.addProperty("end_color_a", &ParticleSystem::GetEndColorA, &ParticleSystem::SetEndColorA)
			.addFunction("GetPosition", &ParticleSystem::GetPosition)
			.addFunction("SetPosition", &ParticleSystem::SetPosition)
			.addFunction("Stop", &ParticleSystem::Stop)
			.addFunction("Play", &ParticleSystem::Play)
			.addFunction("Burst", &ParticleSystem::Burst)
		.endClass()
		.beginClass<Light>("Light")
			.addConstructor<void(*)()>()
			.addFunction("OnStart", &Light::OnStart)
			.addFunction("OnUpdate", &Light::OnUpdate)
			.addFunction("OnDestroy", &Light::OnDestroy)
			.addProperty("key", &Light::GetKey, &Light::SetKey)
			.addProperty("enabled", &Light::GetEnabled, &Light::SetEnabled)
			.addProperty("actor", &Light::GetActor, &Light::SetActor)
			.addProperty("x", &Light::GetX, &Light::SetX)
			.addProperty("y", &Light::GetY, &Light::SetY)
			.addFunction("GetPosition", &Light::GetPosition)
			.addFunction("SetPosition", &Light::SetPosition)
			.addProperty("intensity", &Light::GetIntensity, &Light::SetIntensity)
			.addProperty("radius", &Light::GetRadius, &Light::SetRadius)
			.addProperty("direction", &Light::GetDirection, &Light::SetDirection)
			.addProperty("angle", &Light::GetAngle, &Light::SetAngle)
			.addProperty("cast_shadows", &Light::GetCastShadows, &Light::SetCastShadows)
			.addProperty("num_rays", &Light::GetNumRays, &Light::SetNumRays)
			.addProperty("light_type", &Light::GetTypeStr, &Light::SetType)
			.addProperty("r", &Light::GetR, &Light::SetR)
			.addProperty("g", &Light::GetG, &Light::SetG)
			.addProperty("b", &Light::GetB, &Light::SetB)
			.addProperty("a", &Light::GetA, &Light::SetA)
		.endClass();
}

void LuaManager::registerFunctions() {
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Debug")
			.addFunction("Log", &LuaManager::debugLog)
		.endNamespace()
		.beginNamespace("Actor")
			.addFunction("Find", &LuaManager::findActor)
			.addFunction("FindAll", &LuaManager::findAllActor)
			.addFunction("Instantiate", &LuaManager::instantiateActor)
			.addFunction("Destroy", &LuaManager::destroyActor)
		.endNamespace()
		.beginNamespace("Application")
			.addFunction("Quit", &LuaManager::quit)
			.addFunction("Sleep", &LuaManager::sleep)
			.addFunction("GetFrame", &LuaManager::getFrame)
			.addFunction("OpenURL", &LuaManager::openURL)
		.endNamespace()
		.beginNamespace("Input")
			.addFunction("GetKey", &Input::GetKey)
			.addFunction("GetKeyDown", &Input::GetKeyDown)
			.addFunction("GetKeyUp", &Input::GetKeyUp)
			.addFunction("GetMousePosition", &Input::GetMousePosition)
			.addFunction("GetMouseButton", &Input::GetMouseButton)
			.addFunction("GetMouseButtonDown", &Input::GetMouseButtonDown)
			.addFunction("GetMouseButtonUp", &Input::GetMouseButtonUp)
			.addFunction("GetMouseScrollDelta", &Input::GetMouseScrollDelta)
			.addFunction("HideCursor", &Input::HideCursor)
			.addFunction("ShowCursor", &Input::ShowCursor)
		.endNamespace()
		.beginNamespace("Text")
			.addFunction("Draw", &TextDB::Draw)
		.endNamespace()
		.beginNamespace("Audio")
			.addFunction("Play", &AudioDB::Play)
			.addFunction("Halt", &AudioDB::Halt)
			.addFunction("SetVolume", &AudioDB::SetVolume)
		.endNamespace()
		.beginNamespace("Image")
			.addFunction("DrawUI", &ImageDB::DrawUI)
			.addFunction("DrawUIEx", &ImageDB::DrawUIEx)
			.addFunction("Draw", &ImageDB::Draw)
			.addFunction("DrawEx", &ImageDB::DrawEx)
			.addFunction("DrawPixel", &ImageDB::DrawPixel)
		.endNamespace()
		.beginNamespace("Camera")
			.addFunction("SetPosition", &Renderer::SetPosition)
			.addFunction("GetPositionX", &Renderer::GetPositionX)
			.addFunction("GetPositionY", &Renderer::GetPositionY)
			.addFunction("SetZoom", &Renderer::SetZoom)
			.addFunction("GetZoom", &Renderer::GetZoom)
		.endNamespace()
		.beginNamespace("Scene")
			.addFunction("Load", &Game::Load)
			.addFunction("GetCurrent", &Game::GetCurrent)
			.addFunction("DontDestroy", &Game::DontDestroy)
		.endNamespace()
		.beginNamespace("Physics")
			.addFunction("Raycast", &Physics::Raycast)
			.addFunction("RaycastAll", &Physics::RaycastAll)
		.endNamespace()
		.beginNamespace("Event")
			.addFunction("Publish", &EventBus::Publish)
			.addFunction("Subscribe", &EventBus::Subscribe)
			.addFunction("Unsubscribe", &EventBus::Unsubscribe)
		.endNamespace();
}

void LuaManager::loadComponentType(const string& componentType) {
	auto it = componentTypes.find(componentType);
	if (it != componentTypes.end()) {
		return;
	}

	if (componentType == "Rigidbody" || componentType == "ParticleSystem" || componentType == "Light") {
		ComponentFunctions functions;
		functions.hasOnStart = true;
		functions.hasOnUpdate = (componentType == "ParticleSystem" || componentType == "Light");
		functions.hasOnLateUpdate = false;
		functions.hasOnDestroy = (componentType == "Rigidbody" || componentType == "Light");
		componentFunctions[componentType] = functions;
		
		componentTypes[componentType] = make_shared<luabridge::LuaRef>(
			luabridge::newTable(lua_state));
		return;
	}

	filesystem::path componentPath = "resources/component_types/" + componentType + ".lua";

	if (!filesystem::exists(componentPath)) {
		cout << "error: failed to locate component " + componentType;
		exit(0);
	}
	
	if (luaL_dofile(lua_state, componentPath.string().c_str()) != LUA_OK) {
		cout << "problem with lua file " << componentPath.stem().string();
		exit(0);
	}

	luabridge::LuaRef typeTable = luabridge::getGlobal(lua_state, componentType.c_str());
	componentTypes[componentType] = make_shared<luabridge::LuaRef>(typeTable);

	ComponentFunctions functions;
	functions.hasOnStart = typeTable["OnStart"].isFunction();
	functions.hasOnUpdate = typeTable["OnUpdate"].isFunction();
	functions.hasOnLateUpdate = typeTable["OnLateUpdate"].isFunction();
	functions.hasOnDestroy = typeTable["OnDestroy"].isFunction();
	componentFunctions[componentType] = functions;
}

void LuaManager::establishInheritance(luabridge::LuaRef& instanceTable, luabridge::LuaRef& baseTable) {
	luabridge::LuaRef new_metatable = luabridge::newTable(lua_state);
	new_metatable["__index"] = baseTable;

	instanceTable.push(lua_state);
	new_metatable.push(lua_state);
	lua_setmetatable(lua_state, -2);
	lua_pop(lua_state, 1);
}

shared_ptr<luabridge::LuaRef> LuaManager::createComponentInstance(const string& componentType, const string& key,
	const unordered_map<string, shared_ptr<luabridge::LuaRef>>& properties, const Actor* actor) {
	loadComponentType(componentType);

	if (componentType == "Rigidbody") {
		Rigidbody* newRigidbody = new Rigidbody();
		newRigidbody->SetKey(key);
		newRigidbody->SetActor(const_cast<Actor*>(actor));
		newRigidbody->SetEnabled(true);
		
		setRigidbodyProperties(newRigidbody, properties);

		shared_ptr<luabridge::LuaRef> instance = make_shared<luabridge::LuaRef>(
			luabridge::LuaRef(lua_state, newRigidbody));
		return instance;
	}
	else if (componentType == "ParticleSystem") {
		ParticleSystem* newParticleSystem = new ParticleSystem();
		newParticleSystem->SetKey(key);
		newParticleSystem->SetActor(const_cast<Actor*>(actor));
		newParticleSystem->SetEnabled(true);

		setParticleSystemProperties(newParticleSystem, properties);

		shared_ptr<luabridge::LuaRef> instance = make_shared<luabridge::LuaRef>(
			luabridge::LuaRef(lua_state, newParticleSystem));
		return instance;
	}
	else if (componentType == "Light") {
		Light* newLight = new Light();
		newLight->SetKey(key);
		newLight->SetActor(const_cast<Actor*>(actor));
		newLight->SetEnabled(true);

		setLightProperties(newLight, properties);

		shared_ptr<luabridge::LuaRef> instance = make_shared<luabridge::LuaRef>(
			luabridge::LuaRef(lua_state, newLight));
		return instance;
	}

	luabridge::LuaRef instanceTable = luabridge::newTable(lua_state);
	instanceTable["key"] = key;
	instanceTable["actor"] = const_cast<Actor*>(actor);
	instanceTable["enabled"] = true;

	auto baseTablePtr = componentTypes[componentType];
	
	establishInheritance(instanceTable, *baseTablePtr);

	for (auto& [name, value] : properties) {
		instanceTable[name] = *value;
	}

	return make_shared<luabridge::LuaRef>(instanceTable);
}

void LuaManager::setRigidbodyProperties(Rigidbody* newRigidBody,
	const unordered_map<string, shared_ptr<luabridge::LuaRef>>& properties) {
	for (auto& [name, value] : properties) {
		if (name == "body_type" && (*value).isString()) {
			newRigidBody->InitSetBodyType((*value).cast<string>());
		}
		else if (name == "x" && (*value).isNumber()) {
			newRigidBody->InitSetPositionX((*value).cast<float>());
		}
		else if (name == "y" && (*value).isNumber()) {
			newRigidBody->InitSetPositionY((*value).cast<float>());
		}
		else if (name == "rotation" && (*value).isNumber()) {
			newRigidBody->InitSetRotation((*value).cast<float>());
		}
		else if (name == "collider_type" && (*value).isString()) {
			newRigidBody->InitSetColliderType((*value).cast<string>());
		}
		else if (name == "width" && (*value).isNumber()) {
			newRigidBody->InitSetWidth((*value).cast<float>());
		}
		else if (name == "height" && (*value).isNumber()) {
			newRigidBody->InitSetHeight((*value).cast<float>());
		}
		else if (name == "radius" && (*value).isNumber()) {
			newRigidBody->InitSetRadius((*value).cast<float>());
		}
		else if (name == "density" && (*value).isNumber()) {
			newRigidBody->InitSetDensity((*value).cast<float>());
		}
		else if (name == "friction" && (*value).isNumber()) {
			newRigidBody->InitSetFriction((*value).cast<float>());
		}
		else if (name == "bounciness" && (*value).isNumber()) {
			newRigidBody->InitSetBounciness((*value).cast<float>());
		}
		else if (name == "gravity_scale" && (*value).isNumber()) {
			newRigidBody->InitSetGravityScale((*value).cast<float>());
		}
		else if (name == "angular_friction" && (*value).isNumber()) {
			newRigidBody->InitSetAngularFriction((*value).cast<float>());
		}
		else if (name == "precise" && (*value).isBool()) {
			newRigidBody->InitSetPrecise((*value).cast<bool>());
		}
		else if (name == "has_collider" && (*value).isBool()) {
			newRigidBody->InitSetHasCollider((*value).cast<bool>());
		}
		else if (name == "has_trigger" && (*value).isBool()) {
			newRigidBody->InitSetHasTrigger((*value).cast<bool>());
		}
		else if (name == "trigger_type" && (*value).isString()) {
			newRigidBody->InitSetTriggerType((*value).cast<string>());
		}
		else if (name == "trigger_width" && (*value).isNumber()) {
			newRigidBody->InitSetTriggerWidth((*value).cast<float>());
		}
		else if (name == "trigger_height" && (*value).isNumber()) {
			newRigidBody->InitSetTriggerHeight((*value).cast<float>());
		}
		else if (name == "trigger_radius" && (*value).isNumber()) {
			newRigidBody->InitSetTriggerRadius((*value).cast<float>());
		}
	}
}

void LuaManager::setParticleSystemProperties(ParticleSystem* newParticleSystem,
	const unordered_map<string, shared_ptr<luabridge::LuaRef>>& properties) {
	for (auto& [name, value] : properties) {
		if (name == "x" && (*value).isNumber()) {
			newParticleSystem->SetX((*value).cast<float>());
		}
		else if (name == "y" && (*value).isNumber()) {
			newParticleSystem->SetY((*value).cast<float>());
		}
		else if (name == "frames_between_bursts" && (*value).isNumber()) {
			newParticleSystem->SetBurstInterval((*value).cast<int>());
		}
		else if (name == "burst_quantity" && (*value).isNumber()) {
			newParticleSystem->SetPixelPerBurst((*value).cast<int>());
		}
		else if (name == "start_scale_min" && (*value).isNumber()) {
			newParticleSystem->SetScaleMin((*value).cast<float>());
		}
		else if (name == "start_scale_max" && (*value).isNumber()) {
			newParticleSystem->SetScaleMax((*value).cast<float>());
		}
		else if (name == "rotation_min" && (*value).isNumber()) {
			newParticleSystem->SetRotationMin((*value).cast<float>());
		}
		else if (name == "rotation_max" && (*value).isNumber()) {
			newParticleSystem->SetRotationMax((*value).cast<float>());
		}
		else if (name == "start_color_r" && (*value).isNumber()) {
			newParticleSystem->SetR((*value).cast<int>());
		}
		else if (name == "start_color_g" && (*value).isNumber()) {
			newParticleSystem->SetG((*value).cast<int>());
		}
		else if (name == "start_color_b" && (*value).isNumber()) {
			newParticleSystem->SetB((*value).cast<int>());
		}
		else if (name == "start_color_a" && (*value).isNumber()) {
			newParticleSystem->SetA((*value).cast<int>());
		}
		else if (name == "emit_radius_min" && (*value).isNumber()) {
			newParticleSystem->SetRadiusMin((*value).cast<float>());
		}
		else if (name == "emit_radius_max" && (*value).isNumber()) {
			newParticleSystem->SetRadiusMax((*value).cast<float>());
		}
		else if (name == "emit_angle_min" && (*value).isNumber()) {
			newParticleSystem->SetAngleMin((*value).cast<float>());
		}
		else if (name == "emit_angle_max" && (*value).isNumber()) {
			newParticleSystem->SetAngleMax((*value).cast<float>());
		}
		else if (name == "start_speed_min" && (*value).isNumber()) {
			newParticleSystem->SetSpeedMin((*value).cast<float>());
		}
		else if (name == "start_speed_max" && (*value).isNumber()) {
			newParticleSystem->SetSpeedMax((*value).cast<float>());
		}
		else if (name == "rotation_speed_min" && (*value).isNumber()) {
			newParticleSystem->SetRotationSpeedMin((*value).cast<float>());
		}
		else if (name == "rotation_speed_max" && (*value).isNumber()) {
			newParticleSystem->SetRotationSpeedMax((*value).cast<float>());
		}
		else if (name == "image" && (*value).isString()) {
			newParticleSystem->SetImage((*value).cast<string>());
		}
		else if (name == "sorting_order" && (*value).isNumber()) {
			newParticleSystem->SetSortingOrder((*value).cast<int>());
		}
		else if (name == "duration_frames" && (*value).isNumber()) {
			newParticleSystem->SetDurationFrames((*value).cast<int>());
		}
		else if (name == "gravity_scale_x" && (*value).isNumber()) {
			newParticleSystem->SetGravityScaleX((*value).cast<float>());
		}
		else if (name == "gravity_scale_y" && (*value).isNumber()) {
			newParticleSystem->SetGravityScaleY((*value).cast<float>());
		}
		else if (name == "drag_factor" && (*value).isNumber()) {
			newParticleSystem->SetDrag((*value).cast<float>());
		}
		else if (name == "angular_drag_factor" && (*value).isNumber()) {
			newParticleSystem->SetAngularDrag((*value).cast<float>());
		}
		else if (name == "end_scale" && (*value).isNumber()) {
			newParticleSystem->SetEndScale((*value).cast<float>());
		}
		else if (name == "end_color_r" && (*value).isNumber()) {
			newParticleSystem->SetEndColorR((*value).cast<int>());
		}
		else if (name == "end_color_g" && (*value).isNumber()) {
			newParticleSystem->SetEndColorG((*value).cast<int>());
		}
		else if (name == "end_color_b" && (*value).isNumber()) {
			newParticleSystem->SetEndColorB((*value).cast<int>());
		}
		else if (name == "end_color_a" && (*value).isNumber()) {
			newParticleSystem->SetEndColorA((*value).cast<int>());
		}
	}
}

void LuaManager::setLightProperties(Light* newLight,
	const unordered_map<string, shared_ptr<luabridge::LuaRef>>& properties) {
	for (auto& [name, value] : properties) {
		if (name == "x" && (*value).isNumber()) {
			newLight->SetX((*value).cast<float>());
		}
		else if (name == "y" && (*value).isNumber()) {
			newLight->SetY((*value).cast<float>());
		}
		else if (name == "intensity" && (*value).isNumber()) {
			newLight->SetIntensity((*value).cast<float>());
		}
		else if (name == "radius" && (*value).isNumber()) {
			newLight->SetRadius((*value).cast<float>());
		}
		else if (name == "direction" && (*value).isTable()) {
			float x = 0.0f, y = 1.0f;
			if ((*value)["x"].isNumber()) x = (*value)["x"].cast<float>();
			if ((*value)["y"].isNumber()) y = (*value)["y"].cast<float>();
			newLight->SetDirection(b2Vec2(x, y));
		}
		else if (name == "angle" && (*value).isNumber()) {
			newLight->SetAngle((*value).cast<float>());
		}
		else if (name == "light_type" && (*value).isString()) {
			newLight->SetType((*value).cast<string>());
		}
		else if (name == "cast_shadows" && (*value).isBool()) {
			newLight->SetCastShadows((*value).cast<bool>());
		}
		else if (name == "num_rays" && (*value).isNumber()) {
			newLight->SetNumRays((*value).cast<int>());
		}
		else if (name == "color" && (*value).isTable()) {
			SDL_Color color = { 255, 255, 255, 255 };
			if ((*value)["r"].isNumber()) color.r = static_cast<Uint8>((*value)["r"].cast<int>());
			if ((*value)["g"].isNumber()) color.g = static_cast<Uint8>((*value)["g"].cast<int>());
			if ((*value)["b"].isNumber()) color.b = static_cast<Uint8>((*value)["b"].cast<int>());
			if ((*value)["a"].isNumber()) color.a = static_cast<Uint8>((*value)["a"].cast<int>());
			newLight->SetColor(color);
		}
		else if (name == "r" && (*value).isNumber()) {
			newLight->SetR((*value).cast<Uint8>());
		}
		else if (name == "g" && (*value).isNumber()) {
			newLight->SetG((*value).cast<Uint8>());
		}
		else if (name == "b" && (*value).isNumber()) {
			newLight->SetB((*value).cast<Uint8>());
		}
		else if (name == "a" && (*value).isNumber()) {
			newLight->SetA((*value).cast<Uint8>());
		}
	}
}

void LuaManager::addComponentToActor(Actor* actor, const string& key, const string& componentType,
	const unordered_map<string, shared_ptr<luabridge::LuaRef>>& properties) {
	auto instancePtr = createComponentInstance(componentType, key, properties, actor);

	unsigned int actorId = actor->id;

	Actor::Component component;
	component.type = componentType;
	component.instance = instancePtr;
	component.actorId = actorId;
	actor->components[key] = component;

	ComponentQueueItem item;
	item.key = key;
	item.type = componentType;
	item.instance = instancePtr;
	item.actorId = actorId;

	if (onStartQueue.find(actor->id) == onStartQueue.end()) {
		actorProcessingOrder.push_back(actor->id);
	}
	onStartQueue[actor->id].push_back(item);
}

void LuaManager::addComponentToActor(Actor* actor, const string& key, const string& type, 
	shared_ptr<luabridge::LuaRef> instance) {
	unsigned int actorId = actor->id;
	
	Actor::Component component;
	component.type = type;
	component.instance = instance;
	component.actorId = actorId;
	actor->components[key] = component;

	ComponentQueueItem item;
	item.key = key;
	item.type = type;
	item.instance = instance;
	item.actorId = actorId;

	if (onStartQueue.find(actor->id) == onStartQueue.end()) {
		actorProcessingOrder.push_back(actor->id);
	}
	onStartQueue[actor->id].push_back(item);
}

void LuaManager::queueComponentAddition(const string& key, const string& type, unsigned int actorId,
	shared_ptr<luabridge::LuaRef> instance) {
	ComponentQueueItem item;
	item.key = key;
	item.type = type;
	item.actorId = actorId;
	item.instance = instance;
	
	componentsToAdd.push_back(item);
}

void LuaManager::queueComponentRemoval(unsigned int actorId, const string& key) {
	RemoveComponentItem item;
	item.actorId = actorId;
	item.componentKey = key;
	
	componentsToRemove.push_back(item);
}

void LuaManager::queueActorForOnStart(Actor* actor) {
	if (!actor) return;

	for (auto& [key, component] : actor->components) {
		ComponentQueueItem item;
		item.key = key;
		item.type = component.type;
		item.instance = component.instance;
		item.actorId = actor->id;

		if (onStartQueue.find(actor->id) == onStartQueue.end()) {
			actorProcessingOrder.push_back(actor->id);
		}
		onStartQueue[actor->id].push_back(item);
	}
}

void LuaManager::processOnStartQueue() {
	vector<unsigned int> currentProcessingOrder = actorProcessingOrder;
	unordered_map<unsigned int, vector<ComponentQueueItem>> currentOnStartQueue = onStartQueue;
	actorProcessingOrder.clear();
	onStartQueue.clear();

	for (unsigned int actorId : currentProcessingOrder) {
		auto& components = currentOnStartQueue[actorId];

		sort(components.begin(), components.end(),
			[](const ComponentQueueItem& a, const ComponentQueueItem& b) {
				return a.key < b.key;
			});

		for (const auto& item : components) {
			Actor* curActorPtr = SceneDB::getInstance().getActorByID(item.actorId);
			if (curActorPtr) {
				(*item.instance)["actor"] = curActorPtr;

				if (curActorPtr->dontDestroy && curActorPtr->processedOnStart) {
					continue;
				}

				auto it = componentFunctions.find(item.type);
				if (it != componentFunctions.end() && it->second.hasOnStart &&
					(*item.instance)["enabled"].cast<bool>()) {
					try {
						luabridge::LuaRef onStart = (*item.instance)["OnStart"];
						onStart(*item.instance);
						curActorPtr->processedOnStart = true;
					}
					catch (const luabridge::LuaException& e) {
						EngineUtils::ReportError(curActorPtr->name, e);
					}
				}
			}
		}
	}
}

void LuaManager::processUpdate(bool lateUpdate) {
	vector<Actor*>& actors = SceneDB::getInstance().getActors();

	for (Actor* curActor : actors) {
		if (!curActor) continue;

		unordered_map<string, Actor::Component>& components = curActor->components;

		vector<string> keys;
		keys.reserve(components.size());
		for (const auto& [key, component] : components) {
			keys.push_back(key);
		}
		sort(keys.begin(), keys.end());

		for (const string& key : keys) {
			Actor::Component& component = components[key];

			if (!(*component.instance)["enabled"].cast<bool>()) continue;

			auto it = componentFunctions.find(component.type);
			if (it != componentFunctions.end()) {
				const ComponentFunctions& functions = it->second;

				if (!lateUpdate && functions.hasOnUpdate) {
					try {
						luabridge::LuaRef updateFunc = (*component.instance)["OnUpdate"];
						updateFunc(*component.instance);
					}
					catch (const luabridge::LuaException& e) {
						EngineUtils::ReportError(curActor->name, e);
					}
				}
				else if (lateUpdate && functions.hasOnLateUpdate) {
					try {
						luabridge::LuaRef lateUpdateFunc = (*component.instance)["OnLateUpdate"];
						lateUpdateFunc(*component.instance);
					}
					catch (const luabridge::LuaException& e) {
						EngineUtils::ReportError(curActor->name, e);
					}
				}
			}
		}
	}
}

void LuaManager::processAddComponent() {
	for (const auto& item : componentsToAdd) {
		Actor* actor = SceneDB::getInstance().getActorByID(item.actorId);
		if (actor) {
			if (componentFunctions.find(item.type) == componentFunctions.end()) {
				loadComponentType(item.type);
			}
			addComponentToActor(actor, item.key, item.type, item.instance);
		}
	}
	componentsToAdd.clear();
}

void LuaManager::processRemoveComponent() {
	for (const auto& item : componentsToRemove) {
		Actor* actor = SceneDB::getInstance().getActorByID(item.actorId);
		if (actor) {
			auto it = actor->components.find(item.componentKey);
			if (it != actor->components.end()) {
				actor->components.erase(item.componentKey);
			}
		}
	}
	componentsToRemove.clear();
}

void LuaManager::debugLog(const string& message) {
	cout << message << endl;
}

luabridge::LuaRef LuaManager::findActor(const string& name) {
	vector<Actor*> matchingActors = SceneDB::getInstance().getActorsByName(name);
	if (matchingActors.empty()) {
		return luabridge::LuaRef(getInstance().getLuaState());
	}
	return luabridge::LuaRef(getInstance().getLuaState(), &matchingActors[0]);
}

luabridge::LuaRef LuaManager::findAllActor(const string& name) {
	vector<Actor*> matchingActors = SceneDB::getInstance().getActorsByName(name);
	lua_State* state = getInstance().getLuaState();
	luabridge::LuaRef table = luabridge::newTable(state);
	for (int i = 0; i < matchingActors.size(); ++i) {
		table[i + 1] = &matchingActors[i];
	}
	return table;
}

void LuaManager::quit() {
	exit(0);
}

void LuaManager::sleep(int milliseconds) {
	this_thread::sleep_for(chrono::milliseconds(milliseconds));
}

int LuaManager::getFrame() {
	return Helper::GetFrameNumber();
}

void LuaManager::openURL(const string& url) {
	string command = "";
#ifdef _WIN32
	command = "start " + url;
#elif defined(__APPLE__)
	command = "open " + url;
#else
	command = "xdg-open " + url;
#endif
	system(command.c_str());
}

luabridge::LuaRef LuaManager::instantiateActor(const string& tempName) {
	Actor* newActor = new Actor();

	rapidjson::Document& tempDoc = SceneDB::getInstance().getTemplate(tempName);

	SceneDB::getInstance().createActorFromJson(*newActor, tempDoc);
	SceneDB::getInstance().queueActorAddition(newActor);
	
	return luabridge::LuaRef(getInstance().getLuaState(), newActor);
}

void LuaManager::destroyActor(luabridge::LuaRef actorRef) {
	Actor* actor = actorRef.cast<Actor*>();
	if (!actor) return;
	
	getInstance().processOnDestroyForActor(actor);

	actor->toDestroy = true;
	for (auto& [key, component] : actor->components) {
		luabridge::LuaRef instance = *(component.instance);
		if (instance.isTable() && instance["enabled"].isBool()) {
			instance["enabled"] = false;
		}
		else if (instance.isUserdata()) {
			if (instance.isInstance<ParticleSystem>()) {
				ParticleSystem* ps = instance.cast<ParticleSystem*>();
				if (ps) {
					ps->SetEnabled(false);
				}
			}
			else if (instance.isInstance<Rigidbody>()) {
				Rigidbody* rb = instance.cast<Rigidbody*>();
				if (rb) {
					rb->SetEnabled(false);
				}
			}
		}
	}

	SceneDB::getInstance().queueActorDestroy(actor);
}

void LuaManager::handleCollisionEvent(Actor* actorA, Actor* actorB,
	const CollisionListener::CollisionData& data, bool isBeginContact) {
	if (!actorA || !actorB) return;
	
	string functionName = "";
	if (data.isTrigger) {
		if (isBeginContact) {
			functionName = "OnTriggerEnter";
		}
		else {
			functionName = "OnTriggerExit";
		}
	}
	else {
		if (isBeginContact) {
			functionName = "OnCollisionEnter";
		}
		else {
			functionName = "OnCollisionExit";
		}
	}

	luabridge::LuaRef collisionTableA = luabridge::newTable(lua_state);
	collisionTableA["other"] = actorB;
	collisionTableA["point"] = data.point;
	collisionTableA["relative_velocity"] = data.relativeVelocity;
	collisionTableA["normal"] = data.normal;

	processCollisionForActor(actorA, functionName, collisionTableA);

	luabridge::LuaRef collisionTableB = luabridge::newTable(lua_state);
	collisionTableB["other"] = actorA;
	collisionTableB["point"] = data.point;
	collisionTableB["relative_velocity"] = data.relativeVelocity;
	collisionTableB["normal"] = data.normal;

	processCollisionForActor(actorB, functionName, collisionTableB);
}

void LuaManager::processCollisionForActor(Actor* actor,
	const string& functionName, const luabridge::LuaRef& collisionData) {
	if (!actor) return;

	vector<pair<string, Actor::Component*>> componentsWithFunction;

	for (auto& [key, component] : actor->components) {
		if ((*component.instance)["enabled"].cast<bool>()) {
			luabridge::LuaRef instance = *component.instance;
			if (instance[functionName].isFunction()) {
				componentsWithFunction.push_back(make_pair(key, &component));
			}
		}
	}

	sort(componentsWithFunction.begin(), componentsWithFunction.end(),
		[](const auto& a, const auto& b) {
			return a.first < b.first;
		});

	for (auto& [key, component] : componentsWithFunction) {
		try {
			luabridge::LuaRef instance = *component->instance;
			luabridge::LuaRef collisionFunc = instance[functionName];
			collisionFunc(instance, collisionData);
		}
		catch (const luabridge::LuaException& e) {
			EngineUtils::ReportError(actor->name, e);
		}
	}
}

void LuaManager::processOnDestroyForActor(Actor* actor) {
	if (!actor) return;

	vector<pair<string, Actor::Component*>> componentsWithOnDestroy;

	for (auto& [key, component] : actor->components) {
		if (componentFunctions[component.type].hasOnDestroy) {
			componentsWithOnDestroy.push_back(make_pair(key, &component));
		}
	}

	sort(componentsWithOnDestroy.begin(), componentsWithOnDestroy.end(),
		[](const auto& a, const auto& b) {
			return a.first < b.first;
		});

	for (auto& [key, component] : componentsWithOnDestroy) {
		processOnDestroyForComponent(actor, key, *component);
	}
}

void LuaManager::processOnDestroyForComponent(Actor* actor, const string& key,
	Actor::Component& component) {
	try {
		luabridge::LuaRef instance = *component.instance;
		if (instance["OnDestroy"].isFunction()) {
			luabridge::LuaRef OnDestroy = instance["OnDestroy"];
			OnDestroy(instance);
		}
	}
	catch (const luabridge::LuaException& e) {
		EngineUtils::ReportError(actor->name, e);
	}
}