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

    // Constructor
    GameState();

    // Status query helpers
    int countSurvivorsByStatus(SurvivorStatus status) const;
    int countHealthySurvivors() const;
    int countWeakSurvivors() const;
    int countMutatedSurvivors() const;
    int countLivingSurvivors() const;

    // Helper: does any LIVING survivor have the given trait?
    bool hasLivingSurvivorWithTrait(SurvivorTrait trait) const;

    // Daily reset
    void resetDailyStates();

    // Daily resource consumption requirements
    int calculateRequiredFood() const;
    int calculateRequiredWater() const;
};

#endif // GAMESTATE_H
