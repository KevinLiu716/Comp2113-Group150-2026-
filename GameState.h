// gamestate.h
// 游戏状态和核心数据结构的定义

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <vector>
#include <string>

// ==================== 枚举类型 ====================
// 游戏难度
enum class Difficulty {
    EASY,
    HARD
};

// 幸存者状态类型
enum class SurvivorStatus {
    HEALTHY,    // 健康
    WEAK,       // 病弱
    MUTATED,    // 变异
    DECEASED    // 死亡
};

// 幸存者状态池（每种状态人数）
struct SurvivorPool {
    int healthy = 0;
    int weak = 0;
    int mutated = 0;
    int deceased = 0;
    
    // 总人数（固定为6，但计算时可从状态累加）
    int total() const { return healthy + weak + mutated + deceased; }
    
    // 存活人数
    int alive() const { return healthy + weak + mutated; }
    
    // 可以外出的人数（健康+变异）÷ 2（向下取整）
// 主循环里给玩家不超过这个数的选项
    int maxExpedition() const { return (healthy + mutated) / 2; }
    
    // 状态变更方法
    void changeStatus(SurvivorStatus from, SurvivorStatus to, int count = 1);
    
    // 获取特定状态的幸存者索引列表（用于随机选择）
    std::vector<int> getIndicesByStatus(SurvivorStatus status) const;
};

/// 游戏核心状态
struct GameState {
    // --- 人员状态 ---
    SurvivorPool survivors;
    
    // --- 物资 ---
    int food = 0;
    int water = 0;
    int medicine = 0;
    
    // --- 特殊物品 ---
    bool hasRadio = false;      // 收音机
    bool hasNote = false;       // 有信息的纸条
    
    // --- 游戏进度 ---
    int currentDay = 1;
    Difficulty difficulty = Difficulty::EASY;
    
    // --- 事件相关标志 ---
    bool forceEvent5NextDay = false;  // 下一天强制触发"不速之客"
    bool endingSignalTriggered = false; // 是否触发过异常信号（结局1条件）
    
    // --- 统计信息（用于结局判定）---
    int campRobbedCount = 0;  // 抢劫营地次数
    
    // --- 延迟获得的资源（如"水厂"事件第二天获得水）---
    struct DelayedResource {
        int food = 0;
        int water = 0;
        int medicine = 0;
        int daysRemaining = 0;  // 剩余天数，每天减1，为0时生效
    };
    std::vector<DelayedResource> delayedResources;
    
    // 应用延迟资源（每天调用一次）
    void applyDelayedResources();
    
    // 添加延迟资源
    void addDelayedResource(int foodAmt, int waterAmt, int medicineAmt, int days);
};

// ==================== 辅助函数声明 ====================
// 游戏状态初始化
void initGameState(GameState& state, Difficulty diff, int supplyChoice);

// 资源消耗（返回缺失的食物和水数量）
bool consumeResources(GameState& state, int& missingFood, int& missingWater);

// 更新状态（每天结束时调用，包括状态转换等）
void updateDailyState(GameState& state);

#endif // GAMESTATE_H
