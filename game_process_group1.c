#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define NUM_PLAYERS 2
#define ROUND 3
#define READ 0
#define WRITE 1

// Board size will be changed from command line
int BOARD_SIZE = 10;

struct player {
    int turn;
    int x;
    int y;
    int last_guess_x;
    int last_guess_y;
    int distance;
};

typedef struct player player;

player players[NUM_PLAYERS];

int calculateManhattanDistance(player *player1, int guess_x, int guess_y) {
    return abs(player1->x - guess_x) + abs(player1->y - guess_y);
}

void goodGuess(player *p) {
    // get data from the previous guess
    int lastX = p->last_guess_x - p->x;
    int lastY = p->last_guess_y - p->y;

    // Make an intelligent guess based on the direction of the previous guess
    if (abs(lastX) > abs(lastY)) {
        // If the previous guess was more in the X direction, continue in that direction
        p->last_guess_x = (lastX > 0) ? (p->last_guess_x + 1) : (p->last_guess_x - 1);
        p->last_guess_y = p->last_guess_y;
    } else {
        // If the previous guess was more in the Y direction, continue in that direction
        p->last_guess_x = p->last_guess_x;
        p->last_guess_y = (lastY > 0) ? (p->last_guess_y + 1) : (p->last_guess_y - 1);
    }

    // Ensure the guess is within the board boundaries
    p->last_guess_x = (p->last_guess_x < 0) ? 0 : (p->last_guess_x >= BOARD_SIZE) ? (BOARD_SIZE - 1) : p->last_guess_x;
    p->last_guess_y = (p->last_guess_y < 0) ? 0 : (p->last_guess_y >= BOARD_SIZE) ? (BOARD_SIZE - 1) : p->last_guess_y;
}




// player2 is here to calculate the distance
void play(player *p, player *p2) {
    if (p->distance == BOARD_SIZE) {
        // First guess is random
        p->last_guess_x = rand() % BOARD_SIZE;
        p->last_guess_y = rand() % BOARD_SIZE;
    } else {
        // Make an intelligent guess based on last guess
        goodGuess(p);
    }

    printf("Guess of player %d: [%d, %d]\n", p->turn + 1, p->last_guess_x, p->last_guess_y);

    // calculate the distance from the other player
    int distance = calculateManhattanDistance(p2, p->last_guess_x, p->last_guess_y);

    if (p->turn == 0) {
        printf("Distance from player %d: %d\n", 2, distance);
    } else {
        printf("Distance from player %d: %d\n", 1, distance);
    }

    // if distance is 0, then the player has won
    if (distance == 0) {
        printf("*********************************************\n");
        printf("Player %d has won!\n", p->turn + 1);
        printf("*********************************************\n");

        // Reset last guess coordinates
        p->last_guess_x = 0;
        p->last_guess_y = 0;

        exit(0);
    }

    // update the distance
    if (distance < p->distance) {
        p->distance = distance;
    }
}


int main(int argc, char *argv[]) {

    if(argc == 2)
    {
        BOARD_SIZE = atoi(argv[1]);
    }

    printf("%dx%d map is created\n", BOARD_SIZE, BOARD_SIZE);
    printf("A child process is created\n");
    srand(time(NULL));

    int childX = BOARD_SIZE;
    int childY = BOARD_SIZE;
    int enter = 0;

    // pipes for communication between parent and child
    int childToParent[2];

    if (pipe(childToParent) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    printf("Position of players:\n");
    for (int i = 0; i < NUM_PLAYERS; i++) {
        players[i].turn = i;
        players[i].x = rand() % BOARD_SIZE;
        players[i].y = rand() % BOARD_SIZE;
        players[i].distance = BOARD_SIZE; // Initialize distance to maximum
    }

    printf("Coordinates of the players are chosen randomly\n");
    printf("Player 1: [%d, %d] , Player 2: [%d, %d]\n", players[0].x, players[0].y, players[1].x, players[1].y);
    printf("Game laucnhes-->\n");	

    for (int round = 0; round < ROUND; round++) {
        // only parent process executes this
        printf("---------- ROUND %d ----------\n", round + 1);

        pid_t pid = fork();

        if (pid == 0) {
            
            // close unused pipes
            close(childToParent[READ]);

            if(enter == 0)
            {
                enter = 1;
            }
            else
            {
                players[0].last_guess_x = childX;
                players[0].last_guess_y = childY;
            }
            

            play(&players[0], &players[1]);

            childX = players[0].last_guess_x;
            childY = players[0].last_guess_y;
            

            // write the distance to the parent
            write(childToParent[WRITE], &players[0].distance, sizeof(int));
            close(childToParent[WRITE]);
            exit(0);
        }
        
         else if (pid > 0) {
            // close unused pipes
        close(childToParent[WRITE]);

        // Wait for child to finish
        wait(NULL);

        // read the distance from the child and update the distance
        int childDistance;
        read(childToParent[READ], &childDistance, sizeof(int));
        if(childDistance < players[0].distance) {
            players[0].distance = childDistance;
        }

        // Parent makes a guess
        play(&players[1], &players[0]);

        close(childToParent[READ]);

            wait(NULL);
        }

         else {
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }

    // Determine winner based on distance
    if (players[0].distance < players[1].distance) {
        printf("Player 1 wins with the shortest distance: %d\n", players[0].distance);
    } else if (players[1].distance < players[0].distance) {
        printf("Player 2 wins with the shortest distance: %d\n", players[1].distance);
    } else {
        printf("It's a tie! Both players have the same distance: %d\n", players[0].distance);
    }

    return 0;
}
