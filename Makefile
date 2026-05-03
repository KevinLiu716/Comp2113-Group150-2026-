# Makefile for SHELTER: 10 DAYS
# A post-apocalyptic survival text game.
#
# Usage:
#   make          Build the game (creates the 'game' executable)
#   make run      Build and run the game
#   make clean    Remove the 'game' executable and any object files
#   make help     Show this help text

# ----- Configuration -----

# Compiler to use
CXX      = g++

# Flags: C++17 standard, show warnings, optimize for speed
CXXFLAGS = -std=c++17 -Wall -O2

# Output executable name
TARGET   = game

# Grab all .cpp files in this directory automatically
SRCS     = $(wildcard *.cpp)

# Turn each .cpp into a matching .o for compilation
OBJS     = $(SRCS:.cpp=.o)

# All headers, so changing any header triggers a rebuild
HEADERS  = $(wildcard *.h)


# ----- Targets -----

# Default: just build the game
all: $(TARGET)

# Link everything together into the final executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)
	@echo ""
	@echo "  Build successful! Run with:  ./$(TARGET)   or   make run"
	@echo ""

# Compile each .cpp into .o; rebuild if the source or any header changed
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build if needed, then run immediately
run: $(TARGET)
	./$(TARGET)

# Delete all compiled files so next build starts fresh
clean:
	rm -f $(OBJS) $(TARGET)
	@echo "  Cleaned all build artifacts."

# Print available targets
help:
	@echo "Available make targets:"
	@echo "  make          Build the game (default target)"
	@echo "  make run      Build the game and run it"
	@echo "  make clean    Remove the game executable and object files"
	@echo "  make help     Show this help"

# Not real files, always run these when requested
.PHONY: all run clean help
