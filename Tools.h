// Tools.h
// Utility functions used across the game

#ifndef TOOLS_H
#define TOOLS_H

#include "GameState.h"
#include <vector>
#include <string>

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

// ===========================================================================
// Save / load (File I/O)
// ===========================================================================

// The fixed filename used for saved games.
extern const std::string SAVE_FILE_NAME;

// Function: hasSaveFile
// What it does: Check whether a save file exists in the current directory.
// Input:  None.
// Output: true if a save file is found, false otherwise.
bool hasSaveFile();

// Function: saveGame
// What it does: Write the entire GameState to disk so the player can resume
//               later. Format is a simple line-based text file: each line is
//               one field. Called automatically at the end of every day.
// Input:  state - the GameState to save (read-only).
// Output: true if the save succeeded, false on file error.
bool saveGame(const GameState& state);

// Function: loadGame
// What it does: Read a previously saved GameState from disk into the given
//               state object. Restores survivors, resources, items, flags,
//               current day, and ending status.
// Input:  state - the GameState to populate (will be overwritten).
// Output: true if the load succeeded, false on file error or bad format.
bool loadGame(GameState& state);

// Function: deleteSaveFile
// What it does: Remove the save file from disk. Called when a game ends so
//               the next run starts fresh.
// Input:  None.
// Output: None.
void deleteSaveFile();

#endif // TOOLS_H
