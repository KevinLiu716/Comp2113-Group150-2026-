# Comp2113-Group150-2026-
This is a prviate group project for comp2113 group 150

流程：每日report - 吃药 - 外出 - 随机事件（辐射雨判定） - 物资消耗 - 病弱计时或死亡判定（0当天结束变成病弱计数为0，1，2当天结束计时是2则死亡） - 结束或进入下一天

Game Background
The apocalypse descended abruptly, shrouding the familiar world in radioactive dust. The six of you have taken shelter in this makeshift refuge, facing only limited supplies and unknown threats outside the door. Over the next ten days, every choice is a matter of survival.

Select Difficulty & Starting Supplies:
Easy Difficulty:
A. Basic Stockpile: Food x18, Water x18, Medicine x1
B. Medical Priority: Food x8, Water x8, Medicine x2
Hard Difficulty:
A. All-in: Food x18, Water x18, Medicine x0 (One survivor starts as Weak)
B. Frugal Start: Food x6, Water x6, Medicine x2

难度分级：简单/困难
结算方法：全员死亡直接结算（在每日最开始生成报告时判定），否则在第10天结束时判定结局
文档内概率或物资的“/”表示难度区分，前面的是简单难度下的结果
——————————————————————————————————
人员总数：6
每人每日消耗一份食物一份水，物资不足所有人消耗时，消耗现有所有物资并可能：
缺少n份食物，缺少m份水

简单模式：取k = max(m, n)只做一次判定，k 个健康的人50%的概率变成病弱；
困难模式：判定所有健康的人有30% + (m+n)/15 (不低于50%，不超过100%)的概率变为病弱；
——————————————————————————————————
状态类型：健康/病弱/变异/死亡
健康：正常状态，无负面影响
病弱：需要消耗药品治疗，否则两天后死亡，不能外出
变异：不消耗食物和水，不可逆转，可以死亡，影响结局
——————————————————————————————————
物资类型：
食物，水，药品（群体治疗），纸条（后续简单模式后两天/困难模式下一天外出必然触发外出事件7，只判定一次修改外出事件，用完后不消失），收音机（改变每日事件判定，不消失，bool值定义）
————————每日流程———————————————————————
[第X天]
状态报告
选择行动  顺序：治疗(y/n)->外出(y/n)(max=存活人数//2，UI给玩家不大于最大值的选项)->休息
夜间结算（随机事件先，然后物资消耗，最后判定生病是否死亡）

=== DAY [X] ===
[STATUS REPORT]
Personnel Status (Total: 6):
Healthy: [Number]
Weak: [Number]
Mutated: [Number]
Deceased: [Number]

Status Descriptions:
Healthy: Normal condition. Can go on Expeditions.
Weak: Requires treatment with 1 Medicine within 2 days, or they will die. Cannot go on Expeditions.
Mutated: Does not consume Food or Water. Condition is permanent and may influence endings. Can be changed to deceased.
Deceased: Permanently lost.

Current Supplies:
Food: [Number]
Water: [Number]
Medicine: [Number]

Special Items:
[DAYTIME ACTIONS]

Please choose your actions in order:
Treat Survivors (Y/N):
If Y: Use 1 Medicine to change all Weak survivors to Healthy.
If N: Proceed to the next action.

Send Expedition (Y/N):
Maximum members allowed: [Number of (Healthy + Mutated) survivors] // 2 (rounded down).
If Y:
How any member(s) will go out? Choose from (e.g. 1,2,max)
If N: Proceed to Rest.

Rest:
All survivors who did not go on an expedition remain in the shelter.

[NIGHT PHASE - SETTLEMENT]
Random Event: ...

Resource Consumption:
Normally, each Healthy and Weak survivor consumes 1 Food and 1 Water.
Mutated survivors consume nothing.

If supplies are insufficient, all remaining supplies are consumed. For each missing unit:
Easy Mode:​ Basically, we look at which resource you're shorter on—food or water. We take the bigger shortage number called "k". Then, k of your healthy survivors each face a 50% chance to become Weak.

Hard Mode:​ Things get tougher. Every single healthy survivor is at risk. The chance all of them become Weak is 30% + (Total Shortage / 15). This chance can never drop below 50%, and it caps at 100%.

[The game proceeds to Day X+1...]

————————每日事件———————————————————————
辐射雨
效果：当天病弱的人死亡倒计时+1天
Radiation Rain: The sky outside changes ominously; Those who went out today are likely in great danger.
2.内部冲突
效果：下一天每日事件必为5
对策：若有“收音机”道具，则无负面效果
Internal Conflict: Under immense survival pressure, long-suppressed arguments finally erupt into suspicions inside the shelter.
3.神秘梦境
效果：1人（健康或病弱）听到低语
结果：50%变为变异，50%无事发生
Mysterious Dream: In the night, an indistinguishable whisper invades someone's mind directly.
4.物资腐败
效果：随机损失食物×2或水×2
Spoiled Supplies: A undeniable smell of rot fills the air; you discover that part of your precious reserves has spoiled in the dampness.
5.不速之客
选择：开门/不开门
开门：30%获得帮助（1食物1水），30%被抢（失去3食物3水），40%概率获得收音机。收音机不会重复获得，40%概率变成无事发生
不开门：无事
Unexpected Visitor: Heavy knocks suddenly sound at the door. Is it a fellow survivor in need of help or a marauder with ill intentions?
6.异常信号
效果：若有“收音机”道具，触发该事件且判定结局时有人存活进入结局1
无收音机：下一天每日事件必为5
Anomalous Signal: A rhythmic, distinctly unnatural static noise comes from the distance. What could this signal mean?
————————外出事件———————————————————————
1.经过超市：
基础：获得食物×4/3
风险：结算时多消耗等于外出人数的1/2份水
Passing by a Supermarket: The shelves are mostly empty, but beneath the rubble, there might still be forgotten cans.
2.水厂：
基础：获得水×6/4
风险：结算时多消耗等于外出人数的1/2份食物
Water Plant: The city's water purification system has long failed, but the plant's deep storage tanks might still hold uncontaminated water.
3.药店
基础：获得药物×2
风险：当天会多消耗份等同于外出人数的食物和水
Pharmacy: Shattered glass counters litter the floor. Scavenging among the scattered medicine bottles and bandages is a race against death.
4.其他幸存者营地
A. 请求帮助（70%获得1食物和2水，30%被拒绝）
困难难度概率变为40/60
B. 抢劫（获得总共4份食物和4水，50%/100%的概率一人死亡）
Other Survivors' Camp: Smoke rising in the distance indicates others are there. Do you choose to negotiate or to take by force?
5.周边清理
A. 外围清理（获得2份食物或2份水，困难只有一份）
B.深入内部（得到道具：有信息的纸条）
Clearing the Perimeter: Searching the destroyed houses around the shelter is less risky, but the rewards are usually meager.
6.实验室
外出人员全体变异（困难仅1人变异）
Laboratory: Inside might lie the answers to what caused all this, or deeper nightmares.
7.隐藏仓库（需要有有信息的纸条）
获得3份食物和3份水
Hidden Storage: Following the vague markings on the note, you actually find an unlooted storage point
————————结局判定———————————————————————
1.秩序重建：“救援到达，秩序被重新建立”
判定条件：第10天健康≥4人，食物+水>=10，拥有收音机且发生至少一次异常信号事件。
Order Restored: "Rescue arrives, and order is reestablished."
2.孤独幸存者：“只剩你一人，守着废墟和回忆”
判定条件：第10天只有一人存活，食物+水>=10
Lone Survivor: "Only you remain, guarding the ruins and your memories."
3.悲剧收场​：“避难所陷入死寂。桌上散落着日记本，最后一页写着：我们尽力了。”
判定条件：第10天或更早无人存活
Tragic End: "The shelter falls into dead silence. Diaries are scattered on the table; the last page reads: 'We did our best.'"
4.变异共生：“收音机里的声音逐渐清晰...但说的不是人类的语言。”
判定条件：第10天变异人数占存活人数大于1/2
Symbiotic Evolution: "The voice on the radio gradually becomes clear... but it's not speaking any human language."
5.艰难求生：“门终于开了，但眼前的世界已面目全非。幸存，只是另一种开始。”
判定条件：第10天有人存活
Struggle for Survival: "The door finally opens, but the world before you is unrecognizable. Survival is just another beginning."
6.掠夺者​：“你们成为了自己曾经恐惧的人”
判定条件：第10天存活人数不少于2人，总共抢劫营地≥2次
Marauders: "You have become the very people you once feared."
