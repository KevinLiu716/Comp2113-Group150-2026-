/**
 * @file EventSystem.h
 * @author WU Bozhou (UID: 3036588955) - Game design & Event System
 * @brief Declarations for the daily (night) and expedition event systems.
 * 
 * This header defines the core enumeration types and function signatures 
 * required for processing random events, handling state flags (e.g., forceEvent5NextDay),
 * and managing survivor expeditions.
 */

#ifndef EVENTSYSTEM_H
#define EVENTSYSTEM_H

#include "Tools.h"
#include "GameState.h"
#include <string>
#include <vector>

/**
 * @enum DailyEventType
 * @brief Categorization of nocturnal random events affecting the shelter.
 */
enum class DailyEventType {
    RADIATION_RAIN,    
    INTERNAL_CONFLICT, 
    MYSTERIOUS_DREAM,
    SPOILED_SUPPLIES,
    UNEXPECTED_VISITOR,
    ANOMALOUS_SIGNAL   
};

/**
 * @brief Main dispatcher for daily night events.
 */
std::string processDailyEvent(GameState& state, DailyEventType eventType);

/**
 * @brief Randomly selects the next daily event.
 * @note Considers the state.forceEvent5NextDay flag. If true, forces UNEXPECTED_VISITOR.
 */
DailyEventType selectRandomDailyEvent(GameState& state);

std::string handleRadiationRain(GameState& state);

/**
 * @brief Handles internal conflicts among survivors.
 * @note May set flag state.forceEvent5NextDay to true if no radio is present.
 */
std::string handleInternalConflict(GameState& state);

std::string handleMysteriousDream(GameState& state);

std::string handleSpoiledSupplies(GameState& state);

std::string handleUnexpectedVisitor(GameState& state, bool openTheDoor);  

/**
 * @brief Handles the anomalous radio signal event.
 * @note May set flag state.forceEvent5NextDay to true if unresolved.
 */
std::string handleAnomalousSignal(GameState& state);


/**
 * @enum ExpeditionEventType
 * @brief Categorization of external locations available for daytime expeditions.
 */
enum class ExpeditionEventType {
    SUPERMARKET,
    WATER_PLANT,
    PHARMACY,
    OTHER_CAMP,
    PERIMETER_CLEAR,
    LABORATORY,
    HIDDEN_STORAGE
};

/**
 * @enum CampOption
 * @brief Interaction choices when encountering another survivor camp.
 */
enum class CampOption {
    REQUEST_HELP,
    ROB
};

/**
 * @enum ClearOption
 * @brief Strategic choices during perimeter clearing operations.
 */
enum class ClearOption {
    OUTER_CLEAR,
    INNER_SEARCH
};

/**
 * @brief Main dispatcher for daytime expedition events.
 * @param state The global game state.
 * @param eventType The selected location/event type.
 * @param memberIndices The indices of survivors participating in the expedition.
 * @param choice Optional parameter for specific event branches (defaults to -1).
 */
std::string processExpeditionEvent(
    GameState& state, 
    ExpeditionEventType eventType,
    const std::vector<int>& memberIndices,
    int choice = -1
);

/**
 * @brief Randomly selects a daytime expedition destination/event.
 * @return ExpeditionEventType Evaluates state flags like hasNote/usedNoteEffect.
 */
ExpeditionEventType selectRandomExpeditionEvent(GameState& state);

// Standard Expedition Handlers
std::string handleSupermarket(GameState& state, const std::vector<int>& memberIndices);
std::string handleWaterPlant(GameState& state, const std::vector<int>& memberIndices);
std::string handlePharmacy(GameState& state, const std::vector<int>& memberIndices);
std::string handleLaboratory(GameState& state, const std::vector<int>& memberIndices);

/**
 * @brief Handles the hidden storage event.
 * @note Sets the usedNoteEffect flag upon completion.
 */
std::string handleHiddenStorage(GameState& state, const std::vector<int>& memberIndices);

// Branching Expedition Handlers (Requires player options)
std::string handleOtherCamp(GameState& state, const std::vector<int>& memberIndices, CampOption option);
std::string handlePerimeterClear(GameState& state, const std::vector<int>& memberIndices, ClearOption option);

#endif // EVENTSYSTEM_H
