#include "CollisionListener.h"
#include "Rigidbody.h"

void CollisionListener::BeginContact(b2Contact* contact) {
	if (beginContactHandler) {
		CollisionData data = GetCollisionData(contact, true);
		beginContactHandler(data);
	}
}

void CollisionListener::EndContact(b2Contact* contact) {
	if (endContactHandler) {
		CollisionData data = GetCollisionData(contact, false);
		endContactHandler(data);
	}
}

CollisionListener::CollisionData CollisionListener::GetCollisionData(b2Contact* contact, bool isBeginContact) {
	CollisionData data;

	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	b2Body* bodyA = fixtureA->GetBody();
	b2Body* bodyB = fixtureB->GetBody();

	data.actorA = reinterpret_cast<Actor*>(bodyA->GetUserData().pointer);
	data.actorB = reinterpret_cast<Actor*>(bodyB->GetUserData().pointer);

	data.isTrigger = fixtureA->IsSensor() || fixtureB->IsSensor();

	if (isBeginContact && !data.isTrigger) {
		b2WorldManifold worldManifold;
		contact->GetWorldManifold(&worldManifold);
		data.point = worldManifold.points[0];
		data.normal = worldManifold.normal;
	}
	else {
		data.point = b2Vec2(-999.0f, -999.0f);
		data.normal = b2Vec2(-999.0f, -999.0f);
	}
	data.relativeVelocity = bodyA->GetLinearVelocity() - bodyB->GetLinearVelocity();

	return data;
}