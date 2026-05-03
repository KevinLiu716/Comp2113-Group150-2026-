// GameState.h
// Defines the global game state data structure and related enums.
// Holds everything the game needs: survivors, resources, items, flags.

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <vector>
#include <string>

// Difficulty levels chosen at game start.
enum class Difficulty { EASY, HARD };

// Possible status of an individual survivor.
enum class SurvivorStatus { HEALTHY, WEAK, MUTATED, DECEASED };

// Personality / role traits for survivors. Each trait gives one passive
// effect that influences certain events. Three are currently active in
// the game logic; the others are placeholders for future expansion.
enum class SurvivorTrait {
    NONE,
    DOCTOR,    // Treatment does not consume medicine when this survivor is alive
    FRAIL,     // +20% chance to become weak from supply shortage
    SCOUT,     // +1 to expedition resource gain when participating
    ENGINEER,  // (placeholder) reduces extra costs on expeditions
    SOLDIER,   // (placeholder) protects from random death on robbery
    LUCKY      // (placeholder) reduces mutation chance from dreams
};

// One of the six possible endings. Set at game over by determineEnding(),
// and used by the UI to show a matching banner and color.
enum class EndingType {
    NONE,                  // Game still in progress
    TRAGIC_END,            // Everyone died
    ORDER_RESTORED,        // Rescue arrived (best ending)
    LONE_SURVIVOR,         // Only one person survived
    SYMBIOTIC_EVOLUTION,   // Mutated outnumber the living
    MARAUDERS,             // Survived by robbing other camps
    STRUGGLE_FOR_SURVIVAL  // Default survival ending
};

// A single survivor in the shelter.
struct Survivor {
    SurvivorStatus status;   // Current health status
    int daysWeak;            // Days spent in the WEAK state
    std::string name;        // Display name (e.g. "Alice")
    SurvivorTrait trait;     // Passive ability

    Survivor()
        : status(SurvivorStatus::HEALTHY), daysWeak(0),
          name(""), trait(SurvivorTrait::NONE) {}

    Survivor(SurvivorStatus s)
        : status(s), daysWeak(0),
          name(""), trait(SurvivorTrait::NONE) {}
};

// Holds the entire game state. Passed by reference to almost every function.
class GameState {
public:
    // Core state
    Difficulty difficulty;
    int currentDay;

    // The 6 shelter survivors
    std::vector<Survivor> survivors;

    // Main resources
    int food;
    int water;
    int medicine;

    // Special items / flags
    bool hasRadio;
    bool hasNote;
    bool usedNoteEffect;

    // Temporary daily state (reset each day)
    std::vector<int> expeditionMemberIds;
    bool wasTreatedToday;

    // Event and ending tracking flags
    bool triggeredEvent6;
    int campRobberyCount;
    bool forceEvent5NextDay;

    // Ending state
    bool gameEnded;
    std::string endingMessage;
    EndingType endingType;

    // Function: GameState (constructor)
    // What it does: Initialize all fields to safe defaults. Creates 6 survivors and assigns traits.
    // Input:  none.
    // Output: A fully initialized GameState object ready for day 1.
    GameState();

    // Function: countSurvivorsByStatus
    // What it does: Loops through the survivor list and counts how many currently have the given status.
    // Input:  status - the SurvivorStatus to look for (HEALTHY, WEAK, MUTATED, or DECEASED).
    // Output: Returns an integer, the number of survivors with that status.
    int countSurvivorsByStatus(SurvivorStatus status) const;

    // Function: countHealthySurvivors
    // What it does: Counts how many survivors are currently HEALTHY.
    // Input:  none.
    // Output: Returns the number of healthy survivors.
    int countHealthySurvivors() const;

    // Function: countWeakSurvivors
    // What it does: Counts how many survivors are currently WEAK.
    // Input:  none.
    // Output: Returns the number of weak survivors.
    int countWeakSurvivors() const;

    // Function: countMutatedSurvivors
    // What it does: Counts how many survivors are currently MUTATED.
    // Input:  none.
    // Output: Returns the number of mutated survivors.
    int countMutatedSurvivors() const;

    // Function: countLivingSurvivors
    // What it does: Counts everyone who is not DECEASED (HEALTHY + WEAK + MUTATED).
    // Input:  none.
    // Output: Returns the total number of living survivors.
    int countLivingSurvivors() const;

    // Function: hasLivingSurvivorWithTrait
    // What it does: Returns true if any non-deceased, non-mutated survivor has the given trait.
    // Input:  trait - the SurvivorTrait to search for.
    // Output: Returns true if at least one qualifying survivor has it, false otherwise.
    bool hasLivingSurvivorWithTrait(SurvivorTrait trait) const;

    // Function: resetDailyStates
    // What it does: Clears the expedition member list from the previous day and resets the treatment flag.
    // Input:  none.
    // Output: Modifies the GameState object internally.
    void resetDailyStates();

    // Function: calculateRequiredFood
    // What it does: Calculates how much food is needed for one day (1 per HEALTHY or WEAK survivor).
    // Input:  none.
    // Output: Returns the total food needed for today.
    int calculateRequiredFood() const;

    // Function: calculateRequiredWater
    // What it does: Calculates how much water is needed for one day (1 per HEALTHY or WEAK survivor).
    // Input:  none.
    // Output: Returns the total water needed for today.
    int calculateRequiredWater() const;
};

#endif // GAMESTATE_H
