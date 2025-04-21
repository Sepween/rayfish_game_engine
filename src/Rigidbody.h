#pragma once

#include <string>

#include "box2d/box2d.h"

#include "Actor.h"

using namespace std;

class Rigidbody {
public:
	Rigidbody() :
		position({ 0.0f, 0.0f }),
		velocity({ 0.0f, 0.0f }),
		upDirection({ 0.0f, 0.0f }),
		rightDirection({ 0.0f, 0.0f }),
		angular_velocity(0.0f),
		body_type("dynamic"),
		precise(true),
		gravity_scale(1.0f),
		density(1.0f),
		angular_friction(0.3f),
		rotation(0.0f),
		has_collider(true),
		has_trigger(true),
		actor(nullptr),
		key(""),
		enabled(true),
		body(nullptr),
		collider_type("box"),
		width(1.0f),
		height(1.0f),
		radius(0.5f),
		friction(0.3f),
		bounciness(0.3f),
		trigger_type("box"),
		trigger_width(1.0f),
		trigger_height(1.0f),
		trigger_radius(0.5f) {
	}

	b2Vec2 GetPosition() const { return body ? body->GetPosition() : position; }
	float GetRotation() const { return body ? body->GetAngle() * (180.0f / b2_pi) : rotation; }
	const string& GetKey() const { return key; }
	bool GetEnabled() const { return enabled; }
	Actor* GetActor() const { return actor; }
	float GetX() const { return body ? body->GetPosition().x : position.x; }
	float GetY() const { return body ? body->GetPosition().y : position.y; }

	void OnStart();
	void OnDestroy();

	void SetKey(const string& k) { key = k; }
	void SetEnabled(bool e) { enabled = e; }
	void SetActor(Actor* actorIn) { actor = actorIn; }
	void SetX(float x);
	void SetY(float y);

	void InitSetBodyType(const string& type) { body_type = type; }
	void InitSetPositionX(float x) { position.x = x; }
	void InitSetPositionY(float y) { position.y = y; }
	void InitSetColliderType(const string& type) { collider_type = type; }
	void InitSetWidth(float w) { width = w; }
	void InitSetHeight(float h) { height = h; }
	void InitSetRadius(float r) { radius = r; }
	void InitSetDensity(float d) { density = d; }
	void InitSetFriction(float f) { friction = f; }
	void InitSetBounciness(float b) { bounciness = b; }
	void InitSetGravityScale(float g) { gravity_scale = g; }
	void InitSetAngularFriction(float f) { angular_friction = f; }
	void InitSetRotation(float r) { rotation = r; }
	void InitSetPrecise(bool p) { precise = p; }
	void InitSetHasCollider(bool hasCollider) { has_collider = hasCollider; }
	void InitSetHasTrigger(bool hasTrigger) { has_trigger = hasTrigger; }
	void InitSetTriggerType(const string& type) { trigger_type = type; }
	void InitSetTriggerWidth(float w) { trigger_width = w; }
	void InitSetTriggerHeight(float h) { trigger_height = h; }
	void InitSetTriggerRadius(float r) { trigger_radius = r; }

	static void StepWorld();
	static b2World* GetWorld() { return world; }

	// lua utility funcs
	void AddForce(b2Vec2 force);
	void SetVelocity(b2Vec2 newVelocity);
	void SetPosition(b2Vec2 newPosition);
	void SetRotation(float degrees_clockwise);
	void SetAngularVelocity(float degrees_clockwise);
	void SetGravityScale(float gravity);
	void SetUpDirection(b2Vec2 direction);
	void SetRightDirection(b2Vec2 right_direction);

	b2Vec2 GetVelocity() const;
	float GetAngularVelocity() const;
	float GetGravityScale() const;
	b2Vec2 GetUpDirection() const;
	b2Vec2 GetRightDirection() const;

	string GetColliderType() { return collider_type; }
	float GetWidth() { return width; }
	float GetHeight() { return height; }
	float GetRadius() { return radius; }

	// fixture types
	static const uint16 CATEGORY_COLLIDER = 0x0001;
	static const uint16 CATEGORY_TRIGGER = 0x0002;
	static const uint16 CATEGORY_PHANTOM = 0x0004;

private:
	// body
	b2Vec2 position;
	b2Vec2 velocity;
	b2Vec2 upDirection;
	b2Vec2 rightDirection;
	string body_type;
	bool precise;
	float gravity_scale;
	float density;
	float angular_friction;
	float angular_velocity;
	float rotation;
	bool has_collider;
	bool has_trigger;
	vector<b2Vec2> pendingForces;

	// collider
	string collider_type;
	float width;
	float height;
	float radius;
	float friction;
	float bounciness;

	// trigger
	string trigger_type;
	float trigger_width;
	float trigger_height;
	float trigger_radius;

	string key;
	bool enabled;
	Actor* actor;
	b2Body* body;
	static inline b2World* world = nullptr;
	static inline int num_rigidbody = 0;
};