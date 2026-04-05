# Comp2113-Group150-2026-
This is a prviate group project for comp2113 group 150

Game Background
The apocalypse descended abruptly, shrouding the familiar world in radioactive dust. The six of you have taken shelter in this makeshift refuge, facing only limited supplies and unknown threats outside the door. Over the next ten days, every choice is a matter of survival.

难度分级：简单/困难
结算方法：全员死亡直接结算，否则在第11天进入结局
——————————————————————————————————
人员总数：6
每人每日消耗一份食物一份水，物资不足所有人消耗时，消耗现有所有物资并：
缺少n份食物：判定n个人每人30/50%几率状态变成病弱；
缺少n份水：判定n个人每人70/100%几率状态变成病弱；
——————————————————————————————————
状态类型：健康/病弱/变异/死亡
健康：正常状态，无负面影响
病弱：需要消耗药品治疗，否则两天后死亡，不能外出
变异：不消耗食物和水，不可逆转，影响结局
——————————————————————————————————
物资类型：
食物，水，药品（群体治疗），有信息的纸条（后续2/1天外出必然触发外出事件7），收音机（改变每日事件判定）

————————每日流程———————————————————————
[第X天]
状态报告
选择行动  顺序：治疗(y/n)->外出(y/n)(max=存活人数//2)->休息
夜间结算（消耗物资+随机事件）

=== DAY [X] ===

[STATUS REPORT]

**Personnel Status (Total: 6):**
- Healthy: [Number]
- Weak: [Number]
- Mutated: [Number]
- Deceased: [Number]

*Status Descriptions:*
- **Healthy**: Normal condition. Can go on Expeditions.
- **Weak**: Requires treatment with 1 **Medicine** within 2 days, or they will die. Cannot go on Expeditions.
- **Mutated**: Does not consume Food or Water. Condition is permanent and may influence endings.
- **Deceased**: Permanently lost.

**Current Supplies:**
- Food: [Number]
- Water: [Number]
- Medicine: [Number]
- Special Items: 

[DAYTIME ACTIONS]
Please choose your actions in order:

1. **Treat Survivors (Y/N):**
   - If Y: Use 1 **Medicine** to change all **Weak** survivors to **Healthy**.
   - If N: Proceed to the next action.

2. **Send Expedition (Y/N):**
   - Maximum members allowed: [Number of (Healthy + Mutated) survivors] // 2 (rounded down).
   - If Y:
   - How any member(s) will go out? Choose from (1,2,max)
   - If N: Proceed to Rest.

3. **Rest:**
   - All survivors who did not go on an expedition remain in the shelter.

[NIGHT PHASE - SETTLEMENT]

1. **Resource Consumption:**
   - Each **Healthy** and **Weak** survivor consumes 1 Food and 1 Water.
   - **Mutated** survivors consume nothing.
   - If supplies are insufficient, all remaining supplies are consumed. For each missing unit:
     - **Missing Food**: Roll for [Number missing] survivors, each has a 30% (Easy) / 50% (Hard) chance to become **Weak**.
     - **Missing Water**: Roll for [Number missing] survivors, each has a 70% (Easy) / 100% (Hard) chance to become **Weak**.

2. **Random Event:**

[The game proceeds to Day X+1...]
————————每日事件———————————————————————
辐射雨
效果：当日外出人员全部健康→病弱
Radiation Rain: The sky outside changes ominously; Those who went out today are likely in great danger.

2.内部冲突
效果：随机1-2人受伤（健康→病弱）
对策：若有“收音机”道具，则无负面效果
Internal Conflict: Under immense survival pressure, long-suppressed arguments finally erupt into violence inside the shelter.

3.神秘梦境
效果：随机1人听到低语
结果：50%变为变异，50%无事发生
Mysterious Dream: In the night, an indistinguishable whisper invades someone's mind directly.

4.物资腐败
效果：随机损失食物×2或水×2
Spoiled Supplies: A undeniable smell of rot fills the air; you discover that part of your precious reserves has spoiled in the dampness.

5.不速之客
选择：开门/不开门
开门：30%获得帮助（1食物1水），30%被抢（失去3食物3水），40%概率获得收音机
不开门：无事
Unexpected Visitor: Heavy knocks suddenly sound at the door. Is it a fellow survivor in need of help or a marauder with ill intentions?

6.异常信号
效果：若有“收音机”道具，触发该事件且判定结局时有人存活进入结局1
无收音机：下一天每日事件必为5
Anomalous Signal: A rhythmic, distinctly unnatural static noise comes from the distance. What could this signal mean?
————————外出事件———————————————————————

1.经过超市：
- 基础：获得食物×4/3
- 风险：外出人员20%概率变为病弱（简单只判定一人，困难全体判定）
Passing by a Supermarket: The shelves are mostly empty, but beneath the rubble, there might still be forgotten cans.

2.水厂：
- 基础：第二天获得水×6/4
- 风险：需要1人留守取水（当天无法返回，困难难度下20%概率死亡，也不会获得水）
Water Plant: The city's water purification system has long failed, but the plant's deep storage tanks might still hold uncontaminated water.

3.药店
- 基础：获得药物×2
- 风险：外出人员30%概率染病（简单只判定一人，困难全体判定）
Pharmacy: Shattered glass counters litter the floor. Scavenging among the scattered medicine bottles and bandages is a race against death.

4.其他幸存者营地
A. 请求帮助（70%获得总共3份食物和水，具体比例随机，30%被拒绝）
困难难度概率变为40/60
B. 抢劫（100%获得总共8份食物和水，具体比例随机，但1人受伤→病弱）
困难难度下一人直接死亡
Other Survivors' Camp: Smoke rising in the distance indicates others are there. Do you choose to negotiate or to take by force?

5.周边清理
A. 外围清理（获得2份食物或2份水，困难只有一份）
B.深入内部（得到道具：有信息的纸条）
Clearing the Perimeter: Searching the destroyed houses around the shelter is less risky, but the rewards are usually meager.

6.实验室
外出人员全体变异（困难仅1人变异）
Laboratory: Inside might lie the answers to what caused all this, or deeper nightmares.

7.隐藏仓库（需要有有信息的纸条）
获得总共6份食物和水，具体比例随机
Hidden Storage: Following the vague markings on the note, you actually find an unlooted storage point
————————结局判定———————————————————————

1.秩序重建：“救援到达，秩序被重新建立”
判定条件：第11天健康≥4人，食物+水>=10，每日事件6效果触发
Order Restored: "Rescue arrives, and order is reestablished."

2.孤独幸存者：“只剩你一人，守着废墟和回忆”
判定条件：第11天只有一人存活，食物+水>=10
Lone Survivor: "Only you remain, guarding the ruins and your memories."

3.悲剧收场​：“避难所陷入死寂。桌上散落着日记本，最后一页写着：我们尽力了。”
判定条件：第11天或更早无人存活
Tragic End: "The shelter falls into dead silence. Diaries are scattered on the table; the last page reads: 'We did our best.'"

4.变异共生：“收音机里的声音逐渐清晰...但说的不是人类的语言。”
判定条件：第11天变异人数占存活人数大于1/2
Symbiotic Evolution: "The voice on the radio gradually becomes clear... but it's not speaking any human language."

5.艰难求生：“门终于开了，但眼前的世界已面目全非。幸存，只是另一种开始。”
判定条件：第11天有人存活
Struggle for Survival: "The door finally opens, but the world before you is unrecognizable. Survival is just another beginning."

6.掠夺者​：“你们成为了自己曾经恐惧的人”
判定条件：第11天存活人数不少于2人，总共抢劫营地≥2次
Marauders: "You have become the very people you once feared."
