#include "ui.h"
#include "GameState.h"
#include "EventSystem.h"
#include "player.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

void determineEnding(GameState& state) {
    int alive = state.countLivingSurvivors();
    int healthy = state.countHealthySurvivors();
    int mutated = state.countMutatedSurvivors();
    int totalSupplies = state.food + state.water;

    if (alive == 0) {
        state.endingMessage =
            "The shelter falls into dead silence.\n"
            "Diaries are scattered on the table.\n"
            "The last page reads: \"We did our best.\"";
        return;
    }

    if (healthy >= 4 && totalSupplies >= 10 &&
        state.hasRadio && state.triggeredEvent6) {
        state.endingMessage =
            "Rescue arrives, and order is reestablished.\n"
            "The long nightmare finally ends.";
        return;
    }

    if (mutated * 2 > alive) {
        state.endingMessage =
            "The voice on the radio gradually becomes clear...\n"
            "But it's not speaking any human language.";
        return;
    }

    if (alive >= 2 && state.campRobberyCount >= 2) {
        state.endingMessage =
            "You have become the very people you once feared.\n"
            "Survival came at a terrible cost.";
        return;
    }

    if (alive == 1 && totalSupplies >= 10) {
        state.endingMessage =
            "Only you remain, guarding the ruins and your memories.\n"
            "The wind whistles through the empty shelter.";
        return;
    }

    state.endingMessage =
        "The door finally opens, but the world before you is unrecognizable.\n"
        "Survival is just another beginning.";
}

int main() {
    srand(time(nullptr));

    UI ui;

    ui.showStartMenu();
    Difficulty diff = ui.askDifficulty();
    int plan = ui.askSupplyPlan(diff);

    GameState state;
    initResources(state, diff, plan);

    while (state.currentDay <= 10 && state.countLivingSurvivors() > 0) {
        ui.showDailyReport(state);

        if (state.countLivingSurvivors() == 0) break;

        if (ui.askTreat(state)) {
            treat(state);
            ui.showMessage("All weak survivors have been healed!");
            ui.waitForEnter();
        }

        std::vector<int> expeditionMembers;
        if (ui.askExpedition(state)) {
            int able = state.countHealthySurvivors() + state.countMutatedSurvivors();
            int maxN = able / 2;
            int n = ui.askExpeditionCount(maxN);
            expeditionMembers = ui.askExpeditionMembers(state, n);
            state.expeditionMemberIds = expeditionMembers;

            ExpeditionEventType expEvent = selectRandomExpeditionEvent(state);
            std::string expResult = processExpeditionEvent(state, expEvent, expeditionMembers);
            ui.showEventResult("EXPEDITION RESULT", expResult);
        }

        DailyEventType nightEvent = selectRandomDailyEvent(state);
        std::string nightResult = processDailyEvent(state, nightEvent);
        ui.showEventResult("NIGHT EVENT", nightResult);

        consumeDaily(state);
        updateSickCounters(state);

        ui.showDayEnd(state);

        state.resetDailyStates();
        state.currentDay++;
    }

    if (state.currentDay > 10) state.currentDay = 10;

    determineEnding(state);
    ui.showEnding(state);

    return 0;
}
