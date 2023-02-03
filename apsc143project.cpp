//We attest to the originality of our code (Isaiah Iruoha & Blaise Elliot)
#include <cstdio> //all required libraries are included
#include <cstdlib>
#include <ctime>
#include "colours.h"
char* fileTitle = (char *) "map.txt"; //input the file name and extension, allowing file name changes in a singular location
#define PACMAN 'P' //constants are defined for the objects in the game and the possible directions
#define GHOST 'G'
#define DOT '.'
#define WALL 'W'
#define EMPTY ' '
#define UP 'w'
#define LEFT 'a'
#define DOWN 's'
#define RIGHT 'd'
#define LINES 9
#define CHARS 9
#define outOfBounds 999

char **readMap(){ //function reads the map from a .txt file into a 2D array of strings with dynamic memory allocation
    FILE *mapFile = fopen(fileTitle, "r"); //reading the file therefore (r) read mode is satisfactory
    if (mapFile == nullptr){ //if the file is not found print output and exit program
        printf("File not found.\n");
        exit(0);
    }
    char **map = (char **) malloc(LINES * sizeof(char *)); //allocate the correct memory for the 2D array based on the vertical and horizontal size
    for (int i = 0; i < LINES; ++i) {
        map[i] = (char *) malloc(CHARS * sizeof(char));
    }
    char character = (char) getc(mapFile); //stores the first character, this allows the character to be compared without advancing to the next character
    int k = 0, j = 0;
    while(character != EOF) { //while the character does not read the end of the file, continue
        if(character == DOT || character == GHOST || character == PACMAN || character == WALL) { //filter the objects that will be interacted with throughout the game
            map[k][j] = character; //store the filtered characters into the array
            character = (char) getc(mapFile);
            j++; //increment the characters
        } else if (character == '\n') { //if a new line has been reached the array must begin filling up a new row and therefore the column must be set to 0 as well
            k++;
            j = 0;
            character = (char) getc(mapFile);
        } else {
            character = (char) getc(mapFile); //if none of the characters match the filtered characters, advance to the next
        }
    }
    map[0][CHARS-1] = GHOST; //adds the third ghost to the array (allows for the provided text file to remain the same)
    map[LINES-1][0] = GHOST; //adds the fourth ghost to the array
    fclose(mapFile); //closes the map txt file
    return map;
}

char **pacman = readMap(); //sets the array returned from the readMap function to a global 2D array of strings so that it can be directly referenced in other functions and will not have to be input as a parameter
char ghostPreviousChar = DOT, ghostTwoPreviousChar = DOT, ghostThreePreviousChar = DOT, ghostFourPreviousChar = DOT;
int playerX = CHARS/2, playerY = LINES/2;
int ghostOneX = 0, ghostOneY = 0; //global variables are created for the tracking of ghost positions, players position, and the previous character the ghosts were located on
int ghostTwoX = CHARS - 1, ghostTwoY = LINES - 1;
int ghostThreeX = CHARS -1, ghostThreeY = 0;
int ghostFourX = 0, ghostFourY = LINES - 1;

void printMap(){ //function alters the pacman array to print a proper looking map with spaces and walls on all sides, this function also includes the addition of colours
    for (int i = 0; i < LINES + 2; ++i) { //top row of walls
        colourChange(BLUE);
        printf("W  ");
    }
    printf("\nW  ");
    for (int i = 0; i < LINES; ++i) { //checks what characters are where and changes their colours while printing them
        for (int j = 0; j < CHARS; ++j) {
            switch (pacman[i][j]) {
                case WALL:
                    colourChange(BLUE);
                    printf("%c  ", pacman[i][j]);
                    break;
                case GHOST:
                    colourChange(PINK);
                    printf("%c  ", pacman[i][j]);
                    break;
                case PACMAN:
                    colourChange(YELLOW);
                    printf("%c  ", pacman[i][j]);
                    break;
                case DOT:
                    colourChange(WHITE);
                    printf("%c  ", pacman[i][j]);
                    break;
                default:
                    printf("%c  ", ' ');
                    break;
            }
            if (j == CHARS -1) { //if the last character in a column has been reached a final wall and a newline character will be printed
                colourChange(BLUE);
                printf("W  %cW  ", '\n');
            }
        }
    }
    for (int i = 0; i < LINES + 1; ++i) { //the last line of walls is printed
        colourChange(BLUE);
        printf("W  ");
    }
    printf("\n");
    colourChange(WHITE); //sets the default colour to white to avoid future colour changes
}

int winCheck(){ //checks if there are any dots on the map, if there are none the game has been won
    for (int i = 0; i < LINES; ++i) {
        for (int j = 0; j < CHARS; ++j) {
            if (pacman[i][j] == DOT)
                return 0; //game won = false
        }
    }
    return 1; //game won = true
}

int isWall(int x, int y, char direction) { //isWall() Accepts the coordinates and the direction as parameters. It returns 1 if the nearest tile or 0 if not
    switch (direction) {
        case UP:
            if (y >= 1) { //stops out of bounds parameters to be input
                if (pacman[y - 1][x] == WALL || pacman[y - 1][x] == '\0' || pacman[y - 1][x] =='\n') { //checks if the desired position is occupied by the characters mentioned (which prohibits the pacmans movement to that location)
                    return 1;
                } else {
                    return 0;
                }
            } return 1;
        case DOWN:
            if (y <= LINES - 2) { //stops out of bounds parameters to be input
                if (pacman[y + 1][x] == WALL || pacman[y + 1][x] == '\0' || pacman[y + 1][x] =='\n') { //checks if the desired position is occupied by the characters mentioned (which prohibits the pacmans movement to that location)
                    return 1;
                } else {
                    return 0;
                }
            } return 1;
        case RIGHT:
            if (pacman[y][x + 1] == WALL || pacman[y][x + 1] == '\0' || pacman[y][x + 1] =='\n') { //checks if the desired position is occupied by the characters mentioned (which prohibits the pacmans movement to that location)
                return 1;
            } else {
                return 0;
            }
        case LEFT:
            if (pacman[y][x - 1] == WALL || pacman[y][x - 1] == '\0' || pacman[y][x - 1] =='\n') { //checks if the desired position is occupied by the characters mentioned (which prohibits the pacmans movement to that location)
                return 1;
            } else {
                return 0;
            }
        default:
            return 1;
    }
}

char randomGhostDirection(int ghostX, int ghostY) { //function randomly determines a viable direction given a current position
    char randomDirection = EMPTY;
    srand(time(nullptr)); //allows variance in the random directions supplied by the function
    while(isWall(ghostX, ghostY, randomDirection) == 1) { //while the random direction will cause a crash into a wall, continue looking for a new random direction
        int randomNumber = rand() % 4+1; //randomly determines an integer
        if(randomNumber == 1) { //depending on the integer, assign a direction
            randomDirection = UP;
        } else if (randomNumber == 2) {
            randomDirection = RIGHT;
        } else if (randomNumber == 3) {
            randomDirection = DOWN;
        } else {
            randomDirection = LEFT;
        }
    }
    return randomDirection; //return the direction
}

char ghostOneDirection = randomGhostDirection(ghostOneX, ghostOneY); //function randomly determines the initial direction of the 4 ghosts and stores them as global variables
char ghostTwoDirection = randomGhostDirection(ghostTwoX, ghostTwoY);
char ghostThreeDirection = randomGhostDirection(ghostThreeX, ghostThreeY);
char ghostFourDirection = randomGhostDirection(ghostFourX, ghostFourY);

void ghostDirectionMatch (int x, int y, char direction)  { //necessary check to assign the direction of a ghost if it matches the input location parameters
    if (y == ghostOneY && x == ghostOneX)
        ghostOneDirection = direction;
    if (y == ghostTwoY && x == ghostTwoX)
        ghostTwoDirection = direction;
    if (y == ghostThreeY && x == ghostThreeX)
        ghostThreeDirection = direction;
    if (y == ghostFourY && x == ghostFourX)
        ghostFourDirection = direction;
}

void ghostPositionMatch(char previous, char temporary) { //function determines if any ghosts are colliding and makes necessary adjustments to their replacement character storage
    if ((ghostOneY == ghostTwoY && ghostOneX == ghostTwoX) || (ghostOneY == ghostThreeY && ghostOneX == ghostThreeX) || (ghostOneY == ghostFourY && ghostOneX == ghostFourX))
        previous = temporary;
    if ((ghostTwoY == ghostOneY && ghostTwoX == ghostOneX) || (ghostTwoY == ghostThreeY && ghostTwoX == ghostThreeX) || (ghostTwoY == ghostFourY && ghostTwoX == ghostFourX))
        previous = temporary;
    if ((ghostThreeY == ghostTwoY && ghostThreeX == ghostTwoX) || (ghostThreeY == ghostOneY && ghostThreeX == ghostOneX) || (ghostThreeY == ghostFourY && ghostThreeX == ghostFourX))
        previous = temporary;
    if ((ghostFourY == ghostTwoY && ghostFourX == ghostTwoX) || (ghostFourY == ghostThreeY && ghostFourX == ghostThreeX) || (ghostFourY == ghostOneY && ghostFourX == ghostOneX))
        previous = temporary;
}

void playerTrack(int ghostX, int ghostY) {//function causes the ghosts to track the player if they are seen head on without any walls
    if (playerY == ghostY) { //if they are in line vertically
        int difference = playerX - ghostX;
        if (difference < 0) { //the difference between positions is calculated and used to determine which direction the movement must be in
            for (int i = ghostX; i > playerX; i--){ //considers the increments necessary to be at the players position and checks if there is a wall at any point
                if (isWall(i, ghostY, LEFT) == 1) {
                    return;
                }
            }
            ghostDirectionMatch(ghostX, ghostY, LEFT);
            return;
        }
        if (difference > 0) { //the difference between positions is calculated and used to determine which direction the movement must be in
            for (int i = ghostX; i < playerX; i++){ //considers the increments necessary to be at the players position and checks if there is a wall at any point
                if (isWall(i, ghostY, RIGHT) == 1) {
                    return;
                }
            }
            ghostDirectionMatch(ghostX, ghostY, RIGHT);
            return;
        }
    } else if (playerX == ghostX) { //if they are in line horizontally
        int difference = playerY - ghostY;
        if (difference < 0) { //the difference between positions is calculated and used to determine which direction the movement must be in
            for (int i = ghostY; i > playerY; i--){ //considers the increments necessary to be at the players position and checks if there is a wall at any point
                if (isWall(ghostX, i, UP) == 1) {
                    return;
                }
            }
            ghostDirectionMatch(ghostX, ghostY, UP);
            return;
        }
        if (difference > 0) { //the difference between positions is calculated and used to determine which direction the movement must be in
            for (int i = ghostY; i < playerY; i++){ //considers the increments necessary to be at the players position and checks if there is a wall at any point
                if (isWall(ghostX, i, DOWN) == 1) {
                    return;
                }
            }
            ghostDirectionMatch(ghostX, ghostY, DOWN);
            return;
        }
    }
}

void generalGhostMove(int* x, int* y, char* direction, char* previous) { //function is created to allow any global variable position for the 4 ghosts and determine then go through viable movement
    playerTrack(*x, *y); //checks if the playerTrack function has seen a player and therefore changed the direction for the ghost in question
    char temp = '\0', temp2 = '\0';
    if (*direction == UP && isWall(*x, *y, *direction) == 0) { //checks if the direction will cause a collision with the wall
        temp = pacman[*y - 1][*x]; //stores the character in the ghosts desired position
        temp2 = *previous; //updates where the character in the ghosts current position is stored
        pacman[*y - 1][*x] = GHOST; //sets the ghost in the desired location based on the direction provided
        *previous = temp; //sets the global variable for previous position to the temp value (which stores the character in the ghosts desired position as seen above)
        ghostPositionMatch(*previous, temp2); //determines if after the movements any ghosts are colliding and makes the necessary changes to the storage of previous characters
        if(*previous == PACMAN || *previous == GHOST) { //used to avoid the creation of multiple ghosts and pacman characters during movement
            *previous = EMPTY;
        }
        pacman[*y][*x] = *previous; //updates the previous position of the ghost to display what was located there before it initially moved there
        *y = *y - 1; //increments the globally stored position of the ghost
    } else if (*direction == DOWN && isWall(*x, *y, *direction) == 0) { //acts the same as the UP function with minimal changes to the incrementing to account for downwards movement
        temp = pacman[*y + 1][*x];
        temp2 = *previous;
        pacman[*y + 1][*x] = GHOST;
        *previous = temp;
        ghostPositionMatch(*previous, temp2);
        if(*previous == PACMAN || *previous == GHOST) {
            *previous = EMPTY;
        }
        pacman[*y][*x] = *previous;
        *y = *y + 1;
    } else if (*direction == RIGHT && isWall(*x, *y, *direction) == 0) { //acts the same as the UP function with minimal changes to the incrementing to account for right movement
        temp = pacman[*y][*x + 1];
        temp2 = *previous;
        pacman[*y][*x + 1] = GHOST;
        *previous = temp;
        ghostPositionMatch(*previous, temp2);
        if(*previous == PACMAN || *previous == GHOST) {
            *previous = EMPTY;
        }
        pacman[*y][*x] = *previous;
        *x = *x + 1;
    } else if (*direction == LEFT && isWall(*x, *y, *direction) == 0) { //acts the same as the UP function with minimal changes to the incrementing to account for left movement
        temp = pacman[*y][*x - 1];
        temp2 = *previous;
        pacman[*y][*x - 1] = GHOST;
        *previous = temp;
        ghostPositionMatch(*previous, temp2);
        if(*previous == PACMAN || *previous == GHOST) {
            *previous = EMPTY;
        }
        pacman[*y][*x] = *previous;
        *x = *x - 1;
    } else {
        *direction = randomGhostDirection(*x, *y); //if none of the directions are input the direction is randomized
        generalGhostMove(x, y, direction, previous); //runs the ghost movement again in a recursive fashion to successfully move the previously stalled ghost
    }
}

int loseCheck() { //checks if any of the ghosts are currently located in the same position as the player
    if (ghostOneY == playerY && ghostOneX == playerX || ghostTwoY == playerY && ghostTwoX == playerX || ghostThreeY == playerY && ghostThreeX == playerX || ghostFourY == playerY && ghostFourX == playerX) {
        return 1; //player is dead
    }
    return 0; //player is alive
}

void endProcess (char endStatement[]){ //function is used within the win and loss functions to print the provided statements, print a final map, end the game upon keyboard input and exit the program
    printMap();
    printf("%s\n", endStatement);
    getchar(); //gets rid of any left-over characters so that an addition button press can be recorded to end the program
    if (getchar() != '\0') { //checks for button press
        free(pacman);
        exit(0);
    }
}

void movePlayer() { //function allows the player to move, replaces previous position with an empty tile removing the dots
    fflush(stdin); //clears any left-over characters allowing multiple character inputs be ignored
    char inputDirection = (char) getchar(); //retrieves one character from the user
    if(isWall(playerX, playerY, inputDirection) == 0) { //can move there meaning no wall is located there
        switch (inputDirection) { //given the direction increment the players location differently
            case UP:
                pacman[playerY][playerX] = EMPTY; //previous position is empty (removes dots)
                playerY--; //increments the global variable
                pacman[playerY][playerX] = PACMAN; //current position is set based on the previously incremented index
                break;
            case DOWN:
                pacman[playerY][playerX] = EMPTY;
                playerY++;
                pacman[playerY][playerX] = PACMAN;
                break;
            ck    pacman[playerY][playerX] = EMPTY;
                playerX++;
                pacman[playerY][playerX] = PACMAN;
                break;
            case LEFT:
                pacman[playerY][playerX] = EMPTY;
                playerX--;
                pacman[playerY][playerX] = PACMAN;
                break;
            default:
                break;
        }
    }
}

int main() { //main function, loops the process of the movement in the objects, also prompts the player for the number of ghosts to include in the game
    int numGhosts = 0; //base number of ghosts is zero
    char winStatement[] = "Congratulations! You win! Press any key to exit the game";
    char loseStatement[] = "Sorry, you lose. Press any key to exit the game";
    while(numGhosts == 0) { //while ghosts is zero continue prompting for a valid number of ghosts
        printf("Enter Number of Ghosts (1-4)\n"); //prompt for number of ghosts between 1-4
        switch (getchar()) { //given the provided number (character)
            case '1':
                numGhosts = 1;
                pacman[ghostTwoY][ghostTwoX] = DOT; //sets all the other static ghosts on the map too dots
                pacman[ghostThreeY][ghostThreeX] = DOT;
                pacman[ghostFourY][ghostFourX] = DOT;
                ghostTwoX = outOfBounds, ghostTwoY = outOfBounds; //ensures that the static ghosts (now appearing as dots) do not act as ghosts
                ghostThreeX = outOfBounds, ghostThreeY = outOfBounds;
                ghostFourX = outOfBounds, ghostFourY = outOfBounds;
                break;
            case '2':
                numGhosts = 2;
                pacman[ghostThreeY][ghostThreeX] = DOT;
                pacman[ghostFourY][ghostFourX] = DOT;
                ghostThreeX = outOfBounds, ghostThreeY = outOfBounds;
                ghostFourX = outOfBounds, ghostFourY = outOfBounds;
                break;
            case '3':
                numGhosts = 3;
                pacman[ghostFourY][ghostFourX] = DOT;
                ghostFourX = outOfBounds, ghostFourY = outOfBounds;
                break;
            case '4':
                numGhosts = 4;
                break;
            default:
                getchar();
                numGhosts = 0;
        }
    }
    do {
        printMap(); //prints the map
        movePlayer(); //moves the player
        if(numGhosts >= 1) { //each if statement will allow the different amount of ghosts to run depending on the number input
            generalGhostMove(&ghostOneX, &ghostOneY, &ghostOneDirection, &ghostPreviousChar);
            if (loseCheck() == 1) //checks if player has lost, must check after each ghost is moved
                endProcess(loseStatement);
        }
        if (numGhosts >= 2) {
            generalGhostMove(&ghostTwoX, &ghostTwoY, &ghostTwoDirection, &ghostTwoPreviousChar);
            if (loseCheck() == 1) //checks if player has lost, must check after each ghost is moved
                endProcess(loseStatement);
        }
        if (numGhosts >= 3) {
            generalGhostMove(&ghostThreeX, &ghostThreeY, &ghostThreeDirection, &ghostThreePreviousChar);
            if (loseCheck() == 1) //checks if player has lost, must check after each ghost is moved
                endProcess(loseStatement);
        }
        if (numGhosts == 4) {
            generalGhostMove(&ghostFourX, &ghostFourY, &ghostFourDirection, &ghostFourPreviousChar);
            if (loseCheck() == 1) //checks if player has lost, must check after each ghost is moved
                endProcess(loseStatement);
        }
        if (winCheck() == 1) //checks if the player has won
            endProcess(winStatement);
    } while (winCheck() == 0);
}