// EventSystem.h
// Declarations for the daily (night) and expedition event systems.
// Defines event type enums, option enums, and all handler function signatures.

#ifndef EVENTSYSTEM_H
#define EVENTSYSTEM_H

#include "Tools.h"
#include "GameState.h"
#include <string>
#include <vector>

// The 6 types of random events that can happen at night in the shelter.
enum class DailyEventType {
    RADIATION_RAIN,    
    INTERNAL_CONFLICT, 
    MYSTERIOUS_DREAM,
    SPOILED_SUPPLIES,
    UNEXPECTED_VISITOR,
    ANOMALOUS_SIGNAL   
};

// Function: processDailyEvent
// What it does: Main dispatcher for nightly events. Routes to the correct
//               handler based on eventType.
// Input:  state - the game state, eventType - which night event to process.
// Output: Returns a narrative string describing what happened.
std::string processDailyEvent(GameState& state, DailyEventType eventType);

// Function: selectRandomDailyEvent
// What it does: Picks a random nightly event. If forceEvent5NextDay is set,
//               forces UNEXPECTED_VISITOR instead of rolling randomly.
// Input:  state - game state (checks forceEvent5NextDay flag).
// Output: Returns one DailyEventType.
DailyEventType selectRandomDailyEvent(GameState& state);

// Function: handleRadiationRain
// What it does: Handles the radiation rain event. Some survivors may get sick.
// Input:  state - game state.
// Output: Returns a description of what happened.
std::string handleRadiationRain(GameState& state);

// Function: handleInternalConflict
// What it does: Handles fights among survivors. May set forceEvent5NextDay
//               flag if no radio is present.
// Input:  state - game state.
// Output: Returns a description of the conflict.
std::string handleInternalConflict(GameState& state);

// Function: handleMysteriousDream
// What it does: A survivor has a strange dream. May cause mutation.
// Input:  state - game state.
// Output: Returns a description of the dream event.
std::string handleMysteriousDream(GameState& state);

// Function: handleSpoiledSupplies
// What it does: Some stored food or water goes bad overnight.
// Input:  state - game state.
// Output: Returns a description of spoilage.
std::string handleSpoiledSupplies(GameState& state);

// Function: handleUnexpectedVisitor
// What it does: A stranger knocks on the shelter door. The player decides
//               whether to open (choice = true) or ignore (choice = false).
// Input:  state - game state, choice - true means open the door.
// Output: Returns a description of the outcome.
std::string handleUnexpectedVisitor(GameState& state, bool choice);  

// Function: handleAnomalousSignal
// What it does: A strange radio signal is detected. May set
//               forceEvent5NextDay flag if the signal is unresolved.
// Input:  state - game state.
// Output: Returns a description of the signal event.
std::string handleAnomalousSignal(GameState& state);


// The 7 possible expedition destinations the player can visit during the day.
enum class ExpeditionEventType {
    SUPERMARKET,
    WATER_PLANT,
    PHARMACY,
    OTHER_CAMP,
    PERIMETER_CLEAR,
    LABORATORY,
    HIDDEN_STORAGE
};

// Player choices when encountering another survivor camp.
enum class CampOption {
    REQUEST_HELP,
    ROB
};

// Player choices when clearing the perimeter around the shelter.
enum class ClearOption {
    OUTER_CLEAR,
    INNER_SEARCH
};

// Function: processExpeditionEvent
// What it does: Main dispatcher for daytime expeditions. Routes to the
//               correct handler based on eventType.
// Input:  state - game state, eventType - which location to visit,
//         memberIndices - indices of survivors who went out,
//         choice - optional sub-choice for branching events (default -1).
// Output: Returns a narrative string describing the expedition result.
std::string processExpeditionEvent(
    GameState& state, 
    ExpeditionEventType eventType,
    const std::vector<int>& memberIndices,
    int choice = -1
);

// Function: selectRandomExpeditionEvent
// What it does: Picks a random expedition destination. If hasNote is set
//               and usedNoteEffect is false, forces HIDDEN_STORAGE.
// Input:  state - game state (checks hasNote and usedNoteEffect flags).
// Output: Returns one ExpeditionEventType.
ExpeditionEventType selectRandomExpeditionEvent(GameState& state);

// Standard expedition handlers (no sub-choice needed)
std::string handleSupermarket(GameState& state, const std::vector<int>& memberIndices);
std::string handleWaterPlant(GameState& state, const std::vector<int>& memberIndices);
std::string handlePharmacy(GameState& state, const std::vector<int>& memberIndices);
std::string handleLaboratory(GameState& state, const std::vector<int>& memberIndices);

// Triggers when the player found a note earlier. Sets usedNoteEffect flag.
std::string handleHiddenStorage(GameState& state, const std::vector<int>& memberIndices);

// Branching expedition handlers (player picks a sub-option)
std::string handleOtherCamp(GameState& state, const std::vector<int>& memberIndices, CampOption option);
std::string handlePerimeterClear(GameState& state, const std::vector<int>& memberIndices, ClearOption option);

#endif // EVENTSYSTEM_H
