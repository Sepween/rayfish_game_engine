#pragma once

#include "box2d/box2d.h"
#include "Actor.h"
#include <functional>

class CollisionListener : public b2ContactListener {
public:
	struct CollisionData {
		Actor* actorA;
		Actor* actorB;
		b2Vec2 point;
		b2Vec2 normal;
		b2Vec2 relativeVelocity;
		bool isTrigger;
	};

	using CollisionHandlerFunc = function<void(const CollisionData&)>;

	static CollisionListener& getInstance(){
		static CollisionListener instance;
		return instance;
	}

	CollisionListener(const CollisionListener&) = delete;
	CollisionListener& operator=(const CollisionListener&) = delete;

	void SetBeginContactHandler(CollisionHandlerFunc handler) {
		beginContactHandler = handler;
	}

	void SetEndContactHandler(CollisionHandlerFunc handler) {
		endContactHandler = handler;
	}

	void BeginContact(b2Contact* contract) override;
	void EndContact(b2Contact* contact) override;

private:
	CollisionListener() = default;

	CollisionHandlerFunc beginContactHandler;
	CollisionHandlerFunc endContactHandler;

	CollisionData GetCollisionData(b2Contact* contact, bool isBeginContact);
};