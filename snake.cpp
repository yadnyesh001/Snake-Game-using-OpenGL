#include <GL/glut.h>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <cmath>

int gridX, gridY;
int snake_length1 = 5;
int snake_length2 = 5;
int posX1[100] = {20, 20, 20, 20, 20}, posY1[100] = {20, 19, 18, 17, 16};
int posX2[100] = {10, 10, 10, 10, 10}, posY2[100] = {10, 9, 8, 7, 6};
bool food = true;
int foodX, foodY;
int direction1 = 1; // Direction for player 1's snake
int direction2 = 1; // Direction for player 2's snake
bool game_over = false;
int points1 = 0;
int points2 = 0;
bool game_over_displayed = false;
bool difficulty_selected = false;
bool game_started = false;
bool draw = false;
bool right_wins = false;
bool left_wins = false;

const int grid_width = 30;
const int grid_height = 30;

int game_speed = 150; // Adjusted Default speed
int selected_difficulty = 1;  // Default difficulty level
const int speeds[] = {100, 85, 70, 55, 40};  // Easy to Insane

void initGrid(int x, int y) {
    gridX = x;
    gridY = y;
}

void drawGrid() {
    for (int x = 0; x < gridX; x++) {
        for (int y = 0; y < gridY; y++) {
            // Calculate gradient factor based on position
            float gradientX = (float)x / gridX;
            float gradientY = (float)y / gridY;

            // Create a gradient effect with a blue-to-purple transition
            glColor3f(gradientX, 0.2 + gradientY * 0.8, 1.0 - gradientX * 0.5); 
            
            glBegin(GL_LINES);
            glVertex2f(x, y);
            glVertex2f(x, y + 1);
            glVertex2f(x, y);
            glVertex2f(x + 1, y);
            glEnd();
        }
    }
}


void randomFood() {
    if (food) {
        foodX = rand() % gridX;
        foodY = rand() % gridY;
        food = false;
    }
}

void drawSnake1() {
    for (int i = 0; i < snake_length1; i++) {
        if (i == 0) {
            glColor3f(0.0, 1.0, 0.5);  // Bright green for Player 1's head
        } else {
            float gradient = (float)i / snake_length1;  // Calculate gradient factor
            glColor3f(0.0, 1.0 - gradient * 0.5, 0.5 + gradient * 0.5);  // Gradient from bright green to a softer green
        }
        glRectd(posX1[i], posY1[i], posX1[i] + 1, posY1[i] + 1);
    }
}

void drawSnake2() {
    for (int i = 0; i < snake_length2; i++) {
        if (i == 0) {
            glColor3f(1.0, 1.0, 0.3);  // Yellowish head for Player 2
        } else {
            float gradient = (float)i / snake_length2;  // Calculate gradient factor
            glColor3f(1.0, 1.0 - gradient * 0.7, gradient * 0.5);  // Gradient from yellow to orange
        }
        glRectd(posX2[i], posY2[i], posX2[i] + 1, posY2[i] + 1);
    }
}


void drawFood() {
    // Use a time-based factor to create a pulsating gradient effect
    float timeFactor = (sin(glutGet(GLUT_ELAPSED_TIME) * 0.005) + 1.0) / 2.0;

    // Gradient from red to orange based on time factor
    float red = 1.0;
    float green = timeFactor * 0.5;  // Ranges between 0.0 and 0.5
    float blue = timeFactor * 0.2;   // Ranges between 0.0 and 0.2

    glColor3f(red, green, blue);
    glRectd(foodX, foodY, foodX + 1, foodY + 1);
}

void keyboard(int key, int, int) {
    if (!difficulty_selected) return;

    // Player 1 controls
    switch (key) {
        case GLUT_KEY_UP:
            if (direction1 != 2) direction1 = 0;
            break;
        case GLUT_KEY_RIGHT:
            if (direction1 != 3) direction1 = 1;
            break;
        case GLUT_KEY_DOWN:
            if (direction1 != 0) direction1 = 2;
            break;
        case GLUT_KEY_LEFT:
            if (direction1 != 1) direction1 = 3;
            break;
    }
}

void player2Keyboard(unsigned char key, int, int) {
    if (!difficulty_selected) return;

    // Player 2 controls using WASD keys
    switch (key) {
        case 'w':
            if (direction2 != 2) direction2 = 0;
            break;
        case 'd':
            if (direction2 != 3) direction2 = 1;
            break;
        case 's':
            if (direction2 != 0) direction2 = 2;
            break;
        case 'a':
            if (direction2 != 1) direction2 = 3;
            break;
    }
}

void checkGameOutcome() {
    // Head-to-Head Collision
    if (posX1[0] == posX2[0] && posY1[0] == posY2[0]) {
        game_over = true;
        draw = true;
    }

    // Player 1 hitting Player 2's body
    for (int i = 1; i < snake_length2; i++) {
        if (posX1[0] == posX2[i] && posY1[0] == posY2[i]) {
            game_over = true;
            left_wins = true;
        }
    }

    // Player 2 hitting Player 1's body
    for (int i = 1; i < snake_length1; i++) {
        if (posX2[0] == posX1[i] && posY2[0] == posY1[i]) {
            game_over = true;
            right_wins = true;
        }
    }
     // Check for collisions with food
    if (posX1[0] == foodX && posY1[0] == foodY) {
        snake_length1++;
        points1 += 1;
        food = true;
        randomFood();
    }
    if (posX2[0] == foodX && posY2[0] == foodY) {
        snake_length2++;
        points2 += 1;
        food = true;
        randomFood();
    }
}

void update(int) {
    if (game_over) {
        if (!game_over_displayed) {
            if (draw)
                printf("Match Draw\n");
            else if (right_wins)
                printf("Right Wins\n");
            else if (left_wins)
                printf("Left Wins\n");

            game_over_displayed = true;
        }
        return;
    }

    // Update positions for Player 1's snake
    for (int i = snake_length1 - 1; i > 0; i--) {
        posX1[i] = posX1[i - 1];
        posY1[i] = posY1[i - 1];
    }
    // Update positions for Player 2's snake
    for (int i = snake_length2 - 1; i > 0; i--) {
        posX2[i] = posX2[i - 1];
        posY2[i] = posY2[i - 1];
    }

    // Move Player 1's snake
    switch (direction1) {
        case 0: posY1[0]++; break;
        case 1: posX1[0]++; break;
        case 2: posY1[0]--; break;
        case 3: posX1[0]--; break;
    }

    // Wrap around logic for Player 1
    if (posX1[0] < 0) posX1[0] = gridX - 1;
    if (posX1[0] >= gridX) posX1[0] = 0;
    if (posY1[0] < 0) posY1[0] = gridY - 1;
    if (posY1[0] >= gridY) posY1[0] = 0;

    // Move Player 2's snake
    switch (direction2) {
        case 0: posY2[0]++; break;
        case 1: posX2[0]++; break;
        case 2: posY2[0]--; break;
        case 3: posX2[0]--; break;
    }

    // Wrap around logic for Player 2
    if (posX2[0] < 0) posX2[0] = gridX - 1;
    if (posX2[0] >= gridX) posX2[0] = 0;
    if (posY2[0] < 0) posY2[0] = gridY - 1;
    if (posY2[0] >= gridY) posY2[0] = 0;

   

    // Self-collision checks
    for (int i = 1; i < snake_length1; i++) {
        if (posX1[0] == posX1[i] && posY1[0] == posY1[i]) {
            game_over = true;
            left_wins = true;
        }
    }
    for (int i = 1; i < snake_length2; i++) {
        if (posX2[0] == posX2[i] && posY2[0] == posY2[i]) {
            game_over = true;
            right_wins = true;
        }
    }

    // Check collisions between players
    checkGameOutcome();

    glutPostRedisplay();
    glutTimerFunc(game_speed, update, 0);
}


void renderBitmapString(float x, float y, void *font, const char *string) {
    const char *c;
    glRasterPos2f(x, y);
    for (c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

void renderDifficultySelection() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    renderBitmapString(gridX / 2 - 4, gridY / 2 + 4, GLUT_BITMAP_HELVETICA_18, "Select Difficulty:");
    renderBitmapString(gridX / 2 - 4, gridY / 2 + 2, GLUT_BITMAP_HELVETICA_18, "1 - Easy");
    renderBitmapString(gridX / 2 - 4, gridY / 2, GLUT_BITMAP_HELVETICA_18, "2 - Normal");
    renderBitmapString(gridX / 2 - 4, gridY / 2 - 2, GLUT_BITMAP_HELVETICA_18, "3 - Hard");
    renderBitmapString(gridX / 2 - 4, gridY / 2 - 4, GLUT_BITMAP_HELVETICA_18, "4 - Insane");
    glutSwapBuffers();
}

void displayScoreboard() {
    char scoreText1[50], scoreText2[50];
    sprintf(scoreText1, "Right Player: %d", points1);
    sprintf(scoreText2, "Left Player: %d", points2);

    glColor3f(1.0, 1.0, 1.0);  // Set the color to white for the scoreboard text

    // Render Player 1's score on the top left
    renderBitmapString(10, gridY - 2, GLUT_BITMAP_HELVETICA_18, scoreText2);  // Adjusted to smaller font size and visible position

    // Render Player 2's score on the top right
    // Adjusted position for Player 2's score with a more dynamic positioning.
    renderBitmapString(gridX - 10, gridY - 2, GLUT_BITMAP_HELVETICA_18, scoreText1);  // Ensure it's far enough on the right
}


void renderGame() {
    glClear(GL_COLOR_BUFFER_BIT);
    displayScoreboard();
    drawGrid();
    drawSnake1();
    drawSnake2();
    drawFood();
    glutSwapBuffers();
}

void display() {
    if (!difficulty_selected) {
        renderDifficultySelection();
    } else if (game_over) {
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3f(1.0, 1.0, 1.0);
        if (draw) {
            renderBitmapString(gridX / 2 - 4, gridY / 2, GLUT_BITMAP_HELVETICA_18, "Match Draw");
        } else if (left_wins) {
            renderBitmapString(gridX / 2 - 4, gridY / 2, GLUT_BITMAP_HELVETICA_18, "Left Wins");
        } else if (right_wins) {
            renderBitmapString(gridX / 2 - 4, gridY / 2, GLUT_BITMAP_HELVETICA_18, "Right Wins");
        }
        glutSwapBuffers();
    } else {
        renderGame();
    }
}

void keyPress(unsigned char key, int x, int y) {
    if (!difficulty_selected && key >= '1' && key <= '4') {
        selected_difficulty = key - '0';
        game_speed = speeds[selected_difficulty - 1];
        difficulty_selected = true;
        glutPostRedisplay();
    }
    if (key == 27) {
        exit(0);  // Exit the game when Escape is pressed
    }
    player2Keyboard(key, x, y); // Handle player 2 movement
}

int main(int argc, char **argv) {
    srand(time(0));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    
    glutInitWindowSize(1920, 1080); 
    glutCreateWindow("Two-Player Snake Game");
    glutFullScreen(); 

    glutDisplayFunc(display);
    glutSpecialFunc(keyboard);
    glutKeyboardFunc(keyPress);
    glutTimerFunc(0, update, 0);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glOrtho(0.0, grid_width, 0.0, grid_height, -1.0, 1.0);
    initGrid(grid_width, grid_height);
    glutMainLoop();
    return 0;
}
