#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <queue>

#include "SDL2_ttf/SDL_ttf.h"
#include "glm/glm.hpp"

#include "EngineUtils.h"

using namespace std;

class TextDB {
public:
	struct TextDrawEvent {
		string content;
		glm::ivec2 screen_coord;
		string font_name;
		int font_size;
		SDL_Color color;
	};

	TextDB() {
		TTF_Init();
	}

	~TextDB() {
		for (auto& fontPair : fonts) {
			for (auto& sizePair : fontPair.second) {
				TTF_CloseFont(sizePair.second);
			}
		}
	}

	void queueDrawEvent(const string& content, int x, int y, const string& font_name, int font_size,
		int r, int g, int b, int a);

	static void Draw(const string& content, int x, int y, const string& font_name, int font_size,
		int r, int g, int b, int a);

	queue<TextDrawEvent>& getQueue() { return drawQueue; }

	TTF_Font* getFont(const string& font_name, int font_size);

	bool getTextTexture(SDL_Renderer* renderer, SDL_Texture** texture,
		float& textWidth, float& textHeight, const string& text,
		const string& font_name, int font_size, SDL_Color color);

	static TextDB& getInstance() {
		static TextDB instance;
		return instance;
	}

private:
	struct TextureInfo {
		SDL_Texture* texture;
		float width;
		float height;
	};

	unordered_map<string, unordered_map<int, TTF_Font*>> fonts;
	unordered_map<string, TextureInfo> textureMap;
	queue<TextDrawEvent> drawQueue;
	
	string createCacheKey(const string& content, const string& font_name,
		int font_size, SDL_Color color);
};