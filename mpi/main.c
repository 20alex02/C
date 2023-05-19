#include <stdio.h>
#include <mpi/mpi.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

int get_processes_playing(int processes, const int coinflips[processes]) {
    int sum = 0;
    for (int i = 0; i < processes; ++i) {
        sum += coinflips[i];
    }
    return sum;
}

int get_winner(int processes, const int coinflips[processes]) {
    for (int i = 0; i < processes; ++i) {
        if (coinflips[i] == 1) {
            return i;
        }
    }
    return -1;
}

void play(int processes, int rank) {
    bool is_playing = true;
    int processes_playing;
    int coinflips[processes];
    int round = 0;
    srand(time(NULL) + rank);
    do {
        coinflips[rank] = is_playing ? rand() % 2 : 0;
        for (int process = 0; process < processes; ++process) {
            MPI_Bcast(&coinflips[process], 1, MPI_INT, process, MPI_COMM_WORLD);
        }
        processes_playing = get_processes_playing(processes, coinflips);
        if (processes_playing != 0 && coinflips[rank] == 0) {
            is_playing = false;
        }
        round++;
    } while (processes_playing != 1);

    int winner = get_winner(processes, coinflips);
    if (rank != winner) {
        printf("Sloužím ti, můj vládče, slunce naše jasné.%d\n", winner);
    }
}

int main(int argc, char *argv[]) {
    int processed, rank;
    MPI_Init (&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &processed);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        printf("525178\n");
    }
    play(processed, rank);

    MPI_Finalize();
}