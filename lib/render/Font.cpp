/*
 * Font.cpp, part of Realms of Eldoria
 * Simple SDL_ttf font rendering
 */
#include "Font.h"
#include "Canvas.h"
#include "Image.h"
#include <SDL2/SDL_ttf.h>
#include <stdexcept>
#include <iostream>
#include <vector>

bool Font::ttfInitialized = false;

Font::Font(const std::string& fontPath, int fontSize)
    : font(nullptr)
    , size(fontSize)
{
    initTTF();

    font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (!font) {
        std::cerr << "Failed to load font: " << fontPath << " - " << TTF_GetError() << std::endl;
        throw std::runtime_error("Failed to load font: " + fontPath);
    }
}

Font::~Font() {
    if (font) {
        TTF_CloseFont(font);
    }
}

SDL_Surface* Font::renderText(const std::string& text, const Color& color) {
    if (!font || text.empty()) {
        return nullptr;
    }

    SDL_Color sdlColor = { color.r, color.g, color.b, color.a };
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), sdlColor);

    if (!surface) {
        std::cerr << "Failed to render text: " << TTF_GetError() << std::endl;
    }

    return surface;
}

void Font::renderTo(Canvas& canvas, const std::string& text, const Point& pos, const Color& color) {
    SDL_Surface* textSurface = renderText(text, color);
    if (!textSurface) {
        return;
    }

    // Create temporary image and draw it
    Image textImage(textSurface);
    canvas.draw(textImage, pos);
}

Point Font::measureText(const std::string& text) {
    if (!font || text.empty()) {
        return Point(0, 0);
    }

    int w = 0, h = 0;
    TTF_SizeText(font, text.c_str(), &w, &h);
    return Point(w, h);
}

void Font::initTTF() {
    if (!ttfInitialized) {
        if (TTF_Init() == -1) {
            std::cerr << "Failed to initialize SDL_ttf: " << TTF_GetError() << std::endl;
            throw std::runtime_error("Failed to initialize SDL_ttf");
        }
        ttfInitialized = true;
    }
}

void Font::quitTTF() {
    if (ttfInitialized) {
        TTF_Quit();
        ttfInitialized = false;
    }
}

// FontManager implementation

FontManager::FontManager() {
    Font::initTTF();
}

FontManager::~FontManager() {
    fonts.clear();
    Font::quitTTF();
}

FontManager& FontManager::instance() {
    static FontManager instance;
    return instance;
}

std::shared_ptr<Font> FontManager::getFont(const std::string& fontPath, int size) {
    auto key = std::make_pair(fontPath, size);
    auto it = fonts.find(key);

    if (it != fonts.end()) {
        return it->second;
    }

    // Load new font
    try {
        auto font = std::make_shared<Font>(fontPath, size);
        fonts[key] = font;
        return font;
    } catch (const std::exception& e) {
        std::cerr << "Failed to load font: " << e.what() << std::endl;
        return nullptr;
    }
}

std::shared_ptr<Font> FontManager::getDefaultFont(int size) {
    // Try common system font paths
    std::vector<std::string> fontPaths = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/System/Library/Fonts/Helvetica.ttc",  // macOS
        "C:\\Windows\\Fonts\\arial.ttf",         // Windows
        "assets/fonts/default.ttf"               // Local fallback
    };

    for (const auto& path : fontPaths) {
        auto font = getFont(path, size);
        if (font) {
            return font;
        }
    }

    std::cerr << "Warning: Could not load any default font!" << std::endl;
    return nullptr;
}
