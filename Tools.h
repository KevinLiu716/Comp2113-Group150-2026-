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
