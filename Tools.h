#ifndef TOOLS_H
#define TOOLS_H

#include "gamestate.h"
#include <vector>
#include <random>

//随机数工具与通用游戏逻辑函数库

// 初始化全局随机数引擎
void initializeRandomEngine();

// 概率判定
bool makeProbabilityCheck(double probability);

/**
 * @brief 从符合条件的幸存者中随机选择指定数量
 * @param state 游戏状态引用
 * @param count 需要选择的人数
 * @param includeHealthy 是否包含健康状态
 * @param includeWeak 是否包含病弱状态
 * @param includeMutated 是否包含变异状态
 * @return 被选中的幸存者索引列表
 */
std::vector<int> selectRandomSurvivors(
    const GameState& state,
    int count,
    bool includeHealthy = true,
    bool includeWeak = false,
    bool includeMutated = false
);

#endif // TOOLS_H
