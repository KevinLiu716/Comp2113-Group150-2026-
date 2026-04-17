#include "eventsystem.h"
#include "tools.h"
#include <string>
#include <random>
#include <chrono>

using namespace std;

// 全局随机数引擎
namespace {
    mt19937 randomEngine(chrono::steady_clock::now().time_since_epoch().count());
}

// 工具函数
bool randomDecision(double probability) {
    uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(randomEngine) < probability;
}

// 每日事件处理
DailyEventType selectRandomDailyEvent(GameState& state) {
    if (state.forceEvent5NextDay) {
        state.forceEvent5NextDay = false;
        return DailyEventType::UNEXPECTED_VISITOR;
    }
    
    uniform_int_distribution<int> dist(1, 6);
    int randomNum = dist(randomEngine);
    
    switch(randomNum) {
        case 1: return DailyEventType::RADIATION_RAIN;
        case 2: return DailyEventType::INTERNAL_CONFLICT;
        case 3: return DailyEventType::MYSTERIOUS_DREAM;
        case 4: return DailyEventType::SPOILED_SUPPLIES;
        case 5: return DailyEventType::UNEXPECTED_VISITOR;
        case 6: return DailyEventType::ANOMALOUS_SIGNAL;
        default: return DailyEventType::SPOILED_SUPPLIES;
    }
}

string processDailyEvent(GameState& state, DailyEventType eventType) {
    switch(eventType) {
        case DailyEventType::RADIATION_RAIN:
            return handleRadiationRain(state);
        case DailyEventType::INTERNAL_CONFLICT:
            return handleInternalConflict(state);
        case DailyEventType::MYSTERIOUS_DREAM:
            return handleMysteriousDream(state);
        case DailyEventType::SPOILED_SUPPLIES:
            return handleSpoiledSupplies(state);
        case DailyEventType::UNEXPECTED_VISITOR:
            // 注意：这里只返回描述，实际选择在main.cpp中处理
            return "Unexpected Visitor: Heavy knocks suddenly sound at the door.";
        case DailyEventType::ANOMALOUS_SIGNAL:
            return handleAnomalousSignal(state);
        default:
            return "Nothing special happens tonight.";
    }
}

string handleRadiationRain(GameState& state) {
    int affected = 0;
    for (auto& survivor : state.survivors) {
        if (survivor.status == SurvivorStatus::WEAK) {
            survivor.daysWeak++;
            affected++;
        }
    }
    
    if (affected > 0) {
        return "Radiation Rain: The deadly rain accelerates the condition of " + 
               to_string(affected) + " weak survivor(s).";
    }
    return "Radiation Rain: The sky looks ominous, but no one is currently weak.";
}

string handleInternalConflict(GameState& state) {
    if (state.hasRadio) {
        return "Internal Conflict: Tensions rise, but the radio provides some comfort.";
    }
    
    state.forceEvent5NextDay = true;
    return "Internal Conflict: Arguments escalate. Tomorrow, you will have an unexpected visitor.";
}

string handleMysteriousDream(GameState& state) {
    auto candidates = selectRandomSurvivors(state, 1, true, true, true);
    if (candidates.empty()) return "Mysterious Dream: Silence in the shelter.";
    
    int idx = candidates[0];
    auto& survivor = state.survivors[idx];
    
    if (randomDecision(0.5)) {
        survivor.status = SurvivorStatus::MUTATED;
        return "Mysterious Dream: " + to_string(idx) + " hears whispers and... changes.";
    }
    return "Mysterious Dream: " + to_string(idx) + " has a restless night but wakes up unchanged.";
}

string handleSpoiledSupplies(GameState& state) {
    if (randomDecision(0.5)) {
        int loss = min(2, state.food);
        state.food -= loss;
        return "Spoiled Supplies: You lost " + to_string(loss) + " Food to rot.";
    } else {
        int loss = min(2, state.water);
        state.water -= loss;
        return "Spoiled Supplies: You lost " + to_string(loss) + " Water to contamination.";
    }
}

string handleUnexpectedVisitor(GameState& state, bool openDoor) {
    if (!openDoor) {
        return "Unexpected Visitor: You decide not to open the door. Silence returns.";
    }
    
    double roll = uniform_real_distribution<double>(0.0, 1.0)(randomEngine);
    
    if (roll < 0.3) {
        // 获得帮助
        state.food += 1;
        state.water += 1;
        return "Unexpected Visitor: A friendly survivor shares supplies. +1 Food, +1 Water.";
    } 
    else if (roll < 0.6) {
        // 被抢劫
        int foodLoss = min(3, state.food);
        int waterLoss = min(3, state.water);
        state.food -= foodLoss;
        state.water -= waterLoss;
        return "Unexpected Visitor: Raiders! You lost " + to_string(foodLoss) + 
               " Food and " + to_string(waterLoss) + " Water.";
    }
    else {
        // 获得收音机（如果还没有）
        if (!state.hasRadio) {
            state.hasRadio = true;
            return "Unexpected Visitor: You find a working radio! This might be useful.";
        } else {
            // 已经有收音机，无事发生
            return "Unexpected Visitor: A survivor leaves something, but it's just junk.";
        }
    }
}

string handleAnomalousSignal(GameState& state) {
    if (state.hasRadio) {
        state.triggeredEvent6 = true;
        return "Anomalous Signal: The radio picks up a strange, rhythmic pattern...";
    } else {
        state.forceEvent5NextDay = true;
        return "Anomalous Signal: You hear strange noises. Tomorrow, someone will come.";
    }
}
