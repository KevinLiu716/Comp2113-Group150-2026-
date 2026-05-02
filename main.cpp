// main.cpp
// Entry point of the game. Drives the day-by-day main loop:
//   start menu -> (load saved game OR difficulty + supplies) ->
//   daily loop (10 days, with save-and-quit at any time) -> ending.

#include "ui.h"
#include "GameState.h"
#include "EventSystem.h"
#include "player.h"
#include "Tools.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

// We explicitly turn on the terminal's line-editing features so that
// the Backspace key actually deletes a character on screen rather than
// echoing as '^H'. Without this, some shells / PTYs (especially when
// launched from inside VS Code, tmux, or `make run`) leave ECHOE off
// and Backspace looks broken. We snapshot the original settings on
// entry and restore them when the program exits, so we never leave
// the terminal in a bad state.
#include <termios.h>
#include <unistd.h>

static struct termios g_originalTermios;
static bool g_termiosSaved = false;

static void restoreTerminal() {
    if (g_termiosSaved) {
        tcsetattr(STDIN_FILENO, TCSANOW, &g_originalTermios);
    }
}

// Enable canonical mode + echo + erase-on-backspace. Safe to call on
// non-tty stdin (e.g. when input is piped); in that case it just no-ops.
static void enableTerminalLineEditing() {
    if (!isatty(STDIN_FILENO)) return;
    if (tcgetattr(STDIN_FILENO, &g_originalTermios) != 0) return;
    g_termiosSaved = true;
    std::atexit(restoreTerminal);

    struct termios t = g_originalTermios;
    // ICANON: line-by-line input (Enter terminates a line).
    // ECHO:   echo typed characters.
    // ECHOE:  Backspace visually erases the previous character.
    // ECHOK:  the Kill character (Ctrl-U) erases the whole line.
    t.c_lflag |= (ICANON | ECHO | ECHOE | ECHOK);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

// Function: determineEnding
// Purpose:  Inspect the final game state and decide which of the six
//           endings the player reached. Sets both endingType (used by the
//           UI to pick a banner and color) and endingMessage (the text).
// Input:    state - reference to the final game state
// Output:   none (mutates state.endingType and state.endingMessage)
void determineEnding(GameState& state) {
    int alive = state.countLivingSurvivors();
    int healthy = state.countHealthySurvivors();
    int mutated = state.countMutatedSurvivors();
    int totalSupplies = state.food + state.water;

    if (alive == 0) {
        state.endingType = EndingType::TRAGIC_END;
        state.endingMessage =
            "The shelter falls into dead silence.\n"
            "Diaries are scattered on the table.\n"
            "The last page reads: \"We did our best.\"";
        return;
    }

    if (healthy >= 4 && totalSupplies >= 10 &&
        state.hasRadio && state.triggeredEvent6) {
        state.endingType = EndingType::ORDER_RESTORED;
        state.endingMessage =
            "Rescue arrives, and order is reestablished.\n"
            "The long nightmare finally ends.";
        return;
    }

    if (mutated * 2 > alive) {
        state.endingType = EndingType::SYMBIOTIC_EVOLUTION;
        state.endingMessage =
            "The voice on the radio gradually becomes clear...\n"
            "But it's not speaking any human language.";
        return;
    }

    if (alive >= 2 && state.campRobberyCount >= 2) {
        state.endingType = EndingType::MARAUDERS;
        state.endingMessage =
            "You have become the very people you once feared.\n"
            "Survival came at a terrible cost.";
        return;
    }

    if (alive == 1 && totalSupplies >= 10) {
        state.endingType = EndingType::LONE_SURVIVOR;
        state.endingMessage =
            "Only you remain, guarding the ruins and your memories.\n"
            "The wind whistles through the empty shelter.";
        return;
    }

    state.endingType = EndingType::STRUGGLE_FOR_SURVIVAL;
    state.endingMessage =
        "The door finally opens, but the world before you is unrecognizable.\n"
        "Survival is just another beginning.";
}

// Macro to break out of the daily loop early when the player typed 'q'.
// We declare it inside main() with a local label, but C++ does not allow
// labeled break out of multiple constructs, so we use a flag we already
// have: UI::isQuitRequested(). Each UI call is followed by a check.
#define CHECK_QUIT() if (UI::isQuitRequested()) { quitMidGame = true; break; }

int main() {
    // Make sure Backspace actually erases on screen instead of echoing
    // as '^H'. No-op on Windows and on non-interactive stdin (pipes).
    enableTerminalLineEditing();

    // Seed the random number generator once at program start.
    initRandom();

    UI ui;
    ui.showStartMenu();

    GameState state;
    bool resumed = false;

    // If a save file exists, ask the player whether to resume.
    if (hasSaveFile()) {
        if (ui.askContinueGame()) {
            if (UI::isQuitRequested()) {
                // Player typed 'q' at the resume prompt - keep the save and exit.
                ui.showQuitConfirmation(0);
                return 0;
            }
            if (loadGame(state)) {
                resumed = true;
                ui.showMessage("Save loaded. Resuming your game...");
                ui.waitForEnter();
            } else {
                ui.showMessage("Save file was corrupted. Starting a new game.");
                ui.waitForEnter();
                deleteSaveFile();
            }
        } else {
            // Quit-at-resume-prompt also arrives here (getYesNo returns false on q).
            if (UI::isQuitRequested()) {
                ui.showQuitConfirmation(0);
                return 0;
            }
            // Otherwise the player declined; wipe the old save before fresh start.
            deleteSaveFile();
        }
    }

    // If we did not resume, run the normal new-game setup.
    if (!resumed) {
        Difficulty diff = ui.askDifficulty();
        if (UI::isQuitRequested()) {
            // Quit during difficulty pick - nothing to save yet.
            ui.showQuitConfirmation(0);
            return 0;
        }
        int plan = ui.askSupplyPlan(diff);
        if (UI::isQuitRequested()) {
            ui.showQuitConfirmation(0);
            return 0;
        }
        initResources(state, diff, plan);
    }

    bool quitMidGame = false;

    // === Main game loop: at most 10 days, ends early if everyone dies ===
    while (state.currentDay <= 10 && state.countLivingSurvivors() > 0) {

        // Save the state of "the start of this day" - that way, if the
        // player quits anywhere during the day, the next launch will
        // resume cleanly from the daily report of the same day.
        saveGame(state);

        ui.showDailyReport(state);
        CHECK_QUIT();

        if (state.countLivingSurvivors() == 0) break;

        // 1. Treatment phase
        if (ui.askTreat(state)) {
            CHECK_QUIT();
            treat(state);
            ui.showMessage("All weak survivors have been healed!");
            ui.waitForEnter();
            CHECK_QUIT();
        }
        CHECK_QUIT();

        // 2. Expedition phase - player picks the destination.
        std::vector<int> expeditionMembers;
        if (ui.askExpedition(state)) {
            CHECK_QUIT();
            int able = state.countHealthySurvivors() + state.countMutatedSurvivors();
            int maxN = able / 2;
            int n = ui.askExpeditionCount(maxN);
            CHECK_QUIT();
            expeditionMembers = ui.askExpeditionMembers(state, n);
            CHECK_QUIT();
            state.expeditionMemberIds = expeditionMembers;

            ExpeditionEventType expEvent = ui.askExpeditionDestination(state);
            CHECK_QUIT();

            if (expEvent == ExpeditionEventType::HIDDEN_STORAGE) {
                state.usedNoteEffect = true;
            }

            std::string expResult = processExpeditionEvent(state, expEvent, expeditionMembers);
            ui.showEventResult("EXPEDITION RESULT", expResult);
            CHECK_QUIT();
        }
        CHECK_QUIT();

        // 3. Night random event
        DailyEventType nightEvent = selectRandomDailyEvent(state);
        std::string nightResult = processDailyEvent(state, nightEvent);
        ui.showEventResult("NIGHT EVENT", nightResult);
        CHECK_QUIT();

        // 4. Resource consumption + sick day update
        consumeDaily(state);
        updateSickCounters(state);

        // 5. End-of-day summary
        ui.showDayEnd(state);
        // Note: even if the player typed 'q' during the End-of-Day prompt,
        // we still treat the day as complete - they finished all actions.
        // So we advance the day counter and save the *new* day's state
        // before honoring the quit. This way "I quit at end of day 3"
        // means "next time I start at day 4", which feels natural.

        state.resetDailyStates();
        state.currentDay++;

        // Save the just-advanced state, replacing the older "start of
        // previous day" save written at the top of this iteration.
        if (state.currentDay <= 10 && state.countLivingSurvivors() > 0) {
            saveGame(state);
        }

        CHECK_QUIT();
    }

    // ----- Exit path 1: the player saved & quit mid-game -----
    if (quitMidGame) {
        // The save written at the start of this day is still on disk.
        // It points to the *current* day, so the player will replay this
        // day from its daily report when they return.
        ui.showQuitConfirmation(state.currentDay);
        return 0;
    }

    // ----- Exit path 2: the game ended naturally -----
    if (state.currentDay > 10) state.currentDay = 10;

    determineEnding(state);
    ui.showEnding(state);

    // Game finished - wipe the save so the next run starts fresh.
    deleteSaveFile();

    return 0;
}
