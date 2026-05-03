// main.cpp
// Entry point. Runs the start menu, then the 10-day game loop, then the ending.

#include "ui.h"
#include "GameState.h"
#include "EventSystem.h"
#include "player.h"
#include "Tools.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

// Function: determineEnding
// What it does: Looks at the final game state after Day 10 (or after all
//               survivors died) and picks one of the six endings. Sets
//               state.endingType and state.endingMessage accordingly.
// Input:  state - the game state at the end of the game.
// Output: none (modifies state.endingType and state.endingMessage).
void determineEnding(GameState& state) {
    int alive = state.countLivingSurvivors();
    int healthy = state.countHealthySurvivors();
    int mutated = state.countMutatedSurvivors();
    int totalSupplies = state.food + state.water;

    // Everyone died.
    if (alive == 0) {
        state.endingType = EndingType::TRAGIC_END;
        state.endingMessage = "The shelter falls into dead silence...";
        return;
    }

    // Best ending: rescue arrives. Needs the radio, the right night event,
    // enough healthy people, and enough supplies left.
    if (healthy >= 4 && totalSupplies >= 10 && state.hasRadio && state.triggeredEvent6) {
        state.endingType = EndingType::ORDER_RESTORED;
        state.endingMessage = "Rescue arrives, and order is reestablished.";
        return;
    }

    // More than half the survivors are mutated.
    if (mutated * 2 > alive) {
        state.endingType = EndingType::SYMBIOTIC_EVOLUTION;
        state.endingMessage = "The voice on the radio... is no longer human.";
        return;
    }

    // Robbed two or more camps.
    if (alive >= 2 && state.campRobberyCount >= 2) {
        state.endingType = EndingType::MARAUDERS;
        state.endingMessage = "Survival came at a terrible cost: your humanity.";
        return;
    }

    // Only one person left, but with enough supplies to keep going.
    if (alive == 1 && totalSupplies >= 10) {
        state.endingType = EndingType::LONE_SURVIVOR;
        state.endingMessage = "Only you remain, guarding the ruins and your memories.";
        return;
    }

    // Default: just barely made it.
    state.endingType = EndingType::STRUGGLE_FOR_SURVIVAL;
    state.endingMessage = "Survival is just another beginning.";
}

// Macro: CHECK_QUIT
// Used after every UI call. If the player typed 'q', sets quitMidGame
// and breaks out of the main loop so we can save and exit cleanly.
#define CHECK_QUIT() if (UI::isQuitRequested()) { quitMidGame = true; break; }

// Function: main
// What it does: Program entry point. Shows the start menu, optionally loads
//               a save, runs the 10-day game loop (treatment -> expedition ->
//               night event -> consumption -> sick counters -> day end), then
//               determines and displays the ending.
// Input:  none.
// Output: Returns 0 on normal exit.
int main() {
    initRandom();

    UI ui;
    ui.showStartMenu();

    GameState state;
    bool resumed = false;

    // If a save file exists, ask whether to resume.
    if (hasSaveFile()) {
        if (ui.askContinueGame()) {
            if (UI::isQuitRequested()) { ui.showQuitConfirmation(0); return 0; }
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
            if (UI::isQuitRequested()) { ui.showQuitConfirmation(0); return 0; }
            deleteSaveFile();
        }
    }

    // New game setup: pick difficulty and starting supplies.
    if (!resumed) {
        Difficulty diff = ui.askDifficulty();
        if (UI::isQuitRequested()) { ui.showQuitConfirmation(0); return 0; }
        int plan = ui.askSupplyPlan(diff);
        if (UI::isQuitRequested()) { ui.showQuitConfirmation(0); return 0; }
        initResources(state, diff, plan);
    }

    bool quitMidGame = false;

    // Main game loop: runs up to 10 days, ends early if everyone dies.
    while (state.currentDay <= 10 && state.countLivingSurvivors() > 0) {

        // Save at the start of each day so the player can quit anywhere
        // and resume from this same daily report next time.
        saveGame(state);

        ui.showDailyReport(state);
        CHECK_QUIT();

        if (state.countLivingSurvivors() == 0) break;

        // Step 1: Treatment phase
        if (ui.askTreat(state)) {
            CHECK_QUIT();
            treat(state);
            ui.showMessage("All weak survivors have been healed!");
            ui.waitForEnter();
            CHECK_QUIT();
        }
        CHECK_QUIT();

        // Step 2: Expedition phase
        if (ui.askExpedition(state)) {
            CHECK_QUIT();
            int able = state.countHealthySurvivors() + state.countMutatedSurvivors();
            int maxN = able / 2;  // can send at most half the able-bodied people
            int n = ui.askExpeditionCount(maxN);
            CHECK_QUIT();

            std::vector<int> expeditionMembers = ui.askExpeditionMembers(state, n);
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

        // Step 3: Random night event
        DailyEventType nightEvent = selectRandomDailyEvent(state);
        std::string nightResult = processDailyEvent(state, nightEvent);
        ui.showEventResult("NIGHT EVENT", nightResult);
        CHECK_QUIT();

        // Step 4: Daily resource consumption + sick counter update
        consumeDaily(state);
        updateSickCounters(state);

        // Step 5: End of day summary
        ui.showDayEnd(state);

        state.resetDailyStates();
        state.currentDay++;

        // Re-save with the new day count so resume points to the next day.
        if (state.currentDay <= 10 && state.countLivingSurvivors() > 0) {
            saveGame(state);
        }

        CHECK_QUIT();
    }

    // Player quit mid-game: leave the save file so they can resume later.
    if (quitMidGame) {
        ui.showQuitConfirmation(state.currentDay);
        return 0;
    }

    // Game ended normally: determine ending, show it, delete the save.
    if (state.currentDay > 10) state.currentDay = 10;
    determineEnding(state);
    ui.showEnding(state);
    deleteSaveFile();

    return 0;
}
