/**
 * Project: Shelter: 10 Days (COMP2113 Group 150)
 * File: Tools.h
 * -------------------------------------------------------------------------
 * Description: 
 * Global utility toolkit containing the Random Number Generation (RNG) 
 * engine and the File I/O Persistence Layer.
 */

#ifndef TOOLS_H  // Header Guard: Prevents recursive inclusion errors
#define TOOLS_H

#include "GameState.h"
#include <vector>
#include <string>

// ===========================================================================
// SECTION: Stochastic Engine (Randomness & Probability)
// ===========================================================================

// Function: initRandom
// What it does: Initialize the random seed. Call once at the start of main().
// Technical Note: Seeds the Pseudo-Random Number Generator (PRNG) using the 
//                 system clock to ensure entropy across different sessions.
void initRandom();

// Function: checkProbability
// What it does: Decide whether a random event with a given probability happens.
// Input:  probability - a value between 0.0 and 1.0 (e.g. 0.3 means 30% chance).
// Output: true if the event happens, false otherwise.
// Implementation: Performs a Bernoulli trial using floating-point comparison.
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
// Complexity: O(N) where N is the total number of survivors.[cite: 3]
std::vector<int> selectRandomSurvivors(const GameState& state,
                                       int count,
                                       bool includeHealthy = true,
                                       bool includeWeak = false,
                                       bool includeMutated = false);

// ===========================================================================
// SECTION: Persistence Layer (Serialization / File I/O)
// ===========================================================================

// The fixed filename used for saved games. 
// Note: Stored in the local execution directory.[cite: 3]
extern const std::string SAVE_FILE_NAME;

// Function: hasSaveFile
// What it does: Check whether a save file exists in the current directory.
// Input:  None.
// Output: true if a save file is found, false otherwise.
// Method: Utilizes ifstream to attempt a secure file handle open.[cite: 3]
bool hasSaveFile();

// Function: saveGame
// What it does: Write the entire GameState to disk so the player can resume
//               later. Format is a simple line-based text file: each line is
//               one field. Called automatically at the end of every day.
// Input:  state - the GameState to save (read-only).
// Output: true if the save succeeded, false on file error.
// Technical: Serializes the 'GameState' object into a plaintext stream.[cite: 3]
bool saveGame(const GameState& state);

// Function: loadGame
// What it does: Read a previously saved GameState from disk into the given
//               state object. Restores survivors, resources, items, flags,
//               current day, and ending status.
// Input:  state - the GameState to populate (will be overwritten).
// Output: true if the load succeeded, false on file error or bad format.
// Technical: Deserializes and restores the application state machine.[cite: 3]
bool loadGame(GameState& state);

// Function: deleteSaveFile
// What it does: Remove the save file from disk. Called when a game ends so
//               the next run starts fresh.
// Input:  None.
// Output: None.
// Risk Management: Used to reset the 'state' after a game-over or completion.[cite: 3]
void deleteSaveFile();

#endif // TOOLS_H
