/*
 * HeroPanel.h, part of Realms of Eldoria
 * UI component for displaying hero information
 */
#pragma once

#include "../../lib/gui/Widget.h"
#include "../../include/GameTypes.h"
#include <memory>
#include <vector>

class Hero;
class GameState;

/// Hero panel that displays selected hero's stats and army
class HeroPanel : public Panel {
private:
    GameState* gameState;
    Hero* currentHero;

    std::shared_ptr<Label> nameLabel;
    std::shared_ptr<Label> levelLabel;
    std::shared_ptr<Label> experienceLabel;
    std::shared_ptr<Label> movementLabel;
    std::shared_ptr<Label> attackLabel;
    std::shared_ptr<Label> defenseLabel;
    std::shared_ptr<Label> powerLabel;
    std::shared_ptr<Label> knowledgeLabel;

    // Army display
    std::vector<std::shared_ptr<Label>> armySlotLabels;

    std::shared_ptr<Button> nextTurnButton;

    void createWidgets();
    void updateWidgets();

public:
    HeroPanel(GameState* state);

    /// Set the hero to display
    void setHero(Hero* hero);

    /// Refresh display from current hero data
    void refresh();

    void render(Canvas& canvas) override;
    bool onClick(const Point& pos) override;
    void onHover(const Point& pos) override;
};
