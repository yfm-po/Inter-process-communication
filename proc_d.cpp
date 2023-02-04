#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

int semaphore, port, socket_, infile;
char *sharedMemory = NULL;
struct sembuf semb[1];
struct sockaddr_in addr;

auto sigKill() -> void 
{
    printf("d off\n");
    exit(EXIT_SUCCESS);
}

auto throwExceptionOnWrongArguments(int argc, char **argv) -> void
{
    if (argc != 4)
    {
        printf("Wrong number of arguments\n");
        exit(EXIT_FAILURE);
    }
}

auto main(int argc, char **argv) -> int
{
    throwExceptionOnWrongArguments(argc, argv);

    semaphore = atoi(argv[2]);
    port = atoi(argv[3]);
    sharedMemory = (char *) shmat(((int)atoi(argv[1])), NULL, 0);

    if (!sharedMemory)
    {
        printf("Error attaching shared memory\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Attached shared memory by proc_d\n");
    }

    kill(getppid(), SIGUSR1);
    signal(SIGUSR2, (__sighandler_t)sigKill);

    if ((socket_ = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Error creating socket\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Created socket by proc_d\n");
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(port);
    bzero(&(addr.sin_zero), 8);

    if (connect(socket_, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        printf("Error connecting to socket\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Connected to socket by proc_d\n");
    }

    infile = open("dd.txt", O_CREAT | O_WRONLY | O_TRUNC, 0777);

    while (true)
    {
        semb[0].sem_num = 1;
        semb[0].sem_op = -1;
        semb[0].sem_flg = (int)0x1000;

        if (semop(semaphore, semb, 1) < 0)
        {
            printf("semop d\n");
            exit(EXIT_FAILURE);
        }

        if (write(socket_, sharedMemory, strlen(sharedMemory) + 1) != strlen(sharedMemory) + 1)
        {
            printf("Error writing to socket\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            //sleep(3);
            write(infile, sharedMemory, strlen(sharedMemory));
            write(infile, "\n", 1);
            printf("Wrote to socket by proc_d\n");
        }

        //shmctl(((int)atoi(argv[1])), IPC_RMID, NULL);

        semb[0].sem_num = 0;
        semb[0].sem_op = 1;
        semb[0].sem_flg = (int)0x1000;

        if (semop(semaphore, semb, 1) < 0)
        {
            printf("semop d\n");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}
