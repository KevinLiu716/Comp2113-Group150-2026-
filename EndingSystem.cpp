/**
 * @file EndingSystem.cpp
 * @author Jia Chengru (UID: 3036432613) - Game Logic, UI & Save/Load
 * @brief Logic for determining the game's ending based on final GameState.
 * 
 * This module implements the "Multiple Endings" feature. It evaluates 
 * survivor status, remaining resources, and specific event flags (e.g., Event 6)
 * to branch into one of 6 distinct narrative endings.
 */

#include "GameState.h"
#include <string>

using namespace std;

/**
 * @brief Evaluates current game state to determine if an ending condition is met.
 * 
 * Note: This function should be called at the end of every day in main.cpp.
 * It checks for premature failure (Day 1-10) or victory/special conditions (Day 10).
 * 
 * @param state The global game state containing resources and survivor roster.
 */
void checkEndings(GameState& state) {
    if (state.gameEnded) {
        return;
    }
    
    // Aggregate survivor counts by health status for ending evaluation
    int healthyCount = state.countSurvivorsByStatus(SurvivorStatus::HEALTHY);
    int weakCount = state.countSurvivorsByStatus(SurvivorStatus::WEAK);
    int mutatedCount = state.countSurvivorsByStatus(SurvivorStatus::MUTATED);
    int deceasedCount = state.countSurvivorsByStatus(SurvivorStatus::DECEASED);
    
    int livingCount = healthyCount + weakCount + mutatedCount;
    
    
    // Ending 1: Tragic End (All Dead)
    // Priority: Highest. Checked every day. Triggers immediately if living count drops to 0.
    if (livingCount == 0) {
        state.gameEnded = true;
        state.endingMessage = 
            "Tragic End: The shelter falls into dead silence. Diaries are scattered on the table; "
            "the last page reads: 'We did our best.'";
        return;
    }
    
    // Standard survival endings are only evaluated at the conclusion of Day 10
    if (state.currentDay < 10) {
        return;
    }
    
    // Calculate total critical resources for final evaluation
    int totalResources = state.food + state.water;
    
    // Ending 2: Order Restored (Best Ending)
    // Requires: High survival rate, sufficient resources, and decoding the Anomalous Signal.
    if (healthyCount >= 4 && totalResources >= 10 && state.triggeredEvent6) {
        state.gameEnded = true;
        state.endingMessage = "Order Restored: Rescue arrives, and order is reestablished.";
        return;
    }
    
    // Ending 3: Lone Survivor
    // Requires: Only one survivor remains but the shelter has abundant resources.
    if (livingCount == 1 && totalResources >= 10) {
        state.gameEnded = true;
        state.endingMessage = 
            "Lone Survivor: Only you remain, guarding the ruins and your memories.";
        return;
    }
    
    // Ending 4: Marauders (Moral Decay)
    // Requires: High robbery frequency from player expedition choices.
    if (livingCount >= 2 && state.campRobberyCount >= 2) {
        state.gameEnded = true;
        state.endingMessage = 
            "Marauders: You have become the very people you once feared.";
        return;
    }
    
    // Ending 5: Symbiotic Evolution (Mutation Ending)
    // Requires: Mutated survivors outnumber regular survivors.
    if (mutatedCount > 0 && mutatedCount * 2 > livingCount) {
        state.gameEnded = true;
        state.endingMessage = 
            "Symbiotic Evolution: The voice on the radio gradually becomes clear... "
            "but it's not speaking any human language.";
        return;
    }
    
    // Ending 6: Struggle for Survival (Default Ending)
    // Triggers if Day 10 is reached but no special threshold conditions are met.
    if (livingCount > 0) {
        state.gameEnded = true;
        state.endingMessage = 
            "Struggle for Survival: The door finally opens, but the world before you "
            "is unrecognizable. Survival is just another beginning.";
        return;
    }
}
