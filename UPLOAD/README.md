# COMP2113 - Group 150 - 2026

This is a private group project for COMP2113, Group 150.

## Team Members

*   **Wu Bozhou** (UID: 3036588955) - Game Design & Event System
*   **Liu Shuai Kai** (UID: 3036391546) - Resource Management & Code Commenting
*   **Jia Chengru** (UID: 3036432613) - Game Logic, UI & Save/Load
*   **Hu Zhenghua** (UID: 3036395712) - Testing & Video Recording

## Game Description

**Shelter: 10 days** is a text-based survival strategy game set in a post-apocalyptic world overrun by a mysterious infection. As the leader of a group of 6 survivors, you must manage scarce resources, make tough moral choices, and guide your shelter through 10 perilous days. 

**Beware:** Insufficient food or water will cause survivors to fall ill. If sickness persists for two days without medicine or a Doctor’s care, they will perish. Choose your expeditions wisely—each survivor has a unique profession, and sending the right person for the task can mean the difference between a windfall and a disaster.

### Daily Operations
Each Day you will:
1.  **Shelter Status:** View reports on resources, survivors, or choose to quit.
2.  **Treat:** Heal weak survivors (consumes Medicine).
3.  **Expedition:** Select specific members to search for resources.
4.  **Night Event:** Experience a random event (outcomes vary from beneficial to detrimental).
5.  **Consume:** Calculate daily resource consumption and review the day's results.

*Your goal is to survive 10 days. Your final state will automatically unlock one of several endings. Press `Q` at any time to quit and save your progress.*

## Features and Coding Elements

### 1. Random Events
*   **Daily Events (Night Events):** The `selectRandomDailyEvent()` function (in `DailyEvent.cpp`) uses `rand()` to randomly pick one of six night events (e.g., Radiation Rain, Internal Conflict).
*   **Probabilistic Outcomes:** Key functions like `checkProbability()` (in `Tools.cpp`) and `selectRandomSurvivors()` determine the success or failure of events and identify which survivors are affected.

### 2. Data Structures for Game State & Survivor Management
*   **Global Game State:** `struct GameState` (in `GameState.h`) consolidates global variables like current day, resources (Food, Medicine, Parts, Wood), shelter integrity, and game difficulty.
*   **Survivor Roster:** `struct Survivor` and `class Player` (in `player.h`). The `Survivor` struct stores individual attributes (name, health status, profession). The `Player` class manages a dynamic array of pointers to `Survivor` objects, representing the entire group.

### 3. Dynamic Memory Management
*   The survivor roster is managed dynamically throughout the game. 
*   **Dynamic Arrays:** The `loadSurvivors` function uses `new Survivor[size]` to allocate memory for the initial list. During gameplay, events causing deaths or new arrivals trigger the creation of a new array, data copying, and the deletion of the old array using `delete[]` to prevent memory leaks.

### 4. File Input/Output (Save/Load System)
*   **Progress Persistence:** Handled by `saveGame` and `loadGame` functions (in `Tools.cpp`). Players can press `Q` to quit and save at any time. The complete `GameState` and all `Survivor` data are written to `shelter_save.txt`. The game can automatically load this file on startup to resume progress.

### 5. Multi-file Program Structure
*   `main.cpp`: Program entry point and main game loop.
*   `ui.h` / `ui.cpp`: Terminal display handling (using ANSI colors/Unicode) and user input.
*   `GameState.h`: Central definition of game state and data structures.
*   `EventSystem.h`, `DailyEvent.cpp`, `Expedition.cpp`: Event and expedition logic.
*   `ResourceManager.cpp`: Logic for resource calculation and consumption.
*   `EndingSystem.cpp`: Logic for determining the narrative ending.
*   `Tools.h` / `Tools.cpp`: Utility functions (RNG, save/load).
*   `player.h` / `player.cpp`: Data model and dynamic management for survivors.

### 6. Additional Implemented Features
*   **Multiple Difficulty Levels:** "Easy" and "Hard" modes are available at the start, directly affecting initial resources and event severity.
*   **Terminal UI/UX:** Utilizes ANSI escape codes for colored output and Unicode symbols to create an engaging terminal interface.
*   **Multiple Endings:** Features up to 6 different narrative endings based on the final survivor count and resource status.

## Non-Standard C/C++ Libraries
This project **DOES NOT** use any non-standard C/C++ libraries that require external installation. 

*Note: `ui.cpp` utilizes ANSI escape codes to color terminal text (e.g., `\033[1;31m`). These are natively supported by most modern Linux/Unix terminals (including the CS department's academy server) and require no additional setup.*

## Compilation and Execution

We recommend compiling the project using the provided `Makefile`.

**Commands:**
```bash
# Compiles the project and creates the executable file
make

# Compiles (if necessary) and runs the game
make run

# Removes the executable and all intermediate compilation files 
# (Use this for a clean build after modifying source code)
make clean
