#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <vector>

// 难度等级
enum class Difficulty { EASY, HARD };

// 幸存者状态
enum class SurvivorStatus { HEALTHY, WEAK, MUTATED, DECEASED };

// 简化幸存者结构
struct Survivor {
    SurvivorStatus status;         // 当前状态
    int daysWeak;                  // 病弱天数（仅当status==WEAK时有效）
    
    // 简易构造函数
    Survivor() : status(SurvivorStatus::HEALTHY), daysWeak(0) {}
    Survivor(SurvivorStatus s) : status(s), daysWeak(0) {}
};

// 游戏总状态
class GameState {
public:
    // 基础状态
    Difficulty difficulty;
    int currentDay;
    
    // 幸存者
    std::vector<Survivor> survivors;
    
    // 资源
    int food;
    int water;
    int medicine;
    
    // 特殊道具
    bool hasRadio;      // 是否拥有收音机
    bool hasNote;       // 是否拥有纸条
    bool usedNoteEffect;// 纸条效果（强制触发外出事件7）是否已使用
    
    // 临时状态（每日重置）
    std::vector<int> expeditionMemberIds;  // 当日外出人员索引列表
    bool wasTreatedToday;                  // 今日是否已治疗
    
    // 事件与结局标记
    bool triggeredEvent6;       // 是否触发过每日事件6（异常信号）
    int campRobberyCount;       // 抢劫其他营地的次数
    bool forceEvent5NextDay;    // 下一天是否强制触发每日事件5（不速之客）
    
    // 结局
    bool gameEnded;
    std::string endingMessage;
    
    // 构造函数
    GameState();
    
    // 状态查询辅助函数
    int countSurvivorsByStatus(SurvivorStatus status) const;
    int countHealthySurvivors() const;
    int countWeakSurvivors() const;
    int countMutatedSurvivors() const;
    int countLivingSurvivors() const;
    
    // 状态重置函数
    void resetDailyStates();
    
    // 物资消耗计算
    int calculateRequiredFood() const;
    int calculateRequiredWater() const;

};

#endif // GAMESTATE_H
