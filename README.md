# Realms of Eldoria

A Heroes of Might and Magic III inspired strategy game written in C++, based on architectural patterns from the VCMI project.

## Project Structure

```
realmsofeldoria/
├── lib/                    # Core game library
│   ├── entities/           # Game entities (heroes, creatures, towns)
│   │   ├── hero/          # Hero system
│   │   ├── creature/      # Creature system
│   │   ├── town/          # Town system
│   │   └── artifact/      # Artifact system
│   ├── battle/            # Battle system
│   ├── map/               # Game map and objects
│   └── gamestate/         # Game state management
├── client/                # Game client (rendering, UI)
├── server/               # Game server (multiplayer support)
├── include/              # Shared headers
├── .vscode/              # VS Code configuration
├── CMakeLists.txt        # CMake build configuration
└── Makefile             # Simple make build configuration
```

## Building

### Prerequisites

- C++17 compatible compiler (g++)
- Make or CMake
- SDL2 development libraries

### Ubuntu/Debian - Install Dependencies

```bash
sudo apt-get update
sudo apt-get install -y build-essential libsdl2-dev cmake
```

Or use the make target:
```bash
make install-deps
```

### Build Methods

#### Method 1: Using Make (Recommended for simple builds)

```bash
# Build everything
make

# Build only client
make client

# Build only server  
make server

# Clean build
make clean
```

#### Method 2: Using CMake (Recommended for VS Code)

```bash
mkdir build
cd build
cmake ..
make
```

### VS Code Setup

1. Install recommended extensions when prompted
2. Open the project folder in VS Code
3. The CMake extension will automatically configure the project
4. Use Ctrl+Shift+P → "Tasks: Run Task" → "Build Project"
5. Use F5 to debug the client

## Running

### Using Make:
```bash
# Run client
make run-client

# Run server  
make run-server
```

### Using CMake:
```bash
./build/bin/RealmsClient
./build/bin/RealmsServer
```

## VS Code Tasks

Available tasks (Ctrl+Shift+P → "Tasks: Run Task"):
- **Configure CMake** - Set up CMake build
- **Build Project** - Build the entire project
- **Clean Build** - Clean all build artifacts
- **Run Game** - Build and run the client

## VS Code Debugging

Press F5 to start debugging:
- **Debug Realms Client** - Debug the game client
- **Debug Realms Server** - Debug the game server

## Controls

- **ESC**: Exit game
- **SPACE**: Next turn

## Architecture

The game follows a modular architecture inspired by VCMI:

- **GameLib**: Core game logic and data structures
- **GameClient**: Rendering, UI, and player interaction
- **GameServer**: Game state management and networking

Key systems include:
- Entity management (Heroes, Creatures, Towns, Artifacts)
- Turn-based game flow
- Resource management
- Map system with objects and terrain
- Battle system (planned)

## Current Status

This is an early development version with basic:
- Project structure
- Core entity definitions
- Basic game state management
- Simple SDL2-based client
- Turn management system
- Complete VS Code integration

## Planned Features

- [ ] Battle system implementation
- [ ] AI opponents
- [ ] Map editor
- [ ] Spell system
- [ ] Town management
- [ ] Campaign mode
- [ ] Multiplayer support

## Development

### Project Files

- **GameTypes.h/.cpp** - Core type definitions and resource management
- **Hero.h/.cpp** - Hero entity with stats, skills, army management
- **Creature.h/.cpp** - Creature system with abilities and combat stats
- **GameState.h/.cpp** - Central game state and player management
- **GameMap.h/.cpp** - Map system with terrain and objects
- **main.cpp** (client) - SDL2-based game client
- **main.cpp** (server) - Console-based game server

### Adding New Features

1. Define interfaces in `include/` headers
2. Implement core logic in `lib/` subdirectories
3. Add UI/rendering in `client/`
4. Update CMakeLists.txt or Makefile if needed
5. Test with both make and cmake builds

## License

This project is for educational purposes, inspired by Heroes of Might and Magic III and the VCMI project.
