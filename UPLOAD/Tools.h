// Tools.h
// Random number helpers and save/load functions.

#ifndef TOOLS_H
#define TOOLS_H

#include "GameState.h"
#include <vector>
#include <string>

// Seed the random number generator. Call once at the start of main().
void initRandom();

// Returns true with the given probability (0.0 - 1.0).
// e.g. checkProbability(0.3) returns true about 30% of the time.
bool checkProbability(double probability);

// Pick `count` random survivors that match the given status filters.
// Returns indices into state.survivors. If there aren't enough eligible
// survivors, returns whatever is available.
std::vector<int> selectRandomSurvivors(const GameState& state,
                                       int count,
                                       bool includeHealthy = true,
                                       bool includeWeak = false,
                                       bool includeMutated = false);

// Filename used for saved games (in the current directory).
extern const std::string SAVE_FILE_NAME;

// Returns true if a save file exists.
bool hasSaveFile();

// Save the game state to disk. Each field is written on its own line.
// Returns true on success.
bool saveGame(const GameState& state);

// Load a saved game from disk into `state`. Returns true on success.
bool loadGame(GameState& state);

// Delete the save file.
void deleteSaveFile();

// Read one line of input from the terminal with our own line editing
// (so Backspace and DEL always work, regardless of stty settings).
// Returns true on success, false on EOF.
bool readLineWithEditing(std::string& out);

#endif
