// EndingSystem.cpp
// Decides which of the 6 endings the player got, based on the final
// GameState.

#include "GameState.h"
#include <string>

using namespace std;

// Function: countByStatus
// What it does: Counts how many survivors currently have the given status.
//               Local helper used only inside this file.
// Input:  state - the game state, status - the SurvivorStatus to count.
// Output: Returns the number of survivors matching that status.
static int countByStatus(const GameState& state, SurvivorStatus status) {
    int count = 0;
    for (const auto& survivor : state.survivors) {
        if (survivor.status == status) {
            count++;
        }
    }
    return count;
}

// Function: checkEndings
// What it does: Checks whether the game should end. Called at the end of
//               every day in main.cpp. If all survivors are dead, triggers
//               immediately. Otherwise waits until Day 10 to evaluate the
//               remaining 5 endings in priority order.
// Input:  state - the game state (survivors, resources, flags).
// Output: No return value. Sets state.gameEnded and state.endingMessage
//         if an ending condition is met.
void checkEndings(GameState& state) {
    if (state.gameEnded) {
        return;
    }
    
    int healthyCount = countByStatus(state, SurvivorStatus::HEALTHY);
    int weakCount = countByStatus(state, SurvivorStatus::WEAK);
    int mutatedCount = countByStatus(state, SurvivorStatus::MUTATED);
    int deceasedCount = countByStatus(state, SurvivorStatus::DECEASED);
    
    int livingCount = healthyCount + weakCount + mutatedCount;
    
    // Ending 1: Tragic End - everyone is dead, checked every day
    if (livingCount == 0) {
        state.gameEnded = true;
        state.endingMessage = 
            "Tragic End: The shelter falls into dead silence. Diaries are scattered on the table; "
            "the last page reads: 'We did our best.'";
        return;
    }
    
    // The rest only trigger on Day 10
    if (state.currentDay < 10) {
        return;
    }
    
    int totalResources = state.food + state.water;
    
    // Ending 2: Order Restored - best ending, needs 4+ healthy, 10+ resources, and Event 6
    if (healthyCount >= 4 && totalResources >= 10 && state.triggeredEvent6) {
        state.gameEnded = true;
        state.endingMessage = "Order Restored: Rescue arrives, and order is reestablished.";
        return;
    }
    
    // Ending 3: Lone Survivor - only one person left but well-supplied
    if (livingCount == 1 && totalResources >= 10) {
        state.gameEnded = true;
        state.endingMessage = 
            "Lone Survivor: Only you remain, guarding the ruins and your memories.";
        return;
    }
    
    // Ending 4: Marauders - robbed other camps too many times
    if (livingCount >= 2 && state.campRobberyCount >= 2) {
        state.gameEnded = true;
        state.endingMessage = 
            "Marauders: You have become the very people you once feared.";
        return;
    }
    
    // Ending 5: Symbiotic Evolution - mutated outnumber the rest
    if (mutatedCount > 0 && mutatedCount * 2 > livingCount) {
        state.gameEnded = true;
        state.endingMessage = 
            "Symbiotic Evolution: The voice on the radio gradually becomes clear... "
            "but it's not speaking any human language.";
        return;
    }
    
    // Ending 6: Struggle for Survival - default if nothing else matched
    if (livingCount > 0) {
        state.gameEnded = true;
        state.endingMessage = 
            "Struggle for Survival: The door finally opens, but the world before you "
            "is unrecognizable. Survival is just another beginning.";
        return;
    }
}
