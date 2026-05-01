// Tools.cpp
// Implementation of the utility functions declared in Tools.h.

#include "Tools.h"
#include "GameState.h"
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>

// Function: initRandom
// What it does: Initializes the random number generator using the current time
//               as a seed. This should be called once at the start of main()
//               to make sure rand() produces different sequences each run.
// Input:  None.
// Output: None.
void initRandom() {
    srand(static_cast<unsigned int>(time(nullptr)));
}

// Function: checkProbability
// What it does: Decides whether a random event with a given probability happens.
//               It draws a random number in [0, 1) and returns true if that
//               number is less than the supplied probability.
// Input:  probability - a value between 0.0 and 1.0.
// Output: true if the event triggers, false otherwise.
bool checkProbability(double probability) {
    if (probability <= 0.0) return false;
    if (probability >= 1.0) return true;
    double roll = static_cast<double>(rand()) / RAND_MAX;
    return roll < probability;
}

// Function: selectRandomSurvivors
// What it does: Randomly picks survivors whose status matches the requested
//               filters. Deceased survivors are never selected. If fewer
//               eligible survivors exist than requested, all are returned.
// Input:
//   state           - the game state (used to read the survivor list).
//   count           - the number of survivors to pick.
//   includeHealthy  - whether HEALTHY survivors are eligible.
//   includeWeak     - whether WEAK survivors are eligible.
//   includeMutated  - whether MUTATED survivors are eligible.
// Output: A vector of indices into state.survivors.
std::vector<int> selectRandomSurvivors(const GameState& state,
                                       int count,
                                       bool includeHealthy,
                                       bool includeWeak,
                                       bool includeMutated) {
    // Step 1: collect every survivor that matches the filters.
    std::vector<int> candidates;
    for (int i = 0; i < (int)state.survivors.size(); i++) {
        SurvivorStatus s = state.survivors[i].status;
        if (includeHealthy && s == SurvivorStatus::HEALTHY) {
            candidates.push_back(i);
        } else if (includeWeak && s == SurvivorStatus::WEAK) {
            candidates.push_back(i);
        } else if (includeMutated && s == SurvivorStatus::MUTATED) {
            candidates.push_back(i);
        }
    }

    // Step 2: if we have fewer (or equal) candidates than requested, return all.
    if ((int)candidates.size() <= count) {
        return candidates;
    }

    // Step 3: shuffle with Fisher-Yates and keep the first `count` entries.
    for (int i = (int)candidates.size() - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        std::swap(candidates[i], candidates[j]);
    }
    candidates.resize(count);
    return candidates;
}
