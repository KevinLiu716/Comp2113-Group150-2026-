#ifndef TOOLS_H
#define TOOLS_H

#include "gamestate.h"
#include <vector>
#include <random>

//随机数工具与通用游戏逻辑函数库

namespace GameUtils {

    /**
     * @brief 初始化全局随机数引擎
     * @note 应在程序开始时调用一次
     */
    void initializeRandomEngine();

    /**
     * @brief 进行概率判定
     * @param probability 成功概率，范围 [0.0, 1.0]
     * @return 判定成功返回 true，否则返回 false
     */
    bool makeProbabilityCheck(double probability);

    /**
     * @brief 从符合条件的幸存者中随机选择指定数量
     * @param state 游戏状态引用
     * @param count 需要选择的人数
     * @param requireHealthy 是否必须为健康状态
     * @param requireWeak 是否可以为病弱状态
     * @param requireMutated 是否可以为变异状态
     * @return 被选中的幸存者在 `state.survivors` 中的索引列表
     * @note 如果符合条件的幸存者少于所需数量，则返回所有符合条件的幸存者。
     */
    std::vector<int> selectRandomSurvivors(
        const GameState& state,
        int count,
        bool requireHealthy = false,
        bool requireWeak = false,
        bool requireMutated = false
    );

    /**
     * @brief 从健康幸存者中随机选择指定数量
     * @param state 游戏状态引用
     * @param count 需要选择的人数
     * @return 被选中的健康幸存者的索引列表
     */
    std::vector<int> selectRandomHealthySurvivors(const GameState& state, int count);

    /**
     * @brief 从容器中随机选择一个元素
     * @tparam T 容器元素类型
     * @param container 标准库容器（如 std::vector, std::list）
     * @return 指向随机元素的常量引用
     */
    template<typename Container>
    const typename Container::value_type& selectRandomElement(const Container& container);

} // namespace GameUtils

#endif // TOOLS_H
