// ui.h
// Declares the UI class. The UI is responsible for ALL text output and
// user input. It does not contain any game logic.

#ifndef UI_H
#define UI_H

#include <string>
#include <vector>
#include "GameState.h"
#include "EventSystem.h"

class UI {
public:
    // -------- Constructor --------
    UI(); // 新增：构造函数，用于初始化终端输入环境

    // -------- Start and end screens --------
    void showStartMenu();
    Difficulty askDifficulty();
    int askSupplyPlan(Difficulty diff);
    void showEnding(const GameState& state);

    // Save-file prompt: shown at startup if a save exists. Returns true if
    // the player wants to resume, false to start a new game.
    bool askContinueGame();

    // -------- Daily flow --------
    void showDailyReport(const GameState& state);

    // -------- Daytime action prompts --------
    bool askTreat(const GameState& state);
    bool askExpedition(const GameState& state);
    int askExpeditionCount(int maxCount);
    std::vector<int> askExpeditionMembers(const GameState& state, int count);

    // Player picks where the expedition goes (1..7 menu).
    // Hidden Storage is only available when an unused note is held.
    ExpeditionEventType askExpeditionDestination(const GameState& state);

    // -------- In-event player choices --------
    bool askDoorChoice();
    int askCampChoice();
    int askClearChoice();

    // -------- Result and message display --------
    void showEventResult(const std::string& title, const std::string& text);
    void showDayEnd(const GameState& state);
    void showMessage(const std::string& msg);
    void waitForEnter();

    // Quit flag: returns true if the player typed 'q' at any input prompt.
    // The main loop checks this after each UI call and exits cleanly.
    static bool isQuitRequested();
    static void resetQuitFlag();

    // Shown after a save-and-quit, just before the program returns.
    void showQuitConfirmation(int currentDay);

private:
    // -------- Drawing helpers --------
    void clearScreen();
    void drawTopBorder(int width = 60);
    void drawBottomBorder(int width = 60);
    void drawMidBorder(int width = 60);
    void drawCentered(const std::string& text, int width = 60);
    void drawLeft(const std::string& text, int width = 60);
    std::string progressBar(int current, int max, int length = 12);
    std::string statusIcon(SurvivorStatus s);
    std::string statusName(SurvivorStatus s);
    std::string traitName(SurvivorTrait t);
    std::string colorize(const std::string& text, const std::string& color);
    int getValidChoice(int minChoice, int maxChoice);
    bool getYesNo();
    int visibleLength(const std::string& text);

    // -------- ANSI color escape codes --------
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

#endif // UI_H
