#include "LightSystem.h"
#include "EngineUtils.h"
#include "Rigidbody.h"
#include "Physics.h"
#include "Renderer.h"
#include "SceneDB.h"

#include <algorithm>
#include <cmath>

using namespace std;

LightSystem::~LightSystem() {
	ClearLights();
	if (lightingTexture) {
		SDL_DestroyTexture(lightingTexture);
		lightingTexture = nullptr;
	}
}

void LightSystem::RegisterLight(Light* light) {
	if (light && find(lights.begin(), lights.end(), light) == lights.end()) {
		lights.push_back(light);
	}
}

void LightSystem::UnregisterLight(Light* light) {
	auto it = find(lights.begin(), lights.end(), light);
	if (it != lights.end()) {
		lights.erase(it);
	}
}

void LightSystem::ClearLights() {
	lights.clear();
}

void LightSystem::Initialize() {
	if (EngineUtils::has_render_config) {
		rapidjson::Document& renderDoc = EngineUtils::render_config;

		if (renderDoc.HasMember("enable_lighting") && renderDoc["enable_lighting"].IsBool()) {
			enabled = renderDoc["enable_lighting"].GetBool();
		}

		if (renderDoc.HasMember("ambient_intensity") && renderDoc["ambient_intensity"].IsNumber()) {
			ambientIntensity = renderDoc["ambient_intensity"].GetFloat();
		}

		if (renderDoc.HasMember("block_size") && renderDoc["block_size"].IsNumber()) {
			blockSize = renderDoc["block_size"].GetInt();
		}

		if (renderDoc.HasMember("ambient_color")) {
			const rapidjson::Value& color = renderDoc["ambient_color"];
			if (color.IsObject()) {
				int r = 30, g = 30, b = 40;
				if (color.HasMember("r") && color["r"].IsInt()) r = color["r"].GetInt();
				if (color.HasMember("g") && color["g"].IsInt()) g = color["g"].GetInt();
				if (color.HasMember("b") && color["b"].IsInt()) b = color["b"].GetInt();

				ambientColor.r = static_cast<Uint8>(r);
				ambientColor.g = static_cast<Uint8>(g);
				ambientColor.b = static_cast<Uint8>(b);
			}
		}
	}
}

void LightSystem::CreateLightingTexture(SDL_Renderer* renderer, int width, int height) {
	if (lightingTexture) {
		SDL_DestroyTexture(lightingTexture);
	}

	lightingTexture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET,
		width,
		height
	);

	SDL_SetTextureBlendMode(lightingTexture, SDL_BLENDMODE_BLEND);
}

float LightSystem::CalculateSoftShadow(Light* light, const b2Vec2& pixelPos, const b2Vec2& lightPos) {
	if (!light->GetCastShadows()) {
		return 1.0f;
	}

	b2Vec2 direction = pixelPos - lightPos;
	float distance = direction.Length();

	if (distance > light->GetRadius()) {
		return 0.0f;
	}

	luabridge::LuaRef hitResults = Physics::RaycastAll(lightPos, direction, light->GetRadius());

	if (hitResults.isNil() || !hitResults[1].isTable()) {
		return 1.0f;
	}

	float shadowIntensity = 1.0f;

	for (int i = 1; i <= hitResults.length(); ++i) {
		luabridge::LuaRef hit = hitResults[i];

		if (hit["is_trigger"].cast<bool>()) {
			continue;
		}

		b2Vec2 hitPoint = hit["point"].cast<b2Vec2>();
		float hitDistance = (hitPoint - lightPos).Length();
	
		if (hitDistance < distance) {
			Actor* hitActor = hit["actor"].cast<Actor*>();
			b2Vec2 objectCenter;
			float objectRadius = 0.5f;
		
			luabridge::LuaRef rigidbodyRef = hitActor->GetComponent("Rigidbody");
			if (!rigidbodyRef.isNil()) {
				Rigidbody* rb = rigidbodyRef.cast<Rigidbody*>();
				if (!rb) continue;
				objectCenter.x = rb->GetX();
				objectCenter.y = rb->GetY();

				if (rb->GetColliderType() == "box") {
					objectRadius = min(rb->GetWidth(), rb->GetHeight()) * 0.5f;
				}
				else {
					objectRadius = rb->GetRadius();
				}
			}

			b2Vec2 distToCenter = objectCenter - lightPos;
			float projection = b2Dot(distToCenter, direction);
			b2Vec2 closestPoint = lightPos + projection * direction;
			float discriminant = (objectCenter - closestPoint).LengthSquared();

			float softness = hitDistance * 0.035f / sqrt(discriminant + 0.0001f);
			float shadowFactor = max(0.0f, min(1.0f, 1.0f - softness));
			shadowIntensity = min(shadowIntensity, 1.0f - shadowFactor);
		}
	}
	return shadowIntensity;
}

float LightSystem::CalculateLightFalloff(Light* light, const b2Vec2& toPixel) {
	float distance = toPixel.Length();

	if (light->GetType() == LightType::Point) {
		float falloffRadius = light->GetRadius();
		if (distance > falloffRadius) return 0.0f;

		float normalizedDist = distance / falloffRadius;
		return 1.0f - min(1.0f, normalizedDist);
	}
	else {
		b2Vec2 direction = light->GetDirection();
		direction.Normalize();

		b2Vec2 normalizedToPixel = toPixel;
		normalizedToPixel.Normalize();

		float dotProduct = b2Dot(direction, normalizedToPixel);

		if (dotProduct <= 0.0f) return 0.0f;

		float angle = acosf(dotProduct) * 180.0f / b2_pi;
		float halfAngle = light->GetAngle() * 0.5f;

		if (angle > halfAngle) return 0.0f;

		float normalizedAngle = angle / halfAngle;
		float edgeFade = cosf(normalizedAngle * b2_pi * 0.5f);		
		return edgeFade;
	}
}

float LightSystem::CalculatePixelLighting(int pixelX, int pixelY, float cameraX, float cameraY, float zoomFactor) {
	int pixelsPerMeter = 100;

	SDL_Renderer* renderer = Renderer::getInstance().GetSDLRenderer();
	int screenWidth, screenHeight;
	SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

	float screenCenterX = screenWidth * 0.5f;
	float screenCenterY = screenHeight * 0.5f;
	float worldX = (pixelX - screenCenterX) / (pixelsPerMeter * zoomFactor) + cameraX;
	float worldY = (pixelY - screenCenterY) / (pixelsPerMeter * zoomFactor) + cameraY;

	float pixelLight = ambientIntensity;

	for (Light* light : lights) {
		if (!light->GetEnabled()) continue;

		b2Vec2 lightPos(light->GetX(), light->GetY());
		b2Vec2 pixelPos(worldX, worldY);
		b2Vec2 toPixel = pixelPos - lightPos;

		if (Physics::IsPointInCollider(lightPos)) continue;

		float shadow_factor = CalculateSoftShadow(light, pixelPos, lightPos);
		float falloff = CalculateLightFalloff(light, toPixel);
		float lightIntensity = light->GetIntensity() * falloff * shadow_factor;
		pixelLight += lightIntensity;
	}
	return min(1.0f, pixelLight);
}

SDL_Color LightSystem::CalculatePixelColor(int pixelX, int pixelY, float cameraX, float cameraY, float zoomFactor) {
	float lightIntensity = CalculatePixelLighting(pixelX, pixelY, cameraX, cameraY, zoomFactor);

	float r = ambientColor.r * ambientIntensity;
	float g = ambientColor.g * ambientIntensity;
	float b = ambientColor.b * ambientIntensity;

	int pixelsPerMeter = 100;
	SDL_Renderer* renderer = Renderer::getInstance().GetSDLRenderer();
	int screenWidth, screenHeight;
	SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);
	float screenCenterX = screenWidth * 0.5f;
	float screenCenterY = screenHeight * 0.5f;
	float worldX = (pixelX - screenCenterX) / (pixelsPerMeter * zoomFactor) + cameraX;
	float worldY = (pixelY - screenCenterY) / (pixelsPerMeter * zoomFactor) + cameraY;

	if (Physics::IsPointInCollider(b2Vec2(worldX, worldY))) {
		SDL_Color result;
		result.r = static_cast<Uint8>(r);
		result.g = static_cast<Uint8>(g);
		result.b = static_cast<Uint8>(b);
		result.a = static_cast<Uint8>(255 * ambientIntensity);
		return result;
	}

	float totalContribution = ambientIntensity;

	for (Light* light : lights) {
		if (!light->GetEnabled()) continue;

		b2Vec2 lightPos(light->GetX(), light->GetY());
		b2Vec2 pixelPos(worldX, worldY);
		b2Vec2 toPixel = pixelPos - lightPos;

		if (Physics::IsPointInCollider(lightPos)) continue;

		float falloff = CalculateLightFalloff(light, toPixel);
		float shadow = CalculateSoftShadow(light, pixelPos, lightPos);
		float contribution = light->GetIntensity() * falloff * shadow;

		r += light->GetR() * contribution;
		g += light->GetG() * contribution;
		b += light->GetB() * contribution;
		totalContribution += contribution;
	}
	
	r = min(255.0f, r);
	g = min(255.0f, g);
	b = min(255.0f, b);

	float alpha = min(1.0f, totalContribution);
	alpha = 1.0f - pow(1.0f - alpha, 2.0f);

	SDL_Color result = {
		static_cast<Uint8>(r),
		static_cast<Uint8>(g),
		static_cast<Uint8>(b),
		static_cast<Uint8>(255 * alpha)
	};

	return result;
}

vector<LightRayHit> LightSystem::CastRays(Light* light) {
	vector<LightRayHit> rays;
	if (!light || !light->GetEnabled()) return rays;

	const float rayLength = light->GetRadius();
	const b2Vec2 lightPos(light->GetX(), light->GetY());
	const int numRays = light->GetNumRays();

	if (light->GetType() == LightType::Point) {
		for (int i = 0; i < numRays; ++i) {
			float angle = static_cast<float>(i) / numRays * 2.0f * b2_pi;
			b2Vec2 direction(cos(angle), sin(angle));

			LightRayHit ray;
			ray.origin = lightPos;
			ray.direction = direction;
			ray.distance = rayLength;
			ray.hit = false;
			ray.intensity = 1.0f;
			ray.discriminant = 0.0f;

			luabridge::LuaRef hitResult = Physics::Raycast(lightPos, direction, rayLength);
			if (!hitResult.isNil() && hitResult["point"].isTable()) {
				ray.hit = true;
				ray.hitPoint = hitResult["point"].cast<b2Vec2>();
				ray.distance = (ray.hitPoint - lightPos).Length();

				ray.intensity = CalculateSoftShadow(light, ray.hitPoint, lightPos);
			}
			else {
				ray.hitPoint = lightPos + rayLength * direction;
			}

			rays.push_back(ray);
		}
	}
	else if (light->GetType() == LightType::Directional) {
		b2Vec2 direction = light->GetDirection();
		direction.Normalize();

		float angleSpread = light->GetAngle() * b2_pi / 180.0f / 2.0f;
	
		for (int i = 0; i < numRays; ++i) {
			float t = static_cast<float>(i) / (numRays - 1) * 2.0f - 1.0f;
			float angle = t * angleSpread;

			float cosAngle = cos(angle);
			float sinAngle = sin(angle);
			b2Vec2 rayDir(
				direction.x * cosAngle - direction.y * sinAngle,
				direction.x * sinAngle + direction.y * cosAngle
			);

			rayDir.Normalize();

			if (b2Dot(rayDir, direction) <= 0.0f) {
				continue;
			}

			LightRayHit ray;
			ray.origin = lightPos;
			ray.direction = rayDir;
			ray.distance = rayLength;
			ray.hit = false;
			ray.intensity = 1.0f;
			ray.discriminant = 0.0f;

			luabridge::LuaRef hitResult = Physics::Raycast(lightPos, rayDir, rayLength);
			if (!hitResult.isNil() && hitResult["point"].isTable()) {
				ray.hit = true;
				ray.hitPoint = hitResult["point"].cast<b2Vec2>();
				ray.distance = (ray.hitPoint - lightPos).Length();

				ray.intensity = CalculateSoftShadow(light, ray.hitPoint, lightPos);
			}
			else {
				ray.hitPoint = lightPos + rayLength * rayDir;
			}

			rays.push_back(ray);
		}
	}

	return rays;
}

void LightSystem::RenderLighting(SDL_Renderer* renderer, int screenWidth, int screenHeight,
	float cameraX, float cameraY, float zoomFactor) {
	if (!enabled || lights.empty()) {
		return;
	}

	if (!lightingTexture) {
		CreateLightingTexture(renderer, screenWidth, screenHeight);
	}

	SDL_Texture* previousTarget = SDL_GetRenderTarget(renderer);

	if (SDL_SetRenderTarget(renderer, lightingTexture) != 0) {
		cout << "Failed to set render target: " << SDL_GetError() << endl;
		return;
	}

	SDL_SetRenderDrawColor(renderer, ambientColor.r, ambientColor.g, ambientColor.b,
		static_cast<Uint8>(255 * (1.0f - ambientIntensity)));
	SDL_RenderClear(renderer);

	for (int y = 0; y < screenHeight; y += blockSize) {
		for (int x = 0; x < screenWidth; x += blockSize) {
			SDL_Color pixelColor = CalculatePixelColor(
				x + blockSize / 2,
				y + blockSize / 2,
				cameraX,
				cameraY,
				zoomFactor
			);

			SDL_SetRenderDrawColor(renderer, pixelColor.r, pixelColor.g,
				pixelColor.b, pixelColor.a);

			SDL_Rect rect = { x, y, blockSize, blockSize };
			SDL_RenderFillRect(renderer, &rect);
		}
	}

	SDL_SetRenderTarget(renderer, previousTarget);

	SDL_SetTextureBlendMode(lightingTexture, SDL_BLENDMODE_BLEND);
	SDL_RenderCopy(renderer, lightingTexture, NULL, NULL);

	//DEBUG==================================
	/*SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
	for (Light* light : lights) {
		if (!light->GetEnabled() || !light->GetCastShadows()) continue;

		SDL_SetRenderDrawColor(renderer,
			light->GetColor().r,
			light->GetColor().g,
			light->GetColor().b,
			static_cast<Uint8>(64 * light->GetIntensity()));

		vector<LightRayHit> rays = CastRays(light);

		int pixelsPerMeter = 100;
		float screenCenterX = screenWidth * 0.5f;
		float screenCenterY = screenHeight * 0.5f;

		int lightScreenX = (light->GetX() - cameraX) * pixelsPerMeter * zoomFactor + screenCenterX;
		int lightScreenY = (light->GetY() - cameraY) * pixelsPerMeter * zoomFactor + screenCenterY;

		for (const LightRayHit& ray : rays) {
			int endX = (ray.hitPoint.x - cameraX) * pixelsPerMeter * zoomFactor + screenCenterX;
			int endY = (ray.hitPoint.y - cameraY) * pixelsPerMeter * zoomFactor + screenCenterY;

			SDL_RenderDrawLine(renderer, lightScreenX, lightScreenY, endX, endY);
		}
	}

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);*/
}