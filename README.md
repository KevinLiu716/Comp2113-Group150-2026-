# Comp2113-Group150-2026-
This is a prviate group project for comp2113 group 150

# Team Members

WU Bozhou (UID: 3036588955) - Game design & Event System
Liu Shuai Kai (UID: 3036391546) - Resource Management
Jia Chengru (UID: 3036432613) - Game Logic, UI & Save/Load
Hu Zhenghua (UID: 3036395712) - Test & video recording

# Game Description

Shelter​: 10 days is a text-based survival strategy game set in a post-apocalyptic world overrun by a mysterious
infection. As the leader of a group of 6 survivors, you must manage scarce resources, make tough moral choices,
and guide your shelter through 10 perilous days. 

Beware: insufficient food or water will cause survivors to fall ill, and if sickness persists for two days without
medicine or a Doctor’s care, they will perish. Choose your expeditions wisely—each survivor has a unique profession,
and sending the right person for the task can mean the difference between a windfall and a disaster.

Each Day​ you will:
See your Shelter Status​ report (Resources, Survivors, quit...).
Choose to Treat​ weak survivors (costs Medicine).
Choose to send an Expedition​ (select members).
Experience a Night Event​ (random, could be good or bad).
Consume​ daily resources and see the day's results.

Your goal is to survive 10 days. Your final state automatically will unlock one of several endings.
Press 'Q' at any time to quit and save your progress.

# Features and Coding Elements

1. Random Event
• Daily Events (Night Events):​ The selectRandomDailyEvent() function (in DailyEvent.cpp) uses rand()to randomly pick one of six night events (Radiation Rain, Internal Conflict, etc.).
• Probabilistic Outcomes of Expedition Events:​ Key functions like checkProbability()(in Tools.cpp) and selectRandomSurvivors()determine the success/failure of events and who is affected.

2. Data Structures for storing Game State & Survivor Management
• Global Game State​ (struct GameStatein GameState.h): A structure that consolidates global variables like current day, all resources (Food, Medicine, Parts, Wood), shelter integrity, and game difficulty.
• urvivor Roster​ (struct Survivorand class Player in player.h): The Survivor struct stores attributes (name, health status, profession) for an individual. The Player class manages a dynamic array of pointers to Survivorobjects, representing the group.

3. Dynamic Management Management​
• The survivor roster is managed dynamically throughout the game. Dynamic Array for Survivors​ (player.cpp): The loadSurvivorsfunction uses new Survivor[size] to allocate memory for the initial list. During gameplay, events causing deaths or new arrivals trigger the creation of a new array, copying of data, and deletion of the old array using delete[].

4. File Input/Output (for Loading/Saving)
• Save/Load Game Progress​ (saveGameand loadGamefunctions in Tools.cpp): Players can press 'Q' to quit and save at any time. The complete GameStateand all Survivordata are written to shelter_save.txt. The game can automatically load this file on startup to continue.

5. Program Codes in Multiple Files
• main.cpp: Program entry point, main game loop.
• ui.h/cpp: All terminal display (using ANSI colors/Unicode) and user input handling.
• GameState.h: Central definition of game state and data structures.
• EventSystem.h, DailyEvent.cpp, Expedition.cpp: The event and expedition systems.
• ResourceManager.cpp: Logic for resource calculation and consumption.
• EndingSystem.cpp: Logic for determining the game's ending.
• Tools.h/cpp: Utility functions (RNG, save/load).
• player.h/cpp: Data model and management for survivors.

6. Other Implemented Features
• Multiple Difficulty Levels: The game offers "Easy" and "Hard" modes at the start, affecting initial resources and event severity.
• Complete UI/UX: Utilizes ANSI escape codes for colored output and Unicode symbols for an engaging terminal interface.
• Multiple Endings: Provides up to 6 different narrative endings based on the final number of survivors and resource status.

# Non-Standard C/C++ Libraries
This project does NOT​ use any non-standard C/C++ libraries that require additional installation.
The ui.cppuses ANSI escape codes​ to color terminal text (e.g., \\033\[1;31m). These are supported by most modern Linux/Unix terminals (including the CS department's academy server) and do not require any additional library installation.

# Compilation and Execution
Compile using Makefile (Recommended):
make: Compiles the project to create the executable file.
make run: Compiles (if needed) and then runs the game.
make clean: Removes the executable and all intermediate compilation files. Use this if you have modified the source code and need a full recompile.
# To play (again): Simply run 'make run'. If a previous save file exists, you will be prompted to either continue or start a new game. Selecting "New Game" will reset all progress.
