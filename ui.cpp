#include "ui.h"
#include <iostream>
#include <limits>
#include <cstdlib>
#include <sstream>
void UI::clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        std::cout << "\033[2J\033[1;1H";
    #endif
}

int UI::visibleLength(const std::string& text) {
    int len = 0;
    bool inEscape = false;
    for (size_t i = 0; i < text.size(); i++) {
        unsigned char c = text[i];
        if (c == '\033') { inEscape = true; continue; }
        if (inEscape) {
            if (c == 'm') inEscape = false;
            continue;
        }
        // UTF-8 多字节字符（中文、emoji）只计为 1 个可见单位
        if ((c & 0x80) == 0) {
            len += 1;  // ASCII
        } else if ((c & 0xE0) == 0xC0) {
            len += 1; i += 1;  // 2 字节
        } else if ((c & 0xF0) == 0xE0) {
            len += 2; i += 2;  // 3 字节，中文按 2 个宽度
        } else if ((c & 0xF8) == 0xF0) {
            len += 2; i += 3;  // 4 字节，emoji 按 2 个宽度
        }
    }
    return len;
}

void UI::drawTopBorder(int width) {
    std::cout << CYAN << "╔";
    for (int i = 0; i < width - 2; i++) std::cout << "═";
    std::cout << "╗" << RESET << "\n";
}

void UI::drawBottomBorder(int width) {
    std::cout << CYAN << "╚";
    for (int i = 0; i < width - 2; i++) std::cout << "═";
    std::cout << "╝" << RESET << "\n";
}

void UI::drawMidBorder(int width) {
    std::cout << CYAN << "╠";
    for (int i = 0; i < width - 2; i++) std::cout << "═";
    std::cout << "╣" << RESET << "\n";
}

void UI::drawCentered(const std::string& text, int width) {
    int textLen = visibleLength(text);
    int pad = (width - 2 - textLen) / 2;
    if (pad < 0) pad = 0;
    int rightPad = width - 2 - textLen - pad;
    if (rightPad < 0) rightPad = 0;

    std::cout << CYAN << "║" << RESET;
    for (int i = 0; i < pad; i++) std::cout << " ";
    std::cout << text;
    for (int i = 0; i < rightPad; i++) std::cout << " ";
    std::cout << CYAN << "║" << RESET << "\n";
}

void UI::drawLeft(const std::string& text, int width) {
    int textLen = visibleLength(text);
    int rightPad = width - 2 - 2 - textLen;  // 左边留 2 空格
    if (rightPad < 0) rightPad = 0;

    std::cout << CYAN << "║" << RESET << "  " << text;
    for (int i = 0; i < rightPad; i++) std::cout << " ";
    std::cout << CYAN << "║" << RESET << "\n";
}

std::string UI::progressBar(int current, int max, int length) {
    if (max <= 0) max = 1;
    if (current < 0) current = 0;
    if (current > max) current = max;

    int filled = (current * length) / max;
    std::string color;
    double ratio = (double)current / max;
    if (ratio > 0.6) color = GREEN;
    else if (ratio > 0.3) color = YELLOW;
    else color = RED;

    std::string bar = color;
    for (int i = 0; i < filled; i++) bar += "█";
    bar += DIM;
    for (int i = filled; i < length; i++) bar += "░";
    bar += RESET;
    return bar;
}

std::string UI::statusIcon(SurvivorStatus s) {
    switch (s) {
        case SurvivorStatus::HEALTHY:  return GREEN   + std::string("😊") + RESET;
        case SurvivorStatus::WEAK:     return YELLOW  + std::string("🤒") + RESET;
        case SurvivorStatus::MUTATED:  return MAGENTA + std::string("👹") + RESET;
        case SurvivorStatus::DECEASED: return DIM     + std::string("💀") + RESET;
    }
    return "?";
}

std::string UI::statusName(SurvivorStatus s) {
    switch (s) {
        case SurvivorStatus::HEALTHY:  return GREEN   + std::string("Healthy ") + RESET;
        case SurvivorStatus::WEAK:     return YELLOW  + std::string("Weak    ") + RESET;
        case SurvivorStatus::MUTATED:  return MAGENTA + std::string("Mutated ") + RESET;
        case SurvivorStatus::DECEASED: return DIM     + std::string("Deceased") + RESET;
    }
    return "Unknown";
}

std::string UI::colorize(const std::string& text, const std::string& color) {
    return color + text + RESET;
}

int UI::getValidChoice(int minChoice, int maxChoice) {
    int choice;
    while (true) {
        if (std::cin >> choice && choice >= minChoice && choice <= maxChoice) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return choice;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << RED << "  Invalid input. Enter a number between "
                  << minChoice << " and " << maxChoice << ": " << RESET;
    }
}

bool UI::getYesNo() {
    char c;
    while (true) {
        std::cin >> c;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (c == 'Y' || c == 'y') return true;
        if (c == 'N' || c == 'n') return false;
        std::cout << RED << "  Please enter Y or N: " << RESET;
    }
}

// ============================================================
// 开始菜单
// ============================================================

void UI::showStartMenu() {
    clearScreen();
    std::cout << "\n";
    std::cout << RED << BOLD;
    std::cout << "   ███████╗██╗  ██╗███████╗██╗  ████████╗███████╗██████╗ \n";
    std::cout << "   ██╔════╝██║  ██║██╔════╝██║  ╚══██╔══╝██╔════╝██╔══██╗\n";
    std::cout << "   ███████╗███████║█████╗  ██║     ██║   █████╗  ██████╔╝\n";
    std::cout << "   ╚════██║██╔══██║██╔══╝  ██║     ██║   ██╔══╝  ██╔══██╗\n";
    std::cout << "   ███████║██║  ██║███████╗███████╗██║   ███████╗██║  ██║\n";
    std::cout << "   ╚══════╝╚═╝  ╚═╝╚══════╝╚══════╝╚═╝   ╚══════╝╚═╝  ╚═╝\n";
    std::cout << RESET;
    std::cout << YELLOW << "                    10 DAYS TO SURVIVE\n" << RESET;
    std::cout << "\n";

    drawTopBorder();
    drawCentered("☢  POST-APOCALYPTIC SURVIVAL  ☢");
    drawMidBorder();
    drawLeft("The apocalypse came without warning.");
    drawLeft("Radioactive dust covers the world outside.");
    drawLeft("");
    drawLeft("Six survivors. Ten days. Limited supplies.");
    drawLeft("Every choice is a matter of life and death.");
    drawBottomBorder();
    std::cout << "\n";
    waitForEnter();
}

Difficulty UI::askDifficulty() {
    clearScreen();
    std::cout << "\n";
    drawTopBorder();
    drawCentered(BOLD + std::string("  SELECT DIFFICULTY  ") + RESET);
    drawMidBorder();
    drawLeft("");
    drawLeft(GREEN  + std::string("  1.  EASY  ") + RESET + "  More resources, milder events");
    drawLeft(RED    + std::string("  2.  HARD  ") + RESET + "  Tighter resources, harsher fate");
    drawLeft("");
    drawBottomBorder();
    std::cout << "\n  " << CYAN << "▶ " << RESET << "Your choice (1-2): ";
    int c = getValidChoice(1, 2);
    return (c == 1) ? Difficulty::EASY : Difficulty::HARD;
}

int UI::askSupplyPlan(Difficulty diff) {
    clearScreen();
    std::cout << "\n";
    drawTopBorder();
    drawCentered(BOLD + std::string("  STARTING SUPPLIES  ") + RESET);
    drawMidBorder();
    drawLeft("");
    if (diff == Difficulty::EASY) {
        drawLeft(YELLOW + std::string("  A. Basic Stockpile") + RESET);
        drawLeft("     🍞 Food x18   💧 Water x18   💊 Medicine x1");
        drawLeft("");
        drawLeft(YELLOW + std::string("  B. Medical Priority") + RESET);
        drawLeft("     🍞 Food x8    💧 Water x8    💊 Medicine x2");
    } else {
        drawLeft(YELLOW + std::string("  A. All-in") + RESET);
        drawLeft("     🍞 Food x18   💧 Water x18   💊 Medicine x0");
        drawLeft(DIM + std::string("     (One survivor starts as Weak)") + RESET);
        drawLeft("");
        drawLeft(YELLOW + std::string("  B. Frugal Start") + RESET);
        drawLeft("     🍞 Food x6    💧 Water x6    💊 Medicine x2");
    }
    drawLeft("");
    drawBottomBorder();
    std::cout << "\n  " << CYAN << "▶ " << RESET << "Your choice (1=A, 2=B): ";
    return getValidChoice(1, 2);
}

// ============================================================
// 每日报告
// ============================================================

void UI::showDailyReport(const GameState& state) {
    clearScreen();
    std::cout << "\n";

    // 标题：DAY X / 10
    std::ostringstream title;
    title << BOLD << YELLOW << "  ☀  DAY " << state.currentDay << " / 10  ☀  " << RESET;
    drawTopBorder();
    drawCentered(title.str());
    drawMidBorder();

    // 人员状态汇总
    drawLeft(BOLD + std::string("PERSONNEL STATUS") + RESET);
    drawLeft("");
    std::ostringstream summary;
    summary << "😊 " << GREEN  << state.countHealthySurvivors() << RESET
            << "   🤒 " << YELLOW << state.countWeakSurvivors() << RESET
            << "   👹 " << MAGENTA<< state.countMutatedSurvivors() << RESET
            << "   💀 " << DIM    << state.countSurvivorsByStatus(SurvivorStatus::DECEASED) << RESET;
    drawLeft(summary.str());
    drawLeft("");

    // 每个幸存者的详情
    for (size_t i = 0; i < state.survivors.size(); i++) {
        std::ostringstream line;
        line << statusIcon(state.survivors[i].status) << "  Survivor " << (i + 1)
             << "  " << statusName(state.survivors[i].status);
        if (state.survivors[i].status == SurvivorStatus::WEAK) {
            line << DIM << "  (sick day " << state.survivors[i].daysWeak << "/2)" << RESET;
        }
        drawLeft(line.str());
    }
    drawLeft("");

    drawMidBorder();
    drawLeft(BOLD + std::string("SUPPLIES") + RESET);
    drawLeft("");

    // 资源进度条（用 18 作为参考最大值）
    std::ostringstream foodLine, waterLine, medLine;
    foodLine  << "🍞 Food     " << progressBar(state.food, 18) << "  " << state.food;
    waterLine << "💧 Water    " << progressBar(state.water, 18) << "  " << state.water;
    medLine   << "💊 Medicine " << progressBar(state.medicine, 5) << "  " << state.medicine;
    drawLeft(foodLine.str());
    drawLeft(waterLine.str());
    drawLeft(medLine.str());
    drawLeft("");

    // 特殊道具
    std::ostringstream items;
    items << "📻 Radio: " << (state.hasRadio ? (GREEN + std::string("✓") + RESET) : (DIM + std::string("✗") + RESET))
          << "    📜 Note: " << ((state.hasNote && !state.usedNoteEffect) ? (GREEN + std::string("✓") + RESET) : (DIM + std::string("✗") + RESET));
    drawLeft(items.str());
    drawLeft("");
    drawBottomBorder();
    std::cout << "\n";
    waitForEnter();
}

// ============================================================
// 行动选择
// ============================================================

bool UI::askTreat(const GameState& state) {
    std::cout << "\n";
    drawTopBorder();
    drawCentered(BOLD + std::string("  💊  TREATMENT PHASE  💊  ") + RESET);
    drawMidBorder();

    int weakCount = state.countWeakSurvivors();
    std::ostringstream w, m;
    w << "🤒 Weak survivors:    " << weakCount;
    m << "💊 Medicine in stock: " << state.medicine;
    drawLeft(w.str());
    drawLeft(m.str());
    drawLeft("");

    if (weakCount == 0) {
        drawLeft(GREEN + std::string("  ✓ No one needs treatment today.") + RESET);
        drawBottomBorder();
        waitForEnter();
        return false;
    }
    if (state.medicine == 0) {
        drawLeft(RED + std::string("  ✗ No medicine left. Cannot treat.") + RESET);
        drawBottomBorder();
        waitForEnter();
        return false;
    }

    drawLeft("Use 1 medicine to heal " + std::to_string(weakCount) + " weak survivor(s)?");
    drawBottomBorder();
    std::cout << "\n  " << CYAN << "▶ " << RESET << "Treat now? (Y/N): ";
    return getYesNo();
}

bool UI::askExpedition(const GameState& state) {
    std::cout << "\n";
    drawTopBorder();
    drawCentered(BOLD + std::string("  🚪  EXPEDITION PHASE  🚪  ") + RESET);
    drawMidBorder();

    int able = state.countHealthySurvivors() + state.countMutatedSurvivors();
    int maxMembers = able / 2;
    std::ostringstream ableLine, maxLine;
    ableLine << "Able-bodied survivors: " << able;
    maxLine  << "Maximum expedition size: " << maxMembers;
    drawLeft(ableLine.str());
    drawLeft(maxLine.str());
    drawLeft("");

    if (maxMembers == 0) {
        drawLeft(RED + std::string("  ✗ Not enough able survivors to send out.") + RESET);
        drawBottomBorder();
        waitForEnter();
        return false;
    }

    drawLeft("Send an expedition to scavenge outside?");
    drawLeft(DIM + std::string("(Risky, but the only way to find supplies)") + RESET);
    drawBottomBorder();
    std::cout << "\n  " << CYAN << "▶ " << RESET << "Send out? (Y/N): ";
    return getYesNo();
}

int UI::askExpeditionCount(int maxCount) {
    std::cout << "\n  " << CYAN << "▶ " << RESET << "How many to send (1-" << maxCount << ")? ";
    return getValidChoice(1, maxCount);
}

std::vector<int> UI::askExpeditionMembers(const GameState& state, int count) {
    std::vector<int> available;
    std::cout << "\n";
    drawTopBorder();
    drawCentered(BOLD + std::string("  SELECT EXPEDITION MEMBERS  ") + RESET);
    drawMidBorder();
    drawLeft("Available members:");
    drawLeft("");
    for (size_t i = 0; i < state.survivors.size(); i++) {
        SurvivorStatus s = state.survivors[i].status;
        if (s == SurvivorStatus::HEALTHY || s == SurvivorStatus::MUTATED) {
            available.push_back(i);
            std::ostringstream line;
            line << "  " << available.size() << ". " << statusIcon(s)
                 << "  Survivor " << (i + 1) << "  " << statusName(s);
            drawLeft(line.str());
        }
    }
    drawBottomBorder();

    std::vector<int> chosen;
    for (int i = 0; i < count; i++) {
        std::cout << "\n  " << CYAN << "▶ " << RESET << "Pick member #" << (i + 1)
                  << " (1-" << available.size() << "): ";
        int pick = getValidChoice(1, (int)available.size());
        int realIndex = available[pick - 1];
        bool duplicate = false;
        for (int c : chosen) if (c == realIndex) duplicate = true;
        if (duplicate) {
            std::cout << "  " << RED << "Already chosen. Try again." << RESET << "\n";
            i--;
            continue;
        }
        chosen.push_back(realIndex);
    }
    return chosen;
}

// ============================================================
// 事件中的玩家选择
// ============================================================

bool UI::askDoorChoice() {
    std::cout << "\n";
    drawTopBorder();
    drawCentered(BOLD + std::string("  🚪 SOMEONE IS KNOCKING 🚪  ") + RESET);
    drawMidBorder();
    drawLeft("  1. Open the door");
    drawLeft("  2. Keep it shut");
    drawBottomBorder();
    std::cout << "\n  " << CYAN << "▶ " << RESET << "Your choice (1-2): ";
    return getValidChoice(1, 2) == 1;
}

int UI::askCampChoice() {
    std::cout << "\n";
    drawTopBorder();
    drawCentered(BOLD + std::string("  🏕️  OTHER SURVIVORS' CAMP  🏕️  ") + RESET);
    drawMidBorder();
    drawLeft("  1. Request help (negotiate)");
    drawLeft("  2. Rob (take by force)");
    drawBottomBorder();
    std::cout << "\n  " << CYAN << "▶ " << RESET << "Your choice (1-2): ";
    return getValidChoice(1, 2);
}

int UI::askClearChoice() {
    std::cout << "\n";
    drawTopBorder();
    drawCentered(BOLD + std::string("  🏚️  PERIMETER CLEARING  🏚️  ") + RESET);
    drawMidBorder();
    drawLeft("  1. Clear outer area  (safer, smaller reward)");
    drawLeft("  2. Search deeper     (riskier, may find a note)");
    drawBottomBorder();
    std::cout << "\n  " << CYAN << "▶ " << RESET << "Your choice (1-2): ";
    return getValidChoice(1, 2);
}

// ============================================================
// 结果展示
// ============================================================

void UI::showEventResult(const std::string& title, const std::string& text) {
    std::cout << "\n";
    drawTopBorder();
    drawCentered(YELLOW + BOLD + std::string("  ⚡  ") + title + std::string("  ⚡  ") + RESET);
    drawMidBorder();
    drawLeft("");

    // 把多行文本拆开，逐行画进框里
    std::istringstream iss(text);
    std::string line;
    while (std::getline(iss, line)) {
        if (line.empty()) {
            drawLeft("");
        } else {
            drawLeft(line);
        }
    }
    drawLeft("");
    drawBottomBorder();
    waitForEnter();
}

void UI::showDayEnd(const GameState& state) {
    std::cout << "\n";
    drawTopBorder();
    std::ostringstream title;
    title << BOLD << "🌙 END OF DAY " << state.currentDay << " 🌙" << RESET;
    drawCentered(title.str());
    drawMidBorder();
    std::ostringstream s1, s2;
    s1 << "Survivors remaining: " << GREEN << state.countLivingSurvivors() << "/6" << RESET;
    s2 << "Food: " << state.food << "  |  Water: " << state.water
       << "  |  Medicine: " << state.medicine;
    drawLeft(s1.str());
    drawLeft(s2.str());
    drawBottomBorder();
    waitForEnter();
}

// ============================================================
// 结局
// ============================================================

void UI::showEnding(const GameState& state) {
    clearScreen();
    std::cout << "\n";
    std::cout << RED << BOLD;
    std::cout << "    ██████╗  █████╗ ███╗   ███╗███████╗     ██████╗ ██╗   ██╗███████╗██████╗ \n";
    std::cout << "   ██╔════╝ ██╔══██╗████╗ ████║██╔════╝    ██╔═══██╗██║   ██║██╔════╝██╔══██╗\n";
    std::cout << "   ██║  ███╗███████║██╔████╔██║█████╗      ██║   ██║██║   ██║█████╗  ██████╔╝\n";
    std::cout << "   ██║   ██║██╔══██║██║╚██╔╝██║██╔══╝      ██║   ██║╚██╗ ██╔╝██╔══╝  ██╔══██╗\n";
    std::cout << "   ╚██████╔╝██║  ██║██║ ╚═╝ ██║███████╗    ╚██████╔╝ ╚████╔╝ ███████╗██║  ██║\n";
    std::cout << "    ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝╚══════╝     ╚═════╝   ╚═══╝  ╚══════╝╚═╝  ╚═╝\n";
    std::cout << RESET << "\n";

    drawTopBorder();
    drawCentered(BOLD + std::string("  THE STORY ENDS  ") + RESET);
    drawMidBorder();
    drawLeft("");
    // 把结局文字按行画
    std::istringstream iss(state.endingMessage);
    std::string line;
    while (std::getline(iss, line)) {
        drawLeft(line);
    }
    drawLeft("");
    drawMidBorder();
    drawLeft(BOLD + std::string("FINAL STATS") + RESET);
    std::ostringstream f1, f2, f3;
    f1 << "Days survived:  " << state.currentDay;
    f2 << "Survivors:      " << state.countLivingSurvivors() << "/6";
    f3 << "Mutated:        " << state.countMutatedSurvivors();
    drawLeft(f1.str());
    drawLeft(f2.str());
    drawLeft(f3.str());
    drawBottomBorder();
    std::cout << "\n";
    waitForEnter();
}

void UI::showMessage(const std::string& msg) {
    std::cout << "  " << CYAN << "▶ " << RESET << msg << "\n";
}

void UI::waitForEnter() {
    std::cout << "\n  " << DIM << "[ Press Enter to continue... ]" << RESET;
    std::cin.get();
}
