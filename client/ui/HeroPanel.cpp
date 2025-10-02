/*
 * HeroPanel.cpp, part of Realms of Eldoria
 * UI component for displaying hero information
 */
#include "HeroPanel.h"
#include "../../lib/gamestate/GameState.h"
#include "../../lib/entities/hero/Hero.h"
#include "../../lib/render/Canvas.h"
#include <sstream>
#include <iomanip>

HeroPanel::HeroPanel(GameState* state)
    : Panel(Rect(1920 - 300, 50, 300, 1080 - 50), Color(40, 40, 60, 220))
    , gameState(state)
    , currentHero(nullptr)
{
    borderColor = Color(100, 100, 120);
    borderWidth = 2;

    createWidgets();
}

void HeroPanel::createWidgets() {
    const int padding = 10;
    const int labelHeight = 25;
    const int x = pos.x + padding;
    int y = pos.y + padding;

    // Hero name
    nameLabel = std::make_shared<Label>(
        Rect(x, y, pos.w - 2 * padding, 30),
        "No Hero Selected",
        Color(255, 255, 120)
    );
    nameLabel->fontSize = 18;
    nameLabel->alignment = Label::Alignment::Center;
    y += 40;

    // Level and Experience
    levelLabel = std::make_shared<Label>(
        Rect(x, y, pos.w - 2 * padding, labelHeight),
        "Level: -",
        Color(200, 200, 200)
    );
    levelLabel->fontSize = 14;
    y += labelHeight + 5;

    experienceLabel = std::make_shared<Label>(
        Rect(x, y, pos.w - 2 * padding, labelHeight),
        "Experience: -",
        Color(200, 200, 200)
    );
    experienceLabel->fontSize = 14;
    y += labelHeight + 15;

    // Primary stats
    attackLabel = std::make_shared<Label>(
        Rect(x, y, pos.w - 2 * padding, labelHeight),
        "Attack: -",
        Color(255, 100, 100)
    );
    attackLabel->fontSize = 14;
    y += labelHeight + 5;

    defenseLabel = std::make_shared<Label>(
        Rect(x, y, pos.w - 2 * padding, labelHeight),
        "Defense: -",
        Color(100, 100, 255)
    );
    defenseLabel->fontSize = 14;
    y += labelHeight + 5;

    powerLabel = std::make_shared<Label>(
        Rect(x, y, pos.w - 2 * padding, labelHeight),
        "Spell Power: -",
        Color(100, 255, 255)
    );
    powerLabel->fontSize = 14;
    y += labelHeight + 5;

    knowledgeLabel = std::make_shared<Label>(
        Rect(x, y, pos.w - 2 * padding, labelHeight),
        "Knowledge: -",
        Color(255, 100, 255)
    );
    knowledgeLabel->fontSize = 14;
    y += labelHeight + 15;

    // Movement
    movementLabel = std::make_shared<Label>(
        Rect(x, y, pos.w - 2 * padding, labelHeight),
        "Movement: -",
        Color(100, 255, 100)
    );
    movementLabel->fontSize = 14;
    y += labelHeight + 20;

    // Army section
    auto armyHeaderLabel = std::make_shared<Label>(
        Rect(x, y, pos.w - 2 * padding, labelHeight),
        "Army:",
        Color(255, 255, 120)
    );
    armyHeaderLabel->fontSize = 16;
    y += labelHeight + 10;

    // Create 7 army slot labels
    for (int i = 0; i < 7; ++i) {
        auto slotLabel = std::make_shared<Label>(
            Rect(x, y, pos.w - 2 * padding, labelHeight),
            "",
            Color(180, 180, 180)
        );
        slotLabel->fontSize = 13;
        armySlotLabels.push_back(slotLabel);
        y += labelHeight + 3;
    }

    // Next turn button at bottom
    nextTurnButton = std::make_shared<Button>(
        Rect(x, pos.y + pos.h - 60, pos.w - 2 * padding, 40),
        "Next Turn (N)",
        [this]() {
            if (gameState) {
                gameState->nextTurn();
                refresh();
            }
        }
    );
    nextTurnButton->normalColor = Color(60, 100, 60);
    nextTurnButton->hoverColor = Color(80, 120, 80);
    nextTurnButton->pressedColor = Color(40, 80, 40);

    // Store labels that need rendering (not children, we'll manage manually)
    // This is simpler than the full widget tree
}

void HeroPanel::setHero(Hero* hero) {
    currentHero = hero;
    refresh();
}

void HeroPanel::refresh() {
    if (!currentHero) {
        nameLabel->setText("No Hero Selected");
        levelLabel->setText("Level: -");
        experienceLabel->setText("Experience: -");
        attackLabel->setText("Attack: -");
        defenseLabel->setText("Defense: -");
        powerLabel->setText("Spell Power: -");
        knowledgeLabel->setText("Knowledge: -");
        movementLabel->setText("Movement: -");

        for (auto& label : armySlotLabels) {
            label->setText("");
        }
        return;
    }

    // Update hero info
    nameLabel->setText(currentHero->getName());

    std::ostringstream oss;
    oss << "Level: " << currentHero->getLevel();
    levelLabel->setText(oss.str());

    oss.str("");
    oss << "Experience: " << currentHero->getExperience();
    experienceLabel->setText(oss.str());

    oss.str("");
    oss << "Attack: " << currentHero->getAttack();
    attackLabel->setText(oss.str());

    oss.str("");
    oss << "Defense: " << currentHero->getDefense();
    defenseLabel->setText(oss.str());

    oss.str("");
    oss << "Spell Power: " << currentHero->getSpellPower();
    powerLabel->setText(oss.str());

    oss.str("");
    oss << "Knowledge: " << currentHero->getKnowledge();
    knowledgeLabel->setText(oss.str());

    oss.str("");
    oss << "Movement: " << currentHero->getMovementPoints();
    movementLabel->setText(oss.str());

    // Update army
    const auto& army = currentHero->getArmy();
    for (size_t i = 0; i < 7; ++i) {
        const auto& slot = army.getSlot(i);
        if (slot.count > 0) {
            const Creature* creature = GameState::getCreatureData(slot.creatureId);
            oss.str("");
            if (creature) {
                oss << creature->getName() << " x" << slot.count;
            } else {
                oss << "Unknown x" << slot.count;
            }
            armySlotLabels[i]->setText(oss.str());
        } else {
            armySlotLabels[i]->setText("");
        }
    }
}

void HeroPanel::render(Canvas& canvas) {
    // Draw panel background
    Panel::render(canvas);

    // Render all labels
    if (nameLabel) nameLabel->render(canvas);
    if (levelLabel) levelLabel->render(canvas);
    if (experienceLabel) experienceLabel->render(canvas);
    if (attackLabel) attackLabel->render(canvas);
    if (defenseLabel) defenseLabel->render(canvas);
    if (powerLabel) powerLabel->render(canvas);
    if (knowledgeLabel) knowledgeLabel->render(canvas);
    if (movementLabel) movementLabel->render(canvas);

    for (auto& label : armySlotLabels) {
        label->render(canvas);
    }

    if (nextTurnButton) {
        nextTurnButton->render(canvas);
    }
}

bool HeroPanel::onClick(const Point& p) {
    if (!visible || !enabled) return false;

    // Check next turn button
    if (nextTurnButton && nextTurnButton->onClick(p)) {
        return true;
    }

    return Panel::onClick(p);
}

void HeroPanel::onHover(const Point& p) {
    if (!visible || !enabled) return;

    if (nextTurnButton) {
        nextTurnButton->onHover(p);
    }

    Panel::onHover(p);
}
