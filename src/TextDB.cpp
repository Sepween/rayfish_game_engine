#include "TextDB.h"

using namespace std;

void TextDB::queueDrawEvent(const string& content, int x, int y, const string& font_name, int font_size,
	int r, int g, int b, int a) {
	TextDrawEvent event;
	event.content = content;
	event.screen_coord = { x, y };
	event.font_name = font_name;
	event.font_size = font_size;
	event.color = { static_cast<Uint8>(r), static_cast<Uint8>(g),
		static_cast<Uint8>(b), static_cast<Uint8>(a) };

	drawQueue.push(event);
}

void TextDB::Draw(const string& content, int x, int y, const string& font_name, int font_size,
	int r, int g, int b, int a) {
	getInstance().queueDrawEvent(content, x, y, font_name, font_size, r, g, b, a);
}

TTF_Font* TextDB::getFont(const string& font_name, int font_size) {
	TTF_Font* font = nullptr;
	auto fontIt = fonts.find(font_name);
	if (fontIt != fonts.end()) {
		auto sizeIt = fontIt->second.find(font_size);
		if (sizeIt != fontIt->second.end()) {
			font = sizeIt->second;
		}
	}

	if (!font) {
		string fontPath = EngineUtils::getFont(font_name);
		font = TTF_OpenFont(fontPath.c_str(), font_size);

		if (!font) {
			cout << "error: font " << font_name << " missing";
			exit(0);
		}

		fonts[font_name][font_size] = font;
	}

	return font;
}

bool TextDB::getTextTexture(SDL_Renderer* renderer, SDL_Texture** texture,
	float& textWidth, float& textHeight, const string& text,
	const string& font_name, int font_size, SDL_Color color) {
	string cacheKey = createCacheKey(text, font_name, font_size, color);
	
	auto it = textureMap.find(cacheKey);
	if (it != textureMap.end()) {
		*texture = it->second.texture;
		textWidth = it->second.width;
		textHeight = it->second.height;
		return true;
	}

	TTF_Font* font = getFont(font_name, font_size);
	SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
	SDL_Texture* new_texture = SDL_CreateTextureFromSurface(renderer, surface);

	TextureInfo info;
	info.texture = new_texture;
	info.width = static_cast<float>(surface->w);
	info.height = static_cast<float>(surface->h);
	textureMap[cacheKey] = info;

	*texture = info.texture;
	textWidth = info.width;
	textHeight = info.height;

	SDL_FreeSurface(surface);
	return true;
}

string TextDB::createCacheKey(const string& content, const string& font_name,
	int font_size, SDL_Color color) {
	stringstream ss;
	ss << content << ":" << font_name << ":" << font_size << ":"
		<< static_cast<int>(color.r) << ":" << static_cast<int>(color.g) << ":"
		<< static_cast<int>(color.b) << ":" << static_cast<int>(color.a);
	return ss.str();
}