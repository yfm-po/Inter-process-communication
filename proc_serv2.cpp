#include <cerrno>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/socket.h> 
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>

int portTwo, result, sockD;
struct sockaddr_in addr;
char buffer[150];

auto throwExceptionOnWrongArguments(int argc, char **argv) -> void
{
    if (argc != 2)
    {
        printf("Wrong number of arguments\n");
        exit(EXIT_FAILURE);
    }
}

auto main(int argc, char **argv) -> int
{
    throwExceptionOnWrongArguments(argc, argv);

    portTwo = atoi(argv[1]);

    if (portTwo == 0)
    {
        printf("Error converting port number\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Converted port number\n");
    }

    if ((result = open("serv2.txt", O_CREAT | O_WRONLY | O_TRUNC, 0777)) == -1)
    {
        printf("Error opening file\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Opened/created serv2.txt\n");
    }

    if ((sockD = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("Error creating socket\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Created socket\n");
    }

    bzero((char*)&addr, sizeof(addr));
    addr.sin_port = htons(portTwo);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockD, (struct sockaddr *) &addr, sizeof(addr)) < 0)
    {
        printf("Error binding socket\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Bound socket\n");
    }

    kill(getppid(), SIGUSR1);

    int bytes_received = 0;
    while(true)
    {
        bytes_received = recv(sockD, buffer, 150, 0);
        printf("proc_serv2: bytes_received: %d\n", bytes_received);
        if (bytes_received == 0) break;
        else if (bytes_received == -1) {
            printf("proc_serv2: error! errno: %d (%s)\n", errno, strerror(errno));
            break;
        }
        write(result, buffer, strlen(buffer));
        write(result, "\n", 1);
        printf("proc_serv2: buffer: %s\n", buffer);
    }

    kill(getppid(), SIGUSR2);
    exit(EXIT_SUCCESS);
}
