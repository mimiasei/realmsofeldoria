# Simple Makefile for Realms of Eldoria
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O2
INCLUDES = -Iinclude -Llib
LIBS = -lSDL2 -lSDL2main -lSDL2_image -lSDL2_ttf
NCURSES_LIBS = -lncurses

# Source directories
SRCDIR = lib
CLIENT_SRCDIR = client
SERVER_SRCDIR = server

# Output directories
BUILDDIR = build
BINDIR = $(BUILDDIR)/bin
OBJDIR = $(BUILDDIR)/obj

# Find all source files
LIB_SOURCES = $(shell find $(SRCDIR) -name "*.cpp")
SDL_CLIENT_SOURCES = $(CLIENT_SRCDIR)/main.cpp
ASCII_CLIENT_SOURCES = $(CLIENT_SRCDIR)/ascii_client.cpp
NCURSES_CLIENT_SOURCES = $(CLIENT_SRCDIR)/ncurses_client.cpp
GRAPHICS_TEST_SOURCES = $(CLIENT_SRCDIR)/graphics_test.cpp
MAP_TEST_SOURCES = $(CLIENT_SRCDIR)/map_test.cpp $(CLIENT_SRCDIR)/render/MapView.cpp
GRAPHICS_CLIENT_SOURCES = $(CLIENT_SRCDIR)/graphics_client.cpp $(CLIENT_SRCDIR)/render/MapView.cpp $(CLIENT_SRCDIR)/ui/ResourceBar.cpp $(CLIENT_SRCDIR)/ui/HeroPanel.cpp
SERVER_SOURCES = $(shell find $(SERVER_SRCDIR) -name "*.cpp")

# Object files
LIB_OBJECTS = $(LIB_SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/lib/%.o)
SDL_CLIENT_OBJECTS = $(SDL_CLIENT_SOURCES:$(CLIENT_SRCDIR)/%.cpp=$(OBJDIR)/client/%.o)
ASCII_CLIENT_OBJECTS = $(ASCII_CLIENT_SOURCES:$(CLIENT_SRCDIR)/%.cpp=$(OBJDIR)/client/%.o)
NCURSES_CLIENT_OBJECTS = $(NCURSES_CLIENT_SOURCES:$(CLIENT_SRCDIR)/%.cpp=$(OBJDIR)/client/%.o)
GRAPHICS_TEST_OBJECTS = $(GRAPHICS_TEST_SOURCES:$(CLIENT_SRCDIR)/%.cpp=$(OBJDIR)/client/%.o)
MAP_TEST_OBJECTS = $(MAP_TEST_SOURCES:$(CLIENT_SRCDIR)/%.cpp=$(OBJDIR)/client/%.o)
GRAPHICS_CLIENT_OBJECTS = $(GRAPHICS_CLIENT_SOURCES:$(CLIENT_SRCDIR)/%.cpp=$(OBJDIR)/client/%.o)
SERVER_OBJECTS = $(SERVER_SOURCES:$(SERVER_SRCDIR)/%.cpp=$(OBJDIR)/server/%.o)

# Targets
SDL_CLIENT_TARGET = $(BINDIR)/RealmsClient
ASCII_CLIENT_TARGET = $(BINDIR)/RealmsAscii
NCURSES_CLIENT_TARGET = $(BINDIR)/RealmsNcurses
GRAPHICS_TEST_TARGET = $(BINDIR)/GraphicsTest
MAP_TEST_TARGET = $(BINDIR)/MapTest
GRAPHICS_CLIENT_TARGET = $(BINDIR)/RealmsGraphics
SERVER_TARGET = $(BINDIR)/RealmsServer

.PHONY: all clean client ascii ncurses graphics-test map-test graphics server dirs

all: dirs ascii ncurses client server

ascii: dirs $(ASCII_CLIENT_TARGET)
ncurses: dirs $(NCURSES_CLIENT_TARGET)
client: dirs $(SDL_CLIENT_TARGET)
graphics-test: dirs $(GRAPHICS_TEST_TARGET)
map-test: dirs $(MAP_TEST_TARGET)
graphics: dirs $(GRAPHICS_CLIENT_TARGET)
server: dirs $(SERVER_TARGET)

# Create directories
dirs:
	@mkdir -p $(BINDIR)
	@mkdir -p $(OBJDIR)/lib/entities/hero
	@mkdir -p $(OBJDIR)/lib/entities/creature
	@mkdir -p $(OBJDIR)/lib/entities/town
	@mkdir -p $(OBJDIR)/lib/entities/artifact
	@mkdir -p $(OBJDIR)/lib/gamestate
	@mkdir -p $(OBJDIR)/lib/map
	@mkdir -p $(OBJDIR)/lib/battle
	@mkdir -p $(OBJDIR)/lib/geometry
	@mkdir -p $(OBJDIR)/lib/render
	@mkdir -p $(OBJDIR)/lib/gui
	@mkdir -p $(OBJDIR)/client
	@mkdir -p $(OBJDIR)/client/render
	@mkdir -p $(OBJDIR)/client/ui
	@mkdir -p $(OBJDIR)/server

# Build SDL client
$(SDL_CLIENT_TARGET): $(LIB_OBJECTS) $(SDL_CLIENT_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

# Build ASCII client  
$(ASCII_CLIENT_TARGET): $(LIB_OBJECTS) $(ASCII_CLIENT_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Build ncurses client
$(NCURSES_CLIENT_TARGET): $(LIB_OBJECTS) $(NCURSES_CLIENT_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(NCURSES_LIBS)

# Build graphics test (only needs SDL2, not image/ttf for now)
$(GRAPHICS_TEST_TARGET): $(LIB_OBJECTS) $(GRAPHICS_TEST_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lSDL2 -lSDL2main

# Build map test
$(MAP_TEST_TARGET): $(LIB_OBJECTS) $(MAP_TEST_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lSDL2 -lSDL2main

# Build graphics client (full UI) - only needs SDL2 and SDL2_ttf, not SDL2_image
$(GRAPHICS_CLIENT_TARGET): $(LIB_OBJECTS) $(GRAPHICS_CLIENT_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lSDL2 -lSDL2main -lSDL2_ttf

# Build server
$(SERVER_TARGET): $(LIB_OBJECTS) $(SERVER_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile lib object files
$(OBJDIR)/lib/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile client object files
$(OBJDIR)/client/%.o: $(CLIENT_SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile server object files
$(OBJDIR)/server/%.o: $(SERVER_SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build files
clean:
	rm -rf $(BUILDDIR)

# Install dependencies (Ubuntu/Debian)
install-deps:
	sudo apt-get update
	sudo apt-get install -y build-essential libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libncurses5-dev cmake

# Run the games
run-ascii: ascii
	cd $(BINDIR) && ./RealmsAscii

run-ncurses: ncurses
	cd $(BINDIR) && ./RealmsNcurses

run-graphics: graphics
	cd $(BINDIR) && ./RealmsGraphics

run-client: client
	cd $(BINDIR) && ./RealmsClient

run-graphics-test: graphics-test
	cd $(BINDIR) && ./GraphicsTest

run-map-test: map-test
	cd $(BINDIR) && ./MapTest

run-server: server
	cd $(BINDIR) && ./RealmsServer