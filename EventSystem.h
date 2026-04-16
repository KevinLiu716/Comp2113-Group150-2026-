#ifndef EVENTSYSTEM_H
#define EVENTSYSTEM_H

#include "gamestate.h"
#include <string>
#include <vector>

// daily event
enum class DailyEventType {
    RADIATION_RAIN,    
    INTERNAL_CONFLICT, 
    MYSTERIOUS_DREAM,
    SPOILED_SUPPLIES,
    UNEXPECTED_VISITOR,
    ANOMALOUS_SIGNAL   
};

std::string processDailyEvent(GameState& state, DailyEventType eventType);
//return discription/result 

DailyEventType selectRandomDailyEvent(GameState& state);
//return eventType
//!! consider flag: forceEvent5NextDay

std::string handleRadiationRain(GameState& state);
//return discrption


std::string handleInternalConflict(GameState& state);
//return discrption
//!! set flag: forceEvent5NextDay

std::string handleMysteriousDream(GameState& state);
//return discrption

std::string handleSpoiledSupplies(GameState& state);
//return discrption

std::string handleUnexpectedVisitor(GameState& state, bool choice);  
//return discrption
//player's choice true=openTheDoor, false = ...

std::string handleAnomalousSignal(GameState& state);
//!! set flag: forceEvent5NextDay



enum class ExpeditionEventType {
    SUPERMARKET,
    WATER_PLANT,
    PHARMACY,
    OTHER_CAMP,
    PERIMETER_CLEAR,
    LABORATORY,
    HIDDEN_STORAGE
};

//camp
enum class CampOption {
    REQUEST_HELP,
    ROB
};

// 周边清理选项
enum class ClearOption {
    OUTER_CLEAR,        // 外围清理
    INNER_SEARCH        // 深入内部
};

/**
 * @brief 处理外出事件
 * @param state 游戏状态引用
 * @param eventType 外出事件类型
 * @param memberIndices 外出人员索引列表
 * @param choice 额外选择（用于营地事件和周边清理事件）
 * @return 外出结果描述字符串
 */
std::string processExpeditionEvent(
    GameState& state, 
    ExpeditionEventType eventType,
    const std::vector<int>& memberIndices,
    int choice = -1
);

/**
 * @brief 随机选择外出事件
 * @param state 游戏状态引用
 * @return 被选中的外出事件类型
 * @note 考虑特殊道具效果（如hasNote和usedNoteEffect）
 */
ExpeditionEventType selectRandomExpeditionEvent(GameState& state);

/**
 * @brief 处理经过超市事件
 * @param state 游戏状态引用
 * @param memberIndices 外出人员索引列表
 * @return 事件描述
 */
std::string handleSupermarket(GameState& state, const std::vector<int>& memberIndices);

/**
 * @brief 处理水厂事件
 * @param state 游戏状态引用
 * @param memberIndices 外出人员索引列表
 * @return 事件描述
 */
std::string handleWaterPlant(GameState& state, const std::vector<int>& memberIndices);

/**
 * @brief 处理药店事件
 * @param state 游戏状态引用
 * @param memberIndices 外出人员索引列表
 * @return 事件描述
 */
std::string handlePharmacy(GameState& state, const std::vector<int>& memberIndices);

/**
 * @brief 处理其他幸存者营地事件
 * @param state 游戏状态引用
 * @param memberIndices 外出人员索引列表
 * @param option 营地交互选项
 * @return 事件描述
 */
std::string handleOtherCamp(GameState& state, const std::vector<int>& memberIndices, CampOption option);

/**
 * @brief 处理周边清理事件
 * @param state 游戏状态引用
 * @param memberIndices 外出人员索引列表
 * @param option 清理选项
 * @return 事件描述
 */
std::string handlePerimeterClear(GameState& state, const std::vector<int>& memberIndices, ClearOption option);

/**
 * @brief 处理实验室事件
 * @param state 游戏状态引用
 * @param memberIndices 外出人员索引列表
 * @return 事件描述
 */
std::string handleLaboratory(GameState& state, const std::vector<int>& memberIndices);

/**
 * @brief 处理隐藏仓库事件
 * @param state 游戏状态引用
 * @param memberIndices 外出人员索引列表
 * @return 事件描述
 */
std::string handleHiddenStorage(GameState& state, const std::vector<int>& memberIndices);

// ============================
// 工具函数
// ============================

/**
 * @brief 从健康幸存者中随机选择指定数量的人员
 * @param state 游戏状态引用
 * @param count 需要选择的人数
 * @return 被选中人员的索引列表
 */
std::vector<int> selectRandomHealthySurvivors(GameState& state, int count);

/**
 * @brief 从所有幸存者中随机选择指定数量的人员
 * @param state 游戏状态引用
 * @param count 需要选择的人数
 * @param includeWeak 是否包含病弱者
 * @param includeMutated 是否包含变异者
 * @return 被选中人员的索引列表
 */
std::vector<int> selectRandomSurvivors(
    GameState& state, 
    int count, 
    bool includeWeak = false, 
    bool includeMutated = true
);

/**
 * @brief 随机决定事件（按概率）
 * @param probability 成功概率（0.0到1.0之间）
 * @return 是否成功
 */
bool randomDecision(float probability);

/**
 * @brief 初始化事件系统的随机数生成器
 */
void initializeEventSystem();

#endif // EVENTSYSTEM_H
