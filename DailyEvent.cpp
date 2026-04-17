#include "eventsystem.h"
#include "tools.h"
//等待工具函数对于随机数生成，状态修改，物品数量调整
//line 15,38,71,(86,110)to_string
using namespace std;


// 每日事件处理
DailyEventType selectRandomDailyEvent(GameState& state) {
    if (state.forceEvent5NextDay) {
        state.forceEvent5NextDay = false;
        return DailyEventType::UNEXPECTED_VISITOR;
    }
    
//等待随机数系统
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
            //返回的描述，等待协调main获取选项再修改
            return "Unexpected Visitor: Heavy knocks suddenly sound at the door. Is it a fellow survivor in need of help or a marauder with ill intentions?";
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
        return "Radiation Rain: The sky outside changes ominously. The deadly rain accelerates the condition of weak survivor(s).";
    }
    return "Radiation Rain: The sky looks ominous, but nothing fatal happens.";
}

string handleInternalConflict(GameState& state) {
    if (state.hasRadio) {
        return "Internal Conflict: Tensions rise, but the radio provides some comfort.";
    }
    
    state.forceEvent5NextDay = true;
    return "Internal Conflict: Under immense survival pressure, long-suppressed arguments finally erupt into suspicions inside the shelter.";
}
//等工具函数选择特定状态幸存者
string handleMysteriousDream(GameState& state) {
    auto candidates = selectRandomSurvivors(state, 1, true, true, true);
    if (candidates.empty()) return "Mysterious Dream: Silence in the shelter.";
    
    int idx = candidates[0];
    auto& survivor = state.survivors[idx];
    
    if (randomDecision(0.5)) {
        survivor.status = SurvivorStatus::MUTATED;
        return "Mysterious Dream: someone hears whispers and... changes.";
    }
    return "Mysterious Dream: Someone has a restless night but wakes up unchanged.";
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
        if (!state.hasRadio) {
            state.hasRadio = true;
            return "Unexpected Visitor: You find a working radio! This might be useful.";
        } else {
            return "Unexpected Visitor: A survivor leaves something, but it's just junk.";
        }
    }
}

string handleAnomalousSignal(GameState& state) {
    if (state.hasRadio) {
        state.triggeredEvent6 = true;
        return "Anomalous Signal: It seems that some rescue groups are getting closer.";
    } else {
        state.forceEvent5NextDay = true;
        return "Anomalous Signal: A rhythmic, distinctly unnatural static noise comes from the distance. What could this signal mean?";
    }
}
