#include "Renderer.h"

using namespace std;

void Renderer::init(){
	checkLoadResources();
	window = Helper::SDL_CreateWindow(window_title.c_str(), window_position.x, window_position.y,
		window_dimensions.x, window_dimensions.y, SDL_WINDOW_SHOWN);
	sdl_renderer = Helper::SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	SDL_RenderClear(sdl_renderer);
	audioDB.init();
	lightSystem.Initialize();
}

void Renderer::checkLoadResources() {
	if (!EngineUtils::InitializeConfigs()) {
		exit(0);
	}
	rapidjson::Document& gameDoc = EngineUtils::game_config;
	EngineUtils::loadFromDoc(gameDoc, "game_title", window_title);

	if (EngineUtils::has_render_config) {
		rapidjson::Document& renderDoc = EngineUtils::render_config;
		EngineUtils::loadFromDoc(renderDoc, "x_resolution", window_dimensions.x);
		EngineUtils::loadFromDoc(renderDoc, "y_resolution", window_dimensions.y);
		int r = 255;
		int g = 255;
		int b = 255;
		EngineUtils::loadFromDoc(renderDoc, "clear_color_r", r);
		EngineUtils::loadFromDoc(renderDoc, "clear_color_g", g);
		EngineUtils::loadFromDoc(renderDoc, "clear_color_b", b);
		if (r <= 255) clear_color.r = r;
		if (g <= 255) clear_color.g = g;
		if (b <= 255) clear_color.b = b;
	}
}

void Renderer::renderGame() {
	SDL_SetRenderDrawColor(sdl_renderer, clear_color.r, clear_color.g, clear_color.b, clear_color.a);
	SDL_RenderClear(sdl_renderer); 

	renderImageAndUI(false);
	renderLights();
	renderImageAndUI(true);
	renderUIText();
	renderPixel();

	Helper::SDL_RenderPresent(sdl_renderer);
}

void Renderer::renderImageAndUI(bool render_ui){
	vector<ImageDB::ImageDrawEvent>* drawQueue;
	vector<ImageDB::ImageDrawEvent> visibleQueue;
	if (render_ui) {
		drawQueue = &ImageDB::getInstance().getUiImageQueue();
	}
	else {
		vector<ImageDB::ImageDrawEvent>& sceneQueue = ImageDB::getInstance().getSceneImageQueue();
		SDL_RenderSetScale(sdl_renderer, zoom_factor, zoom_factor);

		int pixels_per_meter = 100;
		float screen_width = static_cast<float>(window_dimensions.x) / (pixels_per_meter * zoom_factor);
		float screen_height = static_cast<float>(window_dimensions.y) / (pixels_per_meter * zoom_factor);

		float left_bound = camera_pos.x - screen_width / 2.0f - 1.0f;
		float right_bound = camera_pos.x + screen_width / 2.0f + 1.0f;
		float top_bound = camera_pos.y - screen_height / 2.0f - 1.0f;
		float bottom_bound = camera_pos.y + screen_height / 2.0f + 1.0f;
		
		visibleQueue.reserve(sceneQueue.size());

		for (const auto& event : sceneQueue) {
			ImageDB::textureItem* item = ImageDB::getInstance().getTextureByName(sdl_renderer, event.image_name);
			
			float width_world = item->width * glm::abs(event.scale.x) / pixels_per_meter;
			float height_world = item->height * glm::abs(event.scale.y) / pixels_per_meter;

			float obj_left = event.screen_coord.x - (event.pivot.x * width_world);
			float obj_right = event.screen_coord.x + ((1.0f - event.pivot.x) * width_world);
			float obj_top = event.screen_coord.y - (event.pivot.y * height_world);
			float obj_bottom = event.screen_coord.y + ((1.0f - event.pivot.y) * height_world);

			if (obj_right < left_bound || obj_left > right_bound ||
				obj_bottom < top_bound || obj_top > bottom_bound) {
				continue;
			}

			visibleQueue.push_back(event);
		}

		drawQueue = &visibleQueue;
	}

	stable_sort(drawQueue->begin(), drawQueue->end(),
		[](const ImageDB::ImageDrawEvent& a, const ImageDB::ImageDrawEvent& b) {
			if (a.sorting_order != b.sorting_order) {
				return a.sorting_order < b.sorting_order;
			}
			return a.call_order < b.call_order;
		});

	for (auto& event : *drawQueue) {
		int pixels_per_meter = 1;
		SDL_FPoint screen_center = { 0, 0 };
		glm::vec2 screen_position = event.screen_coord;
		if (!render_ui) {
			pixels_per_meter = 100;
			screen_center = {
				static_cast<float>(window_dimensions.x) * 0.5f * (1.0f / zoom_factor),
				static_cast<float>(window_dimensions.y) * 0.5f * (1.0f / zoom_factor)
			};
			screen_position -= camera_pos;
		}

		ImageDB::textureItem* item = ImageDB::getInstance().getTextureByName(sdl_renderer, event.image_name);
		SDL_Texture* texture = item->texture;
		SDL_FRect destRect;
		destRect.w = item->width * glm::abs(event.scale.x);
		destRect.h = item->height * glm::abs(event.scale.y);

		int flip_mode = SDL_FLIP_NONE;
		if (event.scale.x < 0) {
			flip_mode |= SDL_FLIP_HORIZONTAL;
		}
		if (event.scale.y < 0) {
			flip_mode |= SDL_FLIP_VERTICAL;
		}

		SDL_FPoint pivot_point = {
			event.pivot.x * destRect.w, 
			event.pivot.y * destRect.h
		};
		
		destRect.x = screen_position.x * pixels_per_meter + screen_center.x - pivot_point.x;
		destRect.y = screen_position.y * pixels_per_meter + screen_center.y - pivot_point.y;
		
		SDL_SetTextureColorMod(texture, event.color.r, event.color.g, event.color.b);
		SDL_SetTextureAlphaMod(texture, event.color.a);

		Helper::SDL_RenderCopyEx(0, "", sdl_renderer, texture, NULL, &destRect,
			event.rotation_degrees, &pivot_point, static_cast<SDL_RendererFlip>(flip_mode));

		SDL_SetTextureColorMod(texture, 255, 255, 255);
		SDL_SetTextureAlphaMod(texture, 255);
	}
	SDL_RenderSetScale(sdl_renderer, 1, 1);
}

void Renderer::renderUIText() {
	queue<TextDB::TextDrawEvent>& drawQueue = textDB.getQueue();

	while (!drawQueue.empty()) {
		const auto& curEvent = drawQueue.front();

		SDL_Texture* texture = nullptr;
		float textWidth, textHeight;

		if (textDB.getTextTexture(sdl_renderer, &texture, textWidth, textHeight,
			curEvent.content, curEvent.font_name, curEvent.font_size, curEvent.color)) {
			SDL_FRect destRect;
			destRect.x = static_cast<float>(curEvent.screen_coord.x);
			destRect.y = static_cast<float>(curEvent.screen_coord.y);
			destRect.w = static_cast<float>(textWidth);
			destRect.h = static_cast<float>(textHeight);

			Helper::SDL_RenderCopyEx(0, "", sdl_renderer, texture, NULL, &destRect, 0, NULL, SDL_FLIP_NONE);
		}

		drawQueue.pop();
	}
}

void Renderer::renderPixel() {
	vector<ImageDB::ImageDrawEvent>* drawQueue = &ImageDB::getInstance().getPixelQueue();
	SDL_SetRenderDrawBlendMode(sdl_renderer, SDL_BLENDMODE_BLEND);

	for (auto& event : *drawQueue) {
		SDL_SetRenderDrawColor(sdl_renderer, event.color.r, event.color.g,
			event.color.b, event.color.a);
		SDL_RenderDrawPoint(sdl_renderer, 
			static_cast<int>(event.screen_coord.x), static_cast<int>(event.screen_coord.y));
	}
	SDL_SetRenderDrawBlendMode(sdl_renderer, SDL_BLENDMODE_NONE);
}

void Renderer::renderLights() {
	if (lightSystem.IsEnabled()) {
		lightSystem.RenderLighting(
			sdl_renderer,
			window_dimensions.x,
			window_dimensions.y,
			camera_pos.x,
			camera_pos.y,
			zoom_factor
		);
	}
}

void Renderer::SetPosition(float x, float y) {
	getInstance().camera_pos.x = x;
	getInstance().camera_pos.y = y;
}

float Renderer::GetPositionX() {
	return getInstance().camera_pos.x;
}

float Renderer::GetPositionY() {
	return getInstance().camera_pos.y;
}

void Renderer::SetZoom(float zoom_factor) {
	getInstance().zoom_factor = zoom_factor;
}

float Renderer::GetZoom() {
	return getInstance().zoom_factor;
}

SDL_Renderer* Renderer::GetSDLRenderer() {
	return sdl_renderer;
}