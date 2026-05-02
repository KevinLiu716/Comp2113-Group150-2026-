#include "GameState.h"
#include <string>

using namespace std;
// use funnction checkEndings at the end of every day in main.cpp

// check if game is end and set ending
void checkEndings(GameState& state) {
    if (state.gameEnded) {
        return;
    }
    
    int healthyCount = state.countSurvivorsByStatus(SurvivorStatus::HEALTHY);
    int weakCount = state.countSurvivorsByStatus(SurvivorStatus::WEAK);
    int mutatedCount = state.countSurvivorsByStatus(SurvivorStatus::MUTATED);
    int deceasedCount = state.countSurvivorsByStatus(SurvivorStatus::DECEASED);
    
    int livingCount = healthyCount + weakCount + mutatedCount;
    
    
    // ending1: all dead - check every day
    if (livingCount == 0) {
        state.gameEnded = true;
        state.endingMessage = 
            "Tragic End: The shelter falls into dead silence. Diaries are scattered on the table; "
            "the last page reads: 'We did our best.'";
        return;
    }
    
    // check other endings at the end of day 10
    if (state.currentDay < 10) {
        return;
    }
    
    int totalResources = state.food + state.water;
    
    // ending2: Order Restored
    if (healthyCount >= 4 && totalResources >= 10 && state.triggeredEvent6) {
        state.gameEnded = true;
        state.endingMessage = "Order Restored: Rescue arrives, and order is reestablished.";
        return;
    }
    
    // ending 3: 1 alive
    if (livingCount == 1 && totalResources >= 10) {
        state.gameEnded = true;
        state.endingMessage = 
            "Lone Survivor: Only you remain, guarding the ruins and your memories.";
        return;
    }
    
    // ending4: Marauders
    if (livingCount >= 2 && state.campRobberyCount >= 2) {
        state.gameEnded = true;
        state.endingMessage = 
            "Marauders: You have become the very people you once feared.";
        return;
    }
    
    // ending 5: mutated
    if (mutatedCount > 0 && mutatedCount * 2 > livingCount) {
        state.gameEnded = true;
        state.endingMessage = 
            "Symbiotic Evolution: The voice on the radio gradually becomes clear... "
            "but it's not speaking any human language.";
        return;
    }
    
    // ending 6: ending by default
    if (livingCount > 0) {
        state.gameEnded = true;
        state.endingMessage = 
            "Struggle for Survival: The door finally opens, but the world before you "
            "is unrecognizable. Survival is just another beginning.";
        return;
    }
}
