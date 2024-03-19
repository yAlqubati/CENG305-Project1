#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define ROUND 3

// the values here will be overwritten by the command line arguments
int BOARD_SIZE = 10;
int NUM_PLAYERS = 4;

struct player {
    int turn;
    int x;
    int y;
    int guess_x;
    int guess_y;
    int distance;
};

typedef struct player player;

// a mutex to protect the shared variable turn
pthread_mutex_t lock;
player players[20];

// a shared variable to keep track of the turn...so we know whose turn it is and to avoid starvation
int turn = 0;
// a shared variable to keep track of the round number
int roundNum = 1;

// calculate the Manhattan distance between the player and the guess
int calculateManhattanDistance(player *player1, int guess_x, int guess_y) {
    return abs(player1->x - guess_x) + abs(player1->y - guess_y);
}

void play(player *p) {
    while(turn != p->turn); // busy wait

        pthread_mutex_lock(&lock);

        // only the first player prints the round number
        if (turn == 0) {
            printf("----------Round-%d----------\n", roundNum);
            roundNum++;
        }

        srand (time(NULL) + p->turn * 600 + roundNum * 100);

        printf("%d.guess of player %d: ", roundNum - 1, p->turn + 1);
        p->guess_x = rand() % BOARD_SIZE;
        p->guess_y = rand() % BOARD_SIZE;
        printf("[%d, %d]\n", p->guess_x, p->guess_y);

        // calculate the distance from the other players
        for(int i = 0; i < NUM_PLAYERS; i++) {
            if (i != p->turn) {
                int distance = calculateManhattanDistance(&players[i], p->guess_x, p->guess_y);
                printf("Distance from player %d: %d\n", i + 1, distance);
                if (distance < p->distance) {
                    p->distance = distance;
                }

                // if the distance is 0, then the player won the game
                if(distance == 0) {
                    printf("Player %d won the game!\n", p->turn + 1);
                    exit(0);
                }
            }
        }

        // update the turn to the next player
        turn = (turn + 1) % NUM_PLAYERS;
        pthread_mutex_unlock(&lock);
        pthread_exit(NULL);
        
        
    
}

// the function that will be called by each thread to play the game and exit after the game is over
void *startRound(void *arg) {
    player *p = (player *)arg;
    play(p);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {

    if(argc == 3) {
        BOARD_SIZE = atoi(argv[1]);
        NUM_PLAYERS = atoi(argv[2]);
    }

    else {
        printf("Please enter the board size and the number of players as command line arguments\n");
        return 0;
    }

    printf("%dx%d map is created\n", BOARD_SIZE, BOARD_SIZE);
    printf("%d threads are created\n", NUM_PLAYERS);

    pthread_t threads[NUM_PLAYERS];

    // Initialize mutex
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("Mutex initialization failed\n");
        return 1;
    }

    srand (time(NULL));
    
    for (int i = 0; i < NUM_PLAYERS; i++) {
        players[i].turn = i;
        players[i].x = rand() % BOARD_SIZE;
        players[i].y = rand() % BOARD_SIZE;
        players[i].distance = BOARD_SIZE * BOARD_SIZE;
    }
    printf("Coordinates of the players are chosen randomly\n");

    for(int i = 0; i < NUM_PLAYERS; i++) {
        printf("Player %d: [%d, %d], ", i + 1, players[i].x, players[i].y);
    }
    printf("\n");
    printf("Game launches\n");
    
        for(int r = 0; r < ROUND; r++) {
            for (int i = 0; i < NUM_PLAYERS; i++) {
                pthread_create(&threads[i], NULL, startRound, (void *)&players[i]);
            }

            for (int i = 0; i < NUM_PLAYERS; i++) {
                pthread_join(threads[i], NULL);
            }
        }

    

    // sort the players according to their distance
    for(int i = 0; i < NUM_PLAYERS; i++) {
        for(int j = i + 1; j < NUM_PLAYERS; j++) {
            if(players[i].distance > players[j].distance) {
                player temp = players[i];
                players[i] = players[j];
                players[j] = temp;
            }
        }
    }



    int index = 0;
    printf("the winner with the shortest distance: %d\n", players[index].distance);
    printf("Player %d ", players[index].turn + 1);

    while(1)
    {
        

        if(players[index].distance == players[index + 1].distance) {
            printf("and Player %d ", players[index + 1].turn + 1);
            
        }

        else {
            break;
        }
        index++;
    }
    printf("\n");
    
    

    // Destroy mutex
    pthread_mutex_destroy(&lock);

    return 0;
}

