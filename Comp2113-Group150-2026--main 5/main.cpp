// main.cpp
// Entry point of the game. Drives the day-by-day main loop:
//   start menu -> (load saved game OR difficulty + supplies) ->
//   daily loop (10 days, autosave each night) -> ending.

#include "ui.h"
#include "GameState.h"
#include "EventSystem.h"
#include "player.h"
#include "Tools.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

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

    // Tragic End: everyone is dead. Highest priority.
    if (alive == 0) {
        state.endingType = EndingType::TRAGIC_END;
        state.endingMessage =
            "The shelter falls into dead silence.\n"
            "Diaries are scattered on the table.\n"
            "The last page reads: \"We did our best.\"";
        return;
    }

    // Order Restored: best ending. Strict requirements.
    if (healthy >= 4 && totalSupplies >= 10 &&
        state.hasRadio && state.triggeredEvent6) {
        state.endingType = EndingType::ORDER_RESTORED;
        state.endingMessage =
            "Rescue arrives, and order is reestablished.\n"
            "The long nightmare finally ends.";
        return;
    }

    // Symbiotic Evolution: more than half the living are mutated.
    if (mutated * 2 > alive) {
        state.endingType = EndingType::SYMBIOTIC_EVOLUTION;
        state.endingMessage =
            "The voice on the radio gradually becomes clear...\n"
            "But it's not speaking any human language.";
        return;
    }

    // Marauders: at least 2 alive AND at least 2 camp robberies.
    if (alive >= 2 && state.campRobberyCount >= 2) {
        state.endingType = EndingType::MARAUDERS;
        state.endingMessage =
            "You have become the very people you once feared.\n"
            "Survival came at a terrible cost.";
        return;
    }

    // Lone Survivor: exactly one alive with enough supplies.
    if (alive == 1 && totalSupplies >= 10) {
        state.endingType = EndingType::LONE_SURVIVOR;
        state.endingMessage =
            "Only you remain, guarding the ruins and your memories.\n"
            "The wind whistles through the empty shelter.";
        return;
    }

    // Default: someone made it through, but barely.
    state.endingType = EndingType::STRUGGLE_FOR_SURVIVAL;
    state.endingMessage =
        "The door finally opens, but the world before you is unrecognizable.\n"
        "Survival is just another beginning.";
}

int main() {
    // Seed the random number generator once at program start.
    initRandom();

    UI ui;
    ui.showStartMenu();

    GameState state;
    bool resumed = false;

    // If a save file exists, ask the player whether to resume.
    if (hasSaveFile()) {
        if (ui.askContinueGame()) {
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
            // Player declined; wipe the old save before starting fresh.
            deleteSaveFile();
        }
    }

    // If we did not resume, run the normal new-game setup.
    if (!resumed) {
        Difficulty diff = ui.askDifficulty();
        int plan = ui.askSupplyPlan(diff);
        initResources(state, diff, plan);
    }

    // === Main game loop: at most 10 days, ends early if everyone dies ===
    while (state.currentDay <= 10 && state.countLivingSurvivors() > 0) {
        ui.showDailyReport(state);

        if (state.countLivingSurvivors() == 0) break;

        // 1. Treatment phase
        if (ui.askTreat(state)) {
            treat(state);
            ui.showMessage("All weak survivors have been healed!");
            ui.waitForEnter();
        }

        // 2. Expedition phase — player picks the destination.
        std::vector<int> expeditionMembers;
        if (ui.askExpedition(state)) {
            int able = state.countHealthySurvivors() + state.countMutatedSurvivors();
            int maxN = able / 2;
            int n = ui.askExpeditionCount(maxN);
            expeditionMembers = ui.askExpeditionMembers(state, n);
            state.expeditionMemberIds = expeditionMembers;

            ExpeditionEventType expEvent = ui.askExpeditionDestination(state);

            // Mark the note as used if the player went to Hidden Storage.
            if (expEvent == ExpeditionEventType::HIDDEN_STORAGE) {
                state.usedNoteEffect = true;
            }

            std::string expResult = processExpeditionEvent(state, expEvent, expeditionMembers);
            ui.showEventResult("EXPEDITION RESULT", expResult);
        }

        // 3. Night random event
        DailyEventType nightEvent = selectRandomDailyEvent(state);
        std::string nightResult = processDailyEvent(state, nightEvent);
        ui.showEventResult("NIGHT EVENT", nightResult);

        // 4. Resource consumption + sick day update
        consumeDaily(state);
        updateSickCounters(state);

        // 5. End-of-day summary, then advance day counter
        ui.showDayEnd(state);

        state.resetDailyStates();
        state.currentDay++;

        // 6. Auto-save after each day.
        saveGame(state);
    }

    // Game over: clamp the displayed day, decide the ending, show it,
    // and remove the save so a new run starts fresh.
    if (state.currentDay > 10) state.currentDay = 10;

    determineEnding(state);
    ui.showEnding(state);

    deleteSaveFile();

    return 0;
}
