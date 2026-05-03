// player.h
// This header file declares the resource management functions.
// These functions are used by the main game loop to initialize resources,
// consume resources daily, treat weak survivors, and update sick counters.

#ifndef PLAYER_H
#define PLAYER_H

#include "GameState.h"

// Function: initResources
// What it does: This function sets up the initial game state based on the
//               difficulty level and the supply plan that the player chose.
//               It creates 6 survivors and assigns the starting food, water,
//               and medicine values according to the chosen plan.
// Input: state - a reference to the GameState object that will be initialized.
//        diff  - the difficulty level, either Difficulty::EASY or Difficulty::HARD.
//        plan  - the supply plan number, 1 for plan A or 2 for plan B.
// Output: This function does not return a value. It modifies the state directly.
void initResources(GameState& state, Difficulty diff, int plan);

// Function: consumeDaily
// What it does: This function handles the daily resource consumption.
//               Each healthy and weak survivor consumes 1 food and 1 water.
//               Mutated survivors do not consume anything.
//               If there is not enough food or water, the function will
//               determine which healthy survivors become weak based on
//               the current difficulty mode.
// Input: state - a reference to the GameState object.
// Output: This function does not return a value. It modifies the states directly.
void consumeDaily(GameState& state);

// Function: treat
// What it does: This function uses 1 medicine to cure all weak survivors.
//               All survivors whose status is WEAK will become HEALTHY,
//               and their daysWeak counter will be reset to 0.
//               If there is no medicine left, this function does nothing.
// Input: state - a reference to the GameState object.
// Output: This function does not return a value. It modifies the state directly.
void treat(GameState& state);

// Function: updateSickCounters
// What it does: This function is called at the end of each day.
//               It increases the daysWeak counter by 1 for every weak survivor.
//               If any weak survivor's daysWeak counter reaches 2, that
//               survivor dies and their status becomes DECEASED.
// Input: state - a reference to the GameState object.
// Output: This function does not return a value. It modifies the state directly.
void updateSickCounters(GameState& state);

#endif // PLAYER_H
