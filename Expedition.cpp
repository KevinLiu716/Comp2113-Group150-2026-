#include "EventSystem.h"
#include "Tools.h"
#include <string>
#include <iostream>
#include <cstdlib>    // rand()
#include <ctime>      // time()

// 注意：random1to6() 已经在 DailyEvent.cpp 里定义，这里删除
// 注意：getBinaryChoice() 仅在本文件使用，保留

// get player's choice 1-2 (Camp & Clearing)
int getBinaryChoice() {
    int choice;
    std::cin >> choice;

    while (choice != 1 && choice != 2) {
        std::cout << "Invalid choice. Please enter 1 or 2: ";
        std::cin >> choice;
    }

    return choice;
}

// main function for processing expedition event
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

// select random event
ExpeditionEventType selectRandomExpeditionEvent(GameState& state) {
    // has note?
    if (state.hasNote && !state.usedNoteEffect) {
        state.usedNoteEffect = true;
        return ExpeditionEventType::HIDDEN_STORAGE;
    }

    int randomNum = (rand() % 6) + 1;  // 改用本地的随机，不依赖 random1to6

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
    std::string result = "Passing by a Supermarket: The shelves are mostly empty,\n";
    result += "but beneath the rubble, there might still be forgotten cans.\n";

    int expeditionSize = memberIndices.size();

    // award: food (depends on difficulty)
    int foodGain = (state.difficulty == Difficulty::EASY) ? 4 : 3;
    state.food += foodGain;
    result += "Found " + std::to_string(foodGain) + " food.\n";

    // consume more water today
    int waterCost = (expeditionSize + 1) / 2;
    if (waterCost > state.water) {
        waterCost = state.water;
    }
    state.water -= waterCost;

    if (waterCost > 0) {
        result += "The journey was more tiring than expected. Extra "
                 + std::to_string(waterCost) + " water consumed.\n";
    }

    return result;
}

std::string handleWaterPlant(GameState& state, const std::vector<int>& memberIndices) {
    std::string result = "Water Plant: The city's water purification system has long failed,\n";
    result += "but the plant's deep storage tanks might still hold uncontaminated water.\n";

    int expeditionSize = memberIndices.size();

    int waterGain = (state.difficulty == Difficulty::EASY) ? 6 : 4;
    state.water += waterGain;
    result += "Found " + std::to_string(waterGain) + " water.\n";

    int foodCost = (expeditionSize + 1) / 2;
    if (foodCost > state.food) {
        foodCost = state.food;
    }
    state.food -= foodCost;

    if (foodCost > 0) {
        result += "Carrying the water back was exhausting. Extra "
                 + std::to_string(foodCost) + " food consumed.\n";
    }

    return result;
}

std::string handlePharmacy(GameState& state, const std::vector<int>& memberIndices) {
    std::string result = "Pharmacy: Shattered glass counters litter the floor.\n";
    result += "Scavenging among the scattered medicine bottles is a race against death.\n";

    int expeditionSize = memberIndices.size();

    int medicineGain = 2;
    state.medicine += medicineGain;
    result += "Found " + std::to_string(medicineGain) + " medicine.\n";

    int resourceCost = expeditionSize;

    int actualFoodCost = resourceCost;
    if (actualFoodCost > state.food) {
        actualFoodCost = state.food;
    }

    int actualWaterCost = resourceCost;
    if (actualWaterCost > state.water) {
        actualWaterCost = state.water;
    }

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
    std::string result = "Other Survivors' Camp: ";

    if (option == CampOption::REQUEST_HELP) {
        result += "You decide to request help.\n";

        double successProbability = (state.difficulty == Difficulty::EASY) ? 0.7 : 0.4;

        if (checkProbability(successProbability)) {
            state.food += 1;
            state.water += 2;
            result += "The other survivors are friendly. They share 1 food and 2 water with you.\n";
        } else {
            result += "The other survivors refuse to help. They seem wary of strangers.\n";
        }
    }
    else {  // CampOption::ROB
        result += "You decide to rob the camp.\n";

        state.food += 4;
        state.water += 4;
        result += "You take 4 food and 4 water from the camp.\n";

        double deathProbability = (state.difficulty == Difficulty::EASY) ? 0.5 : 1.0;

        if (checkProbability(deathProbability)) {
            if (!memberIndices.empty()) {
                int randomIndex = rand() % memberIndices.size();
                int deadSurvivorIndex = memberIndices[randomIndex];

                state.survivors[deadSurvivorIndex].status = SurvivorStatus::DECEASED;
                result += "During the robbery, survivor " + std::to_string(deadSurvivorIndex + 1)
                         + " was killed in the fight.\n";
            }
        } else {
            result += "You manage to escape without casualties.\n";
        }

        state.campRobberyCount++;
    }

    return result;
}

std::string handlePerimeterClear(GameState& state, const std::vector<int>& memberIndices, ClearOption option) {
    std::string result = "Clearing the Perimeter: ";

    if (option == ClearOption::OUTER_CLEAR) {
        result += "You clear the outer area.\n";

        int resourceGain = (state.difficulty == Difficulty::EASY) ? 2 : 1;

        if (checkProbability(0.5)) {
            state.food += resourceGain;
            result += "Found " + std::to_string(resourceGain) + " food.\n";
        } else {
            state.water += resourceGain;
            result += "Found " + std::to_string(resourceGain) + " water.\n";
        }

    } else {  // ClearOption::INNER_SEARCH
        result += "You search deeper inside.\n";

        state.hasNote = true;
        result += "You find a note with some information. This might lead to a hidden storage.\n";
    }

    return result;
}

std::string handleLaboratory(GameState& state, const std::vector<int>& memberIndices) {
    std::string result = "Laboratory: Inside might lie the answers to what caused all this,\n";
    result += "or deeper nightmares.\n";

    if (state.difficulty == Difficulty::EASY) {
        for (int survivorIndex : memberIndices) {
            if (state.survivors[survivorIndex].status == SurvivorStatus::HEALTHY ||
                state.survivors[survivorIndex].status == SurvivorStatus::WEAK) {
                state.survivors[survivorIndex].status = SurvivorStatus::MUTATED;
            }
        }
        result += "All expedition members have mutated!\n";
    } else {
        if (!memberIndices.empty()) {
            int randomIndex = rand() % memberIndices.size();
            int mutatedSurvivorIndex = memberIndices[randomIndex];

            if (state.survivors[mutatedSurvivorIndex].status == SurvivorStatus::HEALTHY ||
                state.survivors[mutatedSurvivorIndex].status == SurvivorStatus::WEAK) {
                state.survivors[mutatedSurvivorIndex].status = SurvivorStatus::MUTATED;
                result += "Survivor " + std::to_string(mutatedSurvivorIndex + 1) + " has mutated!\n";
            }
        }
    }

    return result;
}

std::string handleHiddenStorage(GameState& state, const std::vector<int>& memberIndices) {
    std::string result = "Hidden Storage: Following the vague markings on the note,\n";
    result += "you actually find an unlooted storage point.\n";

    state.food += 3;
    state.water += 3;
    result += "Found 3 food and 3 water in the hidden storage.\n";

    return result;
}
