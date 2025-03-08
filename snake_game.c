#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

#define GRID_SIZE 10
#define MAX_LENGTH 100

// Struct for a point
typedef struct {
    int x, y;
} Point;

// Struct for the snake
typedef struct {
    Point body[MAX_LENGTH];
    int length;
    char direction;
    bool isAlive;
} Snake;

// Global variables
Snake snake1, snake2;
Point food;

void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void initializeGame() {
    // Initialize Player 1
    snake1.length = 3;
    snake1.body[0] = (Point){2, 2};
    snake1.body[1] = (Point){1, 2};
    snake1.body[2] = (Point){0, 2};
    snake1.direction = 'd';
    snake1.isAlive = true;

    // Initialize Player 2
    snake2.length = 3;
    snake2.body[0] = (Point){7, 7};
    snake2.body[1] = (Point){8, 7};
    snake2.body[2] = (Point){9, 7};
    snake2.direction = 'a';
    snake2.isAlive = true;

    // Place initial food
    food.x = rand() % GRID_SIZE;
    food.y = rand() % GRID_SIZE;
}

bool isFoodOnSnake(Snake *snake) {
    // Check if food is on the snake's body
    for (int i = 0; i < snake->length; i++) {
        if (snake->body[i].x == food.x && snake->body[i].y == food.y) {
            return true;
        }
    }
    return false;
}

void spawnFood() {
    // Ensure food doesn't spawn at the same position as either snake
    do {
        food.x = rand() % GRID_SIZE;
        food.y = rand() % GRID_SIZE;
    } while ((food.x == snake1.body[0].x && food.y == snake1.body[0].y) || 
             (food.x == snake2.body[0].x && food.y == snake2.body[0].y));

}

void printGrid() {
    system("cls");
    setColor(15); // White text for grid

    // Print the title and instructions
    gotoxy(0, 0);
    printf("Welcome to Two Player Snake Game\n");
    printf("Player 1 (WASD) vs Player 2 (IJKL)\n");
    printf("Press any key to start...\n");

    // Print the game grid
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            bool printed = false;

            // Check if food is at this position
            if (food.x == x && food.y == y) {
                setColor(12); // Red for food
                printf(" F ");
                printed = true;
            }

            // Check if Player 1's snake is at this position
            for (int i = 0; i < snake1.length; i++) {
                if (snake1.body[i].x == x && snake1.body[i].y == y) {
                    setColor(10); // Green for Player 1
                    printf(" 1 ");
                    printed = true;
                    break;
                }
            }

            // Check if Player 2's snake is at this position
            for (int i = 0; i < snake2.length; i++) {
                if (snake2.body[i].x == x && snake2.body[i].y == y) {
                    setColor(14); // Yellow for Player 2
                    printf(" 2 ");
                    printed = true;
                    break;
                }
            }

            if (!printed) {
                setColor(15); // White for empty spaces
                printf(" . ");
            }
        }
        printf("\n");
    }

    // Print the score at the top
    gotoxy(0, GRID_SIZE + 2);
    printf("Score: Player 1 - %d  |  Player 2 - %d", snake1.length - 3, snake2.length - 3);
}


void moveSnake(Snake *snake) {
    // Shift the body segments
    for (int i = snake->length - 1; i > 0; i--) {
        snake->body[i] = snake->body[i - 1];
    }

    // Update the head position
    switch (snake->direction) {
        case 'w': snake->body[0].y--; break;
        case 's': snake->body[0].y++; break;
        case 'a': snake->body[0].x--; break;
        case 'd': snake->body[0].x++; break;
    }

    // Wrap around the grid
    if (snake->body[0].x < 0) snake->body[0].x = GRID_SIZE - 1;
    if (snake->body[0].x >= GRID_SIZE) snake->body[0].x = 0;
    if (snake->body[0].y < 0) snake->body[0].y = GRID_SIZE - 1;
    if (snake->body[0].y >= GRID_SIZE) snake->body[0].y = 0;
}

void growSnake(Snake *snake) {
    int tailIndex = snake->length - 1;
    int newX = snake->body[tailIndex].x;
    int newY = snake->body[tailIndex].y;

    // Snake growth based on the direction it just moved
    switch (snake->direction) {
        case 'w': newY += 1; break;
        case 's': newY -= 1; break;
        case 'a': newX += 1; break;
        case 'd': newX -= 1; break;
    }

    // Add the new tail segment
    snake->body[snake->length].x = newX;
    snake->body[snake->length].y = newY;
    snake->length++;
}

bool checkCollision(Snake *snake1, Snake *snake2) {
    // Check head-on collision between snakes
    if (snake1->body[0].x == snake2->body[0].x && snake1->body[0].y == snake2->body[0].y) {
        snake1->isAlive = false;
        snake2->isAlive = false;
        return true;  // Both snakes collided at their heads
    }

    // Check collision between snake1's head and snake2's body (snake1 dies)
    for (int i = 0; i < snake2->length; i++) {
        if (snake1->body[0].x == snake2->body[i].x && snake1->body[0].y == snake2->body[i].y) {
            snake1->isAlive = false;
            return true;  // Snake 1 collided with snake 2's body
        }
    }

    // Check collision between snake2's head and snake1's body (snake2 dies)
    for (int i = 0; i < snake1->length; i++) {
        if (snake2->body[0].x == snake1->body[i].x && snake2->body[0].y == snake1->body[i].y) {
            snake2->isAlive = false;
            return true;  // Snake 2 collided with snake 1's body
        }
    }

    // Check self-collision for snake1
    for (int i = 1; i < snake1->length; i++) {
        if (snake1->body[0].x == snake1->body[i].x && snake1->body[0].y == snake1->body[i].y) {
            snake1->isAlive = false;
            return true;  // Snake 1 collided with itself
        }
    }

    // Check self-collision for snake2
    for (int i = 1; i < snake2->length; i++) {
        if (snake2->body[0].x == snake2->body[i].x && snake2->body[0].y == snake2->body[i].y) {
            snake2->isAlive = false;
            return true;  // Snake 2 collided with itself
        }
    }

    return false;
}

void updateGame() {
    if (!snake1.isAlive || !snake2.isAlive) return;

    // Move both snakes
    moveSnake(&snake1);
    moveSnake(&snake2);

    // Check if the snakes ate food
    if (snake1.body[0].x == food.x && snake1.body[0].y == food.y) {
        growSnake(&snake1);
        spawnFood();
    }

    if (snake2.body[0].x == food.x && snake2.body[0].y == food.y) {
        growSnake(&snake2);
        spawnFood();
    }

    // Check for collisions
    checkCollision(&snake1, &snake2);
}

void displayEndGame() {
    printGrid();

    if (!snake1.isAlive && !snake2.isAlive) {
        printf("\nBoth players collided and are dead.\n");
    } else if (!snake1.isAlive) {
        printf("\nPlayer 1 collided and is dead.\n");
        printf("Player 2 wins!\n");
    } else if (!snake2.isAlive) {
        printf("\nPlayer 2 collided and is dead.\n");
        printf("Player 1 wins!\n");
    }

    printf("\nGame Over!\n");
    printf("Press any key to exit...\n");
    _getch();
}

int main() {
    srand(time(NULL));  // Seed random number generator
    initializeGame();

    printGrid();

    // Wait for key press to start the game
    _getch();

    // Game loop
    while (snake1.isAlive && snake2.isAlive) {
        if (_kbhit()) {
            char key = _getch();

            // Player 1 controls (WASD)
            if (key == 'w' && snake1.direction != 's') snake1.direction = 'w';
            if (key == 's' && snake1.direction != 'w') snake1.direction = 's';
            if (key == 'a' && snake1.direction != 'd') snake1.direction = 'a';
            if (key == 'd' && snake1.direction != 'a') snake1.direction = 'd';

            // Player 2 controls (IJKL)
            if (key == 'i' && snake2.direction != 's') snake2.direction = 'w';
            if (key == 'k' && snake2.direction != 'w') snake2.direction = 's';
            if (key == 'j' && snake2.direction != 'd') snake2.direction = 'a';
            if (key == 'l' && snake2.direction != 'a') snake2.direction = 'd';
        }

        updateGame();
        printGrid();
        Sleep(500); // 500ms delay to slow down the game loop
    }

    displayEndGame();
    return 0;
}
