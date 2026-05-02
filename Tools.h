// Tools.h
// Utility functions used across the game

#ifndef TOOLS_H
#define TOOLS_H

#include "GameState.h"
#include <vector>

// Function: initRandom
// What it does: Initialize the random seed. Call once at the start of main().
// Input:  None.
// Output: None.
void initRandom();

// Function: checkProbability
// What it does: Decide whether a random event with a given probability happens.
// Input:  probability - a value between 0.0 and 1.0 (e.g. 0.3 means 30% chance).
// Output: true if the event happens, false otherwise.
bool checkProbability(double probability);

// Function: selectRandomSurvivors
// What it does: Randomly pick survivors that match the requested status filters.
// Input:
//   state           - the game state (used to read the survivor list).
//   count           - how many survivors to pick.
//   includeHealthy  - whether to consider HEALTHY survivors.
//   includeWeak     - whether to consider WEAK survivors.
//   includeMutated  - whether to consider MUTATED survivors.
// Output: A vector of indices into state.survivors. If fewer eligible survivors
//         exist than requested, all of them are returned.
std::vector<int> selectRandomSurvivors(const GameState& state,
                                       int count,
                                       bool includeHealthy = true,
                                       bool includeWeak = false,
                                       bool includeMutated = false);

#endif // TOOLS_H
