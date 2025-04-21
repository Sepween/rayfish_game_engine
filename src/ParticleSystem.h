#pragma once

#include <vector>
#include <queue>

#include "glm/glm.hpp"
#include "SDL2/SDL.h"
#include "box2d/box2d.h"

#include "Helper.h"
#include "Actor.h"
#include "ImageDB.h"
#include "Renderer.h"

using namespace std;

class ParticleSystem {
public:
	ParticleSystem();
	~ParticleSystem();

	void onStart();
	void onUpdate();

	const string& GetKey() const { return key; }
	bool GetEnabled() const { return enabled; }
	Actor* GetActor() const { return actor; }
	float GetX() const { return position.x; }
	float GetY() const { return position.y; }
	glm::vec2 GetPosition() const { return position; }
	int GetBurstInterval() const { return burstInterval; }
	int GetPixelPerBurst() const { return pixelPerBurst; }
	float GetRadiusMin() const { return radiusMin; }
	float GetRadiusMax() const { return radiusMax; }
	float GetAngleMin() const { return angleMin; }
	float GetAngleMax() const { return angleMax; }
	float GetScaleMin() const { return scaleMin; }
	float GetScaleMax() const { return scaleMax; }
	float GetRotationMin() const { return rotationMin; }
	float GetRotationMax() const { return rotationMax; }
	float GetSpeedMin() const { return speedMin; }
	float GetSpeedMax() const { return speedMax; }
	float GetRotationSpeedMin() const { return rotationSpeedMin; }
	float GetRotationSpeedMax() const { return rotationSpeedMax; }
	int GetR() const { return tint.r; }
	int GetG() const { return tint.g; }
	int GetB() const { return tint.b; }
	int GetA() const { return tint.a; }
	int GetSortingOrder() const { return sortingOrder; }
	const string& GetImage() const { return image; }
	int GetDurationFrames() const { return durationFrames; }
	float GetGravityScaleX() const { return gravityScaleX; }
	float GetGravityScaleY() const { return gravityScaleY; }
	float GetDrag() const { return dragFactor; }
	float GetAngularDrag() const { return angularDragFactor; }
	float GetEndScale() const { return endScale; }
	int GetEndColorR() const { return endColor.r; }
	int GetEndColorG() const { return endColor.g; }
	int GetEndColorB() const { return endColor.b; }
	int GetEndColorA() const { return endColor.a; }

	void SetKey(const string& keyIn) { key = keyIn; }
	void SetEnabled(bool enabledIn) { enabled = enabledIn; }
	void SetActor(Actor* actorIn) { actor = actorIn; }
	void SetX(float x) { position.x = x; }
	void SetY(float y) { position.y = y; }
	void SetPosition(glm::vec2 positionIn) { position = positionIn; }
	void SetBurstInterval(int burstIntervalIn) { burstInterval = max(1, burstIntervalIn); }
	void SetPixelPerBurst(int pixelPerBurstIn) { pixelPerBurst = max(1, pixelPerBurstIn); }
	void SetRadiusMin(float radiusMinIn) { radiusMin = radiusMinIn; }
	void SetRadiusMax(float radiusMaxIn) { radiusMax = radiusMaxIn; }
	void SetAngleMin(float angleMinIn) { angleMin = angleMinIn; }
	void SetAngleMax(float angleMaxIn) { angleMax = angleMaxIn; }
	void SetScaleMin(float scaleMinIn) { scaleMin = scaleMinIn; }
	void SetScaleMax(float scaleMaxIn) { scaleMax = scaleMaxIn; }
	void SetRotationMin(float rotationMinIn) { rotationMin = rotationMinIn; }
	void SetRotationMax(float rotationMaxIn) { rotationMax = rotationMaxIn; }
	void SetSpeedMin(float speedMinIn) { speedMin = speedMinIn; }
	void SetSpeedMax(float speedMaxIn) { speedMax = speedMaxIn; }
	void SetRotationSpeedMin(float rotationSpeedMinIn) { rotationSpeedMin = rotationSpeedMinIn; }
	void SetRotationSpeedMax(float rotationSpeedMaxIn) { rotationSpeedMax = rotationSpeedMaxIn; }
	void SetR(int rIn) { tint.r = rIn; }
	void SetG(int gIn) { tint.g = gIn; }
	void SetB(int bIn) { tint.b = bIn; }
	void SetA(int aIn) { tint.a = aIn; }
	void SetSortingOrder(int sortingOrderIn) { sortingOrder = sortingOrderIn; }
	void SetImage(const string& imageIn) { image = imageIn; }
	void SetDurationFrames(int durationFramesIn) { durationFrames = max(1, durationFramesIn); }
	void SetGravityScaleX(float gravityScaleXIn) { gravityScaleX = gravityScaleXIn; }
	void SetGravityScaleY(float gravityScaleYIn) { gravityScaleY = gravityScaleYIn; }
	void SetDrag(float dragIn) { dragFactor = dragIn; }
	void SetAngularDrag(float angularDragIn) { angularDragFactor = angularDragIn; }
	void SetEndScale(float value) {
		endScale = value;
		endScaleConfigured = true;
	}
	void SetEndColorR(int value) {
		endColor.r = max(0, min(255, value));
		endColorRConfigured = true;
	}
	void SetEndColorG(int value) {
		endColor.g = max(0, min(255, value));
		endColorGConfigured = true;
	}
	void SetEndColorB(int value) {
		endColor.b = max(0, min(255, value));
		endColorBConfigured = true;
	}
	void SetEndColorA(int value) {
		endColor.a = max(0, min(255, value));
		endColorAConfigured = true;
	}

	void Stop();
	void Play();
	void Burst();

private:
	int burstInterval;
	int pixelPerBurst;
	int sortingOrder;
	int curFrame;
	int durationFrames;
	float radius;
	glm::vec2 pivotPoint;
	glm::vec2 position;
	SDL_Color tint;
	string image;
	bool emission_allowed;
	bool burst_this_frame;

	float radiusMin;
	float radiusMax;
	float angleMin;
	float angleMax;
	float scaleMin;
	float scaleMax;
	float rotationMin;
	float rotationMax;
	float speedMin;
	float speedMax;
	float rotationSpeedMin;
	float rotationSpeedMax;
	float gravityScaleX;
	float gravityScaleY;
	float dragFactor;
	float angularDragFactor;
	float endScale;
	bool endScaleConfigured;
	SDL_Color endColor;
	bool endColorRConfigured;
	bool endColorGConfigured;
	bool endColorBConfigured;
	bool endColorAConfigured;

	RandomEngine* angleRandomEngine;
	RandomEngine* radiusRandomEngine;
	RandomEngine* scaleRandomEngine;
	RandomEngine* rotationRandomEngine;
	RandomEngine* speedRandomEngine;
	RandomEngine* rotationSpeedRandomEngine;

	string key;
	bool enabled;
	Actor* actor;

	// particles
	vector<glm::vec2> particle_position;
	vector<glm::vec2> particle_velocity;
	vector<int> particle_frameCreated;
	vector<float> particle_initial_scale;
	vector<float> particle_scale;
	vector<SDL_Color> particle_color;
	vector<float> particle_rotation;
	vector<float> particle_rotation_speed;
	vector<bool> particle_active;

	queue<int> free_list;

	void UpdateParticles();
	void EmitParticles();
	void QueueParticlesForRendering();
};