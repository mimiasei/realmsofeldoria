# Simple Makefile for Realms of Eldoria
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O2
INCLUDES = -Iinclude -Llib
LIBS = -lSDL2 -lSDL2main

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
CLIENT_SOURCES = $(shell find $(CLIENT_SRCDIR) -name "*.cpp")
SERVER_SOURCES = $(shell find $(SERVER_SRCDIR) -name "*.cpp")

# Object files
LIB_OBJECTS = $(LIB_SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/lib/%.o)
CLIENT_OBJECTS = $(CLIENT_SOURCES:$(CLIENT_SRCDIR)/%.cpp=$(OBJDIR)/client/%.o)
SERVER_OBJECTS = $(SERVER_SOURCES:$(SERVER_SRCDIR)/%.cpp=$(OBJDIR)/server/%.o)

# Targets
CLIENT_TARGET = $(BINDIR)/RealmsClient
SERVER_TARGET = $(BINDIR)/RealmsServer

.PHONY: all clean client server dirs

all: dirs client server

client: dirs $(CLIENT_TARGET)
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
	@mkdir -p $(OBJDIR)/client
	@mkdir -p $(OBJDIR)/server

# Build client
$(CLIENT_TARGET): $(LIB_OBJECTS) $(CLIENT_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

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
	sudo apt-get install -y build-essential libsdl2-dev cmake

# Run the game
run-client: client
	cd $(BINDIR) && ./RealmsClient

run-server: server
	cd $(BINDIR) && ./RealmsServer