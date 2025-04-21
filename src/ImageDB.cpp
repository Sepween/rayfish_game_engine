#include <filesystem>
#include <iostream>

#include "ImageDB.h"
#include "EngineUtils.h"

using namespace std;

void ImageDB::DrawUI(const string& image_name, float x, float y) {
    SDL_Color color = { 255, 255, 255, 255 };
    getInstance().queueDrawImageEvent(image_name, x, y, 0.0f,
        1.0f, 1.0f, 0.0f, 0.0f, color, 0, ImageRenderType::UI);
}

void ImageDB::DrawUIEx(const string& image_name, float x, float y, float r, float g, float b, float a,
    float sorting_order) {
    int rInt = static_cast<int>(r);
    int gInt = static_cast<int>(g);
    int bInt = static_cast<int>(b);
    int aInt = static_cast<int>(a);
    int sorting_order_int = static_cast<int>(sorting_order);
    SDL_Color color = { static_cast<Uint8>(rInt), static_cast<Uint8>(gInt),
        static_cast<Uint8>(bInt), static_cast<Uint8>(aInt) };
    getInstance().queueDrawImageEvent(image_name, x, y, 0.0f,
        1.0f, 1.0f, 0.0f, 0.0f, color, sorting_order_int, ImageRenderType::UI);
}

void ImageDB::Draw(const string& image_name, float x, float y) {
    SDL_Color color = { 255, 255, 255, 255 };
    getInstance().queueDrawImageEvent(image_name, x, y, 0.0f,
        1.0f, 1.0f, 0.5f, 0.5f, color, 0, ImageRenderType::SceneSpace);
}

void ImageDB::DrawEx(const string& image_name, float x, float y, float rotation_degrees,
    float scale_x, float scale_y, float pivot_x, float pivot_y, float r, float g, float b, float a,
    float sorting_order) {
    int rInt = static_cast<int>(r);
    int gInt = static_cast<int>(g);
    int bInt = static_cast<int>(b);
    int aInt = static_cast<int>(a);
    int sorting_order_int = static_cast<int>(sorting_order);
    SDL_Color color = { static_cast<Uint8>(rInt), static_cast<Uint8>(gInt),
        static_cast<Uint8>(bInt), static_cast<Uint8>(aInt) };
    getInstance().queueDrawImageEvent(image_name, x, y, rotation_degrees,
        scale_x, scale_y, pivot_x, pivot_y, color, sorting_order_int, ImageRenderType::SceneSpace);
}

void ImageDB::DrawPixel(float x, float y, float r, float g, float b, float a) {
    int rInt = static_cast<int>(r);
    int gInt = static_cast<int>(g);
    int bInt = static_cast<int>(b);
    int aInt = static_cast<int>(a);
    SDL_Color color = { static_cast<Uint8>(rInt), static_cast<Uint8>(gInt),
        static_cast<Uint8>(bInt), static_cast<Uint8>(aInt) };
    getInstance().queueDrawImageEvent("", x, y, 0.0f,
        1.0f, 1.0f, 0.0f, 0.0f, color, 0, ImageRenderType::Pixel);
}

void ImageDB::queueDrawImageEvent(const string& image_name, float x, float y, float rotation_degrees,
    float scale_x, float scale_y, float pivot_x, float pivot_y, SDL_Color color,
    int sorting_order, ImageRenderType render_type){
    ImageDrawEvent event;
    event.image_name = image_name;
    event.screen_coord = { x, y };
    event.rotation_degrees = rotation_degrees;
    event.scale.x = scale_x;
    event.scale.y = scale_y;
    event.pivot.x = pivot_x;
    event.pivot.y = pivot_y;
    event.color = color;
    event.sorting_order = sorting_order;
    event.call_order = next_call_order++;

    switch (render_type) {
    case ImageRenderType::SceneSpace:
        sceneImageQueue.push_back(event);
        break;
    case ImageRenderType::UI:
        uiImageQueue.push_back(event);
        break;
    case ImageRenderType::Pixel:
        pixelQueue.push_back(event);
        break;
    }
}

void ImageDB::CreateDefaultParticleTextureWithName(SDL_Renderer* renderer, const string& name) {
    if (textureCache.find(name) != textureCache.end()) return;

    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, 8, 8, 32, SDL_PIXELFORMAT_RGBA8888);
    
    Uint32 whiteColor = SDL_MapRGBA(surface->format, 255, 255, 255, 255);
    SDL_FillRect(surface, NULL, whiteColor);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FreeSurface(surface);
    textureCache[name].texture = texture;
    Helper::SDL_QueryTexture(texture, &(textureCache[name].width), &(textureCache[name].height));
}

ImageDB::textureItem* ImageDB::getTextureByName(SDL_Renderer* renderer, const string& imageName) {
    loadImageFromFolder(renderer, imageName);
    return &textureCache[imageName];
}

void ImageDB::loadImageFromFolder(SDL_Renderer* renderer, const string& imageName) {
    auto it = textureCache.find(imageName);
    if (it != textureCache.end()) {
        return;
    }

    string imgPath = EngineUtils::getImage(imageName);
    SDL_Texture* texture = IMG_LoadTexture(renderer, imgPath.c_str());

    if (!texture) {
        cout << "error: missing image " << imageName;
        exit(0);
    }

    textureCache[imageName].texture = texture;
    Helper::SDL_QueryTexture(texture, &(textureCache[imageName].width), &(textureCache[imageName].height));
}