// Tools.h
// Utility functions shared across the project: random number helpers
// and save/load file operations.

#ifndef TOOLS_H
#define TOOLS_H

#include "GameState.h"
#include <vector>
#include <string>

// ---- Random / Probability helpers ----

// Function: initRandom
// What it does: Seeds the random number generator using the system clock.
//               Call once at the start of main().
// Input:  none.
// Output: none.
void initRandom();

// Function: checkProbability
// What it does: Returns true with the given probability (0.0 to 1.0).
//               For example 0.3 means a 30% chance of returning true.
// Input:  probability - a double between 0.0 and 1.0.
// Output: true if the event happens, false otherwise.
bool checkProbability(double probability);

// Function: selectRandomSurvivors
// What it does: Randomly picks survivors that match the requested status
//               filters. If fewer eligible survivors exist than count,
//               returns all of them.
// Input:  state - game state (to read the survivor list),
//         count - how many to pick,
//         includeHealthy / includeWeak / includeMutated - which statuses
//         are eligible for selection.
// Output: A vector of indices into state.survivors.
std::vector<int> selectRandomSurvivors(const GameState& state,
                                       int count,
                                       bool includeHealthy = true,
                                       bool includeWeak = false,
                                       bool includeMutated = false);

// ---- Save / Load (file persistence) ----

// The filename used for saved games, stored in the working directory.
extern const std::string SAVE_FILE_NAME;

// Function: hasSaveFile
// What it does: Checks whether a save file exists on disk.
// Input:  none.
// Output: true if the file is found, false otherwise.
bool hasSaveFile();

// Function: saveGame
// What it does: Writes the entire GameState to a text file so the player
//               can resume later. Called automatically at end of each day.
// Input:  state - the GameState to save (read-only).
// Output: true if the save succeeded, false on file error.
bool saveGame(const GameState& state);

// Function: loadGame
// What it does: Reads a saved GameState from disk into the given object.
//               Restores survivors, resources, items, flags, and day count.
// Input:  state - the GameState to populate (will be overwritten).
// Output: true if loading succeeded, false on error or bad format.
bool loadGame(GameState& state);

// Function: deleteSaveFile
// What it does: Removes the save file from disk. Called when a game ends
//               so the next run starts fresh.
// Input:  none.
// Output: none.
void deleteSaveFile();

#endif // TOOLS_H
