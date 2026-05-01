#ifndef UI_H
#define UI_H

#include <string>
#include <vector>
#include "GameState.h"

class UI {
public:
    // 开始与结束
    void showStartMenu();
    Difficulty askDifficulty();
    int askSupplyPlan(Difficulty diff);
    void showEnding(const GameState& state);

    // 每日流程
    void showDailyReport(const GameState& state);

    // 行动选择
    bool askTreat(const GameState& state);
    bool askExpedition(const GameState& state);
    int askExpeditionCount(int maxCount);
    std::vector<int> askExpeditionMembers(const GameState& state, int count);

    // 事件中的玩家选择
    bool askDoorChoice();
    int askCampChoice();
    int askClearChoice();

    // 结果展示
    void showEventResult(const std::string& title, const std::string& text);
    void showDayEnd(const GameState& state);
    void showMessage(const std::string& msg);
    void waitForEnter();

private:
    void clearScreen();
    void drawTopBorder(int width = 60);
    void drawBottomBorder(int width = 60);
    void drawMidBorder(int width = 60);
    void drawCentered(const std::string& text, int width = 60);
    void drawLeft(const std::string& text, int width = 60);
    std::string progressBar(int current, int max, int length = 12);
    std::string statusIcon(SurvivorStatus s);
    std::string statusName(SurvivorStatus s);
    std::string colorize(const std::string& text, const std::string& color);
    int getValidChoice(int minChoice, int maxChoice);
    bool getYesNo();
    int visibleLength(const std::string& text);

    // ANSI color codes
    const std::string RESET   = "\033[0m";
    const std::string BOLD    = "\033[1m";
    const std::string DIM     = "\033[2m";
    const std::string RED     = "\033[31m";
    const std::string GREEN   = "\033[32m";
    const std::string YELLOW  = "\033[33m";
    const std::string BLUE    = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN    = "\033[36m";
    const std::string WHITE   = "\033[37m";
};

#endif
