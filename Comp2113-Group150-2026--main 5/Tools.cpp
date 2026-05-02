// Tools.cpp
// Implementation of the utility functions declared in Tools.h.

#include "Tools.h"
#include "GameState.h"
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>

// The default filename for save data.
const std::string SAVE_FILE_NAME = "shelter_save.txt";

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

// ===========================================================================
// Save / load
//
// Save format is a simple line-based text file. Each piece of state is
// written on its own line. This format is human-readable so it is easy to
// debug and easy to demonstrate the File I/O coding requirement.
// ===========================================================================

// Function: hasSaveFile
// What it does: Check whether a save file exists in the current directory by
//               attempting to open it for reading.
// Input:  None.
// Output: true if the save file can be opened, false otherwise.
bool hasSaveFile() {
    std::ifstream f(SAVE_FILE_NAME);
    return f.good();
}

// Function: deleteSaveFile
// What it does: Remove the save file from disk. Used after a game finishes so
//               the next run starts fresh.
// Input:  None.
// Output: None.
void deleteSaveFile() {
    std::remove(SAVE_FILE_NAME.c_str());
}

// Function: saveGame
// What it does: Write the entire GameState to disk as a line-based text file.
//               One line per field, in a fixed order. Survivors are written
//               in a block: a count line followed by 4 lines per survivor.
// Input:  state - the GameState to save (read-only reference).
// Output: true on success, false on file error.
bool saveGame(const GameState& state) {
    std::ofstream out(SAVE_FILE_NAME);
    if (!out.is_open()) return false;

    // Header / version marker so a malformed file can be detected.
    out << "SHELTER_SAVE_V1\n";

    // Core scalar state.
    out << static_cast<int>(state.difficulty) << "\n";
    out << state.currentDay << "\n";
    out << state.food << "\n";
    out << state.water << "\n";
    out << state.medicine << "\n";

    // Special items.
    out << (state.hasRadio ? 1 : 0) << "\n";
    out << (state.hasNote ? 1 : 0) << "\n";
    out << (state.usedNoteEffect ? 1 : 0) << "\n";

    // Event flags.
    out << (state.triggeredEvent6 ? 1 : 0) << "\n";
    out << state.campRobberyCount << "\n";
    out << (state.forceEvent5NextDay ? 1 : 0) << "\n";

    // Survivors.
    out << state.survivors.size() << "\n";
    for (size_t i = 0; i < state.survivors.size(); i++) {
        out << state.survivors[i].name << "\n";
        out << static_cast<int>(state.survivors[i].status) << "\n";
        out << state.survivors[i].daysWeak << "\n";
        out << static_cast<int>(state.survivors[i].trait) << "\n";
    }

    out.close();
    return true;
}

// Function: loadGame
// What it does: Read a previously saved GameState from disk. The format must
//               match what saveGame() writes. On any failure (missing file,
//               bad header, parse error) the function returns false and
//               leaves `state` in an undefined partial state.
// Input:  state - the GameState to populate (will be overwritten).
// Output: true on success, false on any error.
bool loadGame(GameState& state) {
    std::ifstream in(SAVE_FILE_NAME);
    if (!in.is_open()) return false;

    std::string header;
    std::getline(in, header);
    if (header != "SHELTER_SAVE_V1") {
        return false;  // Wrong file or wrong version.
    }

    int diffInt;
    in >> diffInt;
    state.difficulty = static_cast<Difficulty>(diffInt);

    in >> state.currentDay;
    in >> state.food;
    in >> state.water;
    in >> state.medicine;

    int b;
    in >> b; state.hasRadio = (b != 0);
    in >> b; state.hasNote = (b != 0);
    in >> b; state.usedNoteEffect = (b != 0);
    in >> b; state.triggeredEvent6 = (b != 0);
    in >> state.campRobberyCount;
    in >> b; state.forceEvent5NextDay = (b != 0);

    int n;
    in >> n;
    in.ignore();  // Skip the newline after the count so getline works.

    state.survivors.clear();
    for (int i = 0; i < n; i++) {
        Survivor s;
        std::getline(in, s.name);
        int statusInt;
        in >> statusInt;
        s.status = static_cast<SurvivorStatus>(statusInt);
        in >> s.daysWeak;
        int traitInt;
        in >> traitInt;
        s.trait = static_cast<SurvivorTrait>(traitInt);
        in.ignore();  // Eat the trailing newline before the next name.
        state.survivors.push_back(s);
    }

    // Reset transient daily state — these are not saved and should always
    // start cleanly when resuming.
    state.expeditionMemberIds.clear();
    state.wasTreatedToday = false;
    state.gameEnded = false;
    state.endingMessage = "";
    state.endingType = EndingType::NONE;

    in.close();
    return !in.bad();
}
