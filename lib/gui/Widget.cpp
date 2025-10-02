/*
 * Widget.cpp, part of Realms of Eldoria
 * Based on VCMI's CIntObject
 *
 * Original VCMI code:
 * License: GNU General Public License v2.0 or later
 *
 * Simplified for Realms of Eldoria
 */
#include "Widget.h"
#include "../render/Canvas.h"
#include "../render/Image.h"
#include "../render/Font.h"
#include <algorithm>

// Widget base class implementation

Widget::Widget(const Rect& position)
    : pos(position)
    , parent(nullptr)
    , visible(true)
    , enabled(true)
{
}

void Widget::render(Canvas& canvas) {
    if (!visible) return;

    // Render all children
    for (auto& child : children) {
        child->render(canvas);
    }
}

bool Widget::onClick(const Point& p) {
    if (!visible || !enabled) return false;

    // Check children first (front to back)
    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        if ((*it)->onClick(p)) {
            return true;
        }
    }

    return false;
}

void Widget::onHover(const Point& p) {
    if (!visible || !enabled) return;

    for (auto& child : children) {
        child->onHover(p);
    }
}

void Widget::onScroll(int delta) {
    if (!visible || !enabled) return;

    for (auto& child : children) {
        child->onScroll(delta);
    }
}

void Widget::onKeyPress(int key) {
    if (!visible || !enabled) return;

    for (auto& child : children) {
        child->onKeyPress(key);
    }
}

void Widget::update(uint32_t deltaMs) {
    if (!visible) return;

    for (auto& child : children) {
        child->update(deltaMs);
    }
}

void Widget::addChild(std::unique_ptr<Widget> child) {
    child->parent = this;
    children.push_back(std::move(child));
}

void Widget::removeChild(Widget* child) {
    auto it = std::find_if(children.begin(), children.end(),
        [child](const std::unique_ptr<Widget>& ptr) { return ptr.get() == child; });

    if (it != children.end()) {
        children.erase(it);
    }
}

bool Widget::contains(const Point& p) const {
    return pos.isInside(p);
}

void Widget::moveTo(const Point& newPos) {
    Point delta = newPos - Point(pos.x, pos.y);
    pos.x = newPos.x;
    pos.y = newPos.y;

    // Move children as well
    for (auto& child : children) {
        child->moveTo(Point(child->pos.x, child->pos.y) + delta);
    }
}

void Widget::resize(const Point& newSize) {
    pos.w = newSize.x;
    pos.h = newSize.y;
}

void Widget::setVisible(bool vis) {
    visible = vis;
}

void Widget::setEnabled(bool en) {
    enabled = en;
}

// ImageWidget implementation

ImageWidget::ImageWidget(const Rect& position, std::shared_ptr<Image> img)
    : Widget(position)
    , image(img)
{
}

void ImageWidget::render(Canvas& canvas) {
    if (!visible || !image) return;

    canvas.draw(*image, Point(pos.x, pos.y));

    Widget::render(canvas);
}

// Label implementation

Label::Label(const Rect& position, const std::string& txt, const Color& color)
    : Widget(position)
    , text(txt)
    , textColor(color)
    , fontSize(16)
    , alignment(Alignment::Left)
{
}

void Label::setText(const std::string& newText) {
    text = newText;
}

void Label::setColor(const Color& color) {
    textColor = color;
}

void Label::render(Canvas& canvas) {
    if (!visible) return;

    if (!text.empty()) {
        auto font = FontManager::instance().getDefaultFont(fontSize);
        if (font) {
            Point textSize = font->measureText(text);

            // Calculate position based on alignment
            int x = pos.x;
            if (alignment == Alignment::Center) {
                x = pos.x + (pos.w - textSize.x) / 2;
            } else if (alignment == Alignment::Right) {
                x = pos.x + pos.w - textSize.x;
            }

            int y = pos.y + (pos.h - textSize.y) / 2;  // Vertically center

            font->renderTo(canvas, text, Point(x, y), textColor);
        }
    }

    Widget::render(canvas);
}

// Button implementation

Button::Button(const Rect& position, const std::string& txt, std::function<void()> onClick)
    : Widget(position)
    , text(txt)
    , normalColor(Color(80, 80, 120))
    , hoverColor(Color(100, 100, 150))
    , pressedColor(Color(60, 60, 100))
    , textColor(Color(255, 255, 255))
    , callback(onClick)
    , hovered(false)
    , pressed(false)
{
}

void Button::render(Canvas& canvas) {
    if (!visible) return;

    // Choose color based on state
    Color bgColor = normalColor;
    if (pressed) {
        bgColor = pressedColor;
    } else if (hovered) {
        bgColor = hoverColor;
    }

    // Draw button background
    canvas.drawRect(pos, bgColor);

    // Draw border
    canvas.drawBorder(pos, Color(200, 200, 200), 2);

    // Draw text
    if (!text.empty()) {
        auto font = FontManager::instance().getDefaultFont(16);
        if (font) {
            Point textSize = font->measureText(text);
            int x = pos.x + (pos.w - textSize.x) / 2;
            int y = pos.y + (pos.h - textSize.y) / 2;
            font->renderTo(canvas, text, Point(x, y), textColor);
        }
    }

    Widget::render(canvas);
}

bool Button::onClick(const Point& p) {
    if (!visible || !enabled) return false;

    if (contains(p)) {
        pressed = true;
        if (callback) {
            callback();
        }
        return true;
    }

    pressed = false;
    return Widget::onClick(p);
}

void Button::onHover(const Point& p) {
    if (!visible || !enabled) {
        hovered = false;
        return;
    }

    hovered = contains(p);
    Widget::onHover(p);
}

// Panel implementation

Panel::Panel(const Rect& position, const Color& bgColor)
    : Widget(position)
    , backgroundColor(bgColor)
    , borderColor(Color(100, 100, 100))
    , borderWidth(1)
{
}

void Panel::render(Canvas& canvas) {
    if (!visible) return;

    // Draw background
    canvas.drawRect(pos, backgroundColor);

    // Draw border
    if (borderWidth > 0) {
        canvas.drawBorder(pos, borderColor, borderWidth);
    }

    // Render children
    Widget::render(canvas);
}
