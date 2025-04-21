#include "Rigidbody.h"
#include "CollisionListener.h"
#include "LuaManager.h"

using namespace std;

void Rigidbody::OnStart() {
	if (!world) {
		world = new b2World(b2Vec2(0.0f, 9.8f));

		CollisionListener& collisionListener = CollisionListener::getInstance();
		
		collisionListener.SetBeginContactHandler([](const CollisionListener::CollisionData& data) {
			if (data.actorA && data.actorB) {
				LuaManager::getInstance().handleCollisionEvent(data.actorA, data.actorB, data, true);
			}
		});

		collisionListener.SetEndContactHandler([](const CollisionListener::CollisionData& data) {
			if (data.actorA && data.actorB) {
				LuaManager::getInstance().handleCollisionEvent(data.actorA, data.actorB, data, false);
			}
		});

		world->SetContactListener(&collisionListener);
	}
	num_rigidbody++;

	b2BodyDef bodyDef;
	if (body_type == "static") {
		bodyDef.type = b2_staticBody;
	}
	else if (body_type == "kinematic") {
		bodyDef.type = b2_kinematicBody;
	}
	else {
		bodyDef.type = b2_dynamicBody;
	}
	bodyDef.position.Set(position.x, position.y);
	bodyDef.angle = rotation * (b2_pi / 180.0f);
	bodyDef.bullet = precise;
	bodyDef.gravityScale = gravity_scale;
	bodyDef.angularDamping = angular_friction;

	bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(actor);

	body = world->CreateBody(&bodyDef);

	while (body->GetFixtureList()) {
		body->DestroyFixture(body->GetFixtureList());
	}

	if (!has_collider && !has_trigger) {
		b2PolygonShape phantom_shape;
		phantom_shape.SetAsBox(width * 0.5f, height * 0.5f);

		b2FixtureDef phantom_fixture_def;
		phantom_fixture_def.shape = &phantom_shape;
		phantom_fixture_def.density = density;
		phantom_fixture_def.isSensor = true;
		phantom_fixture_def.friction = friction;
		phantom_fixture_def.restitution = bounciness;
		phantom_fixture_def.filter.categoryBits = CATEGORY_PHANTOM;
		phantom_fixture_def.filter.maskBits = 0;


		body->CreateFixture(&phantom_fixture_def);
	}
	else {
		if (has_collider) {
			b2FixtureDef fixtureDef;
			fixtureDef.density = density;
			fixtureDef.isSensor = false;
			fixtureDef.friction = friction;
			fixtureDef.restitution = bounciness;
			fixtureDef.filter.categoryBits = CATEGORY_COLLIDER;
			fixtureDef.filter.maskBits = CATEGORY_COLLIDER;
			if (collider_type == "box") {
				b2PolygonShape boxShape;
				boxShape.SetAsBox(width * 0.5f, height * 0.5f);
				fixtureDef.shape = &boxShape;
				body->CreateFixture(&fixtureDef);
			}
			else {
				b2CircleShape circleShape;
				circleShape.m_radius = radius;
				fixtureDef.shape = &circleShape;
				body->CreateFixture(&fixtureDef);
			}
		}
		if (has_trigger) {
			b2FixtureDef fixtureDef;
			fixtureDef.density = density;
			fixtureDef.isSensor = true;
			fixtureDef.filter.categoryBits = CATEGORY_TRIGGER;
			fixtureDef.filter.maskBits = CATEGORY_TRIGGER;
			if (trigger_type == "box") {
				b2PolygonShape boxShape;
				boxShape.SetAsBox(trigger_width * 0.5f, trigger_height * 0.5f);
				fixtureDef.shape = &boxShape;
				body->CreateFixture(&fixtureDef);
			}
			else {
				b2CircleShape circleShape;
				circleShape.m_radius = trigger_radius;
				fixtureDef.shape = &circleShape;
				body->CreateFixture(&fixtureDef);
			}
		}
	}

	if (body) {
		if (velocity.x != 0.0f || velocity.y != 0.0f) {
			body->SetLinearVelocity(velocity);
		}

		if (angular_velocity != 0.0f) {
			body->SetAngularVelocity(angular_velocity);
		}

		for (const auto& force : pendingForces) {
			body->ApplyForceToCenter(force, true);
		}
		pendingForces.clear();
	}
}

void Rigidbody::OnDestroy() {
	if (body && world) {
		world->DestroyBody(body);
		body = nullptr;
	}
}

void Rigidbody::SetX(float x) {
	position.x = x;
	if (body) {
		b2Vec2 pos = body->GetPosition();
		pos.x = x;
		body->SetTransform(pos, body->GetAngle());
	}
}

void Rigidbody::SetY(float y) {
	position.y = y;
	if (body) {
		b2Vec2 pos = body->GetPosition();
		pos.y = y;
		body->SetTransform(pos, body->GetAngle());
	}
}

void Rigidbody::StepWorld() {
	if (world) {
		world->Step(1.0f / 60.0f, 8, 3);
	}
}

// lua utility funcs
void Rigidbody::AddForce(b2Vec2 force) {
	if (body) {
		body->ApplyForceToCenter(force, true);
	}
	else {
		pendingForces.push_back(force);
	}
}

void Rigidbody::SetVelocity(b2Vec2 newVelocity) {
	velocity = newVelocity;
	if (body) {
		body->SetLinearVelocity(velocity);
	}
}

void Rigidbody::SetPosition(b2Vec2 newPosition) {
	position = newPosition;
	if (body) {
		body->SetTransform(position, body->GetAngle());
	}
}

void Rigidbody::SetRotation(float degrees_clockwise) {
	rotation = degrees_clockwise;
	if (body) {
		float radians = degrees_clockwise * (b2_pi / 180.0f);
		body->SetTransform(body->GetPosition(), radians);
	}
}

void Rigidbody::SetAngularVelocity(float degrees_clockwise) {
	angular_velocity = degrees_clockwise;
	if (body) {
		float radians = degrees_clockwise * (b2_pi / 180.0f);
		body->SetAngularVelocity(radians);
	}
}

void Rigidbody::SetGravityScale(float gravity) {
	gravity_scale = gravity;
	if (body) {
		body->SetGravityScale(gravity);
	}
}

void Rigidbody::SetUpDirection(b2Vec2 direction) {
	upDirection = direction;
	upDirection.Normalize();
	
	float angle = glm::atan(upDirection.x, -upDirection.y);
	if (body) {
		body->SetTransform(body->GetPosition(), angle);
	}
	else {
		rotation = angle * (180.0f / b2_pi);
	}
}

void Rigidbody::SetRightDirection(b2Vec2 direction) {
	rightDirection = direction;
	rightDirection.Normalize();

	float angle = -glm::atan(rightDirection.y, -rightDirection.x) - (b2_pi / 2.0f);
	if (body) {
		body->SetTransform(body->GetPosition(), angle);
	}
	else {
		rotation = angle * (180.0f / b2_pi);
	}
}

b2Vec2 Rigidbody::GetVelocity() const {
	if (body) {
		return body->GetLinearVelocity();
	}
	return velocity;
}

float Rigidbody::GetAngularVelocity() const {
	if (body) {
		float degrees = body->GetAngularVelocity() * (180.0f / b2_pi);
		return degrees;
	}
	return angular_velocity;
}

float Rigidbody::GetGravityScale() const {
	if (body) {
		return body->GetGravityScale();
	}
	return gravity_scale;
}

b2Vec2 Rigidbody::GetUpDirection() const {
	if (body) {
		float angle = body->GetAngle();
		b2Vec2 curUpDirection;
		curUpDirection.x = glm::sin(angle);
		curUpDirection.y = -glm::cos(angle);
		curUpDirection.Normalize();
		return curUpDirection;
	}
	return upDirection;
}

b2Vec2 Rigidbody::GetRightDirection() const {
	if (body) {
		float angle = body->GetAngle();
		b2Vec2 curRightDirection;
		curRightDirection.x = glm::cos(angle);
		curRightDirection.y = glm::sin(angle);
		curRightDirection.Normalize();
		return curRightDirection;
	}
	return rightDirection;
}