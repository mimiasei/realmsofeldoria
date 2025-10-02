/*
 * Font.h, part of Realms of Eldoria
 * Simple SDL_ttf font rendering
 */
#pragma once

#include "../geometry/Point.h"
#include "../geometry/Color.h"
#include <string>
#include <memory>
#include <map>
#include <vector>

struct _TTF_Font;
typedef struct _TTF_Font TTF_Font;
struct SDL_Surface;
class Canvas;

/// Font rendering class using SDL_ttf
class Font {
private:
    TTF_Font* font;
    int size;

    // Cache for rendered text surfaces
    static bool ttfInitialized;

public:
    Font(const std::string& fontPath, int fontSize);
    ~Font();

    /// Render text to a surface
    SDL_Surface* renderText(const std::string& text, const Color& color);

    /// Render text directly to canvas at position
    void renderTo(Canvas& canvas, const std::string& text, const Point& pos, const Color& color);

    /// Get size of text when rendered
    Point measureText(const std::string& text);

    /// Get font size
    int getSize() const { return size; }

    /// Initialize SDL_ttf (called automatically)
    static void initTTF();

    /// Cleanup SDL_ttf (called automatically)
    static void quitTTF();
};

/// Font manager for loading and caching fonts
class FontManager {
private:
    std::map<std::pair<std::string, int>, std::shared_ptr<Font>> fonts;

    FontManager();
    ~FontManager();

public:
    static FontManager& instance();

    /// Get or load a font
    std::shared_ptr<Font> getFont(const std::string& fontPath, int size);

    /// Get default font
    std::shared_ptr<Font> getDefaultFont(int size = 16);
};
