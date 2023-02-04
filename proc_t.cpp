#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

int input, out, shm, sem; 
int readIdx = 0;
char buffer[1024];
char *output = NULL;
struct sembuf semStructure[1];
int shmid;

auto throwExceptionOnWrongArguments(int argc, char **argv) -> void
{
    if (argc < 4)
    {
        printf("Wrong number of arguments\n");
        exit(EXIT_FAILURE);
    }
}

auto endProcess() -> void 
{
    exit(EXIT_SUCCESS);
}

auto setUpValues(char **argv) -> void
{
    memset(buffer, '\0', sizeof(buffer));
    input = atoi(argv[1]);
    shmid = atoi(argv[2]);
    sem = atoi(argv[3]);

    output = (char *) shmat(shmid, NULL, 0);

    if (!output)
    {
        printf("Error attaching shared memory\n");
        exit(EXIT_FAILURE);
    }
}

auto readInput(int input, char *buffer, char** argv) -> void
{
    char reader;
    while (true)
    {
        read(input, &reader, 1);
        if (reader == '\n') break;
        buffer[readIdx++] = reader;
    }

    buffer[readIdx] = '\0';
    readIdx = 0;
}

auto writeToFile(char *output, char buffer[], int out) -> void 
{
    printf("proc_t: buffer: %s\n", buffer);
    write(out, buffer, strlen(buffer) + 1);
    //write(out, "\n", strlen("\n"));

    char temp[500] = {0};
    sprintf(temp, "%s", buffer);
    memcpy(output, temp, strlen(temp) + 1);
}

auto setSemOp(int sem_num, int sem_op) -> void
{
    semStructure[0].sem_num = sem_num;
    semStructure[0].sem_op = sem_op;
    semStructure[0].sem_flg = (int)0x1000;
    semop(sem, semStructure, 1);
}

auto main(int argc, char **argv) -> int
{
    throwExceptionOnWrongArguments(argc, argv);
    setUpValues(argv);

    kill(getppid(), SIGUSR1);
    signal(SIGUSR2, (__sighandler_t)endProcess);

    if ((out = open("t.txt", O_WRONLY | O_CREAT | O_TRUNC, 0777)) == -1) printf("Error opening file.\n");

    struct sembuf sops = {0, -1, 0};
    while (true)
    {
        // Wait for us to become 1:
        //printf("proc_t: waiting for our semaphore to become 1...\n");
        sops.sem_num = 0;
        sops.sem_op = -1;
        semop(sem, &sops, 1);
        //printf("proc_t: semaphore[0]: 1\n");
        
        readInput(input, buffer, argv);
        writeToFile(output, buffer, out);

        // printf("proc_t: we wrote something to the file!\n");

        // Signal/set semaphore(s):
        sops.sem_num = 1;
        sops.sem_op = 1;
        semop(sem, &sops, 1);

        //printf("proc_t: end of loop\n");
    }

    exit(EXIT_SUCCESS);
}
