#ifndef EVENTSYSTEM_H
#define EVENTSYSTEM_H

#include "gamestate.h"
#include <string>
#include <vector>

// daily event
enum class DailyEventType {
    RADIATION_RAIN,    
    INTERNAL_CONFLICT, 
    MYSTERIOUS_DREAM,
    SPOILED_SUPPLIES,
    UNEXPECTED_VISITOR,
    ANOMALOUS_SIGNAL   
};

std::string processDailyEvent(GameState& state, DailyEventType eventType);
//return discription/result 

DailyEventType selectRandomDailyEvent(GameState& state);
//return eventType
//!! consider flag: forceEvent5NextDay

std::string handleRadiationRain(GameState& state);
//return discrption


std::string handleInternalConflict(GameState& state);
//return discrption
//!! set flag: forceEvent5NextDay

std::string handleMysteriousDream(GameState& state);
//return discrption

std::string handleSpoiledSupplies(GameState& state);
//return discrption

std::string handleUnexpectedVisitor(GameState& state, bool choice);  
//return discrption
//player's choice true=openTheDoor, false = ...

std::string handleAnomalousSignal(GameState& state);
//!! set flag: forceEvent5NextDay



enum class ExpeditionEventType {
    SUPERMARKET,
    WATER_PLANT,
    PHARMACY,
    OTHER_CAMP,
    PERIMETER_CLEAR,
    LABORATORY,
    HIDDEN_STORAGE
};

//camp
enum class CampOption {
    REQUEST_HELP,
    ROB
};

//clearing
enum class ClearOption {
    OUTER_CLEAR,
    INNER_SEARCH
};

//memberIndices -- how go out today
std::string processExpeditionEvent(
    GameState& state, 
    ExpeditionEventType eventType,
    const std::vector<int>& memberIndices,
    int choice = -1
);

ExpeditionEventType selectRandomExpeditionEvent(GameState& state);
//return eventType
//flag hasNote/usedNoteEffect

std::string handleSupermarket(GameState& state, const std::vector<int>& memberIndices);
std::string handleWaterPlant(GameState& state, const std::vector<int>& memberIndices);
std::string handlePharmacy(GameState& state, const std::vector<int>& memberIndices);
std::string handleLaboratory(GameState& state, const std::vector<int>& memberIndices);
std::string handleHiddenStorage(GameState& state, const std::vector<int>& memberIndices);
//set flag usedNoteEffect

//has opions
std::string handleOtherCamp(GameState& state, const std::vector<int>& memberIndices, CampOption option);
std::string handlePerimeterClear(GameState& state, const std::vector<int>& memberIndices, ClearOption option)

#endif // EVENTSYSTEM_H
