#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

FILE* filePointer;
int outputPipe;
size_t BUFFER_SIZE = 1024;

auto throwExceptionOnWrongArguments(int argc, char **argv) -> void
{
    if (argc < 3)
    {
        printf("Wrong number of arguments. Expected 2 more after '%s', got %d\n", argv[0], argc - 1);
        printf("Usage: %s <ppid> <pipe[0]>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
}

auto pipeWriter() -> void
{
    char* buffer = (char *) calloc(BUFFER_SIZE, sizeof(char));

    if (getline(&buffer, &BUFFER_SIZE, filePointer) == -1)
    {
        printf("Error reading from file 'p1.txt'\n");
        exit(EXIT_FAILURE);
    }

    if (write(outputPipe, buffer, strlen(buffer)) == -1)
    {
        printf("Error writing to pipe\n");
        exit(EXIT_FAILURE);
    }

    if (buffer) free(buffer);
}

auto pipeKiller() -> void
{
    fclose(filePointer);
    printf("P1: Finished\n");
    exit(EXIT_SUCCESS);
}

auto main(int argc, char **argv) -> int
{
    throwExceptionOnWrongArguments(argc, argv);

    outputPipe = atoi(argv[1]);
    pid_t ppid = getppid();
    
    kill(ppid, SIGUSR1);

    if ((filePointer = fopen("p1.txt", "r")) == NULL)
    {
        printf("Error opening file 'p1.txt'\n");
        exit(EXIT_FAILURE);
    }

    printf("P1: Started\n");

    signal(SIGUSR1, (__sighandler_t)pipeWriter);
    signal(SIGUSR2, (__sighandler_t)pipeKiller);

    printf("P1: Ended\n");

    while (true) pause();

    if (filePointer) fclose(filePointer);
    exit(EXIT_SUCCESS);
}