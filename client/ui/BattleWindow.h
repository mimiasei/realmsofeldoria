#pragma once

#include "../../lib/gui/Widget.h"
#include "../../lib/battle/Battle.h"
#include "../../lib/render/Font.h"
#include <string>
#include <vector>
#include <functional>

class BattleEngine;
class GameState;

/// Battle log widget showing combat messages
class BattleLog : public Widget {
private:
    std::vector<std::string> messages;
    Font* font;
    int maxMessages = 10;

public:
    BattleLog(const Point& pos, const Point& size);

    void addMessage(const std::string& message);
    void clear();
    void render(Canvas& canvas) override;
};

/// Unit info panel showing current unit stats
class UnitInfoPanel : public Widget {
private:
    const BattleUnit* currentUnit;
    Font* titleFont;
    Font* textFont;

public:
    UnitInfoPanel(const Point& pos, const Point& size);

    void setUnit(const BattleUnit* unit);
    void render(Canvas& canvas) override;
};

/// Battlefield display showing unit positions
class BattleField : public Widget {
private:
    std::vector<BattleUnit> playerUnits;
    std::vector<BattleUnit> enemyUnits;
    Font* font;

public:
    BattleField(const Point& pos, const Point& size);

    void setUnits(const std::vector<BattleUnit>& player, const std::vector<BattleUnit>& enemy);
    void render(Canvas& canvas) override;
};

/// Main battle window containing all battle UI
class BattleWindow : public Widget {
private:
    BattleEngine* battleEngine;
    GameState* gameState;

    std::unique_ptr<BattleField> battlefield;
    std::unique_ptr<BattleLog> battleLog;
    std::unique_ptr<UnitInfoPanel> unitInfo;
    std::unique_ptr<Button> autoBattleButton;
    std::unique_ptr<Button> closeButton;

    bool battleInProgress;
    bool autoPlaying;
    std::function<void()> onBattleComplete;

    void onAutoBattle();
    void onClose();
    void executeBattleRound();
    void updateUI();

public:
    BattleWindow(const Point& pos, const Point& size);
    ~BattleWindow();

    /// Start a new battle
    void startBattle(BattleEngine* engine, GameState* state);

    /// Set callback for when battle completes
    void setOnBattleComplete(std::function<void()> callback);

    /// Get battle result
    BattleResult getBattleResult() const;

    /// Check if battle is still active
    bool isActive() const { return battleInProgress; }

    void render(Canvas& canvas) override;
    bool onClick(const Point& pos) override;
    void update(uint32_t deltaMs) override;
};
