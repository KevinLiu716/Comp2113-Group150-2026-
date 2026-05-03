/**
 * Project: Shelter: 10 Days (COMP2113 Group 150)
 * File: ResourceManager.cpp
 * -------------------------------------------------------------------------
 * Description: 
 * Implements the core survival mechanics including resource initialization, 
 * depletion logic with difficulty scaling, and medical treatment systems.
 */

#include "player.h"
#include "Tools.h"

// Function: initResources
// What it does: This function sets up the starting resources and survivors
//               based on the difficulty and the supply plan chosen by the player.
//               There are 4 possible combinations:
//               Easy + Plan 1: Food 18, Water 18, Medicine 1
//               Easy + Plan 2: Food 8,  Water 8,  Medicine 2
//               Hard + Plan 1: Food 18, Water 18, Medicine 0, one survivor starts Weak
//               Hard + Plan 2: Food 6,  Water 6,  Medicine 2
// Input: state - a reference to the GameState object that will be initialized.
//        diff  - the difficulty level (Difficulty::EASY or Difficulty::HARD).
//        plan  - the supply plan number (1 for plan A, 2 for plan B).
// Output: This function does not return a value. It modifies the state directly.
void initResources(GameState& state, Difficulty diff, int plan) {
    // Set the difficulty
    state.difficulty = diff;

    // Set the current day to day 1
    state.currentDay = 1;

    // Clear the survivors list and create 6 new healthy survivors,
    // each with a fixed name and a randomly assigned trait.
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
    
    // --- Algorithm: Fisher-Yates Shuffle ---
    // Provides O(n) unbiased permutations of the trait pool to ensure
    // every game session has a unique team composition.
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

    // Reset all special item flags to false (Clean slate for new session)
    state.hasRadio = false;
    state.hasNote = false;
    state.usedNoteEffect = false;

    // Reset daily temporary states
    state.wasTreatedToday = false;
    state.expeditionMemberIds.clear();

    // Reset event and ending flags
    state.triggeredEvent6 = false;
    state.campRobberyCount = 0;
    state.forceEvent5NextDay = false;
    state.gameEnded = false;
    state.endingMessage = "";

    // --- Difficulty Scaling & Starting Loadout ---
    // Logic: Balance starting hunger vs. medical security.
    if (diff == Difficulty::EASY && plan == 1) {
        // Easy mode, Plan A: Basic Stockpile
        state.food = 18;
        state.water = 18;
        state.medicine = 1;
    }
    if (diff == Difficulty::EASY && plan == 2) {
        // Easy mode, Plan B: Medical Priority
        state.food = 8;
        state.water = 8;
        state.medicine = 2;
    }
    if (diff == Difficulty::HARD && plan == 1) {
        // Hard mode, Plan A: High risk, high reward. One member pre-infected (Weak).
        state.food = 18;
        state.water = 18;
        state.medicine = 0;
        state.survivors[0].status = SurvivorStatus::WEAK;
        state.survivors[0].daysWeak = 0;
    }
    if (diff == Difficulty::HARD && plan == 2) {
        // Hard mode, Plan B: Frugal Start
        state.food = 6;
        state.water = 6;
        state.medicine = 2;
    }
}

// Function: consumeDaily
// What it does: This function handles the daily resource consumption phase.
//               Step 1: Calculate how much food and water is needed.
//               Step 2: Subtract the resources. If not enough, consume all remaining.
//               Step 3: Calculate the shortage amounts.
//               Step 4: If there is any shortage, determine who becomes weak.
//                  Easy mode: Pick k healthy survivors (k = the larger shortage),
//                             each has a 50 percent chance to become weak.
//                  Hard mode: Every healthy survivor faces a probability of
//                             30 percent + (total shortage / 15), minimum 50 percent,
//                             maximum 100 percent.
// Input: state - a reference to the GameState object.
// Output: This function does not return a value. It modifies the state directly.
void consumeDaily(GameState& state) {
    // Step 1: Calculate how much food and water we need today
    int requiredFood = state.calculateRequiredFood();
    int requiredWater = state.calculateRequiredWater();

    // Step 2: Calculate the shortage before consuming
    // Shortage means how many units we are missing
    int foodShortage = requiredFood - state.food;
    int waterShortage = requiredWater - state.water;

    // Defensive Check: Ensure shortage is non-negative
    if (foodShortage < 0) foodShortage = 0;
    if (waterShortage < 0) waterShortage = 0;

    // Step 3: Consume the resources
    // Logic: Atomic update of resources, clamping minimum value at 0.
    state.food = state.food - requiredFood;
    if (state.food < 0) state.food = 0;
    state.water = state.water - requiredWater;
    if (state.water < 0) state.water = 0;

    // Step 4: Short-circuit if all needs are met
    if (foodShortage == 0 && waterShortage == 0) {
        return;
    }

    // Step 5: Handle the shortage based on difficulty
    if (state.difficulty == Difficulty::EASY) {
        // Easy mode logic: 
        // Penalize based on the maximum deficit between Food/Water.
        int k = foodShortage;
        if (waterShortage > k) {
            k = waterShortage;
        }

        // Stochastic selection of potential victims
        std::vector<int> candidates = selectRandomSurvivors(state, k, true, false, false);

        for (int i = 0; i < (int)candidates.size(); i++) {
            int survivorIndex = candidates[i];
            double weakChance = 0.5;
            // Trait Influence: FRAIL survivors are more susceptible to malnutrition.
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

    if (state.difficulty == Difficulty::HARD) {
        // Hard mode logic: Global probability scaling based on aggregate shortage.
        int totalShortage = foodShortage + waterShortage;
        double probability = 0.3 + (double)totalShortage / 15.0;

        // --- Math: Probability Clamping [0.5, 1.0] ---
        // Ensures that shortage always presents a significant threat in Hard mode.
        if (probability < 0.5) probability = 0.5;
        if (probability > 1.0) probability = 1.0;

        // Iterate and apply Bernoulli trial for each healthy survivor
        for (int i = 0; i < (int)state.survivors.size(); i++) {
            if (state.survivors[i].status == SurvivorStatus::HEALTHY) {
                double thisChance = probability;
                if (state.survivors[i].trait == SurvivorTrait::FRAIL) {
                    thisChance += 0.2; // Additive penalty for FRAIL trait
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
// What it does: This function uses 1 medicine to treat all weak survivors.
//               Every survivor whose status is WEAK will become HEALTHY.
//               Their daysWeak counter will be reset to 0.
//               TRAIT: If any living survivor has the DOCTOR trait, the
//                      treatment is performed for free (no medicine spent).
//               If there is no medicine AND no doctor, this function does
//               nothing.
// Input: state - a reference to the GameState object.
// Output: This function does not return a value. It modifies the state directly.
void treat(GameState& state) {
    // Check for the "Doctor" optimization to save resources
    bool hasDoctor = state.hasLivingSurvivorWithTrait(SurvivorTrait::DOCTOR);

    // Prerequisite Check: Ensure resources or expertise are available
    if (state.medicine <= 0 && !hasDoctor) {
        return;
    }

    if (!hasDoctor) {
        // Cost Deduction: Generic medicine usage
        state.medicine = state.medicine - 1;
    }

    // Mass healing loop: Resets biological status for all WEAK members
    for (int i = 0; i < (int)state.survivors.size(); i++) {
        if (state.survivors[i].status == SurvivorStatus::WEAK) {
            state.survivors[i].status = SurvivorStatus::HEALTHY;
            state.survivors[i].daysWeak = 0;
        }
    }

    // System Flag: Prevents multiple treatments in a single turn if triggered via UI
    state.wasTreatedToday = true;
}

// Function: updateSickCounters
// What it does: This function is called at the end of each day during night settlement.
//               It increases the daysWeak counter for every weak survivor by 1.
//               If a weak survivor's daysWeak counter reaches 2 or more,
//               that survivor dies and their status is changed to DECEASED.
//               The timeline is: become weak (daysWeak=0) -> end of day 1 (daysWeak=1)
//               -> end of day 2 (daysWeak=2) -> the survivor dies.
// Input: state - a reference to the GameState object.
// Output: This function does not return a value. It modifies the state directly.
void updateSickCounters(GameState& state) {
    // Survival Window Check: Survivors only have a 48-hour window to receive medicine.
    for (int i = 0; i < (int)state.survivors.size(); i++) {
        if (state.survivors[i].status == SurvivorStatus::WEAK) {
            state.survivors[i].daysWeak = state.survivors[i].daysWeak + 1;

            // Fatality Logic: Transition from WEAK to DECEASED status.
            if (state.survivors[i].daysWeak >= 2) {
                state.survivors[i].status = SurvivorStatus::DECEASED;
                state.survivors[i].daysWeak = 0;
            }
        }
    }
}
