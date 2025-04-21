#pragma once

#include <string>
#include <vector>
#include "box2d/box2d.h"
#include "Actor.h"
#include "SDL2/SDL.h"
#include "glm/glm.hpp"

using namespace std;

enum class LightType {
	Point,
	Directional
};

class Light {
public:
	Light() :
		key(""),
		enabled(true),
		actor(nullptr),
		position({ 0.0f, 0.0f }),
		color({ 255, 255, 255, 255 }),
		intensity(1.0f),
		radius(5.0f),
		direction({ 0.0f, 1.0f }),
		angle(90.0f),
		type(LightType::Point),
		castShadows(true),
		numRays(60) {}

	string GetKey() const { return key; }
	void SetKey(const string& keyIn) { key = keyIn; }
	bool GetEnabled() const { return enabled; }
	void SetEnabled(bool enabledIn) { enabled = enabledIn; }
	Actor* GetActor() const { return actor; }
	void SetActor(Actor* actorIn) { actor = actorIn; }

	b2Vec2 GetPosition() const { return position; }
	void SetPosition(b2Vec2 positionIn) { position = positionIn; }
	float GetX() const { return position.x; }
	void SetX(float xIn) { position.x = xIn; }
	float GetY() const { return position.y; }
	void SetY(float yIn) { position.y = yIn; }

	SDL_Color GetColor() const { return color; }
	void SetColor(SDL_Color colorIn) { color = colorIn; }
	float GetIntensity() const { return intensity; }
	void SetIntensity(float intensityIn) { intensity = intensityIn; }

	float GetRadius() const { return radius; }
	void SetRadius(float radiusIn) { radius = radiusIn; }

	b2Vec2 GetDirection() const { return direction; }
	void SetDirection(b2Vec2 directionIn) { direction = directionIn; }
	float GetAngle() const { return angle; }
	void SetAngle(float angleIn) { angle = angleIn; }

	string GetTypeStr() const {
		if (type == LightType::Directional) {
			return "directional";
		}
		else {
			return "point";
		}
	}
	LightType GetType() const {
		return type;
	}
	void SetType(string typeIn) { 
		if (typeIn == "directional") {
			type = LightType::Directional;
		}
		else {
			type = LightType::Point;
		}
	}

	bool GetCastShadows() const { return castShadows; }
	void SetCastShadows(bool castShadowsIn) { castShadows = castShadowsIn; }

	int GetNumRays() const { return numRays; }
	void SetNumRays(int numRaysIn) { numRays = numRaysIn; }

	Uint8 GetR() const { return color.r; }
	void SetR(Uint8 rIn) { color.r = rIn; }
	Uint8 GetG() const { return color.g; }
	void SetG(Uint8 gIn) { color.g = gIn; }
	Uint8 GetB() const { return color.b; }
	void SetB(Uint8 bIn) { color.b = bIn; }
	Uint8 GetA() const { return color.a; }
	void SetA(Uint8 aIn) { color.a = aIn; }

	void OnStart();
	void OnUpdate();
	void OnDestroy();

private:
	string key;
	bool enabled;
	Actor* actor;
	b2Vec2 position;
	SDL_Color color;
	float intensity;
	float radius;
	b2Vec2 direction;
	float angle;
	LightType type;
	bool castShadows;
	int numRays;
};