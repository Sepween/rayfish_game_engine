#pragma once

#include <string>
#include <vector>

#include "SDL2_image/SDL_image.h"

#include "Actor.h"
#include "Helper.h"

using namespace std;

class ImageDB {
public:
	enum class ImageRenderType {
		SceneSpace,
		UI,
		Pixel
	};

	struct ImageDrawEvent {
		string image_name = "";
		glm::vec2 screen_coord = { 0, 0 };
		float rotation_degrees = 0;
		glm::vec2 scale = { 0.0f, 0.0f };
		glm::vec2 pivot = { 0.0f, 0.0f };
		SDL_Color color = { 255, 255, 255, 255 };
		int sorting_order = 0;
		int call_order;
	};

	struct textureItem {
		SDL_Texture* texture;
		float width;
		float height;
	};

	ImageDB() : next_call_order(0) {}
	~ImageDB() {
		for (auto& pair : textureCache) {
			if (pair.second.texture) {
				SDL_DestroyTexture(pair.second.texture);
			}
		}
	}
	
	static void DrawUI(const string& image_name, float x, float y);
	static void DrawUIEx(const string& image_name, float x, float y, float r, float g, float b, float a,
		float sorting_order);
	static void Draw(const string& image_name, float x, float y);
	static void DrawEx(const string& image_name, float x, float y, float rotation_degrees,
		float scale_x, float scale_y, float pivot_x, float pivot_y, float r, float g, float b, float a,
		float sorting_order);
	static void DrawPixel(float x, float y, float r, float g, float b, float a);
	
	ImageDB::textureItem* getTextureByName(SDL_Renderer* renderer, const string& imageName);
	void loadImageFromFolder(SDL_Renderer* renderer, const string& imageName);
	void queueDrawImageEvent(const string& image_name, float x, float y, float rotation_degrees,
		float scale_x, float scale_y, float pivot_x, float pivot_y, SDL_Color color,
		int sorting_order, ImageRenderType render_type);
	void CreateDefaultParticleTextureWithName(SDL_Renderer* renderer, const string& name);

	void lateUpdate() { 
		sceneImageQueue.clear();
		uiImageQueue.clear();
		pixelQueue.clear();
		next_call_order = 0; 
	}
	vector<ImageDrawEvent>& getSceneImageQueue() { return sceneImageQueue; }
	vector<ImageDrawEvent>& getUiImageQueue() { return uiImageQueue; }
	vector<ImageDrawEvent>& getPixelQueue() { return pixelQueue; }

	static ImageDB& getInstance() {
		static ImageDB instance;
		return instance;
	}

private:
	int next_call_order;
	vector<ImageDrawEvent> sceneImageQueue;
	vector<ImageDrawEvent> uiImageQueue;
	vector<ImageDrawEvent> pixelQueue;
	unordered_map<string, textureItem> textureCache;
};