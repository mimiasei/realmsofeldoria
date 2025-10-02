# Claude Development Instructions

This file contains development commands and instructions for working with Realms of Eldoria in Claude Code.

## Build Commands

### ASCII Client (Playable Game)
```bash
make ascii
```

### SDL2 Client (Graphical)
```bash
make client
```

### Server
```bash
make server
```

### Build All
```bash
make all
```

## Run Commands

### Play ASCII Game
```bash
make run-ascii
```

### Run SDL2 Client
```bash
make run-client
```

### Run Server
```bash
make run-server
```

## Testing Commands

### Lint and Type Check
```bash
# Add lint/typecheck commands here when available
```

### Build and Test
```bash
make clean
make ascii
make run-ascii
```

## Development Workflow

1. **Build the ASCII client**: `make ascii`
2. **Test the game**: `make run-ascii` 
3. **Make changes to code**
4. **Rebuild**: `make ascii`
5. **Test again**: `make run-ascii`

## Game Controls (ASCII Version)

- **WASD**: Move hero
- **TAB**: Switch between heroes
- **H**: Show hero information
- **N**: Next turn
- **Q**: Quit to main menu
- **1/2**: Main menu navigation

## Common Issues

### Build Errors
- Make sure all dependencies are installed: `make install-deps`
- Clean and rebuild: `make clean && make ascii`

### Input Not Working
- The ASCII client uses terminal input - make sure your terminal supports it
- Try running directly: `./build/bin/RealmsAscii`

## File Structure

- `client/ascii_client.cpp` - ASCII game client
- `client/main.cpp` - SDL2 graphical client  
- `server/main.cpp` - Game server
- `lib/` - Core game library
- `include/` - Shared headers

## Next Development Steps

1. Add battle system when heroes encounter monsters
2. Implement town/dwelling visits
3. Add more map objects and interactions
4. Expand hero progression system
5. Add AI opponents

## Quick Test Sequence

```bash
# Build and run the playable ASCII game
make ascii && make run-ascii
```

This will build and immediately run the ASCII version of the game for testing.
- always reference the public vcmi github repository for code, realms of eldoria is mainly reusing the code from there.
- always add summary of changes to the changes file
- copy and thus reuse as much of the code from the vcmi repo on github as possible. do not reinvent the wheel!