# Makefile for SHELTER: 10 DAYS
# A post-apocalyptic survival text game.
#
# Usage:
#   make          Build the game (creates the 'game' executable)
#   make run      Build and run the game
#   make clean    Remove the 'game' executable, object files, and save data
#   make help     Show this help text

# ----- Configuration -----

# C++ compiler. g++ is the GNU C++ compiler, available on macOS, Linux,
# and the CS department server.
CXX      = g++

# Compiler flags:
#   -std=c++17   Use the C++17 language standard
#   -Wall        Enable common compiler warnings
#   -O2          Optimize the generated code (faster runtime)
CXXFLAGS = -std=c++17 -Wall -O2

# Name of the final executable produced by `make`.
TARGET   = game

# All source files. Using wildcard so adding a new .cpp does not require
# editing this Makefile.
SRCS     = $(wildcard *.cpp)

# Corresponding object files (.cpp -> .o). Object files are intermediate
# build artifacts that get linked together into the final executable.
OBJS     = $(SRCS:.cpp=.o)

# All header files. The build will re-compile if any header changes.
HEADERS  = $(wildcard *.h)


# ----- Targets -----

# Default target. Just running `make` triggers this.
all: $(TARGET)

# Link all object files into the final executable.
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)
	@echo ""
	@echo "  Build successful! Run with:  ./$(TARGET)   or   make run"
	@echo ""

# Pattern rule: how to compile any .cpp file into a .o file.
# The .o is rebuilt whenever the .cpp or any header changes.
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build (if needed) and immediately run the game.
run: $(TARGET)
	./$(TARGET)

# Remove all build artifacts and any saved game so you can rebuild from scratch.
clean:
	rm -f $(OBJS) $(TARGET) shelter_save.txt
	@echo "  Cleaned all build artifacts and saved data."

# Print a quick help message.
help:
	@echo "Available make targets:"
	@echo "  make          Build the game (default target)"
	@echo "  make run      Build the game and run it"
	@echo "  make clean    Remove the game executable, object files, and save data"
	@echo "  make help     Show this help"

# These targets are not real files, so make should always run them when asked.
.PHONY: all run clean help
