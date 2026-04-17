// tools.h 的简单示例
#ifndef TOOLS_H
#define TOOLS_H

#include "gamestate.h"
#include <vector>

// 初始化随机种子，在main函数开始时调用一次即可
void initRandom();

// 概率判定函数
// 输入：probability - 一个介于0.0和1.0之间的概率值，例如0.3表示30%的概率
// 返回：true 表示事件发生，false 表示未发生
bool checkProbability(double probability);

// 随机选择幸存者函数
// 输入： 
//   state - 游戏状态（用来读取幸存者列表）
//   count - 想要选择几个人
//   includeHealthy - 是否从健康(Healthy)状态的人里选
//   includeWeak - 是否从病弱(Weak)状态的人里选  
//   includeMutated - 是否从变异(Mutated)状态的人里选
// 返回：一个向量，里面装着被选中的幸存者在state.survivors中的下标(索引)
// 注意：只会从活人（非DECEASED）中选，如果符合条件的活人少于count，则返回所有符合条件的。
std::vector<int> selectRandomSurvivors(const GameState& state, 
                                      int count, 
                                      bool includeHealthy = true, 
                                      bool includeWeak = false, 
                                      bool includeMutated = false);

#endif // TOOLS_H


// tools.cpp 的简单实现示例
#include "tools.h"
#include <cstdlib> // 为了 rand() 和 srand()
#include <ctime>   // 为了 time()
#include <algorithm> // 为了 std::swap

// 初始化随机种子
void initRandom() {
    // 用当前时间作为随机种子，这样每次运行程序得到的随机数都不一样
    srand(static_cast<unsigned int>(time(nullptr)));
}

// 概率判定函数
bool checkProbability(double probability) {
    if (probability <= 0.0) return false;
    if (probability >= 1.0) return true;
    
    // rand() 会返回一个0到RAND_MAX之间的随机整数
    // 除以 RAND_MAX 就得到一个0.0到1.0之间的随机小数
    double randomValue = static_cast<double>(rand()) / RAND_MAX;
    
    // 如果这个随机小数小于给定的概率，就认为事件发生了
    return randomValue < probability;
}

// 随机选择幸存者函数
std::vector<int> selectRandomSurvivors(const GameState& state, 
                                      int count, 
                                      bool includeHealthy, 
                                      bool includeWeak, 
                                      bool includeMutated) {
    std::vector<int> candidates; // 第一步：先找出所有符合条件的候选人
    std::vector<int> result;     // 最后要返回的结果
    
    // 遍历所有幸存者
    for (int i = 0; i < static_cast<int>(state.survivors.size()); ++i) {
        const Survivor& s = state.survivors[i];
        
        // 跳过死者
        if (s.status == SurvivorStatus::DECEASED) {
            continue;
        }
        
        // 检查状态是否符合要求
        bool statusOK = false;
        if (includeHealthy && s.status == SurvivorStatus::HEALTHY) statusOK = true;
        if (includeWeak && s.status == SurvivorStatus::WEAK) statusOK = true;
        if (includeMutated && s.status == SurvivorStatus::MUTATED) statusOK = true;
        
        if (statusOK) {
            candidates.push_back(i); // 记录这个候选人的索引
        }
    }
    
    // 如果候选人数量不够，就返回所有候选人
    if (candidates.size() <= static_cast<size_t>(count)) {
        return candidates;
    }
    
    // 第二步：打乱候选人数组的顺序（洗牌）
    // 用一个简单的循环：从后往前，每个位置和前面随机一个位置交换
    for (int i = static_cast<int>(candidates.size()) - 1; i > 0; --i) {
        // 生成一个0到i之间的随机数
        int j = rand() % (i + 1);
        // 交换 candidates[i] 和 candidates[j]
        std::swap(candidates[i], candidates[j]);
    }
    
    // 第三步：取出前count个作为结果
    result.assign(candidates.begin(), candidates.begin() + count);
    return result;
}

// 一个简单的测试示例，演示函数如何使用
// 注：实际提交时可以删掉这个main函数
int main() {
    // 1. 初始化随机数
    initRandom();
    
    // 2. 测试概率函数
    int trueCount = 0;
    for (int i = 0; i < 1000; ++i) {
        if (checkProbability(0.3)) { // 测试30%概率
            trueCount++;
        }
    }
    // 理论上trueCount应该接近300
    
    // 3. 测试选择函数（这里需要有一个GameState对象，略）
    // GameState state;
    // auto selected = selectRandomSurvivors(state, 2); // 选2个健康的
    
    return 0;
}
