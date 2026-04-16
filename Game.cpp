//举例：如何在主循环中控制外出人数和应用延迟资源

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


//延迟资源的处理
// 在事件系统中，如"水厂"事件
gameState.addDelayedResource(0, 6, 0, 1);  // 1天后获得6水

// 在主循环的每日更新中
gameState.applyDelayedResources();  // 应用所有到期的延迟资源
