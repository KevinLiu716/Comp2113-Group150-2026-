// player.cpp
// Implements GameState's constructor and helper methods.
// Now also assigns survivor names and traits at construction.

#include "GameState.h"
#include <cstdlib>

// Function: GameState (constructor)
// Purpose:  Initialize all fields to safe defaults. Creates 6 survivors,
//           gives each a fixed name (Alice, Bob, ...), and randomly
//           assigns a unique trait to each by shuffling the trait pool.
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

// Count how many survivors currently have the given status.
int GameState::countSurvivorsByStatus(SurvivorStatus status) const {
    int count = 0;
    for (int i = 0; i < (int)survivors.size(); i++) {
        if (survivors[i].status == status) count++;
    }
    return count;
}

int GameState::countHealthySurvivors() const {
    int count = 0;
    for (int i = 0; i < (int)survivors.size(); i++) {
        if (survivors[i].status == SurvivorStatus::HEALTHY) count++;
    }
    return count;
}

int GameState::countWeakSurvivors() const {
    int count = 0;
    for (int i = 0; i < (int)survivors.size(); i++) {
        if (survivors[i].status == SurvivorStatus::WEAK) count++;
    }
    return count;
}

int GameState::countMutatedSurvivors() const {
    int count = 0;
    for (int i = 0; i < (int)survivors.size(); i++) {
        if (survivors[i].status == SurvivorStatus::MUTATED) count++;
    }
    return count;
}

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

// Returns true if any non-deceased survivor has the given trait.
bool GameState::hasLivingSurvivorWithTrait(SurvivorTrait trait) const {
    for (int i = 0; i < (int)survivors.size(); i++) {
        if (survivors[i].status != SurvivorStatus::DECEASED &&
            survivors[i].trait == trait) {
            return true;
        }
    }
    return false;
}

void GameState::resetDailyStates() {
    expeditionMemberIds.clear();
    wasTreatedToday = false;
}

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
