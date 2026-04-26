#include "GameState.h"
#include <string>

using namespace std;
// 主循环应在每日结束时调用checkEndings函数
// 辅助函数：计算各种状态的幸存者数量
static int countByStatus(const GameState& state, SurvivorStatus status) {
    int count = 0;
    for (const auto& survivor : state.survivors) {
        if (survivor.status == status) {
            count++;
        }
    }
    return count;
}

// 检查游戏是否结束并设置结局
void checkEndings(GameState& state) {
    // 如果游戏已经结束，直接返回
    if (state.gameEnded) {
        return;
    }
    
    int healthyCount = countByStatus(state, SurvivorStatus::HEALTHY);
    int weakCount = countByStatus(state, SurvivorStatus::WEAK);
    int mutatedCount = countByStatus(state, SurvivorStatus::MUTATED);
    int deceasedCount = countByStatus(state, SurvivorStatus::DECEASED);
    
    int livingCount = healthyCount + weakCount + mutatedCount;
    
    // 条件1: 全员死亡（悲剧收场）
    if (livingCount == 0) {
        state.gameEnded = true;
        state.endingMessage = 
            "Tragic End: The shelter falls into dead silence. Diaries are scattered on the table; "
            "the last page reads: 'We did our best.'";
        return;
    }
    
    // 只在第10天结束时判断其他结局
    if (state.currentDay < 10) {
        return;
    }
    
    int totalResources = state.food + state.water;
    
    // 条件2: 秩序重建
    if (healthyCount >= 4 && totalResources >= 10 && state.triggeredEvent6) {
        state.gameEnded = true;
        state.endingMessage = "Order Restored: Rescue arrives, and order is reestablished.";
        return;
    }
    
    // 条件3: 孤独幸存者
    if (livingCount == 1 && totalResources >= 10) {
        state.gameEnded = true;
        state.endingMessage = 
            "Lone Survivor: Only you remain, guarding the ruins and your memories.";
        return;
    }
    
    // 条件4: 掠夺者
    if (livingCount >= 2 && state.campRobberyCount >= 2) {
        state.gameEnded = true;
        state.endingMessage = 
            "Marauders: You have become the very people you once feared.";
        return;
    }
    
    // 条件5: 变异共生
    if (mutatedCount > 0 && mutatedCount * 2 > livingCount) {
        state.gameEnded = true;
        state.endingMessage = 
            "Symbiotic Evolution: The voice on the radio gradually becomes clear... "
            "but it's not speaking any human language.";
        return;
    }
    
    // 条件6: 艰难求生（默认存活结局）
    if (livingCount > 0) {
        state.gameEnded = true;
        state.endingMessage = 
            "Struggle for Survival: The door finally opens, but the world before you "
            "is unrecognizable. Survival is just another beginning.";
        return;
    }
}
