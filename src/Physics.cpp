#include "Physics.h"
#include "Rigidbody.h"

float RaycastCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point,
	const b2Vec2& normal, float fraction){
	b2Body* body = fixture->GetBody();
	if (!body) return -1;

	Actor* actor = reinterpret_cast<Actor*>(body->GetUserData().pointer);
	if (!actor) return -1;

	HitResult hit;
	hit.actor = actor;
	hit.point = point;
	hit.normal = normal;
	hit.is_trigger = fixture->IsSensor();
	hit.fraction = fraction;

	results.push_back(hit);
	return 1;
}

luabridge::LuaRef Physics::Raycast(b2Vec2 pos, b2Vec2 dir, float dist) {
	lua_State* state = LuaManager::getInstance().getLuaState();
	if (dist <= 0 || !Rigidbody::GetWorld()) {
		return luabridge::LuaRef(state);
	}
	vector<HitResult> results;
	getRaycastResults(pos, dir, dist, results);

	if (results.empty()) {
		return luabridge::LuaRef(state);
	}

	HitResult closestHit = results[0];
	
	luabridge::LuaRef hitTable = luabridge::newTable(state);
	hitTable["actor"] = closestHit.actor;
	hitTable["point"] = closestHit.point;
	hitTable["normal"] = closestHit.normal;
	hitTable["is_trigger"] = closestHit.is_trigger;

	return hitTable;
}

luabridge::LuaRef Physics::RaycastAll(b2Vec2 pos, b2Vec2 dir, float dist) {
	lua_State* state = LuaManager::getInstance().getLuaState();
	luabridge::LuaRef resultsTable = luabridge::newTable(state);
	if (dist <= 0 || !Rigidbody::GetWorld()) {
		return resultsTable;
	}
	vector<HitResult> results;
	getRaycastResults(pos, dir, dist, results);
	if (results.empty()) {
		return resultsTable;
	}
	
	for (int i = 0; i < results.size(); ++i) {
		luabridge::LuaRef hitTable = luabridge::newTable(state);
		hitTable["actor"] = results[i].actor;
		hitTable["point"] = results[i].point;
		hitTable["normal"] = results[i].normal;
		hitTable["is_trigger"] = results[i].is_trigger;

		resultsTable[i + 1] = hitTable;
	}

	return resultsTable;
}

void Physics::getRaycastResults(b2Vec2 pos, b2Vec2 dir, float dist, vector<HitResult>& results) {
	dir.Normalize();
	b2Vec2 end = pos + dist * dir;

	RaycastCallback callback;
	Rigidbody::GetWorld()->RayCast(&callback, pos, end);

	results = callback.GetResults();
	if (results.empty()) {
		return;
	}

	sort(results.begin(), results.end(),
		[](const HitResult& a, const HitResult& b) {
			return a.fraction < b.fraction;
		});
}

bool Physics::IsPointInCollider(b2Vec2 point) {
	if (!Rigidbody::GetWorld()) {
		return false;
	}

	const float radius = 0.001f;
	b2AABB aabb;
	aabb.lowerBound = point - b2Vec2(radius, radius);
	aabb.upperBound = point + b2Vec2(radius, radius);

	PointQueryCallback callback(point);
	Rigidbody::GetWorld()->QueryAABB(&callback, aabb);

	return callback.Found();
}