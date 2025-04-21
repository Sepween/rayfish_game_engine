#pragma once

#include <vector>
#include <unordered_map>
#include "Light.h"
#include "SDL2/SDL.h"
#include "box2d/box2d.h"

struct LightRayHit {
	b2Vec2 origin;
	b2Vec2 direction;
	float distance;
	bool hit;
	b2Vec2 hitPoint;
	float intensity;
	float discriminant;
};

class LightSystem {
public:
	static LightSystem& getInstance() {
		static LightSystem instance;
		return instance;
	}

	LightSystem() :
		enabled(false),
		ambientIntensity(0.2f),
		blockSize(4),
		ambientColor({ 30, 30, 40, 255 }),
		lightingTexture(nullptr) {}
	~LightSystem();

	void RegisterLight(Light* light);
	void UnregisterLight(Light* light);
	void ClearLights();

	bool IsEnabled() const { return enabled; }
	void SetEnabled(bool enabledIn) { enabled = enabledIn; }
	float GetAmbientIntensity() const { return ambientIntensity; }
	void SetAmbientIntensity(float aiIn) { ambientIntensity = aiIn; }
	SDL_Color GetAmbientColor() const { return ambientColor; }
	void SetAmbientColor(SDL_Color colorIn) { ambientColor = colorIn; }

	vector<LightRayHit> CastRays(Light* light);
	float CalculatePixelLighting(int pixelX, int pixelY, float cameraX, float cameraY, float zoomFactor);
	SDL_Color CalculatePixelColor(int pixelX, int pixelY, float cameraX, float cameraY, float zoomFactor);

	void RenderLighting(SDL_Renderer* renderer, int screenWidth, int screenHeight,
		float cameraX, float cameraY, float zoomFactor);

	void Initialize();

private:
	vector<Light*> lights;
	bool enabled;
	float ambientIntensity;
	int blockSize;
	SDL_Color ambientColor;
	SDL_Texture* lightingTexture;

	void CreateLightingTexture(SDL_Renderer* renderer, int width, int height);
	float CalculateSoftShadow(Light* light, const b2Vec2& pixelPos, const b2Vec2& lightPos);
	float CalculateLightFalloff(Light* light, const b2Vec2& toPixel);
};