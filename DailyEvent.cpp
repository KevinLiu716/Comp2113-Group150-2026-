#include "EventSystem.h"
#include "Tools.h"
#include <string>
#include <iostream>
#include <cstdlib>  // rand()
#include <ctime>    // time()

// 注意：random1to6() 只在这里定义一次，Expedition.cpp 里的同名函数已删除
int random1to6() {
    return (rand() % 6) + 1;
}

bool getPlayerChoice(const std::string& prompt) {
    std::cout << prompt << std::endl;
    std::cout << "1. Open the door" << std::endl;
    std::cout << "2. Keep the door closed" << std::endl;
    std::cout << "Please enter your choice (1 or 2): ";

    int choice;
    std::cin >> choice;

    while (choice != 1 && choice != 2) {
        std::cout << "Invalid choice. Please enter 1 or 2: ";
        std::cin >> choice;
    }

    return (choice == 1);  // openTheDoor = 1
}

// main function for processing daily event
std::string processDailyEvent(GameState& state, DailyEventType eventType) {
    switch(eventType) {
        case DailyEventType::RADIATION_RAIN:
            return handleRadiationRain(state);
        case DailyEventType::INTERNAL_CONFLICT:
            return handleInternalConflict(state);
        case DailyEventType::MYSTERIOUS_DREAM:
            return handleMysteriousDream(state);
        case DailyEventType::SPOILED_SUPPLIES:
            return handleSpoiledSupplies(state);
        case DailyEventType::UNEXPECTED_VISITOR: {
            std::cout << "Unexpected Visitor: Heavy knocks suddenly sound at the door. "
                      << "Is it a fellow survivor in need of help or a marauder with ill intentions?"
                      << std::endl;
            bool choice = getPlayerChoice("What do you want to do?");
            return handleUnexpectedVisitor(state, choice);
        }
        case DailyEventType::ANOMALOUS_SIGNAL:
            return handleAnomalousSignal(state);
        default:
            return "Nothing special happens tonight.";
    }
}

// choose random daily event
DailyEventType selectRandomDailyEvent(GameState& state) {
    if (state.forceEvent5NextDay) {
        state.forceEvent5NextDay = false;  // reset flag
        return DailyEventType::UNEXPECTED_VISITOR;
    }

    int randomNum = random1to6();

    switch(randomNum) {
        case 1: return DailyEventType::RADIATION_RAIN;
        case 2: return DailyEventType::INTERNAL_CONFLICT;
        case 3: return DailyEventType::MYSTERIOUS_DREAM;
        case 4: return DailyEventType::SPOILED_SUPPLIES;
        case 5: return DailyEventType::UNEXPECTED_VISITOR;
        case 6: return DailyEventType::ANOMALOUS_SIGNAL;
        default: return DailyEventType::MYSTERIOUS_DREAM;
    }
}

std::string handleRadiationRain(GameState& state) {
    std::string result = "Radiation Rain: The sky outside changes ominously.\n";
    result += "Those who went out today are likely in great danger.\n";

    bool hasWeakSurvivors = false;

    for (int i = 0; i < static_cast<int>(state.survivors.size()); ++i) {
        if (state.survivors[i].status == SurvivorStatus::WEAK) {
            hasWeakSurvivors = true;
            state.survivors[i].daysWeak += 1;

            if (state.survivors[i].daysWeak >= 2) {
                state.survivors[i].status = SurvivorStatus::DECEASED;
                result += "Survivor " + std::to_string(i + 1) + " has died from prolonged weakness.\n";
            } else {
                result += "Survivor " + std::to_string(i + 1) + "'s condition worsens (weakness counter: "
                         + std::to_string(state.survivors[i].daysWeak) + ").\n";
            }
        }
    }

    if (!hasWeakSurvivors) {
        result += "Fortunately, no one is currently weak, so no one is affected.\n";
    }

    return result;
}

std::string handleInternalConflict(GameState& state) {
    std::string result = "Internal Conflict: Under immense survival pressure, long-suppressed arguments finally erupt into suspicions inside the shelter.\n";

    if (state.hasRadio) {
        result += "The radio provides calming music, easing tensions. No negative effects.\n";
    } else {
        state.forceEvent5NextDay = true;
        result += "Tensions are high. This may attract unwanted attention tomorrow.\n";
    }

    return result;
}

std::string handleMysteriousDream(GameState& state) {
    std::string result = "Mysterious Dream: In the night, an indistinguishable whisper invades someone's mind directly.\n";

    std::vector<int> candidates = selectRandomSurvivors(state, 1, true, true, false);

    if (candidates.empty()) {
        result += "No healthy or weak survivors to be affected.\n";
        return result;
    }

    int targetIndex = candidates[0];

    if (checkProbability(0.5)) {
        state.survivors[targetIndex].status = SurvivorStatus::MUTATED;
        result += "Survivor " + std::to_string(targetIndex + 1) + " has mutated!\n";
    } else {
        result += "Survivor " + std::to_string(targetIndex + 1) + " resisted the whispers. Nothing happened.\n";
    }

    return result;
}

std::string handleSpoiledSupplies(GameState& state) {
    std::string result = "Spoiled Supplies: An undeniable smell of rot fills the air.\n";
    result += "You discover that part of your precious reserves has spoiled in the dampness.\n";

    if (checkProbability(0.5)) {
        // food
        int loss = 2;
        if (state.food >= loss) {
            state.food -= loss;
            result += "Lost " + std::to_string(loss) + " food.\n";
        } else {
            result += "Lost all remaining " + std::to_string(state.food) + " food.\n";
            state.food = 0;
        }
    } else {
        // water
        int loss = 2;
        if (state.water >= loss) {
            state.water -= loss;
            result += "Lost " + std::to_string(loss) + " water.\n";
        } else {
            result += "Lost all remaining " + std::to_string(state.water) + " water.\n";
            state.water = 0;
        }
    }

    return result;
}

std::string handleUnexpectedVisitor(GameState& state, bool openTheDoor) {
    std::string result = "";
    if (openTheDoor) {
        result += "You open the door. ";

        double roll = static_cast<double>(rand()) / RAND_MAX;

        if (roll < 0.3) {
            state.food += 1;
            state.water += 1;
            result += "A friendly survivor shares some supplies with you.\n";
            result += "Gained 1 food and 1 water.\n";
        } else if (roll < 0.6) {
            int foodLoss = (state.food >= 3) ? 3 : state.food;
            int waterLoss = (state.water >= 3) ? 3 : state.water;

            state.food -= foodLoss;
            state.water -= waterLoss;

            result += "Marauders force their way in and steal supplies.\n";
            result += "Lost " + std::to_string(foodLoss) + " food and "
                   + std::to_string(waterLoss) + " water.\n";
        } else {
            if (!state.hasRadio) {
                state.hasRadio = true;
                result += "The visitor leaves behind a working radio. You now have a radio!\n";
            } else {
                result += "The visitor leaves without incident. Nothing happens.\n";
            }
        }
    } else {
        result += "You decide not to open the door.\n";
        result += "The knocking eventually stops. Nothing happens.\n";
    }

    return result;
}

std::string handleAnomalousSignal(GameState& state) {
    std::string result = "Anomalous Signal: A rhythmic, distinctly unnatural static noise comes from the distance.\n";

    if (state.hasRadio) {
        result += "With the radio, you can analyze the signal. This may be important for the ending.\n";
        state.triggeredEvent6 = true;
    } else {
        state.forceEvent5NextDay = true;
        result += "The strange noise unsettles everyone. This may attract unwanted attention tomorrow.\n";
    }

    return result;
}
