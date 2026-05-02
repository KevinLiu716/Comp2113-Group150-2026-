# SHELTER: 10 DAYS

A post-apocalyptic survival text game written in C++.

> COMP2113 / ENGG1340 Group Project — Group 150, 2025-26 Sem 2

---

## Game Background

The apocalypse descended without warning, shrouding the familiar world in
radioactive dust. Six survivors have taken shelter in a makeshift refuge,
facing only limited supplies and unknown threats outside the door. Over
the next ten days, every choice is a matter of survival.

You play as the leader making decisions for the group. Will you keep
everyone safe? Will you risk the laboratory for answers? Will you become
something more — or less — than human?

## Game Type

Turn-based, single-player, text-based survival game with multiple endings.
Plays in a Linux / macOS terminal.

## Core Gameplay

Each in-game day follows the same rhythm:

1. **Status report** — see every survivor's health, your supplies, and
   any special items you hold.
2. **Treatment phase** — optionally spend medicine to heal weak survivors.
3. **Expedition phase** — optionally send some survivors outside, choose
   how many, who goes, and **where they go** (player picks the destination
   from 7 locations).
4. **Night event** — a random nightly event resolves: radiation rain,
   strange dreams, an unexpected visitor, and so on.
5. **Resource consumption** — food and water are consumed; shortages can
   make survivors weak.
6. **End-of-day summary** — game state is automatically saved to disk.

After 10 days (or sooner if everyone dies) the game evaluates which of
six possible endings you reached and shows a unique screen for it.

## Features

### Resource management (food, water, medicine)
You manage three resources with progress-bar visualization. Water and
food are consumed every day by every healthy or weak survivor; mutated
survivors do not consume anything. Medicine is spent on treating weak
survivors — unless you have a doctor in the group.

### Daily random events (6 types, with text variants)
Every night one of six events fires: radiation rain, internal conflict,
a mysterious dream, spoiled supplies, an unexpected visitor at the door,
or an anomalous radio signal. Each event has 2–3 narrative variants
chosen at random for replayability.

### Expedition system (7 destinations, player choice)
When you send survivors out you choose where they go: supermarket,
water plant, pharmacy, another camp, the perimeter, the laboratory, or
(with a note) a hidden storage. Each location has its own mechanic —
the lab might mutate everyone, another camp might be robbed, the
pharmacy yields medicine but costs food and water.

### Survivor status system (Healthy / Weak / Mutated / Deceased)
Each of the six survivors has a status that affects what they can do
and what they cost the group. Weak survivors die after 2 days unless
treated. Mutated survivors are permanent and influence the ending.

### Survivor traits (Doctor / Frail / Scout / and more)
Each game randomly assigns a unique trait to each survivor:
- **Doctor** — treatment uses no medicine while alive.
- **Frail** — +20 % chance to fall sick when supplies run short.
- **Scout** — +1 resource on every supply expedition they join.
- **Engineer / Soldier / Lucky** — placeholders shown in the UI for
  future expansion.

### Multiple endings (6 unique outcomes)
Each ending has a distinct ASCII banner and color: **Tragic End**
(red), **Order Restored** (green, the rescue ending), **Lone Survivor**
(yellow), **Symbiotic Evolution** (purple), **Marauders** (bold red),
**Struggle for Survival** (cyan).

### Multiple difficulty levels
Easy and Hard difficulty, each with two starting supply plans (A: bulk
supplies, B: medical priority). Hard mode increases the chance of
weakness from shortages and tightens reward yields.

### Save / load (File I/O)
The game auto-saves to `shelter_save.txt` at the end of each day.
On the next launch, if a save is detected, you are asked whether to
resume. Saves include every survivor's name, status, sick day count,
and trait, as well as all resources, items, flags, and the current day.

---

## How the implementation maps to coding requirements

| Requirement                            | Where in the code                             |
| -------------------------------------- | --------------------------------------------- |
| 1. Random event generation             | `DailyEvent.cpp`, `Expedition.cpp`, `Tools.cpp` (`checkProbability`, `selectRandomSurvivors`) |
| 2. Data structures for storing data    | `GameState.h` (struct `Survivor`, class `GameState`, four enums) |
| 3. Dynamic memory management           | `std::vector<Survivor>` and `std::vector<int>` in `GameState`; std::string |
| 4. File input / output                 | `Tools.cpp` (`saveGame`, `loadGame`, `hasSaveFile`, `deleteSaveFile`); auto-save at end of every day in `main.cpp` |
| 5. Multiple source files               | 8 .cpp files + 4 .h files (see file list below) |
| 6. Multiple difficulty levels          | `enum class Difficulty { EASY, HARD }`; affects supplies, weakness chance, expedition yields, and other camp behavior |

---

## How to compile and run

### Quick start

```bash
make run
```

That's it. `make run` compiles the project (if needed) and launches the
game. The first run will compile every source file; later runs only
recompile what changed.

### Other useful commands

```bash
make          # Just compile, leave the executable in ./game
./game        # Run after a manual build
make clean    # Remove the executable, object files, and any save data
make help     # Show this help inside the terminal
```

### Manual compile (if `make` is unavailable)

```bash
g++ -std=c++17 -Wall -O2 *.cpp -o game
./game
```

### Requirements

- A C++17 compiler (`g++` 7+ or Clang 6+)
- A terminal with UTF-8 support (any modern macOS / Linux terminal works)
- No external libraries — only the C++ standard library is used

The game compiles cleanly on the CS department's academy server.

---

## Project file layout

```
Comp2113-Group150-2026--main/
├── Makefile           ← build rules (make / make run / make clean)
├── README.md          ← this file
├── main.cpp           ← entry point and main loop
├── ui.h / ui.cpp      ← all screens, input, formatting (Member D)
├── GameState.h        ← global data structure and enums
├── player.cpp         ← GameState constructor and helper methods (Member B)
├── ResourceManager.cpp← daily consumption, treatment, sick counter (Member B)
├── EventSystem.h      ← event interface
├── DailyEvent.cpp     ← night events (Member C)
├── Expedition.cpp     ← daytime expeditions (Member C)
├── Tools.h / Tools.cpp← utilities + save/load (Member A + shared)
├── player.h           ← (legacy header, kept for compatibility)
└── shelter_save.txt   ← save data (created at runtime)
```

`main.cpp` is the entry point. Just run `make run` from this folder.

---

## Team members and contributions

| Member | Responsibility                                            |
| ------ | --------------------------------------------------------- |
| A      | Main loop, day flow, ending logic, GitHub management      |
| B      | Player attributes, resources, daily consumption           |
| C      | Random events, expedition events, story text              |
| D      | UI, input, formatting, documentation, testing             |

---

## Non-standard libraries

**None.** The project uses only the C++17 standard library. No
additional libraries need to be installed by the grader.

---

## Tips for new players

- The **Order Restored** (rescue) ending requires a radio AND triggering
  the Anomalous Signal event AND ending day 10 with at least 4 healthy
  survivors and at least 10 total food+water. Plan ahead.
- Going to the **Laboratory** mutates everyone you send (or one person
  on Hard mode). Mutated survivors don't eat or drink, but a majority
  of mutants triggers the **Symbiotic Evolution** ending.
- **Robbing other camps** twice or more leads to the **Marauders**
  ending — easy supplies, but a dark path.
- The **note** found by deep-searching the perimeter unlocks the
  **Hidden Storage** destination, which gives a free 3 food + 3 water.
- Save your medicine for emergencies — but if you have a doctor,
  treatment is free!
