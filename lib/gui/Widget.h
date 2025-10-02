/*
 * Widget.h, part of Realms of Eldoria
 * Based on VCMI's CIntObject
 *
 * Original VCMI code:
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 * Simplified for Realms of Eldoria
 */
#pragma once

#include "../geometry/Point.h"
#include "../geometry/Rect.h"
#include "../geometry/Color.h"
#include <vector>
#include <memory>
#include <functional>
#include <string>

class Canvas;

/// Base class for all UI widgets
class Widget {
public:
    /// Position and size of widget on screen
    Rect pos;

    /// Parent widget (nullptr for root widgets)
    Widget* parent;

    /// Child widgets
    std::vector<std::unique_ptr<Widget>> children;

    /// Whether widget is visible
    bool visible;

    /// Whether widget can receive input events
    bool enabled;

    Widget(const Rect& position = Rect());
    virtual ~Widget() = default;

    /// Draw widget (called each frame)
    virtual void render(Canvas& canvas);

    /// Handle mouse click at position (in screen coordinates)
    /// Returns true if click was handled
    virtual bool onClick(const Point& pos);

    /// Handle mouse move/hover at position
    virtual void onHover(const Point& pos);

    /// Handle mouse wheel scroll
    virtual void onScroll(int delta);

    /// Handle keyboard input
    virtual void onKeyPress(int key);

    /// Update widget state (called each frame with delta time in ms)
    virtual void update(uint32_t deltaMs);

    /// Add child widget
    void addChild(std::unique_ptr<Widget> child);

    /// Remove child widget
    void removeChild(Widget* child);

    /// Check if point is inside widget bounds
    bool contains(const Point& p) const;

    /// Move widget to new position
    void moveTo(const Point& newPos);

    /// Resize widget
    void resize(const Point& newSize);

    /// Show/hide widget
    void setVisible(bool vis);

    /// Enable/disable widget
    void setEnabled(bool en);
};

/// Image widget - displays a static image
class ImageWidget : public Widget {
public:
    std::shared_ptr<class Image> image;

    ImageWidget(const Rect& position, std::shared_ptr<class Image> img);

    void render(Canvas& canvas) override;
};

/// Label widget - displays text
class Label : public Widget {
public:
    std::string text;
    Color textColor;
    int fontSize;

    enum class Alignment {
        Left,
        Center,
        Right
    };
    Alignment alignment;

    Label(const Rect& position, const std::string& text, const Color& color = Color(255, 255, 255));

    void setText(const std::string& newText);
    void setColor(const Color& color);

    void render(Canvas& canvas) override;
};

/// Button widget - clickable button with callback
class Button : public Widget {
public:
    std::string text;
    Color normalColor;
    Color hoverColor;
    Color pressedColor;
    Color textColor;

    std::function<void()> callback;

    bool hovered;
    bool pressed;

    Button(const Rect& position, const std::string& text, std::function<void()> onClick);

    void render(Canvas& canvas) override;
    bool onClick(const Point& pos) override;
    void onHover(const Point& pos) override;
};

/// Panel widget - container for other widgets with background
class Panel : public Widget {
public:
    Color backgroundColor;
    Color borderColor;
    int borderWidth;

    Panel(const Rect& position, const Color& bgColor = Color(50, 50, 50, 200));

    void render(Canvas& canvas) override;
};
