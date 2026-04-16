//举例：如何在主循环中控制外出人数

// 获取最大外出人数
int maxExp = gameState.survivors.maxExpedition();

// 提供给玩家的选项
std::vector<int> options;
for (int i = 0; i <= maxExp; ++i) {
    options.push_back(i);
}

// 玩家选择外出人数（比如选择2）
int chosenExp = 2;  // 假设玩家选择了2
if (chosenExp > maxExp) {
    // 错误处理：选择不能超过最大值
    chosenExp = maxExp;
}


