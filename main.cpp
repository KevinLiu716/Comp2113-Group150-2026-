/**
 *[cite: 1]
 * Project: Shelter: 10 Days (COMP2113 Group 150)
 * File: main.cpp
 * -------------------------------------------------------------------------
 * Description: 
 * This file serves as the central orchestration layer (Game Loop) of the project.
 * It manages the lifecycle of the game, including:
 *   1. Low-level TTY terminal configuration for improved UX.
 *   2. Persistent state management (Save/Load system).
 *   3. The 10-day iterative survival logic and branching narrative endings.
 */

#include "ui.h"
#include "GameState.h"
#include "EventSystem.h"
#include "player.h"
#include "Tools.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <termios.h>
#include <unistd.h>

// =========================================================================
// SECTION: System-Level Terminal Configuration (POSIX)
// =========================================================================

static struct termios g_originalTermios;
static bool g_termiosSaved = false;

/**
 * Restores the terminal to its original state before the program started.
 * Registered via std::atexit to ensure system integrity even on crashes.[cite: 1]
 */
static void restoreTerminal() {
    if (g_termiosSaved) {
        tcsetattr(STDIN_FILENO, TCSANOW, &g_originalTermios);
    }
}

/**
 * Configures the terminal I/O behavior.
 * Specifically enables ECHOE (Backspace visual erase) and ICANON (Canonical mode)
 * to ensure consistent behavior across different SSH clients/shells (e.g., Academy server).[cite: 1]
 */
static void enableTerminalLineEditing() {
    if (!isatty(STDIN_FILENO)) return;
    if (tcgetattr(STDIN_FILENO, &g_originalTermios) != 0) return;
    g_termiosSaved = true;
    std::atexit(restoreTerminal);

    struct termios t = g_originalTermios;
    // ICANON: Enable line-buffered input.
    // ECHOE:  Map backspace to the "Erase" sequence (essential for modern PTYs).
    t.c_lflag |= (ICANON | ECHO | ECHOE | ECHOK);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

// =========================================================================
// SECTION: Game Logic & Branching Narrative
// =========================================================================

/**
 * Procedural Ending Determinism:
 * Evaluates the 'GameState' metrics (survival count, health, mutations, resources)
 * to trigger one of the six distinct narrative outcomes.[cite: 1]
 */
void determineEnding(GameState& state) {
    int alive = state.countLivingSurvivors();
    int healthy = state.countHealthySurvivors();
    int mutated = state.countMutatedSurvivors();
    int totalSupplies = state.food + state.water;

    // Priority 1: Total Extinction
    if (alive == 0) {
        state.endingType = EndingType::TRAGIC_END;
        state.endingMessage = "The shelter falls into dead silence..."; 
        return;
    }

    // Priority 2: True Ending (Hidden Requirements)
    if (healthy >= 4 && totalSupplies >= 10 && state.hasRadio && state.triggeredEvent6) {
        state.endingType = EndingType::ORDER_RESTORED;
        state.endingMessage = "Rescue arrives, and order is reestablished.";
        return;
    }

    // Priority 3: Mutation/Evolution Path
    if (mutated * 2 > alive) {
        state.endingType = EndingType::SYMBIOTIC_EVOLUTION;
        state.endingMessage = "The voice on the radio... is no longer human.";
        return;
    }

    // Priority 4: Morality Falloff
    if (alive >= 2 && state.campRobberyCount >= 2) {
        state.endingType = EndingType::MARAUDERS;
        state.endingMessage = "Survival came at a terrible cost: your humanity.";
        return;
    }

    // Priority 5: Solo Survival
    if (alive == 1 && totalSupplies >= 10) {
        state.endingType = EndingType::LONE_SURVIVOR;
        state.endingMessage = "Only you remain, guarding the ruins and your memories.";
        return;
    }

    // Default: Generic Survival
    state.endingType = EndingType::STRUGGLE_FOR_SURVIVAL;
    state.endingMessage = "Survival is just another beginning.";
}

// Control-flow macro: Ensures high responsiveness to the 'Quit' (Q) signal.[cite: 1]
#define CHECK_QUIT() if (UI::isQuitRequested()) { quitMidGame = true; break; }

// =========================================================================
// SECTION: Main Entry Point (Game Orchestrator)
// =========================================================================

int main() {
    // 1. Environmental Initialization
    enableTerminalLineEditing();
    initRandom(); // Seed CSRNG (Pseudo-random)[cite: 1]

    UI ui;
    ui.showStartMenu();

    GameState state;
    bool resumed = false;

    // 2. Persistence Check: Load existing game state if available
    if (hasSaveFile()) {
        if (ui.askContinueGame()) {
            if (UI::isQuitRequested()) { ui.showQuitConfirmation(0); return 0; }
            if (loadGame(state)) {
                resumed = true;
                ui.showMessage("System: State restoration successful.");
                ui.waitForEnter();
            } else {
                ui.showMessage("Error: Save corrupted. Initializing fresh state...");
                ui.waitForEnter();
                deleteSaveFile();
            }
        } else {
            if (UI::isQuitRequested()) { ui.showQuitConfirmation(0); return 0; }
            deleteSaveFile(); // Clean slate for new game
        }
    }

    // 3. New Game Configuration (Difficulty Scaling)
    if (!resumed) {
        Difficulty diff = ui.askDifficulty();
        if (UI::isQuitRequested()) { ui.showQuitConfirmation(0); return 0; }
        int plan = ui.askSupplyPlan(diff);
        if (UI::isQuitRequested()) { ui.showQuitConfirmation(0); return 0; }
        initResources(state, diff, plan);
    }

    bool quitMidGame = false;

    // 4. Primary Game Loop (Iterative Logic for 10 Cycles)
    while (state.currentDay <= 10 && state.countLivingSurvivors() > 0) {

        // Auto-save at the start of each day for crash/quit recovery
        saveGame(state);[cite: 1]

        ui.showDailyReport(state);
        CHECK_QUIT();

        if (state.countLivingSurvivors() == 0) break;

        // --- Phase A: Medical Treatment ---
        if (ui.askTreat(state)) {
            CHECK_QUIT();
            treat(state);
            ui.showMessage("Status: Medical supplies applied. Vitals stabilizing.");
            ui.waitForEnter();
            CHECK_QUIT();
        }
        CHECK_QUIT();

        // --- Phase B: Expedition & Resource Acquisition ---
        if (ui.askExpedition(state)) {
            CHECK_QUIT();
            int able = state.countHealthySurvivors() + state.countMutatedSurvivors();
            int maxN = able / 2; // Logic: Maintain shelter security by limiting expedition size
            int n = ui.askExpeditionCount(maxN);
            CHECK_QUIT();
            
            std::vector<int> expeditionMembers = ui.askExpeditionMembers(state, n);
            CHECK_QUIT();
            state.expeditionMemberIds = expeditionMembers;

            ExpeditionEventType expEvent = ui.askExpeditionDestination(state);
            CHECK_QUIT();

            // Tactical choice: Using knowledge from found notes
            if (expEvent == ExpeditionEventType::HIDDEN_STORAGE) state.usedNoteEffect = true;

            std::string expResult = processExpeditionEvent(state, expEvent, expeditionMembers);
            ui.showEventResult("EXPEDITION LOG", expResult);
            CHECK_QUIT();
        }
        CHECK_QUIT();

        // --- Phase C: Environmental Stochastic Events (Night) ---
        DailyEventType nightEvent = selectRandomDailyEvent(state);
        std::string nightResult = processDailyEvent(state, nightEvent);
        ui.showEventResult("INCIDENT REPORT", nightResult);
        CHECK_QUIT();

        // --- Phase D: State Updates & Resource Depletion ---
        consumeDaily(state);
        updateSickCounters(state);

        ui.showDayEnd(state);
        
        // Finalize Day: Transition state for the next cycle
        state.resetDailyStates();
        state.currentDay++;

        // Update persistent save file with the advanced day count
        if (state.currentDay <= 10 && state.countLivingSurvivors() > 0) {
            saveGame(state);
        }

        CHECK_QUIT();
    }

    // 5. Exit Handling
    if (quitMidGame) {
        ui.showQuitConfirmation(state.currentDay);
        return 0;
    }

    // 6. Ending Sequence & Data Cleanup
    if (state.currentDay > 10) state.currentDay = 10;
    determineEnding(state);
    ui.showEnding(state);

    deleteSaveFile(); // Clear session data upon natural completion
    return 0;
}
