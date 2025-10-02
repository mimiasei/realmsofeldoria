/*
 * ResourceBar.cpp, part of Realms of Eldoria
 * UI component for displaying player resources
 */
#include "ResourceBar.h"
#include "../../lib/gamestate/GameState.h"
#include "../../lib/render/Canvas.h"
#include <sstream>
#include <iomanip>

ResourceBar::ResourceBar(GameState* state)
    : Panel(Rect(0, 0, 1920, 50), Color(40, 40, 60, 220))
    , gameState(state)
{
    borderColor = Color(100, 100, 120);
    borderWidth = 2;

    // Create labels for each resource type
    const int labelWidth = 120;
    const int labelHeight = 40;
    const int spacing = 10;
    const int startX = 20;
    const int y = 5;

    // Resource types in display order
    std::vector<ResourceType> resourceTypes = {
        ResourceType::Gold,
        ResourceType::Wood,
        ResourceType::Ore,
        ResourceType::Mercury,
        ResourceType::Sulfur,
        ResourceType::Crystal,
        ResourceType::Gems
    };

    int x = startX;
    for (size_t i = 0; i < resourceTypes.size(); ++i) {
        auto label = std::make_shared<Label>(
            Rect(x, y, labelWidth, labelHeight),
            "",
            Color(255, 255, 255)
        );
        label->fontSize = 14;
        label->alignment = Label::Alignment::Left;
        resourceLabels.push_back(label);

        x += labelWidth + spacing;
    }

    // Day label on the right side
    dayLabel = std::make_shared<Label>(
        Rect(1920 - 200, y, 180, labelHeight),
        "",
        Color(255, 220, 120)
    );
    dayLabel->fontSize = 16;
    dayLabel->alignment = Label::Alignment::Right;

    refresh();
}

void ResourceBar::refresh() {
    if (!gameState) return;

    Player* player = gameState->getPlayer(gameState->getCurrentPlayer());
    if (!player) return;

    auto& resources = player->getResources();

    // Resource types and their names
    struct ResInfo {
        ResourceType type;
        std::string name;
        Color color;
    };

    std::vector<ResInfo> resInfos = {
        {ResourceType::Gold, "Gold", Color(255, 215, 0)},
        {ResourceType::Wood, "Wood", Color(139, 69, 19)},
        {ResourceType::Ore, "Ore", Color(128, 128, 128)},
        {ResourceType::Mercury, "Merc", Color(192, 192, 192)},
        {ResourceType::Sulfur, "Sulf", Color(255, 255, 0)},
        {ResourceType::Crystal, "Crys", Color(0, 255, 255)},
        {ResourceType::Gems, "Gems", Color(255, 0, 255)}
    };

    for (size_t i = 0; i < resInfos.size() && i < resourceLabels.size(); ++i) {
        int amount = resources[resInfos[i].type];
        std::ostringstream oss;
        oss << resInfos[i].name << ": " << amount;

        resourceLabels[i]->setText(oss.str());
        resourceLabels[i]->setColor(resInfos[i].color);
    }

    // Update day
    std::ostringstream dayOss;
    dayOss << "Day " << gameState->getTurnManager().getDayNumber();
    dayLabel->setText(dayOss.str());
}

void ResourceBar::render(Canvas& canvas) {
    // Draw panel background
    Panel::render(canvas);

    // Render resource labels
    for (auto& label : resourceLabels) {
        label->render(canvas);
    }

    // Render day label
    if (dayLabel) {
        dayLabel->render(canvas);
    }
}
