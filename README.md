# Maze Game

This C++ maze game showcases various data structures like graphs, queues, stacks, and linked lists. Players navigate the maze, encountering obstacles and collectibles. The game employs BFS for pathfinding, offering an exploration of fundamental data structure concepts.

![Game Screenshot](https://pasteboard.co/a5riHDTXWkSJ.png)

## How to Play

1. **Manual Mode (Difficulty Levels)**
   - Use W, A, S, D keys to navigate the maze.
   - Collect green plus icons (collectibles) to win.
   - Avoid obstacles to prevent vehicle health reduction.
   - Exiting without collecting all collectibles results in a loss.
   - Reaching the end with the required collectibles results in a win.

2. **Auto Mode**
   - The game automatically navigates the player to the maze's end.

3. **Main Menu**
   - Choose Auto Mode (1) or Manual Mode (2).
   - Select difficulty levels in Manual Mode.
   - Access the Help Screen (3) for game instructions.

## Installation

1. Clone the repository.
2. Compile the source code using a C++ compiler.
3. Run the executable.

## File Structure

- `Source.cpp`: Main source code file.
- `maze.h`: Maze layouts and printing functions.
- `assets.h`: Constants for colors and symbols.
- `config.h`: Configuration settings.
- `LinkedList.h`: Linked list operations for managing coins.

## Acknowledgments

This project was developed as part of a data structures end-of-semester project, providing a practical implementation of key concepts.

Feel free to explore and enjoy the game!
