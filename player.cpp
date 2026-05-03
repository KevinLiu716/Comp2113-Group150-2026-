++// player.cpp
// Now also assigns survivor names and traits at construction.

#include "GameState.h"
#include <cstdlib>

// Function: GameState (constructor)
// What it does: Initialize all fields to safe defaults. Creates 6 survivors,
//               gives each a fixed name (Alice, Bob, ...), and randomly
//               assigns a unique trait to each by shuffling the trait pool.
// Input:  none (this is the default constructor).
// Output: A fully initialized GameState object with 6 named survivors.
GameState::GameState() {
    difficulty = Difficulty::EASY;
    currentDay = 1;

    food = 0;
    water = 0;++
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
    endingType = EndingType::NONE;

    // Fixed names so the player can refer to "Alice" etc. consistently.
    const char* names[6] = {"Alice", "Bob", "Carol", "David", "Eve", "Frank"};

    SurvivorTrait traits[6] = {
        SurvivorTrait::DOCTOR,
        SurvivorTrait::FRAIL,
        SurvivorTrait::SCOUT,
        SurvivorTrait::ENGINEER,
        SurvivorTrait::SOLDIER,
        SurvivorTrait::LUCKY
    };

    // Fisher-Yates shuffle so each game gives a different trait assignment.
    for (int i = 5; i > 0; i--) {
        int j = rand() % (i + 1);
        SurvivorTrait tmp = traits[i];
        traits[i] = traits[j];
        traits[j] = tmp;
    }

    for (int i = 0; i < 6; i++) {
        Survivor newSurvivor;
        newSurvivor.name = names[i];
        newSurvivor.trait = traits[i];
        survivors.push_back(newSurvivor);
    }
}

// Function: countSurvivorsByStatus
// What it does: Loops through the survivor list and counts how many
//               currently have the given status.
// Input:  status - the SurvivorStatus to look for (HEALTHY, WEAK,
//                  MUTATED, or DECEASED).
// Output: Returns an integer, the number of survivors with that status.
int GameState::countSurvivorsByStatus(SurvivorStatus status) const {
    int count = 0;
    for (int i = 0; i < (int)survivors.size(); i++) {
        if (survivors[i].status == status) count++;
    }
    return count;
}

// Function: countHealthySurvivors
// What it does: Counts how many survivors are currently HEALTHY.
// Input:  none.
// Output: Returns the number of healthy survivors.
int GameState::countHealthySurvivors() const {
    int count = 0;
    for (int i = 0; i < (int)survivors.size(); i++) {
        if (survivors[i].status == SurvivorStatus::HEALTHY) count++;
    }
    return count;
}

// Function: countWeakSurvivors
// What it does: Counts how many survivors are currently WEAK.
//               Weak survivors need medicine or they die in 2 days.
// Input:  none.
// Output: Returns the number of weak survivors.
int GameState::countWeakSurvivors() const {
    int count = 0;
    for (int i = 0; i < (int)survivors.size(); i++) {
        if (survivors[i].status == SurvivorStatus::WEAK) count++;
    }
    return count;
}

// Function: countMutatedSurvivors
// What it does: Counts how many survivors are currently MUTATED.
//               Mutated survivors do not consume food or water.
// Input:  none.
// Output: Returns the number of mutated survivors.
int GameState::countMutatedSurvivors() const {
    int count = 0;
    for (int i = 0; i < (int)survivors.size(); i++) {
        if (survivors[i].status == SurvivorStatus::MUTATED) count++;
    }
    return count;
}

// Function: countLivingSurvivors
// What it does: Counts everyone who is not DECEASED. That means
//               HEALTHY + WEAK + MUTATED are all considered living.
// Input:  none.
// Output: Returns the total number of living survivors.
int GameState::countLivingSurvivors() const {
    int count = 0;
    for (int i = 0; i < (int)survivors.size(); i++) {
        SurvivorStatus s = survivors[i].status;
        if (s == SurvivorStatus::HEALTHY ||
            s == SurvivorStatus::WEAK    ||
            s == SurvivorStatus::MUTATED) {
            count++;
        }
    }
    return count;
}

// Function: hasLivingSurvivorWithTrait
// What it does: Returns true if any non-deceased, non-mutated survivor
//               has the given trait. Mutation is treated as losing one's
//               identity / skills, so a mutated doctor can no longer
//               perform medical treatment, etc.
// Input:  trait - the SurvivorTrait to search for (DOCTOR, FRAIL, etc.).
// Output: Returns true if at least one qualifying survivor has it,
//         false otherwise.
bool GameState::hasLivingSurvivorWithTrait(SurvivorTrait trait) const {
    for (int i = 0; i < (int)survivors.size(); i++) {
        if (survivors[i].status != SurvivorStatus::DECEASED &&
            survivors[i].status != SurvivorStatus::MUTATED  &&
            survivors[i].trait == trait) {
            return true;
        }
    }
    return false;
}

// Function: resetDailyStates
// What it does: Clears the expedition member list from the previous day
//               and resets the treatment flag. Called at the start of
//               each new day.
// Input:  none.
// Output: This function does not return a value. It modifies the object.
void GameState::resetDailyStates() {
    expeditionMemberIds.clear();
    wasTreatedToday = false;
}

// Function: calculateRequiredFood
// What it does: Calculates how much food is needed for one day. Each
//               HEALTHY or WEAK survivor needs 1 food. MUTATED and
//               DECEASED survivors do not consume food.
// Input:  none.
// Output: Returns the total food needed for today.
int GameState::calculateRequiredFood() const {
    int total = 0;
    for (int i = 0; i < (int)survivors.size(); i++) {
        if (survivors[i].status == SurvivorStatus::HEALTHY ||
            survivors[i].status == SurvivorStatus::WEAK) {
            total++;
        }
    }
    return total;
}

// Function: calculateRequiredWater
// What it does: Calculates how much water is needed for one day. Same
//               rule as food: 1 water per HEALTHY or WEAK survivor.
// Input:  none.
// Output: Returns the total water needed for today.
int GameState::calculateRequiredWater() const {
    int total = 0;
    for (int i = 0; i < (int)survivors.size(); i++) {
        if (survivors[i].status == SurvivorStatus::HEALTHY ||
            survivors[i].status == SurvivorStatus::WEAK) {
            total++;
        }
    }
    return total;
}
