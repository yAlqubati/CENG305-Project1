# Multiplayer Game Implementation

This repository contains implementations of a multiplayer game using both multiprocess and multithread approaches.

## Multiprocess Implementation

In the multiprocess implementation, two processes are employed: the child process representing Player 1 and the parent process representing Player 2. Synchronization between the processes is achieved through pipes and a busy-waiting mechanism. The game concludes if a player correctly guesses the opponent's position or if no correct guesses are made, the winner is determined by the shortest distance.

## Multithread Implementation

In the multithreaded game implementation, each player is represented as a thread. Synchronization is achieved through busy-waiting and mutex locks. The game concludes similarly to the multiprocess implementation.

## Usage

1. **Compile the Code**:
    - Use the provided Makefile to compile the code. Simply run:
      ```
      make
      ```

2. **Run the Code**:
    - After compilation, execute the compiled executable files:
      ```
      ./game_process map-size
      ```
      or
      ```
      ./game_thread map_size thread_number
      ```

3. **Cleaning Up**:
    - To remove the generated executables, use:
      ```
      make clean
      ```

## contributors
- [ABDULKAREEM MANSOOR QASEM ABDO AL-SARORI](https://github.com/karim-mansoor)
- [HASAN BERA KABADAĞI](https://github.com/Yesodeli)
- [YAHYA ELİMAM](https://github.com/yhyimm)
- [Yousef Saif](https://github.com/yAlqubati)

