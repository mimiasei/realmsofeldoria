/*
 * ResourceBar.h, part of Realms of Eldoria
 * UI component for displaying player resources
 */
#pragma once

#include "../../lib/gui/Widget.h"
#include "../../include/GameTypes.h"
#include <memory>

class GameState;

/// Resource bar that displays all player resources at the top of screen
class ResourceBar : public Panel {
private:
    GameState* gameState;
    std::vector<std::shared_ptr<Label>> resourceLabels;
    std::shared_ptr<Label> dayLabel;

    void updateLabels();

public:
    ResourceBar(GameState* state);

    /// Update resource display from current game state
    void refresh();

    void render(Canvas& canvas) override;
};
