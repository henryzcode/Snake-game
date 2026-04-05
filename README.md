[README.md](https://github.com/user-attachments/files/26478691/README.md)

[snake1.zip](snake1.zip)
# 🐍 Snake Game (Windows)

A classic Snake game built using **SDL2**, featuring smooth movement, collision detection, and a simple scoring system. This project was created to practice game loops, rendering, and real-time input handling in C++.

---

## Gameplay Preview

<img width="1636" height="1034" alt="image" src="https://github.com/user-attachments/assets/75ce521d-2c57-4aaa-a9f9-a96a1e9e5b10" />

<img width="887" height="593" alt="image" src="https://github.com/user-attachments/assets/7b8b4837-57e9-446e-bb5c-79d9ff47743c" />



---

## Features

- Grid-based movement
- Food spawning system
- Score tracking
- Self-collision and wall collision detection
- Smooth movement with frame control
- Simple and clean game logic

---

## Built With

- **C++**
- **SDL2**
- **SDL2_ttf**

---

## Installation (Windows)

### 1. Install Dependencies

Download and install:

- https://www.libsdl.org/download-2.0.php
- https://www.libsdl.org/projects/SDL_ttf/

Make sure to:
- Place the .dll files in your executable folder
- Link the libraries in your compiler (e.g., CLion / MinGW / Visual Studio)

---

### 2. Clone the Repository

```bash
git clone https://github.com/your-username/snake-game.git
cd snake-game
```

---

### 3. Build the Project

```bash
g++ main.cpp -o snake.exe -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf
```

---

### 4. Run the Game

```bash
./snake.exe
```

---

## Controls

| Key | Action        |
|-----|--------------|
| ↑   | Move Up       |
| ↓   | Move Down     |
| ←   | Move Left     |
| →   | Move Right    |
| ESC | Quit Game     |
| SHIFT | Speed up    |

---

## How It Works

- The snake is represented as a vector of segments.
- Each frame updates the snake’s position based on input.
- Collision detection checks:
  - Snake hitting itself
  - Snake hitting the wall
- Food spawns randomly and increases score when eaten.

---

## Future Improvements

- Better graphics / textures
- Sound effects
- High score system
- Portals or special mechanics
- Difficulty scaling

---

## License

This project is open-source and available under the MIT License.

---

## Acknowledgements

- SDL2 documentation  
- Online tutorials and forums  
