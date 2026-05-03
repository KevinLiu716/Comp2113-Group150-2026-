// ResourceManager.cpp
// Implements initResources, consumeDaily, treat, and updateSickCounters.

#include "player.h"
#include "Tools.h"

// Function: initResources
// What it does: Sets up the starting resources and survivors based on the
//               difficulty and the supply plan chosen by the player.
//               4 possible combos:
//               Easy+1: 18/18/1, Easy+2: 8/8/2,
//               Hard+1: 18/18/0 (one survivor starts weak), Hard+2: 6/6/2.
//               Also assigns names and shuffled traits to all 6 survivors.
// Input:  state - the GameState to initialize,
//         diff - EASY or HARD, plan - 1 or 2.
// Output: No return value. Fills in the state object.
void initResources(GameState& state, Difficulty diff, int plan) {
    state.difficulty = diff;
    state.currentDay = 1;

    // Create 6 survivors with fixed names and randomly shuffled traits.
    state.survivors.clear();
    const char* names[6] = {"Alice", "Bob", "Carol", "David", "Eve", "Frank"};
    SurvivorTrait traitPool[6] = {
        SurvivorTrait::DOCTOR,
        SurvivorTrait::FRAIL,
        SurvivorTrait::SCOUT,
        SurvivorTrait::ENGINEER,
        SurvivorTrait::SOLDIER,
        SurvivorTrait::LUCKY
    };
    
    // Fisher-Yates shuffle so each game gives a different trait assignment.
    for (int i = 5; i > 0; i--) {
        int j = rand() % (i + 1);
        SurvivorTrait tmp = traitPool[i];
        traitPool[i] = traitPool[j];
        traitPool[j] = tmp;
    }

    for (int i = 0; i < 6; i++) {
        Survivor newSurvivor;
        newSurvivor.name = names[i];
        newSurvivor.trait = traitPool[i];
        state.survivors.push_back(newSurvivor);
    }

    // Reset all flags to a clean slate.
    state.hasRadio = false;
    state.hasNote = false;
    state.usedNoteEffect = false;
    state.wasTreatedToday = false;
    state.expeditionMemberIds.clear();
    state.triggeredEvent6 = false;
    state.campRobberyCount = 0;
    state.forceEvent5NextDay = false;
    state.gameEnded = false;
    state.endingMessage = "";

    // Set starting supplies based on difficulty + plan.
    if (diff == Difficulty::EASY && plan == 1) {
        state.food = 18;
        state.water = 18;
        state.medicine = 1;
    }
    if (diff == Difficulty::EASY && plan == 2) {
        state.food = 8;
        state.water = 8;
        state.medicine = 2;
    }
    if (diff == Difficulty::HARD && plan == 1) {
        state.food = 18;
        state.water = 18;
        state.medicine = 0;
        // Hard plan A: one person starts out weak.
        state.survivors[0].status = SurvivorStatus::WEAK;
        state.survivors[0].daysWeak = 0;
    }
    if (diff == Difficulty::HARD && plan == 2) {
        state.food = 6;
        state.water = 6;
        state.medicine = 2;
    }
}

// Function: consumeDaily
// What it does: Handles daily food and water consumption. Subtracts what
//               the survivors need. If there is a shortage, some healthy
//               survivors may become weak:
//               Easy mode - pick k random healthy people (k = bigger shortage),
//                           each has 50% chance (70% if FRAIL) to get sick.
//               Hard mode - all healthy people face 30% + (shortage/15),
//                           clamped to [50%, 100%], +20% extra if FRAIL.
// Input:  state - the game state.
// Output: No return value. Modifies resources and survivor statuses.
void consumeDaily(GameState& state) {
    int requiredFood = state.calculateRequiredFood();
    int requiredWater = state.calculateRequiredWater();

    // Figure out how much we are short.
    int foodShortage = requiredFood - state.food;
    int waterShortage = requiredWater - state.water;
    if (foodShortage < 0) foodShortage = 0;
    if (waterShortage < 0) waterShortage = 0;

    // Subtract resources, floor at 0.
    state.food = state.food - requiredFood;
    if (state.food < 0) state.food = 0;
    state.water = state.water - requiredWater;
    if (state.water < 0) state.water = 0;

    // If we had enough for everyone, no one gets sick.
    if (foodShortage == 0 && waterShortage == 0) {
        return;
    }

    // --- Easy mode shortage ---
    if (state.difficulty == Difficulty::EASY) {
        // Only the larger of the two shortages matters.
        int k = foodShortage;
        if (waterShortage > k) {
            k = waterShortage;
        }

        // Pick k random healthy survivors as candidates.
        std::vector<int> candidates = selectRandomSurvivors(state, k, true, false, false);

        for (int i = 0; i < (int)candidates.size(); i++) {
            int survivorIndex = candidates[i];
            double weakChance = 0.5;
            // FRAIL trait: higher chance to get sick.
            if (state.survivors[survivorIndex].trait == SurvivorTrait::FRAIL) {
                weakChance = 0.7;
            }
            bool becomesWeak = checkProbability(weakChance);
            if (becomesWeak == true) {
                state.survivors[survivorIndex].status = SurvivorStatus::WEAK;
                state.survivors[survivorIndex].daysWeak = 0;
            }
        }
    }

    // --- Hard mode shortage ---
    if (state.difficulty == Difficulty::HARD) {
        int totalShortage = foodShortage + waterShortage;
        double probability = 0.3 + (double)totalShortage / 15.0;

        // Clamp into [0.5, 1.0] so a shortage is always serious.
        if (probability < 0.5) probability = 0.5;
        if (probability > 1.0) probability = 1.0;

        // Roll once for each healthy survivor.
        for (int i = 0; i < (int)state.survivors.size(); i++) {
            if (state.survivors[i].status == SurvivorStatus::HEALTHY) {
                double thisChance = probability;
                // FRAIL trait: +20% on top of the base probability.
                if (state.survivors[i].trait == SurvivorTrait::FRAIL) {
                    thisChance += 0.2;
                    if (thisChance > 1.0) thisChance = 1.0;
                }
                bool becomesWeak = checkProbability(thisChance);
                if (becomesWeak == true) {
                    state.survivors[i].status = SurvivorStatus::WEAK;
                    state.survivors[i].daysWeak = 0;
                }
            }
        }
    }
}

// Function: treat
// What it does: Uses 1 medicine to heal ALL weak survivors back to healthy.
//               If a living survivor has the DOCTOR trait, treatment is free
//               (no medicine spent). Does nothing if no medicine and no doctor.
// Input:  state - the game state.
// Output: No return value. Modifies survivor statuses and medicine count.
void treat(GameState& state) {
    bool hasDoctor = state.hasLivingSurvivorWithTrait(SurvivorTrait::DOCTOR);

    // Need either medicine or a doctor to treat anyone.
    if (state.medicine <= 0 && !hasDoctor) {
        return;
    }

    // Doctor treats for free, otherwise spend 1 medicine.
    if (!hasDoctor) {
        state.medicine = state.medicine - 1;
    }

    // Heal every weak survivor.
    for (int i = 0; i < (int)state.survivors.size(); i++) {
        if (state.survivors[i].status == SurvivorStatus::WEAK) {
            state.survivors[i].status = SurvivorStatus::HEALTHY;
            state.survivors[i].daysWeak = 0;
        }
    }

    state.wasTreatedToday = true;
}

// Function: updateSickCounters
// What it does: End-of-day check. Every weak survivor's daysWeak goes up by 1.
//               If it reaches 2, the survivor dies. Timeline:
//               Become weak (daysWeak=0) -> end of day (daysWeak=1) ->
//               end of next day (daysWeak=2) -> DECEASED.
// Input:  state - the game state.
// Output: No return value. May change survivors from WEAK to DECEASED.
void updateSickCounters(GameState& state) {
    for (int i = 0; i < (int)state.survivors.size(); i++) {
        if (state.survivors[i].status == SurvivorStatus::WEAK) {
            state.survivors[i].daysWeak = state.survivors[i].daysWeak + 1;

            // 2 days without treatment = death.
            if (state.survivors[i].daysWeak >= 2) {
                state.survivors[i].status = SurvivorStatus::DECEASED;
                state.survivors[i].daysWeak = 0;
            }
        }
    }
}
