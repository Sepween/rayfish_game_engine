#pragma once

#include "box2d/box2d.h"
#include "Actor.h"
#include "Rigidbody.h"
#include "LuaManager.h"
#include <vector>
#include <algorithm>

using namespace std;

struct HitResult {
	Actor* actor;
	b2Vec2 point;
	b2Vec2 normal;
	bool is_trigger;
	float fraction;

	HitResult() :
		actor(nullptr),
		point(b2Vec2(0.0f, 0.0f)),
		normal(b2Vec2(0.0f, 0.0f)),
		is_trigger(false),
		fraction(0){ }
};

class RaycastCallback : public b2RayCastCallback {
public:
	RaycastCallback() {}

	float ReportFixture(b2Fixture* fixture, const b2Vec2& point,
		const b2Vec2& normal, float fraction) override;
	
	vector<HitResult> GetResults() { return results; }

private:
	vector<HitResult> results;
};

class Physics {
public:
	static luabridge::LuaRef Raycast(b2Vec2 pos, b2Vec2 dir, float dist);
	static luabridge::LuaRef RaycastAll(b2Vec2 pos, b2Vec2 dir, float dist);
	static bool IsPointInCollider(b2Vec2 point);

private:
	static void getRaycastResults(b2Vec2 pos, b2Vec2 dir, float dist, vector<HitResult>& results);
	class PointQueryCallback : public b2QueryCallback {
	public:
		PointQueryCallback(const b2Vec2& point) :
			m_point(point),
			m_found(false) {
		}

		bool ReportFixture(b2Fixture* fixture) override {
			if (fixture->IsSensor()) return true;

			if (fixture->TestPoint(m_point)) {
				m_found = true;
				return false;
			}
			return true;
		}

		bool Found() const { return m_found; }

	private:
		b2Vec2 m_point;
		bool m_found;
	};
};