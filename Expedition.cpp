#include "EventSystem.h"
#include "Tools.h"
#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>

// Note: getBinaryChoice() is only used in this file.
int getBinaryChoice() {
    int choice;
    while (true) {
        if (std::cin >> choice && (choice == 1 || choice == 2)) {
            return choice;
        }
        if (std::cin.eof()) {
            return 1;  // Default safe option on EOF.
        }
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Invalid choice. Please enter 1 or 2: ";
    }
}

// Helper: pick a variant string at random.
static std::string pickExpVariant(const std::string variants[], int n) {
    return variants[rand() % n];
}

// Helper: returns true if any expedition member has the given trait.
static bool partyHasTrait(const GameState& state,
                          const std::vector<int>& memberIndices,
                          SurvivorTrait trait) {
    for (int idx : memberIndices) {
        if (idx >= 0 && idx < (int)state.survivors.size() &&
            state.survivors[idx].trait == trait) {
            return true;
        }
    }
    return false;
}

// Main dispatcher. Note that the destination is now chosen by the player
// via the UI (see ui.cpp::askExpeditionDestination), not by
// selectRandomExpeditionEvent below.
std::string processExpeditionEvent(
    GameState& state,
    ExpeditionEventType eventType,
    const std::vector<int>& memberIndices,
    int choice
) {
    switch(eventType) {
        case ExpeditionEventType::SUPERMARKET:
            return handleSupermarket(state, memberIndices);
        case ExpeditionEventType::WATER_PLANT:
            return handleWaterPlant(state, memberIndices);
        case ExpeditionEventType::PHARMACY:
            return handlePharmacy(state, memberIndices);
        case ExpeditionEventType::OTHER_CAMP: {
            std::cout << "Other Survivors' Camp: Smoke rising in the distance indicates others are there.\n"
                      << "Do you choose to negotiate or to take by force?" << std::endl;
            std::cout << "1. Request help (negotiate)" << std::endl;
            std::cout << "2. Rob (take by force)" << std::endl;
            std::cout << "Please enter 1 or 2: ";

            int campChoice = getBinaryChoice();
            CampOption option = (campChoice == 1) ? CampOption::REQUEST_HELP : CampOption::ROB;
            return handleOtherCamp(state, memberIndices, option);
        }
        case ExpeditionEventType::PERIMETER_CLEAR: {
            std::cout << "Clearing the Perimeter: Searching the destroyed houses around the shelter is less risky,\n"
                      << "but the rewards are usually meager." << std::endl;
            std::cout << "1. Clear the outer area (safer, smaller reward)" << std::endl;
            std::cout << "2. Search deeper inside (riskier, potentially better reward)" << std::endl;
            std::cout << "Please enter 1 or 2: ";

            int clearChoice = getBinaryChoice();
            ClearOption clearOption = (clearChoice == 1) ? ClearOption::OUTER_CLEAR : ClearOption::INNER_SEARCH;
            return handlePerimeterClear(state, memberIndices, clearOption);
        }
        case ExpeditionEventType::LABORATORY:
            return handleLaboratory(state, memberIndices);
        case ExpeditionEventType::HIDDEN_STORAGE:
            return handleHiddenStorage(state, memberIndices);
        default:
            return "Unknown expedition event.";
    }
}

// Kept for compatibility but no longer called from main.
ExpeditionEventType selectRandomExpeditionEvent(GameState& state) {
    if (state.hasNote && !state.usedNoteEffect) {
        state.usedNoteEffect = true;
        return ExpeditionEventType::HIDDEN_STORAGE;
    }
    int randomNum = (rand() % 6) + 1;
    switch(randomNum) {
        case 1: return ExpeditionEventType::SUPERMARKET;
        case 2: return ExpeditionEventType::WATER_PLANT;
        case 3: return ExpeditionEventType::PHARMACY;
        case 4: return ExpeditionEventType::OTHER_CAMP;
        case 5: return ExpeditionEventType::PERIMETER_CLEAR;
        case 6: return ExpeditionEventType::LABORATORY;
        default: return ExpeditionEventType::SUPERMARKET;
    }
}

std::string handleSupermarket(GameState& state, const std::vector<int>& memberIndices) {
    static const std::string openings[3] = {
        "Passing by a Supermarket: Shelves overturned, glass underfoot.\nA few cans roll out from the rubble.\n",
        "Passing by a Supermarket: The aisles are looted, but the back room\nstill smells faintly of food.\n",
        "Passing by a Supermarket: Faded signs hang above empty racks.\nSomething edible might still be hidden behind them.\n"
    };
    std::string result = pickExpVariant(openings, 3);

    int expeditionSize = memberIndices.size();

    int foodGain = (state.difficulty == Difficulty::EASY) ? 4 : 3;
    if (partyHasTrait(state, memberIndices, SurvivorTrait::SCOUT)) {
        foodGain += 1;
        result += "(Scout's keen eyes spot an extra stash.)\n";
    }
    state.food += foodGain;
    result += "Found " + std::to_string(foodGain) + " food.\n";

    int waterCost = (expeditionSize + 1) / 2;
    if (waterCost > state.water) waterCost = state.water;
    state.water -= waterCost;

    if (waterCost > 0) {
        result += "The journey was tiring. Extra " + std::to_string(waterCost)
                 + " water consumed.\n";
    }

    return result;
}

std::string handleWaterPlant(GameState& state, const std::vector<int>& memberIndices) {
    static const std::string openings[3] = {
        "Water Plant: The pumps are dead, but a sealed reservoir glints below.\n",
        "Water Plant: Pipes drip with reddish residue. Deeper tanks may still be clean.\n",
        "Water Plant: The treatment hall echoes. A side stairwell leads to the storage tanks.\n"
    };
    std::string result = pickExpVariant(openings, 3);

    int expeditionSize = memberIndices.size();

    int waterGain = (state.difficulty == Difficulty::EASY) ? 6 : 4;
    if (partyHasTrait(state, memberIndices, SurvivorTrait::SCOUT)) {
        waterGain += 1;
        result += "(Scout finds a hidden valve still flowing.)\n";
    }
    state.water += waterGain;
    result += "Found " + std::to_string(waterGain) + " water.\n";

    int foodCost = (expeditionSize + 1) / 2;
    if (foodCost > state.food) foodCost = state.food;
    state.food -= foodCost;

    if (foodCost > 0) {
        result += "Carrying water is heavy work. Extra " + std::to_string(foodCost)
                 + " food consumed.\n";
    }

    return result;
}

std::string handlePharmacy(GameState& state, const std::vector<int>& memberIndices) {
    static const std::string openings[3] = {
        "Pharmacy: Glass crunches under your feet. A few intact bottles remain.\n",
        "Pharmacy: The counter is shattered, but a back cabinet is still locked.\n",
        "Pharmacy: Pills are scattered across the floor. Some labels still read clearly.\n"
    };
    std::string result = pickExpVariant(openings, 3);

    int expeditionSize = memberIndices.size();

    int medicineGain = 2;
    if (partyHasTrait(state, memberIndices, SurvivorTrait::SCOUT)) {
        medicineGain += 1;
        result += "(Scout pries open a hidden drawer.)\n";
    }
    state.medicine += medicineGain;
    result += "Found " + std::to_string(medicineGain) + " medicine.\n";

    int resourceCost = expeditionSize;

    int actualFoodCost = resourceCost;
    if (actualFoodCost > state.food) actualFoodCost = state.food;

    int actualWaterCost = resourceCost;
    if (actualWaterCost > state.water) actualWaterCost = state.water;

    state.food -= actualFoodCost;
    state.water -= actualWaterCost;

    if (actualFoodCost > 0 && actualWaterCost > 0) {
        result += "The scavenging took longer than expected. Extra "
                 + std::to_string(actualFoodCost) + " food and "
                 + std::to_string(actualWaterCost) + " water consumed.\n";
    } else if (actualFoodCost > 0) {
        result += "Extra " + std::to_string(actualFoodCost) + " food consumed.\n";
    } else if (actualWaterCost > 0) {
        result += "Extra " + std::to_string(actualWaterCost) + " water consumed.\n";
    }

    return result;
}

std::string handleOtherCamp(GameState& state, const std::vector<int>& memberIndices, CampOption option) {
    std::string result = "";

    if (option == CampOption::REQUEST_HELP) {
        static const std::string openings[3] = {
            "You walk into the camp with hands raised.\n",
            "You ask quietly if they can spare anything.\n",
            "You explain your situation, hoping for kindness.\n"
        };
        result += pickExpVariant(openings, 3);

        double successProbability = (state.difficulty == Difficulty::EASY) ? 0.7 : 0.4;

        if (checkProbability(successProbability)) {
            state.food += 1;
            state.water += 2;
            result += "They are friendly. They share 1 food and 2 water.\n";
        } else {
            result += "They refuse. They are wary of strangers.\n";
        }
    }
    else {  // ROB
        static const std::string openings[3] = {
            "You creep up on the camp under cover of dusk.\n",
            "You move on the camp with weapons drawn.\n",
            "You strike the camp without warning.\n"
        };
        result += pickExpVariant(openings, 3);

        state.food += 4;
        state.water += 4;
        result += "You take 4 food and 4 water.\n";

        double deathProbability = (state.difficulty == Difficulty::EASY) ? 0.5 : 1.0;

        if (checkProbability(deathProbability)) {
            if (!memberIndices.empty()) {
                int randomIndex = rand() % memberIndices.size();
                int deadSurvivorIndex = memberIndices[randomIndex];

                state.survivors[deadSurvivorIndex].status = SurvivorStatus::DECEASED;
                result += "During the firefight, " + state.survivors[deadSurvivorIndex].name
                         + " was killed.\n";
            }
        } else {
            result += "You escape without casualties this time.\n";
        }

        state.campRobberyCount++;
    }

    return result;
}

std::string handlePerimeterClear(GameState& state, const std::vector<int>& memberIndices, ClearOption option) {
    std::string result = "";

    if (option == ClearOption::OUTER_CLEAR) {
        static const std::string openings[3] = {
            "You sweep the broken houses near the shelter.\n",
            "You comb through nearby ruins, careful of unstable walls.\n",
            "You search rubble for anything overlooked.\n"
        };
        result += pickExpVariant(openings, 3);

        int resourceGain = (state.difficulty == Difficulty::EASY) ? 2 : 1;
        if (partyHasTrait(state, memberIndices, SurvivorTrait::SCOUT)) {
            resourceGain += 1;
            result += "(Scout uncovers extra supplies.)\n";
        }

        if (checkProbability(0.5)) {
            state.food += resourceGain;
            result += "Found " + std::to_string(resourceGain) + " food.\n";
        } else {
            state.water += resourceGain;
            result += "Found " + std::to_string(resourceGain) + " water.\n";
        }

    } else {  // INNER_SEARCH
        static const std::string openings[3] = {
            "You push through a half-collapsed building.\n",
            "You crawl into a basement nobody else has touched.\n",
            "You force open a stuck cellar door.\n"
        };
        result += pickExpVariant(openings, 3);

        state.hasNote = true;
        result += "You find a note with markings. It might lead to a hidden storage.\n";
    }

    return result;
}

std::string handleLaboratory(GameState& state, const std::vector<int>& memberIndices) {
    static const std::string openings[3] = {
        "Laboratory: The doors hiss open with stale, glowing air.\n",
        "Laboratory: Containment tanks line the corridor, some still humming.\n",
        "Laboratory: Warning signs in three languages cover every wall.\n"
    };
    std::string result = pickExpVariant(openings, 3);

    if (state.difficulty == Difficulty::EASY) {
        for (int survivorIndex : memberIndices) {
            if (state.survivors[survivorIndex].status == SurvivorStatus::HEALTHY) {
                state.survivors[survivorIndex].status = SurvivorStatus::MUTATED;
            }
        }
        result += "All expedition members have mutated.\n";
    } else {
        if (!memberIndices.empty()) {
            int randomIndex = rand() % memberIndices.size();
            int mutatedSurvivorIndex = memberIndices[randomIndex];

            if (state.survivors[mutatedSurvivorIndex].status == SurvivorStatus::HEALTHY) {
                state.survivors[mutatedSurvivorIndex].status = SurvivorStatus::MUTATED;
                result += state.survivors[mutatedSurvivorIndex].name + " has mutated!\n";
            } else {
                result += "Strangely, no one is affected this time.\n";
            }
        }
    }

    return result;
}

std::string handleHiddenStorage(GameState& state, const std::vector<int>& memberIndices) {
    static const std::string openings[3] = {
        "Hidden Storage: The note's markings lead to a buried hatch.\n",
        "Hidden Storage: A rusted lock yields under pressure. Inside, untouched supplies.\n",
        "Hidden Storage: You uncover a sealed bunker the previous owners never returned to.\n"
    };
    std::string result = pickExpVariant(openings, 3);

    state.food += 3;
    state.water += 3;
    result += "Found 3 food and 3 water in the hidden storage.\n";

    return result;
}
