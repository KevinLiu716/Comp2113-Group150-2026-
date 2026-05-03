// DailyEvent.cpp
// Implements the night-time random events.

#include "EventSystem.h"
#include "Tools.h"
#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>

// random1to6 is defined here. The duplicate in Expedition.cpp was removed.
int random1to6() {
    return (rand() % 6) + 1;
}

// Show a prompt asking the player to pick 1 or 2.
// Returns true for 1 (open the door), false for 2 or EOF.
bool getPlayerChoice(const std::string& prompt) {
    std::cout << prompt << std::endl;
    std::cout << "1. Open the door" << std::endl;
    std::cout << "2. Keep the door closed" << std::endl;
    std::cout << "Please enter your choice (1 or 2): ";

    while (true) {
        std::string line;
        if (!readLineWithEditing(line)) {
            return false;  // EOF: default to safe option
        }
        // Trim whitespace.
        size_t s = line.find_first_not_of(" \t");
        size_t e = line.find_last_not_of(" \t");
        if (s == std::string::npos) {
            std::cout << "Please enter 1 or 2: ";
            continue;
        }
        std::string trimmed = line.substr(s, e - s + 1);
        if (trimmed == "1") return true;
        if (trimmed == "2") return false;
        std::cout << "Invalid choice. Please enter 1 or 2: ";
    }
}

// Function: pickVariant
// What it does: Selects a random string from an array of text variants to add flavor to events.
// Input:  variants[] - Array of strings.
//         n - The size of the array.
// Output: Returns the randomly selected string.
static std::string pickVariant(const std::string variants[], int n) {
    return variants[rand() % n];
}

// Function: processDailyEvent
// What it does: Acts as the main dispatcher, routing the selected event type to its specific handler function.
// Input:  state - Reference to the global GameState.
//         eventType - The specific DailyEventType enum to process.
// Output: Returns a formatted string detailing the outcome of the night event.
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

// Function: selectRandomDailyEvent
// What it does: Randomly determines which event occurs tonight (1-6). 
//               Also checks if forceEvent5NextDay flag is active, overriding the random roll.
// Input:  state - Reference to the global GameState.
// Output: Returns the chosen DailyEventType enum.
DailyEventType selectRandomDailyEvent(GameState& state) {
    if (state.forceEvent5NextDay) {
        state.forceEvent5NextDay = false;
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

// Function: handleRadiationRain
// What it does: Processes the Radiation Rain event. Finds all survivors currently in the WEAK state 
//               and increments their weakness duration. If a survivor is weak for 2 days, their status changes to DECEASED.
// Input:  state - Reference to the global GameState.
// Output: Returns a descriptive string summarizing health changes or deaths.
std::string handleRadiationRain(GameState& state) {
    static const std::string variants[3] = {
        "Radiation Rain: The sky turns sickly green.\nDeadly droplets hammer the shelter walls.\n",
        "Radiation Rain: The geiger counter screams to life.\nA toxic downpour seeps through every crack.\n",
        "Radiation Rain: Ash-black clouds gather low.\nRadioactive rain pools at the door, glowing faintly.\n"
    };
    std::string result = pickVariant(variants, 3);

    bool hasWeakSurvivors = false;

    for (int i = 0; i < static_cast<int>(state.survivors.size()); ++i) {
        if (state.survivors[i].status == SurvivorStatus::WEAK) {
            hasWeakSurvivors = true;
            state.survivors[i].daysWeak += 1;

            if (state.survivors[i].daysWeak >= 2) {
                state.survivors[i].status = SurvivorStatus::DECEASED;
                result += state.survivors[i].name + " has died from prolonged weakness.\n";
            } else {
                result += state.survivors[i].name + "'s condition worsens (weakness counter: "
                         + std::to_string(state.survivors[i].daysWeak) + ").\n";
            }
        }
    }

    if (!hasWeakSurvivors) {
        result += "Fortunately, no one is currently weak, so the shelter holds.\n";
    }

    return result;
}

// Function: handleInternalConflict
// What it does: Processes the Internal Conflict event. Checks if the shelter has a radio. 
//               If yes, negates the event. If no, sets the flag to force an Unexpected Visitor event the next day.
// Input:  state - Reference to the global GameState.
// Output: Returns a string describing the conflict outcome.
std::string handleInternalConflict(GameState& state) {
    static const std::string variants[3] = {
        "Internal Conflict: Tempers flare over the last ration of water.\n",
        "Internal Conflict: Whispered accusations turn into shouting matches.\n",
        "Internal Conflict: A long-buried argument finally erupts in the shelter.\n"
    };
    std::string result = pickVariant(variants, 3);

    if (state.hasRadio) {
        result += "The radio plays an old tune. Tempers cool. No negative effects.\n";
    } else {
        state.forceEvent5NextDay = true;
        result += "Tensions remain high. The noise may attract unwanted attention tomorrow.\n";
    }

    return result;
}

// Function: handleMysteriousDream
// What it does: Processes the Mysterious Dream event. Selects one healthy or weak survivor randomly. 
//               There is a 50% chance the selected survivor's status changes to MUTATED.
// Input:  state - Reference to the global GameState.
// Output: Returns a string detailing who was affected and if they mutated.
std::string handleMysteriousDream(GameState& state) {
    static const std::string variants[3] = {
        "Mysterious Dream: A whisper crawls through one survivor's sleep.\n",
        "Mysterious Dream: Strange voices fill someone's mind with cold light.\n",
        "Mysterious Dream: A vision of a tower made of teeth invades the night.\n"
    };
    std::string result = pickVariant(variants, 3);

    std::vector<int> candidates = selectRandomSurvivors(state, 1, true, true, false);

    if (candidates.empty()) {
        result += "No healthy or weak survivors are present to be affected.\n";
        return result;
    }

    int targetIndex = candidates[0];
    const std::string& victim = state.survivors[targetIndex].name;

    if (checkProbability(0.5)) {
        state.survivors[targetIndex].status = SurvivorStatus::MUTATED;
        result += victim + " has mutated!\n";
    } else {
        result += victim + " resists the whispers. Nothing happens.\n";
    }

    return result;
}

// Function: handleSpoiledSupplies
// What it does: Processes the Spoiled Supplies event. Randomly chooses to reduce either food or water by 2 units. 
//               Safely handles cases where resources are less than 2.
// Input:  state - Reference to the global GameState.
// Output: Returns a string specifying which resource was lost and by how much.
std::string handleSpoiledSupplies(GameState& state) {
    static const std::string variants[3] = {
        "Spoiled Supplies: A foul stench rises from the storage corner.\n",
        "Spoiled Supplies: Mold has crept into the rations overnight.\n",
        "Spoiled Supplies: Damp has ruined part of the precious reserves.\n"
    };
    std::string result = pickVariant(variants, 3);

    if (checkProbability(0.5)) {
        int loss = 2;
        if (state.food >= loss) {
            state.food -= loss;
            result += "Lost " + std::to_string(loss) + " food.\n";
        } else {
            result += "Lost all remaining " + std::to_string(state.food) + " food.\n";
            state.food = 0;
        }
    } else {
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

// Function: handleUnexpectedVisitor
// What it does: Processes the Unexpected Visitor event based on the player's choice. 
//               If opened, rolls for 3 outcomes (gain supplies, lose supplies, gain radio).
// Input:  state - Reference to the global GameState.
//         openTheDoor - Boolean representing the player's prior choice.
// Output: Returns a string describing the outcome of opening or ignoring the door.
std::string handleUnexpectedVisitor(GameState& state, bool openTheDoor) {
    std::string result = "";
    if (openTheDoor) {
        static const std::string openings[3] = {
            "You open the door, hand on a makeshift weapon.\n",
            "You unbolt the door slowly, heart pounding.\n",
            "You crack the door open, just enough to see.\n"
        };
        result += pickVariant(openings, 3);

        double roll = static_cast<double>(rand()) / RAND_MAX;

        if (roll < 0.3) {
            state.food += 1;
            state.water += 1;
            result += "A weary stranger shares some supplies before moving on.\n";
            result += "Gained 1 food and 1 water.\n";
        } else if (roll < 0.6) {
            int foodLoss = (state.food >= 3) ? 3 : state.food;
            int waterLoss = (state.water >= 3) ? 3 : state.water;

            state.food -= foodLoss;
            state.water -= waterLoss;

            result += "Marauders shove their way in and take what they want.\n";
            result += "Lost " + std::to_string(foodLoss) + " food and "
                   + std::to_string(waterLoss) + " water.\n";
        } else {
            if (!state.hasRadio) {
                state.hasRadio = true;
                result += "The visitor leaves behind a battered but working radio.\n";
                result += "You now have a radio!\n";
            } else {
                result += "The visitor mutters thanks, then disappears into the night.\n";
            }
        }
    } else {
        static const std::string ignored[3] = {
            "You hold your breath. The knocking eventually stops.\nNothing happens.\n",
            "You stay silent and still. The footsteps fade.\nNothing happens.\n",
            "You ignore the visitor. After a long minute, they leave.\nNothing happens.\n"
        };
        result += pickVariant(ignored, 3);
    }

    return result;
}

// Function: handleAnomalousSignal
// What it does: Processes the Anomalous Signal event. If the shelter has a radio, sets triggeredEvent6 flag to true. 
//               If not, forces an Unexpected Visitor event the next day.
// Input:  state - Reference to the global GameState.
// Output: Returns a string describing the signal's impact on the shelter.
std::string handleAnomalousSignal(GameState& state) {
    static const std::string variants[3] = {
        "Anomalous Signal: A rhythmic static rises from the dead air.\n",
        "Anomalous Signal: A pulse - three short, three long - repeats outside.\n",
        "Anomalous Signal: A piercing tone cuts the silence at exactly midnight.\n"
    };
    std::string result = pickVariant(variants, 3);

    if (state.hasRadio) {
        result += "Through the radio you decode the pattern. This may matter for the ending.\n";
        state.triggeredEvent6 = true;
    } else {
        state.forceEvent5NextDay = true;
        result += "The strange noise unsettles everyone. It may attract attention tomorrow.\n";
    }

    return result;
}
