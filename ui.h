// ui.h
// Declares the UI class. The UI handles ALL text output and player input.
// It does not contain any game logic.

#ifndef UI_H
#define UI_H

#include <string>
#include <vector>
#include "GameState.h"
#include "EventSystem.h"

class UI {
public:
    // Function: UI (constructor)
    // What it does: Sets up the terminal environment and configures tty
    //               settings (e.g. backspace key mapping) for stable input.
    // Input:  none.
    // Output: A ready-to-use UI object.
    UI(); 

    // -------- Start and end screens --------

    // Shows the main menu / title screen.
    void showStartMenu();

    // Asks the player to pick EASY or HARD difficulty. Returns the choice.
    Difficulty askDifficulty();

    // Asks which supply plan (1 or 2) for the chosen difficulty.
    int askSupplyPlan(Difficulty diff);

    // Displays the ending screen based on final game state.
    void showEnding(const GameState& state);

    // Shown at startup if a save file exists. Returns true if the player
    // wants to resume, false to start a new game.
    bool askContinueGame();

    // -------- Daily flow --------

    // Prints the daily status report (resources, survivor health, etc.).
    void showDailyReport(const GameState& state);

    // -------- Daytime action prompts --------

    // Asks whether to use medicine today. Returns true if yes.
    bool askTreat(const GameState& state);

    // Asks whether to send an expedition today. Returns true if yes.
    bool askExpedition(const GameState& state);

    // Asks how many survivors to send out (1 to maxCount).
    int askExpeditionCount(int maxCount);

    // Lets the player pick which survivors go on the expedition.
    std::vector<int> askExpeditionMembers(const GameState& state, int count);

    // Player picks where the expedition goes (1..7 menu).
    // Hidden Storage only shows up when the player has an unused note.
    ExpeditionEventType askExpeditionDestination(const GameState& state);

    // -------- In-event player choices --------

    // Unexpected visitor: open the door or not? Returns true = open.
    bool askDoorChoice();

    // Other camp encounter: returns 1 for negotiate, 2 for rob.
    int askCampChoice();

    // Perimeter clear: returns 1 for outer clear, 2 for inner search.
    int askClearChoice();

    // -------- Result and message display --------

    // Shows an event result in a bordered box with a title and body text.
    void showEventResult(const std::string& title, const std::string& text);

    // Shows end-of-day summary (resource changes, who got sick, etc.).
    void showDayEnd(const GameState& state);

    // Prints a simple one-line message.
    void showMessage(const std::string& msg);

    // Pauses until the player presses Enter.
    void waitForEnter();

    // Returns true if the player typed 'q' at any input prompt.
    // The main loop checks this after each UI call and exits cleanly.
    static bool isQuitRequested();

    // Resets the quit flag back to false.
    static void resetQuitFlag();

    // Shown after a save-and-quit, just before the program exits.
    void showQuitConfirmation(int currentDay);

private:
    // -------- Drawing helpers --------
    void clearScreen();
    void drawTopBorder(int width = 60);
    void drawBottomBorder(int width = 60);
    void drawMidBorder(int width = 60);
    void drawCentered(const std::string& text, int width = 60);
    void drawLeft(const std::string& text, int width = 60);

    // Makes a text progress bar like [####....] for resource display.
    std::string progressBar(int current, int max, int length = 12);

    // Returns a small icon character for the given survivor status.
    std::string statusIcon(SurvivorStatus s);

    // Returns the display name for a survivor status (e.g. "Healthy").
    std::string statusName(SurvivorStatus s);

    // Returns the display name for a survivor trait (e.g. "Doctor").
    std::string traitName(SurvivorTrait t);

    // Wraps text in ANSI color escape codes.
    std::string colorize(const std::string& text, const std::string& color);

    // Reads an integer from stdin, re-prompts until it is in range.
    int getValidChoice(int minChoice, int maxChoice);

    // Reads yes/no from stdin. Returns true for yes.
    bool getYesNo();

    // Returns the visible character count of a string (ignoring ANSI codes).
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
