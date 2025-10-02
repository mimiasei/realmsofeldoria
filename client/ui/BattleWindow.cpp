#include "BattleWindow.h"
#include "../../lib/render/Canvas.h"
#include "../../lib/geometry/Color.h"
#include "../../lib/gamestate/GameState.h"
#include "../../lib/entities/creature/Creature.h"
#include <sstream>

// BattleLog implementation
BattleLog::BattleLog(const Point& position, const Point& size)
    : Widget(Rect(position.x, position.y, size.x, size.y)) {
}

void BattleLog::addMessage(const std::string& message) {
    // Create a new label for this message
    auto label = std::make_shared<Label>(
        Rect(pos.x + 10, pos.y + 35 + (int)messages.size() * 18, pos.w - 20, 18),
        message,
        ColorRGBA(180, 180, 200, 255)
    );
    label->fontSize = 14;
    messages.push_back(message);

    if (messages.size() > maxMessages) {
        messages.erase(messages.begin());
    }
}

void BattleLog::clear() {
    messages.clear();
}

void BattleLog::render(Canvas& canvas) {
    // Draw background panel
    canvas.drawRect(pos, ColorRGBA(30, 30, 40, 220));
    canvas.drawBorder(pos, ColorRGBA(80, 80, 100, 255), 2);

    // Draw title
    auto titleLabel = Label(
        Rect(pos.x + 10, pos.y + 10, pos.w - 20, 20),
        "Battle Log",
        ColorRGBA(200, 200, 220, 255)
    );
    titleLabel.fontSize = 14;
    titleLabel.render(canvas);

    // Draw messages
    int yOffset = 35;
    for (const auto& msg : messages) {
        auto msgLabel = Label(
            Rect(pos.x + 10, pos.y + yOffset, pos.w - 20, 18),
            msg,
            ColorRGBA(180, 180, 200, 255)
        );
        msgLabel.fontSize = 14;
        msgLabel.render(canvas);
        yOffset += 18;
    }
}

// UnitInfoPanel implementation
UnitInfoPanel::UnitInfoPanel(const Point& position, const Point& size)
    : Widget(Rect(position.x, position.y, size.x, size.y)), currentUnit(nullptr) {
}

void UnitInfoPanel::setUnit(const BattleUnit* unit) {
    currentUnit = unit;
}

void UnitInfoPanel::render(Canvas& canvas) {
    // Draw background panel
    canvas.drawRect(pos, ColorRGBA(30, 30, 40, 220));
    canvas.drawBorder(pos, ColorRGBA(80, 80, 100, 255), 2);

    if (!currentUnit) {
        auto label = Label(
            Rect(pos.x + 10, pos.y + 10, pos.w - 20, 20),
            "No unit selected",
            ColorRGBA(150, 150, 150, 255)
        );
        label.fontSize = 14;
        label.render(canvas);
        return;
    }

    // Get creature info (simplified)
    std::string creatureName = "Creature #" + std::to_string(static_cast<int>(currentUnit->creatureId));

    // Draw unit info
    int yOffset = 15;
    auto nameLabel = Label(
        Rect(pos.x + 10, pos.y + yOffset, pos.w - 20, 20),
        creatureName,
        ColorRGBA(220, 220, 240, 255)
    );
    nameLabel.fontSize = 16;
    nameLabel.render(canvas);

    yOffset += 30;
    std::ostringstream info;
    info << "Count: " << currentUnit->count << "  Health: " << currentUnit->currentHealth;
    auto infoLabel = Label(
        Rect(pos.x + 10, pos.y + yOffset, pos.w - 20, 20),
        info.str(),
        ColorRGBA(180, 180, 200, 255)
    );
    infoLabel.fontSize = 14;
    infoLabel.render(canvas);

    yOffset += 25;
    std::string side = currentUnit->isPlayerControlled ? "Your Forces" : "Enemy Forces";
    auto sideLabel = Label(
        Rect(pos.x + 10, pos.y + yOffset, pos.w - 20, 20),
        side,
        currentUnit->isPlayerControlled ?
        ColorRGBA(100, 220, 100, 255) : ColorRGBA(220, 100, 100, 255)
    );
    sideLabel.fontSize = 14;
    sideLabel.render(canvas);
}

// BattleField implementation
BattleField::BattleField(const Point& position, const Point& size)
    : Widget(Rect(position.x, position.y, size.x, size.y)) {
}

void BattleField::setUnits(const std::vector<BattleUnit>& player, const std::vector<BattleUnit>& enemy) {
    playerUnits = player;
    enemyUnits = enemy;
}

void BattleField::render(Canvas& canvas) {
    // Draw battlefield background
    canvas.drawRect(pos, ColorRGBA(60, 50, 40, 255));
    canvas.drawBorder(pos, ColorRGBA(100, 90, 70, 255), 3);

    // Draw dividing line
    int centerX = pos.x + pos.w / 2;
    canvas.drawLine(Point(centerX, pos.y),
                   Point(centerX, pos.y + pos.h),
                   ColorRGBA(100, 90, 70, 255));

    // Draw player units on left side
    int playerY = pos.y + 50;
    int unitSpacing = 120;
    for (size_t i = 0; i < playerUnits.size() && i < 7; ++i) {
        const auto& unit = playerUnits[i];
        if (unit.count > 0) {
            int unitX = pos.x + 100;
            int unitY = playerY + (int)i * unitSpacing;

            // Draw unit placeholder (colored box)
            canvas.drawRect(Rect(unitX, unitY, 80, 80),
                          ColorRGBA(80, 120, 220, 255));
            canvas.drawBorder(Rect(unitX, unitY, 80, 80),
                            ColorRGBA(120, 160, 255, 255), 2);

            // Draw unit count
            std::string countStr = std::to_string(unit.count);
            auto countLabel = Label(
                Rect(unitX + 10, unitY + 30, 60, 20),
                countStr,
                ColorRGBA(255, 255, 255, 255)
            );
            countLabel.fontSize = 14;
            countLabel.render(canvas);
        }
    }

    // Draw enemy units on right side
    int enemyY = pos.y + 50;
    for (size_t i = 0; i < enemyUnits.size() && i < 7; ++i) {
        const auto& unit = enemyUnits[i];
        if (unit.count > 0) {
            int unitX = pos.x + pos.w - 180;
            int unitY = enemyY + (int)i * unitSpacing;

            // Draw unit placeholder (colored box)
            canvas.drawRect(Rect(unitX, unitY, 80, 80),
                          ColorRGBA(220, 80, 80, 255));
            canvas.drawBorder(Rect(unitX, unitY, 80, 80),
                            ColorRGBA(255, 120, 120, 255), 2);

            // Draw unit count
            std::string countStr = std::to_string(unit.count);
            auto countLabel = Label(
                Rect(unitX + 10, unitY + 30, 60, 20),
                countStr,
                ColorRGBA(255, 255, 255, 255)
            );
            countLabel.fontSize = 14;
            countLabel.render(canvas);
        }
    }
}

// BattleWindow implementation
BattleWindow::BattleWindow(const Point& position, const Point& size)
    : Widget(Rect(position.x, position.y, size.x, size.y)), battleEngine(nullptr), gameState(nullptr),
      battleInProgress(false), autoPlaying(false) {

    // Create battlefield (top center, 1300x800)
    battlefield = std::make_unique<BattleField>(
        Point(pos.x + 310, pos.y + 20),
        Point(1300, 800)
    );

    // Create battle log (bottom left, 300x200)
    battleLog = std::make_unique<BattleLog>(
        Point(pos.x + 10, pos.y + 840),
        Point(300, 200)
    );

    // Create unit info panel (bottom center, 700x200)
    unitInfo = std::make_unique<UnitInfoPanel>(
        Point(pos.x + 320, pos.y + 840),
        Point(700, 200)
    );

    // Create auto-battle button (bottom right)
    autoBattleButton = std::make_unique<Button>(
        Rect(pos.x + 1030, pos.y + 840, 280, 50),
        "Auto Battle",
        [this]() { onAutoBattle(); }
    );

    // Create close button (bottom right)
    closeButton = std::make_unique<Button>(
        Rect(pos.x + 1030, pos.y + 900, 280, 50),
        "Close",
        [this]() { onClose(); }
    );
    closeButton->setEnabled(false); // Disabled until battle ends
}

BattleWindow::~BattleWindow() = default;

void BattleWindow::startBattle(BattleEngine* engine, GameState* state) {
    battleEngine = engine;
    gameState = state;
    battleInProgress = true;
    autoPlaying = false;

    battleLog->clear();
    battleLog->addMessage("Battle started!");

    // Update battlefield with initial units
    if (battleEngine) {
        battlefield->setUnits(battleEngine->getPlayerUnits(),
                            battleEngine->getEnemyUnits());
    }

    closeButton->setEnabled(false);
    visible = true;
}

void BattleWindow::setOnBattleComplete(std::function<void()> callback) {
    onBattleComplete = callback;
}

BattleResult BattleWindow::getBattleResult() const {
    if (battleEngine) {
        // Check which side has units remaining
        bool playerHasUnits = false;
        bool enemyHasUnits = false;

        for (const auto& unit : battleEngine->getPlayerUnits()) {
            if (unit.count > 0) {
                playerHasUnits = true;
                break;
            }
        }

        for (const auto& unit : battleEngine->getEnemyUnits()) {
            if (unit.count > 0) {
                enemyHasUnits = true;
                break;
            }
        }

        if (playerHasUnits && !enemyHasUnits) return BattleResult::Victory;
        if (!playerHasUnits && enemyHasUnits) return BattleResult::Defeat;
    }

    return BattleResult::Defeat;
}

void BattleWindow::onAutoBattle() {
    if (!battleEngine || !battleInProgress) return;

    autoPlaying = true;
    autoBattleButton->setEnabled(false);
    battleLog->addMessage("Auto-battle engaged...");

    // Execute full auto-battle
    BattleResult result = battleEngine->executeAutoBattle();

    // Update display
    battlefield->setUnits(battleEngine->getPlayerUnits(),
                        battleEngine->getEnemyUnits());

    // Show result
    if (result == BattleResult::Victory) {
        battleLog->addMessage("Victory! Enemy defeated!");
    } else {
        battleLog->addMessage("Defeat! Your forces were defeated.");
    }

    int exp = battleEngine->calculateExperienceGained();
    if (exp > 0) {
        battleLog->addMessage("Gained " + std::to_string(exp) + " experience!");
    }

    battleInProgress = false;
    closeButton->setEnabled(true);
}

void BattleWindow::onClose() {
    if (battleInProgress) return; // Can't close during battle

    visible = false;
    if (onBattleComplete) {
        onBattleComplete();
    }
}

void BattleWindow::executeBattleRound() {
    // This would execute a single round (for manual battle mode)
    // Currently we only support auto-battle
}

void BattleWindow::updateUI() {
    if (battleEngine) {
        battlefield->setUnits(battleEngine->getPlayerUnits(),
                            battleEngine->getEnemyUnits());
    }
}

void BattleWindow::render(Canvas& canvas) {
    if (!visible) return;

    // Draw main window background
    canvas.drawRect(pos, ColorRGBA(20, 20, 30, 240));
    canvas.drawBorder(pos, ColorRGBA(100, 100, 120, 255), 3);

    // Render child widgets
    battlefield->render(canvas);
    battleLog->render(canvas);
    unitInfo->render(canvas);
    autoBattleButton->render(canvas);
    closeButton->render(canvas);
}

bool BattleWindow::onClick(const Point& point) {
    if (!visible) return false;

    if (autoBattleButton->onClick(point)) return true;
    if (closeButton->onClick(point)) return true;

    return false;
}

void BattleWindow::update(uint32_t deltaMs) {
    // Update logic (for animations, etc.)
    (void)deltaMs; // Unused for now
}
