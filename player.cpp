// player.cpp
// This file contains the implementation of all the member functions
// that are declared inside the GameState class in GameState.h.

#include "GameState.h"

// Function: GameState (constructor)
// What it does: This is the constructor for the GameState class.
//               It sets all the initial values for the game state.
//               It creates a vector of 6 survivors, all starting as HEALTHY.
//               All resource counts start at 0.
//               All boolean flags start as false.
//               The current day starts at 1.
// Input: This function takes no input parameters.
// Output: This function does not return a value. It initializes the object.
GameState::GameState() {
    difficulty = Difficulty::EASY;
    currentDay = 1;

    food = 0;
    water = 0;
    medicine = 0;

    hasRadio = false;
    hasNote = false;
    usedNoteEffect = false;

    wasTreatedToday = false;

    triggeredEvent6 = false;
    campRobberyCount = 0;
    forceEvent5NextDay = false;

    gameEnded = false;
    endingMessage = "";

    // Create 6 survivors, all starting as HEALTHY with daysWeak = 0
    // The default Survivor constructor already sets HEALTHY and daysWeak = 0
    for (int i = 0; i < 6; i++) {
        Survivor newSurvivor;
        survivors.push_back(newSurvivor);
    }
}

// Function: countSurvivorsByStatus
// What it does: This function counts how many survivors have a specific status.
//               It goes through every survivor in the list one by one,
//               and checks if their status matches the given status.
// Input: status - the SurvivorStatus to look for (HEALTHY, WEAK, MUTATED, or DECEASED).
// Output: Returns an integer, which is the number of survivors with that status.
int GameState::countSurvivorsByStatus(SurvivorStatus status) const {
    int count = 0;
    for (int i = 0; i < (int)survivors.size(); i++) {
        if (survivors[i].status == status) {
            count = count + 1;
        }
    }
    return count;
}

// Function: countHealthySurvivors
// What it does: This function counts how many survivors are in the HEALTHY status.
// Input: This function takes no input parameters.
// Output: Returns an integer, which is the number of healthy survivors.
int GameState::countHealthySurvivors() const {
    int count = 0;
    for (int i = 0; i < (int)survivors.size(); i++) {
        if (survivors[i].status == SurvivorStatus::HEALTHY) {
            count = count + 1;
        }
    }
    return count;
}

// Function: countWeakSurvivors
// What it does: This function counts how many survivors are in the WEAK status.
// Input: This function takes no input parameters.
// Output: Returns an integer, which is the number of weak survivors.
int GameState::countWeakSurvivors() const {
    int count = 0;
    for (int i = 0; i < (int)survivors.size(); i++) {
        if (survivors[i].status == SurvivorStatus::WEAK) {
            count = count + 1;
        }
    }
    return count;
}

// Function: countMutatedSurvivors
// What it does: This function counts how many survivors are in the MUTATED status.
// Input: This function takes no input parameters.
// Output: Returns an integer, which is the number of mutated survivors.
int GameState::countMutatedSurvivors() const {
    int count = 0;
    for (int i = 0; i < (int)survivors.size(); i++) {
        if (survivors[i].status == SurvivorStatus::MUTATED) {
            count = count + 1;
        }
    }
    return count;
}

// Function: countLivingSurvivors
// What it does: This function counts how many survivors are still alive.
//               A living survivor is one whose status is HEALTHY, WEAK, or MUTATED.
//               Survivors with DECEASED status are not counted.
// Input: This function takes no input parameters.
// Output: Returns an integer, which is the total number of living survivors.
int GameState::countLivingSurvivors() const {
    int count = 0;
    for (int i = 0; i < (int)survivors.size(); i++) {
        if (survivors[i].status == SurvivorStatus::HEALTHY) {
            count = count + 1;
        }
        if (survivors[i].status == SurvivorStatus::WEAK) {
            count = count + 1;
        }
        if (survivors[i].status == SurvivorStatus::MUTATED) {
            count = count + 1;
        }
    }
    return count;
}

// Function: resetDailyStates
// What it does: This function resets the temporary daily states at the start of each day.
//               It clears the list of expedition members from the previous day,
//               and sets the wasTreatedToday flag back to false.
// Input: This function takes no input parameters.
// Output: This function does not return a value. It modifies the object directly.
void GameState::resetDailyStates() {
    expeditionMemberIds.clear();
    wasTreatedToday = false;
}

// Function: calculateRequiredFood
// What it does: This function calculates how much food is needed for one day.
//               Each survivor who is HEALTHY or WEAK needs 1 food per day.
//               Survivors who are MUTATED or DECEASED do not need food.
// Input: This function takes no input parameters.
// Output: Returns an integer, which is the total amount of food needed for today.
int GameState::calculateRequiredFood() const {
    int totalFoodNeeded = 0;
    for (int i = 0; i < (int)survivors.size(); i++) {
        if (survivors[i].status == SurvivorStatus::HEALTHY) {
            totalFoodNeeded = totalFoodNeeded + 1;
        }
        if (survivors[i].status == SurvivorStatus::WEAK) {
            totalFoodNeeded = totalFoodNeeded + 1;
        }
    }
    return totalFoodNeeded;
}

// Function: calculateRequiredWater
// What it does: This function calculates how much water is needed for one day.
//               Each survivor who is HEALTHY or WEAK needs 1 water per day.
//               Survivors who are MUTATED or DECEASED do not need water.
// Input: This function takes no input parameters.
// Output: Returns an integer, which is the total amount of water needed for today.
int GameState::calculateRequiredWater() const {
    int totalWaterNeeded = 0;
    for (int i = 0; i < (int)survivors.size(); i++) {
        if (survivors[i].status == SurvivorStatus::HEALTHY) {
            totalWaterNeeded = totalWaterNeeded + 1;
        }
        if (survivors[i].status == SurvivorStatus::WEAK) {
            totalWaterNeeded = totalWaterNeeded + 1;
        }
    }
    return totalWaterNeeded;
}
