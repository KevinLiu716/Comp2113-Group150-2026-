// ui.cpp
// Implementation of the UI class. Renders all screens and handles input.
// Uses ANSI escape codes for color and pure ASCII characters for borders
// and icons. Output is plain 7-bit ASCII; works in any terminal,
// including Linux consoles with no UTF-8 support.

#include "ui.h"
#include <iostream>
#include <limits>
#include <sstream>

// ============================================================
// Quit flag
//
// When the player types 'q' or 'Q' at any input prompt, this flag is
// raised and every subsequent input function returns a safe default
// without blocking. The main loop polls UI::isQuitRequested() after
// each UI call to break out cleanly and save the game.
// ============================================================
static bool g_quitRequested = false;

bool UI::isQuitRequested() { return g_quitRequested; }
void UI::resetQuitFlag()   { g_quitRequested = false; }

// Helper: returns true if the given string starts with q or Q (the
// universal "save and quit" command). Used by every input function.
static bool isQuitInput(const std::string& s) {
    if (s.empty()) return false;
    char c = s[0];
    return c == 'q' || c == 'Q';
}

// ============================================================
// Private helpers
// ============================================================

// Clear the terminal screen using the ANSI escape sequence.
void UI::clearScreen() {
    std::cout << "\033[2J\033[1;1H";
}

// Visible length of a string in terminal columns. Since the UI now uses
// pure ASCII, every printable byte is exactly one column wide; we only
// have to skip over ANSI color escape sequences (ESC [ ... m).
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
        len += 1;
    }
    return len;
}

void UI::drawTopBorder(int width) {
    std::cout << CYAN << "+";
    for (int i = 0; i < width - 2; i++) std::cout << "=";
    std::cout << "+" << RESET << "\n";
}

void UI::drawBottomBorder(int width) {
    std::cout << CYAN << "+";
    for (int i = 0; i < width - 2; i++) std::cout << "=";
    std::cout << "+" << RESET << "\n";
}

void UI::drawMidBorder(int width) {
    std::cout << CYAN << "+";
    for (int i = 0; i < width - 2; i++) std::cout << "-";
    std::cout << "+" << RESET << "\n";
}

void UI::drawCentered(const std::string& text, int width) {
    int textLen = visibleLength(text);
    int pad = (width - 2 - textLen) / 2;
    if (pad < 0) pad = 0;
    int rightPad = width - 2 - textLen - pad;
    if (rightPad < 0) rightPad = 0;

    std::cout << CYAN << "|" << RESET;
    for (int i = 0; i < pad; i++) std::cout << " ";
    std::cout << text;
    for (int i = 0; i < rightPad; i++) std::cout << " ";
    std::cout << CYAN << "|" << RESET << "\n";
}

// Print one already-fitting framed line, with a left margin of 2 spaces
// and a right pad to align the closing "|" at column `width`.
static void emitFramedLine(const std::string& s,
                           int visibleLen,
                           int width,
                           const std::string& cyan,
                           const std::string& reset) {
    int rightPad = width - 2 - 2 - visibleLen;
    if (rightPad < 0) rightPad = 0;
    std::cout << cyan << "|" << reset << "  " << s;
    for (int i = 0; i < rightPad; i++) std::cout << " ";
    std::cout << cyan << "|" << reset << "\n";
}

// Print a left-justified framed line. If `text` is too long to fit the
// inner content width (= width - 4), it is automatically wrapped at
// space boundaries onto multiple framed lines so that nothing ever
// overflows the right border. ANSI color escape sequences are preserved
// (they have zero visible width and are passed through).
void UI::drawLeft(const std::string& text, int width) {
    const int innerWidth = width - 4;  // 2 for borders, 2 for left margin

    int textLen = visibleLength(text);
    if (textLen <= innerWidth) {
        emitFramedLine(text, textLen, width, CYAN, RESET);
        return;
    }

    // The line is too long; wrap on spaces. We walk the string keeping
    // track of three things in parallel:
    //   - the byte position `i`
    //   - the visible column `col` reached if we printed up to `i`
    //   - the most recent space position in the *current chunk*, used as
    //     a break point when we overflow.
    const std::string& t = text;
    size_t i = 0;
    while (i < t.size()) {
        size_t chunkStart = i;
        size_t lastSpaceByte = std::string::npos;  // byte index AFTER the space
        int    lastSpaceCol  = 0;                  // visible col at that point
        int    col = 0;
        bool   inEscape = false;

        size_t j = i;
        while (j < t.size() && col < innerWidth) {
            unsigned char c = (unsigned char)t[j];
            if (c == '\033') { inEscape = true; j++; continue; }
            if (inEscape) {
                if (c == 'm') inEscape = false;
                j++;
                continue;
            }
            if (c == ' ') {
                lastSpaceByte = j + 1;  // resume after the space
                lastSpaceCol  = col;
            }
            col++;
            j++;
        }

        size_t chunkEnd;
        size_t nextStart;
        int    chunkVisLen;

        if (j >= t.size()) {
            // Remaining text fits entirely on this line.
            chunkEnd    = t.size();
            nextStart   = t.size();
            chunkVisLen = col;
        } else if (lastSpaceByte != std::string::npos) {
            // Break at the most recent space we saw.
            chunkEnd    = lastSpaceByte - 1;   // drop the space itself
            nextStart   = lastSpaceByte;
            chunkVisLen = lastSpaceCol;
            // Skip any extra leading spaces on the next line.
            while (nextStart < t.size() && t[nextStart] == ' ') nextStart++;
        } else {
            // No space in this chunk — must hard-break at the column limit.
            chunkEnd    = j;
            nextStart   = j;
            chunkVisLen = col;
        }

        std::string chunk = t.substr(chunkStart, chunkEnd - chunkStart);
        emitFramedLine(chunk, chunkVisLen, width, CYAN, RESET);
        i = nextStart;
    }
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
    for (int i = 0; i < filled; i++) bar += "#";
    bar += DIM;
    for (int i = filled; i < length; i++) bar += "-";
    bar += RESET;
    return bar;
}

std::string UI::statusIcon(SurvivorStatus s) {
    switch (s) {
        case SurvivorStatus::HEALTHY:  return GREEN   + std::string("[OK]") + RESET;
        case SurvivorStatus::WEAK:     return YELLOW  + std::string("[WK]") + RESET;
        case SurvivorStatus::MUTATED:  return MAGENTA + std::string("[MU]") + RESET;
        case SurvivorStatus::DECEASED: return DIM     + std::string("[XX]") + RESET;
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
    std::string input;
    while (true) {
        if (!(std::cin >> input)) {
            // EOF: treat as quit too, so the program does not hang.
            g_quitRequested = true;
            return minChoice;
        }
        // Check for the universal quit command first.
        if (isQuitInput(input)) {
            g_quitRequested = true;
            return minChoice;  // Return any safe value; caller will exit.
        }
        // Try to parse as integer.
        try {
            int choice = std::stoi(input);
            if (choice >= minChoice && choice <= maxChoice) {
                return choice;
            }
        } catch (...) {
            // Not a number - fall through to error message.
        }
        std::cout << RED << "  Invalid input. Enter a number between "
                  << minChoice << " and " << maxChoice
                  << " (or 'q' to save & quit): " << RESET;
    }
}

bool UI::getYesNo() {
    std::string input;
    while (true) {
        if (!(std::cin >> input)) {
            g_quitRequested = true;
            return false;
        }
        if (isQuitInput(input)) {
            g_quitRequested = true;
            return false;
        }
        if (input.size() == 1) {
            char c = input[0];
            if (c == 'Y' || c == 'y') return true;
            if (c == 'N' || c == 'n') return false;
        }
        std::cout << RED << "  Please enter Y or N (or 'q' to save & quit): " << RESET;
    }
}

// ============================================================
// Start menu
// ============================================================

void UI::showStartMenu() {
    clearScreen();
    std::cout << "\n";
    std::cout << RED << BOLD;
    std::cout << "   ####   ##  ##  ######  ##     ######  ######  #####    \n";
    std::cout << "  ##      ##  ##  ##      ##       ##    ##      ##  ##   \n";
    std::cout << "   ###    ######  ####    ##       ##    ####    #####    \n";
    std::cout << "     ##   ##  ##  ##      ##       ##    ##      ##  ##   \n";
    std::cout << "  ####    ##  ##  ######  ######   ##    ######  ##   ##  \n";
    std::cout << RESET;
    std::cout << YELLOW << "                    10 DAYS TO SURVIVE\n" << RESET;
    std::cout << "\n";

    drawTopBorder();
    drawCentered("**  POST-APOCALYPTIC SURVIVAL  **");
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
    drawCentered(BOLD + std::string("  [SAVE]  SAVE FILE FOUND  [SAVE]  ") + RESET);
    drawMidBorder();
    drawLeft("");
    drawLeft("A previous game was saved on this machine.");
    drawLeft("");
    drawLeft(GREEN + std::string("  Y") + RESET + " - Continue from where you left off");
    drawLeft(RED   + std::string("  N") + RESET + " - Start a new game (saved data will be erased)");
    drawLeft("");
    drawBottomBorder();
    std::cout << "\n  " << CYAN << "> " << RESET << "Continue saved game? (Y/N): ";
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
    std::cout << "\n  " << CYAN << "> " << RESET << "Your choice (1-2): ";
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
        drawLeft("     [F] Food x18   [W] Water x18   [M] Medicine x1");
        drawLeft("");
        drawLeft(YELLOW + std::string("  B. Medical Priority") + RESET);
        drawLeft("     [F] Food x8    [W] Water x8    [M] Medicine x2");
    } else {
        drawLeft(YELLOW + std::string("  A. All-in") + RESET);
        drawLeft("     [F] Food x18   [W] Water x18   [M] Medicine x0");
        drawLeft(DIM + std::string("     (One survivor starts as Weak)") + RESET);
        drawLeft("");
        drawLeft(YELLOW + std::string("  B. Frugal Start") + RESET);
        drawLeft("     [F] Food x6    [W] Water x6    [M] Medicine x2");
    }
    drawLeft("");
    drawBottomBorder();
    std::cout << "\n  " << CYAN << "> " << RESET << "Your choice (1=A, 2=B): ";
    return getValidChoice(1, 2);
}

// ============================================================
// Daily report
// ============================================================

void UI::showDailyReport(const GameState& state) {
    clearScreen();
    std::cout << "\n";

    std::ostringstream title;
    title << BOLD << YELLOW << "  *  DAY " << state.currentDay << " / 10  *  " << RESET;
    drawTopBorder();
    drawCentered(title.str());
    drawMidBorder();

    drawLeft(BOLD + std::string("PERSONNEL STATUS") + RESET);
    drawLeft("");
    std::ostringstream summary;
    summary << "[OK] " << GREEN  << state.countHealthySurvivors() << RESET
            << "   [WK] " << YELLOW << state.countWeakSurvivors() << RESET
            << "   [MU] " << MAGENTA<< state.countMutatedSurvivors() << RESET
            << "   [XX] " << DIM    << state.countSurvivorsByStatus(SurvivorStatus::DECEASED) << RESET;
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
    foodLine  << "[F] Food     " << progressBar(state.food, 18) << "  " << state.food;
    waterLine << "[W] Water    " << progressBar(state.water, 18) << "  " << state.water;
    medLine   << "[M] Medicine " << progressBar(state.medicine, 5) << "  " << state.medicine;
    drawLeft(foodLine.str());
    drawLeft(waterLine.str());
    drawLeft(medLine.str());
    drawLeft("");

    std::ostringstream items;
    items << "[R] Radio: " << (state.hasRadio ? (GREEN + std::string("Y") + RESET) : (DIM + std::string("N") + RESET))
          << "    [N] Note: " << ((state.hasNote && !state.usedNoteEffect) ? (GREEN + std::string("Y") + RESET) : (DIM + std::string("N") + RESET));
    drawLeft(items.str());
    drawLeft("");
    drawMidBorder();
    drawLeft(DIM + std::string("Tip: Type 'q' at any prompt to save & quit.") + RESET);
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
    drawCentered(BOLD + std::string("  [M]  TREATMENT PHASE  [M]  ") + RESET);
    drawMidBorder();

    int weakCount = state.countWeakSurvivors();
    bool hasDoctor = state.hasLivingSurvivorWithTrait(SurvivorTrait::DOCTOR);

    std::ostringstream w, m;
    w << "[WK] Weak survivors:    " << weakCount;
    m << "[M]  Medicine in stock: " << state.medicine;
    drawLeft(w.str());
    drawLeft(m.str());
    if (hasDoctor) {
        drawLeft(CYAN + std::string("(A doctor is alive - treatment is free!)") + RESET);
    }
    drawLeft("");

    if (weakCount == 0) {
        drawLeft(GREEN + std::string("  [OK] No one needs treatment today.") + RESET);
        drawBottomBorder();
        waitForEnter();
        return false;
    }
    if (state.medicine == 0 && !hasDoctor) {
        drawLeft(RED + std::string("  [X] No medicine left and no doctor. Cannot treat.") + RESET);
        drawBottomBorder();
        waitForEnter();
        return false;
    }

    drawLeft("Heal " + std::to_string(weakCount) + " weak survivor(s)?");
    drawBottomBorder();
    std::cout << "\n  " << CYAN << "> " << RESET << "Treat now? (Y/N): ";
    return getYesNo();
}

bool UI::askExpedition(const GameState& state) {
    std::cout << "\n";
    drawTopBorder();
    drawCentered(BOLD + std::string("  [E]  EXPEDITION PHASE  [E]  ") + RESET);
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
        drawLeft(RED + std::string("  [X] Not enough able survivors to send out.") + RESET);
        drawBottomBorder();
        waitForEnter();
        return false;
    }

    drawLeft("Send an expedition to scavenge outside?");
    drawLeft(DIM + std::string("(Risky, but the only way to find supplies)") + RESET);
    drawBottomBorder();
    std::cout << "\n  " << CYAN << "> " << RESET << "Send out? (Y/N): ";
    return getYesNo();
}

int UI::askExpeditionCount(int maxCount) {
    std::cout << "\n  " << CYAN << "> " << RESET << "How many to send (1-" << maxCount << ")? ";
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
        std::cout << "\n  " << CYAN << "> " << RESET << "Pick member #" << (i + 1)
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
    drawCentered(BOLD + std::string("  [MAP]  CHOOSE A DESTINATION  [MAP]  ") + RESET);
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
    std::cout << "\n  " << CYAN << "> " << RESET
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
    drawCentered(BOLD + std::string("  [!] SOMEONE IS KNOCKING [!]  ") + RESET);
    drawMidBorder();
    drawLeft("  1. Open the door");
    drawLeft("  2. Keep it shut");
    drawBottomBorder();
    std::cout << "\n  " << CYAN << "> " << RESET << "Your choice (1-2): ";
    return getValidChoice(1, 2) == 1;
}

int UI::askCampChoice() {
    std::cout << "\n";
    drawTopBorder();
    drawCentered(BOLD + std::string("  [C]  OTHER SURVIVORS' CAMP  [C]  ") + RESET);
    drawMidBorder();
    drawLeft("  1. Request help (negotiate)");
    drawLeft("  2. Rob (take by force)");
    drawBottomBorder();
    std::cout << "\n  " << CYAN << "> " << RESET << "Your choice (1-2): ";
    return getValidChoice(1, 2);
}

int UI::askClearChoice() {
    std::cout << "\n";
    drawTopBorder();
    drawCentered(BOLD + std::string("  [P]  PERIMETER CLEARING  [P]  ") + RESET);
    drawMidBorder();
    drawLeft("  1. Clear outer area  (safer, smaller reward)");
    drawLeft("  2. Search deeper     (riskier, may find a note)");
    drawBottomBorder();
    std::cout << "\n  " << CYAN << "> " << RESET << "Your choice (1-2): ";
    return getValidChoice(1, 2);
}

// ============================================================
// Result and message display
// ============================================================

void UI::showEventResult(const std::string& title, const std::string& text) {
    std::cout << "\n";
    drawTopBorder();
    drawCentered(YELLOW + BOLD + std::string("  !  ") + title + std::string("  !  ") + RESET);
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
    title << BOLD << "(  END OF DAY " << state.currentDay << "  )" << RESET;
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
// All banners are made of pure ASCII (#, space) to avoid rendering issues
// on Linux terminals that lack UTF-8 box-drawing fonts.
static EndingTheme getEndingTheme(EndingType type) {
    EndingTheme t;
    switch (type) {
        case EndingType::TRAGIC_END:
            t.color = "\033[31m";
            t.subtitle = "  [XX]  TRAGIC END  [XX]  ";
            t.banner[0] = "    #####     #####    #     #   ######        #####    #     #   ######   #####  ";
            t.banner[1] = "   #          #     #   ##   ##   #            #     #   #     #   #        #    # ";
            t.banner[2] = "   #   ###    #######   # # # #   ####         #     #   #     #   ####     #####  ";
            t.banner[3] = "   #     #    #     #   #  #  #   #            #     #    #   #    #        #  #   ";
            t.banner[4] = "    #####     #     #   #     #   ######        #####      ###     ######   #   # ";
            t.banner[5] = "                                                                                   ";
            break;

        case EndingType::ORDER_RESTORED:
            t.color = "\033[32m";
            t.subtitle = "  [*]  ORDER RESTORED  [*]  ";
            t.banner[0] = "   #     #   #    ######   ######   #####    ######   #     #  ";
            t.banner[1] = "   #     #   #    #           #     #     #   #    #   #   #   ";
            t.banner[2] = "   #     #   #    #           #     #     #   ######    ###    ";
            t.banner[3] = "    #   #    #    #           #     #     #   #  #       #     ";
            t.banner[4] = "     ###     #    ######      #      #####    #    #     #     ";
            t.banner[5] = "                                                                ";
            break;

        case EndingType::LONE_SURVIVOR:
            t.color = "\033[33m";
            t.subtitle = "  (C  LONE SURVIVOR  C)  ";
            t.banner[0] = "      #####    #         #####    #     #   ######  ";
            t.banner[1] = "     #     #   #        #     #   ##    #   #       ";
            t.banner[2] = "     #######   #        #     #   # #   #   ####    ";
            t.banner[3] = "     #     #   #        #     #   #  #  #   #       ";
            t.banner[4] = "     #     #   #######   #####    #   ###   ######  ";
            t.banner[5] = "                                                    ";
            break;

        case EndingType::SYMBIOTIC_EVOLUTION:
            t.color = "\033[35m";
            t.subtitle = "  [MU]  SYMBIOTIC EVOLUTION  [MU]  ";
            t.banner[0] = "   #     #  #     #  ######    #####    #######  ######  ";
            t.banner[1] = "   ##   ##  #     #     #     #     #      #     #       ";
            t.banner[2] = "   # # # #  #     #     #     #######      #     ####    ";
            t.banner[3] = "   #  #  #  #     #     #     #     #      #     #       ";
            t.banner[4] = "   #     #   #####      #     #     #      #     ######  ";
            t.banner[5] = "                                                          ";
            break;

        case EndingType::MARAUDERS:
            t.color = "\033[1m\033[31m";
            t.subtitle = "  XX  MARAUDERS  XX  ";
            t.banner[0] = "   ######    #####    #     #  ######   #  #######  ######  ";
            t.banner[1] = "   #     #   #    #   ##    #  #     #  #     #     #       ";
            t.banner[2] = "   ######    ######   # #   #  #     #  #     #     ######  ";
            t.banner[3] = "   #     #   #    #   #  #  #  #     #  #     #          #  ";
            t.banner[4] = "   ######    #    #   #   ###  ######   #     #     ######  ";
            t.banner[5] = "                                                             ";
            break;

        case EndingType::STRUGGLE_FOR_SURVIVAL:
        default:
            t.color = "\033[36m";
            t.subtitle = "  *  STRUGGLE FOR SURVIVAL  *  ";
            t.banner[0] = "   ######    #     #  ######   #     #  #  #     #  ######   ######   ";
            t.banner[1] = "   #         #     #  #    #   #     #  #  #     #  #        #    #   ";
            t.banner[2] = "   ######    #     #  ######   #     #  #  #     #  ####     #     #  ";
            t.banner[3] = "        #    #     #  #  #      #   #   #   #   #   #        #     #  ";
            t.banner[4] = "   ######     #####   #    #     ###    #    ###    ######   ######   ";
            t.banner[5] = "                                                                       ";
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
    std::cout << "  " << CYAN << "> " << RESET << msg << "\n";
}

// Final screen shown after the player types 'q' to save and quit.
void UI::showQuitConfirmation(int currentDay) {
    clearScreen();
    std::cout << "\n";
    drawTopBorder();
    drawCentered(BOLD + CYAN + std::string("  [SAVE]  GAME SAVED  [SAVE]  ") + RESET);
    drawMidBorder();
    drawLeft("");
    drawLeft("Your progress has been saved.");
    drawLeft("");
    std::ostringstream day;
    day << "When you return, you will resume on day " << currentDay << ".";
    drawLeft(day.str());
    drawLeft("");
    drawLeft(DIM + std::string("Run the game again with:  make run") + RESET);
    drawLeft("");
    drawBottomBorder();
    std::cout << "\n";
}

void UI::waitForEnter() {
    std::cout << "\n  " << DIM << "[ Press Enter to continue, or 'q' to save & quit ]" << RESET;
    int c = std::cin.get();
    if (c == EOF) {
        g_quitRequested = true;
        return;
    }
    if (c == 'q' || c == 'Q') {
        g_quitRequested = true;
        // Eat the rest of the line so it doesn't bleed into the next prompt.
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    // If c was '\n', we're done already.
}
