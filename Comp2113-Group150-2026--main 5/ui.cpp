// ui.cpp
// Implementation of the UI class. Renders all screens and handles input.
// Uses ANSI escape codes for color and Unicode box-drawing characters
// for borders. Output is plain text; works in any UTF-8 capable terminal.

#include "ui.h"
#include <iostream>
#include <limits>
#include <cstdlib>
#include <sstream>

// ============================================================
// Private helpers
// ============================================================

// Clear the terminal screen.
void UI::clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        std::cout << "\033[2J\033[1;1H";
    #endif
}

// Decide the display width (in terminal columns) of a Unicode code point.
// We need this because some characters look like emoji but actually render
// as one column (e.g. ☢ ✓ ✗ ▶), while others render as two (e.g. 😊 🍞).
// Wrong width estimates make our framed boxes misalign.
static int charDisplayWidth(unsigned int cp) {
    // Single-column blocks (these used to break alignment).
    if (cp >= 0x2500 && cp <= 0x259F) return 1;  // Box drawing
    if (cp >= 0x2600 && cp <= 0x26FF) return 1;  // Misc Symbols (☀ ☢ ★)
    if (cp >= 0x2700 && cp <= 0x27BF) return 1;  // Dingbats (✓ ✗ ▶ ⚡)

    // Wide CJK ranges - always 2 columns.
    if (cp >= 0x1100 && cp <= 0x115F) return 2;
    if (cp >= 0x2E80 && cp <= 0x303E) return 2;
    if (cp >= 0x3041 && cp <= 0x33FF) return 2;
    if (cp >= 0x3400 && cp <= 0x4DBF) return 2;
    if (cp >= 0x4E00 && cp <= 0x9FFF) return 2;
    if (cp >= 0xA000 && cp <= 0xA4CF) return 2;
    if (cp >= 0xAC00 && cp <= 0xD7A3) return 2;
    if (cp >= 0xF900 && cp <= 0xFAFF) return 2;
    if (cp >= 0xFE30 && cp <= 0xFE4F) return 2;
    if (cp >= 0xFF00 && cp <= 0xFF60) return 2;
    if (cp >= 0xFFE0 && cp <= 0xFFE6) return 2;

    // Emoji blocks.
    if (cp >= 0x1F300 && cp <= 0x1F64F) return 2;
    if (cp >= 0x1F680 && cp <= 0x1F6FF) return 2;
    if (cp >= 0x1F700 && cp <= 0x1F77F) return 2;
    if (cp >= 0x1F900 && cp <= 0x1F9FF) return 2;
    if (cp >= 0x1FA70 && cp <= 0x1FAFF) return 2;

    // Zero-width helpers used in emoji sequences.
    if (cp == 0xFE0F) return 0;
    if (cp == 0x200D) return 0;
    if (cp >= 0x0300 && cp <= 0x036F) return 0;

    return 1;
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
        unsigned int cp = 0;
        int extra = 0;
        if ((c & 0x80) == 0) {
            cp = c;
        } else if ((c & 0xE0) == 0xC0) {
            cp = c & 0x1F; extra = 1;
        } else if ((c & 0xF0) == 0xE0) {
            cp = c & 0x0F; extra = 2;
        } else if ((c & 0xF8) == 0xF0) {
            cp = c & 0x07; extra = 3;
        } else {
            len += 1;
            continue;
        }
        for (int k = 0; k < extra; k++) {
            if (i + 1 >= text.size()) break;
            unsigned char nb = text[i + 1];
            if ((nb & 0xC0) != 0x80) break;
            cp = (cp << 6) | (nb & 0x3F);
            i++;
        }
        len += charDisplayWidth(cp);
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
    int rightPad = width - 2 - 2 - textLen;
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

// Map a survivor trait to a colored short label.
std::string UI::traitName(SurvivorTrait t) {
    switch (t) {
        case SurvivorTrait::DOCTOR:   return CYAN    + std::string("Doctor")   + RESET;
        case SurvivorTrait::FRAIL:    return RED     + std::string("Frail")    + RESET;
        case SurvivorTrait::SCOUT:    return GREEN   + std::string("Scout")    + RESET;
        case SurvivorTrait::ENGINEER: return YELLOW  + std::string("Engineer") + RESET;
        case SurvivorTrait::SOLDIER:  return MAGENTA + std::string("Soldier")  + RESET;
        case SurvivorTrait::LUCKY:    return BLUE    + std::string("Lucky")    + RESET;
        default: return "";
    }
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
        if (std::cin.eof()) return minChoice;  // Avoid infinite loop on EOF
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << RED << "  Invalid input. Enter a number between "
                  << minChoice << " and " << maxChoice << ": " << RESET;
    }
}

bool UI::getYesNo() {
    char c;
    while (true) {
        if (!(std::cin >> c)) return false;  // EOF
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (c == 'Y' || c == 'y') return true;
        if (c == 'N' || c == 'n') return false;
        std::cout << RED << "  Please enter Y or N: " << RESET;
    }
}

// ============================================================
// Start menu
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

bool UI::askContinueGame() {
    clearScreen();
    std::cout << "\n";
    drawTopBorder();
    drawCentered(BOLD + std::string("  💾  SAVE FILE FOUND  💾  ") + RESET);
    drawMidBorder();
    drawLeft("");
    drawLeft("A previous game was saved on this machine.");
    drawLeft("");
    drawLeft(GREEN + std::string("  Y") + RESET + " - Continue from where you left off");
    drawLeft(RED   + std::string("  N") + RESET + " - Start a new game (saved data will be erased)");
    drawLeft("");
    drawBottomBorder();
    std::cout << "\n  " << CYAN << "▶ " << RESET << "Continue saved game? (Y/N): ";
    return getYesNo();
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
// Daily report
// ============================================================

void UI::showDailyReport(const GameState& state) {
    clearScreen();
    std::cout << "\n";

    std::ostringstream title;
    title << BOLD << YELLOW << "  ☀  DAY " << state.currentDay << " / 10  ☀  " << RESET;
    drawTopBorder();
    drawCentered(title.str());
    drawMidBorder();

    drawLeft(BOLD + std::string("PERSONNEL STATUS") + RESET);
    drawLeft("");
    std::ostringstream summary;
    summary << "😊 " << GREEN  << state.countHealthySurvivors() << RESET
            << "   🤒 " << YELLOW << state.countWeakSurvivors() << RESET
            << "   👹 " << MAGENTA<< state.countMutatedSurvivors() << RESET
            << "   💀 " << DIM    << state.countSurvivorsByStatus(SurvivorStatus::DECEASED) << RESET;
    drawLeft(summary.str());
    drawLeft("");

    // Per-survivor detail with name and trait.
    for (size_t i = 0; i < state.survivors.size(); i++) {
        std::ostringstream line;
        std::string name = state.survivors[i].name;
        while ((int)name.size() < 7) name += " ";

        line << statusIcon(state.survivors[i].status) << "  "
             << name << " " << statusName(state.survivors[i].status);
        if (state.survivors[i].status != SurvivorStatus::DECEASED) {
            line << " [" << traitName(state.survivors[i].trait) << "]";
        }
        if (state.survivors[i].status == SurvivorStatus::WEAK) {
            line << DIM << "  (sick " << state.survivors[i].daysWeak << "/2)" << RESET;
        }
        drawLeft(line.str());
    }
    drawLeft("");

    drawMidBorder();
    drawLeft(BOLD + std::string("SUPPLIES") + RESET);
    drawLeft("");

    std::ostringstream foodLine, waterLine, medLine;
    foodLine  << "🍞 Food     " << progressBar(state.food, 18) << "  " << state.food;
    waterLine << "💧 Water    " << progressBar(state.water, 18) << "  " << state.water;
    medLine   << "💊 Medicine " << progressBar(state.medicine, 5) << "  " << state.medicine;
    drawLeft(foodLine.str());
    drawLeft(waterLine.str());
    drawLeft(medLine.str());
    drawLeft("");

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
// Daytime actions
// ============================================================

bool UI::askTreat(const GameState& state) {
    std::cout << "\n";
    drawTopBorder();
    drawCentered(BOLD + std::string("  💊  TREATMENT PHASE  💊  ") + RESET);
    drawMidBorder();

    int weakCount = state.countWeakSurvivors();
    bool hasDoctor = state.hasLivingSurvivorWithTrait(SurvivorTrait::DOCTOR);

    std::ostringstream w, m;
    w << "🤒 Weak survivors:    " << weakCount;
    m << "💊 Medicine in stock: " << state.medicine;
    drawLeft(w.str());
    drawLeft(m.str());
    if (hasDoctor) {
        drawLeft(CYAN + std::string("(A doctor is alive — treatment is free!)") + RESET);
    }
    drawLeft("");

    if (weakCount == 0) {
        drawLeft(GREEN + std::string("  ✓ No one needs treatment today.") + RESET);
        drawBottomBorder();
        waitForEnter();
        return false;
    }
    if (state.medicine == 0 && !hasDoctor) {
        drawLeft(RED + std::string("  ✗ No medicine left and no doctor. Cannot treat.") + RESET);
        drawBottomBorder();
        waitForEnter();
        return false;
    }

    drawLeft("Heal " + std::to_string(weakCount) + " weak survivor(s)?");
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
            std::string name = state.survivors[i].name;
            while ((int)name.size() < 7) name += " ";
            line << "  " << available.size() << ". " << statusIcon(s)
                 << "  " << name << " " << statusName(s)
                 << " [" << traitName(state.survivors[i].trait) << "]";
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

// Show the 7 expedition destinations and let the player pick.
ExpeditionEventType UI::askExpeditionDestination(const GameState& state) {
    std::cout << "\n";
    drawTopBorder();
    drawCentered(BOLD + std::string("  🗺  CHOOSE A DESTINATION  🗺  ") + RESET);
    drawMidBorder();
    drawLeft("Where will the expedition go today?");
    drawLeft("");
    drawLeft("  1. " + YELLOW + std::string("Supermarket") + RESET
             + "       (food gain, costs water)");
    drawLeft("  2. " + BLUE   + std::string("Water Plant") + RESET
             + "       (water gain, costs food)");
    drawLeft("  3. " + GREEN  + std::string("Pharmacy") + RESET
             + "          (medicine gain, costs both)");
    drawLeft("  4. " + RED    + std::string("Other Camp") + RESET
             + "        (negotiate or rob)");
    drawLeft("  5. " + CYAN   + std::string("Perimeter") + RESET
             + "         (small reward, low risk)");
    drawLeft("  6. " + MAGENTA+ std::string("Laboratory") + RESET
             + "        (DANGER: members may mutate)");

    bool noteAvailable = state.hasNote && !state.usedNoteEffect;
    int maxChoice = 6;
    if (noteAvailable) {
        drawLeft("  7. " + GREEN  + std::string("Hidden Storage") + RESET
                 + "    (note required, big reward)");
        maxChoice = 7;
    } else {
        drawLeft(DIM + std::string("  7. Hidden Storage   (locked: requires a note)") + RESET);
    }

    drawBottomBorder();
    std::cout << "\n  " << CYAN << "▶ " << RESET
              << "Your choice (1-" << maxChoice << "): ";
    int choice = getValidChoice(1, maxChoice);

    switch (choice) {
        case 1: return ExpeditionEventType::SUPERMARKET;
        case 2: return ExpeditionEventType::WATER_PLANT;
        case 3: return ExpeditionEventType::PHARMACY;
        case 4: return ExpeditionEventType::OTHER_CAMP;
        case 5: return ExpeditionEventType::PERIMETER_CLEAR;
        case 6: return ExpeditionEventType::LABORATORY;
        case 7: return ExpeditionEventType::HIDDEN_STORAGE;
        default: return ExpeditionEventType::SUPERMARKET;
    }
}

// ============================================================
// In-event player choices
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
    drawCentered(BOLD + std::string("  🏕  OTHER SURVIVORS' CAMP  🏕  ") + RESET);
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
    drawCentered(BOLD + std::string("  🏚  PERIMETER CLEARING  🏚  ") + RESET);
    drawMidBorder();
    drawLeft("  1. Clear outer area  (safer, smaller reward)");
    drawLeft("  2. Search deeper     (riskier, may find a note)");
    drawBottomBorder();
    std::cout << "\n  " << CYAN << "▶ " << RESET << "Your choice (1-2): ";
    return getValidChoice(1, 2);
}

// ============================================================
// Result and message display
// ============================================================

void UI::showEventResult(const std::string& title, const std::string& text) {
    std::cout << "\n";
    drawTopBorder();
    drawCentered(YELLOW + BOLD + std::string("  ⚡  ") + title + std::string("  ⚡  ") + RESET);
    drawMidBorder();
    drawLeft("");

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
    drawLeft("");
    drawLeft(DIM + std::string("Game saved automatically.") + RESET);
    drawBottomBorder();
    waitForEnter();
}

// ============================================================
// Endings
// ============================================================

// Theme info for one ending: banner color, subtitle, and 6 ASCII rows.
struct EndingTheme {
    std::string color;
    std::string subtitle;
    std::string banner[6];
};

// Returns the visual theme matching the given ending type.
static EndingTheme getEndingTheme(EndingType type) {
    EndingTheme t;
    switch (type) {
        case EndingType::TRAGIC_END:
            t.color = "\033[31m";
            t.subtitle = "  💀  TRAGIC END  💀  ";
            t.banner[0] = "    ██████╗  █████╗ ███╗   ███╗███████╗     ██████╗ ██╗   ██╗███████╗██████╗ ";
            t.banner[1] = "   ██╔════╝ ██╔══██╗████╗ ████║██╔════╝    ██╔═══██╗██║   ██║██╔════╝██╔══██╗";
            t.banner[2] = "   ██║  ███╗███████║██╔████╔██║█████╗      ██║   ██║██║   ██║█████╗  ██████╔╝";
            t.banner[3] = "   ██║   ██║██╔══██║██║╚██╔╝██║██╔══╝      ██║   ██║╚██╗ ██╔╝██╔══╝  ██╔══██╗";
            t.banner[4] = "   ╚██████╔╝██║  ██║██║ ╚═╝ ██║███████╗    ╚██████╔╝ ╚████╔╝ ███████╗██║  ██║";
            t.banner[5] = "    ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝╚══════╝     ╚═════╝   ╚═══╝  ╚══════╝╚═╝  ╚═╝";
            break;

        case EndingType::ORDER_RESTORED:
            t.color = "\033[32m";
            t.subtitle = "  🌅  ORDER RESTORED  🌅  ";
            t.banner[0] = "   ██╗   ██╗██╗ ██████╗████████╗ ██████╗ ██████╗ ██╗   ██╗";
            t.banner[1] = "   ██║   ██║██║██╔════╝╚══██╔══╝██╔═══██╗██╔══██╗╚██╗ ██╔╝";
            t.banner[2] = "   ██║   ██║██║██║        ██║   ██║   ██║██████╔╝ ╚████╔╝ ";
            t.banner[3] = "   ╚██╗ ██╔╝██║██║        ██║   ██║   ██║██╔══██╗  ╚██╔╝  ";
            t.banner[4] = "    ╚████╔╝ ██║╚██████╗   ██║   ╚██████╔╝██║  ██║   ██║   ";
            t.banner[5] = "     ╚═══╝  ╚═╝ ╚═════╝   ╚═╝    ╚═════╝ ╚═╝  ╚═╝   ╚═╝   ";
            break;

        case EndingType::LONE_SURVIVOR:
            t.color = "\033[33m";
            t.subtitle = "  🌙  LONE SURVIVOR  🌙  ";
            t.banner[0] = "      █████╗ ██╗      ██████╗ ███╗   ██╗███████╗";
            t.banner[1] = "     ██╔══██╗██║     ██╔═══██╗████╗  ██║██╔════╝";
            t.banner[2] = "     ███████║██║     ██║   ██║██╔██╗ ██║█████╗  ";
            t.banner[3] = "     ██╔══██║██║     ██║   ██║██║╚██╗██║██╔══╝  ";
            t.banner[4] = "     ██║  ██║███████╗╚██████╔╝██║ ╚████║███████╗";
            t.banner[5] = "     ╚═╝  ╚═╝╚══════╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝";
            break;

        case EndingType::SYMBIOTIC_EVOLUTION:
            t.color = "\033[35m";
            t.subtitle = "  👹  SYMBIOTIC EVOLUTION  👹  ";
            t.banner[0] = "   ███╗   ███╗██╗   ██╗████████╗ █████╗ ████████╗███████╗";
            t.banner[1] = "   ████╗ ████║██║   ██║╚══██╔══╝██╔══██╗╚══██╔══╝██╔════╝";
            t.banner[2] = "   ██╔████╔██║██║   ██║   ██║   ███████║   ██║   █████╗  ";
            t.banner[3] = "   ██║╚██╔╝██║██║   ██║   ██║   ██╔══██║   ██║   ██╔══╝  ";
            t.banner[4] = "   ██║ ╚═╝ ██║╚██████╔╝   ██║   ██║  ██║   ██║   ███████╗";
            t.banner[5] = "   ╚═╝     ╚═╝ ╚═════╝    ╚═╝   ╚═╝  ╚═╝   ╚═╝   ╚══════╝";
            break;

        case EndingType::MARAUDERS:
            t.color = "\033[1m\033[31m";
            t.subtitle = "  ⚔  MARAUDERS  ⚔  ";
            t.banner[0] = "   ██████╗  █████╗ ███╗   ██╗██████╗ ██╗████████╗███████╗";
            t.banner[1] = "   ██╔══██╗██╔══██╗████╗  ██║██╔══██╗██║╚══██╔══╝██╔════╝";
            t.banner[2] = "   ██████╔╝███████║██╔██╗ ██║██║  ██║██║   ██║   ███████╗";
            t.banner[3] = "   ██╔══██╗██╔══██║██║╚██╗██║██║  ██║██║   ██║   ╚════██║";
            t.banner[4] = "   ██████╔╝██║  ██║██║ ╚████║██████╔╝██║   ██║   ███████║";
            t.banner[5] = "   ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═══╝╚═════╝ ╚═╝   ╚═╝   ╚══════╝";
            break;

        case EndingType::STRUGGLE_FOR_SURVIVAL:
        default:
            t.color = "\033[36m";
            t.subtitle = "  ☀  STRUGGLE FOR SURVIVAL  ☀  ";
            t.banner[0] = "   ███████╗██╗   ██╗██████╗ ██╗   ██╗██╗██╗   ██╗███████╗██████╗ ";
            t.banner[1] = "   ██╔════╝██║   ██║██╔══██╗██║   ██║██║██║   ██║██╔════╝██╔══██╗";
            t.banner[2] = "   ███████╗██║   ██║██████╔╝██║   ██║██║██║   ██║█████╗  ██║  ██║";
            t.banner[3] = "   ╚════██║██║   ██║██╔══██╗╚██╗ ██╔╝██║╚██╗ ██╔╝██╔══╝  ██║  ██║";
            t.banner[4] = "   ███████║╚██████╔╝██║  ██║ ╚████╔╝ ██║ ╚████╔╝ ███████╗██████╔╝";
            t.banner[5] = "   ╚══════╝ ╚═════╝ ╚═╝  ╚═╝  ╚═══╝  ╚═╝  ╚═══╝  ╚══════╝╚═════╝ ";
            break;
    }
    return t;
}

void UI::showEnding(const GameState& state) {
    clearScreen();
    std::cout << "\n";

    EndingTheme theme = getEndingTheme(state.endingType);

    std::cout << theme.color << BOLD;
    for (int i = 0; i < 6; i++) {
        std::cout << theme.banner[i] << "\n";
    }
    std::cout << RESET << "\n";

    drawTopBorder();
    drawCentered(theme.color + BOLD + theme.subtitle + RESET);
    drawMidBorder();
    drawLeft("");
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
