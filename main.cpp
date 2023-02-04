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
#include <stdarg.h>
#include <errno.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define COLOR_RED   "\x1B[31m"
#define COLOR_RESET "\x1B[0m"

#define print(format, ...)                                \
    {                                                     \
        printf("%s::%s(): ", __FILENAME__, __FUNCTION__); \
        printf(format, ##__VA_ARGS__);                    \
    }

#define error(format, ...)                                                             \
    {                                                                                  \
        fprintf(stderr, "%s%s::%s(): ERROR: ", COLOR_RED, __FILENAME__, __FUNCTION__); \
        fprintf(stderr, format, ##__VA_ARGS__);                                        \
        fprintf(stderr, COLOR_RESET);                                                  \
        exit(EXIT_FAILURE);                                                            \
    }

int signal_sync_status = 0;

void handle_sync_signal() {
    ++signal_sync_status;
    print("got a sync signal: %d\n", signal_sync_status);
}

void wait_for_signal_sync(const char* proc_name) {
    int prev_signal_sync_status = signal_sync_status;
    print("waiting for signal sync status from process %s...\n", proc_name);
    while (signal_sync_status == prev_signal_sync_status) sleep(1);
}

pid_t start_proc(char* proc_name, int argc, ...) {
    // Init variadic argument handling:
    va_list args;
    va_start(args, argc);

    // Add arguments to the child program (argv[]):
    char* args_to_pass[20] = {0};
    args_to_pass[0] = proc_name; // argv[0] is always expected to be the program name!

    int i = 0;
    for (i = 0; i < argc + 1; ++i) {
        char* result = va_arg(args, char*); // Grab the next argument
        args_to_pass[i + 1] = result;
    }
    args_to_pass[i] = NULL; // Last argument always has to be NULL when using exec__() functions!

    print("[debug] Printing args for process %s:\n", proc_name);
    for (int x = 0; x < i; ++x) {
        if (args_to_pass[x]) 
            print("[%d]: |%s|\n", x, args_to_pass[x]);
    }

    // End of variadic argument handling:
    va_end(args);

    // Fork ourselves to become the child process:
    pid_t proc_fork = fork();
    if (proc_fork == -1) error("fork failed for process %s\n", proc_name);
    if (proc_fork ==  0) {
        int exec_result = execv(proc_name, (char* const*)args_to_pass);
        error("exec_result for process %s was %d (errno: %d -- \"%s\")\n", proc_name, exec_result, errno, strerror(errno));
        exit(EXIT_SUCCESS); // TODO: probably not needed, given that execv() takes over this fork (?)
    }

    // [!!!] Sync:
    wait_for_signal_sync(proc_name);

    // We'll be passing along the fork PIDs sometimes:
    return proc_fork;
}

int main(int argc, char** argv) {
    if (argc != 3) error("Not enough or too many arguments! We need: <port 1> <port 2>\n");

    // Register sync signal handling:
    signal(SIGUSR1, (__sighandler_t)handle_sync_signal);

    {
        // Create pipe set 1:
        int pipe1[2] = {0};
        int pipe1_result = pipe(pipe1);
        if (pipe1_result == -1) error("failed to create pipe set 1!\n");

        int pipe1r = dup(pipe1[0]);
        int pipe1w = dup(pipe1[1]);

        char pipe1r_string[20] = {0};
        char pipe1w_string[20] = {0};
        sprintf(pipe1r_string, "%d", pipe1r);
        sprintf(pipe1w_string, "%d", pipe1w);

        print("pipe1r_string: %s  pipe1w_string: %s\n", pipe1r_string, pipe1w_string);

        // Create pipe set 2:
        int pipe2[2] = {0};
        int pipe2_result = pipe(pipe2);
        if (pipe2_result == -1) error("failed to create pipe set 2!\n");

        int pipe2r = dup(pipe2[0]);
        int pipe2w = dup(pipe2[1]);

        char pipe2r_string[10] = {0};
        char pipe2w_string[10] = {0};
        sprintf(pipe2r_string, "%d", pipe2r);
        sprintf(pipe2w_string, "%d", pipe2w);

        // Semaphores:
        int semaphore_1 = semget(4001, 2, IPC_CREAT | 0666);
        int semaphore_2 = semget(4010, 2, IPC_CREAT | 0666);
        semctl(semaphore_1, 0, SETVAL, 0);
        semctl(semaphore_1, 1, SETVAL, 0);
        semctl(semaphore_2, 0, SETVAL, 0);
        semctl(semaphore_2, 1, SETVAL, 0);

        union semun {
            int val;
            struct semid_ds* buf;
            unsigned short int* array;
            struct seminfo* __buf;
        } sem_un;

        // Initialize default semaphore values:
        sem_un.val = 1;
        semctl(semaphore_1, 0, SETVAL, sem_un);
        semctl(semaphore_2, 0, SETVAL, sem_un);
        sem_un.val = 0;
        semctl(semaphore_1, 1, SETVAL, sem_un);
        semctl(semaphore_2, 1, SETVAL, sem_un);

        char semaphore_1_string[20] = {0};
        char semaphore_2_string[20] = {0};
        sprintf(semaphore_1_string, "%d", semaphore_1);
        sprintf(semaphore_2_string, "%d", semaphore_2);

        // Shared memory:
        int shared_memory_1 = shmget(2005, sizeof(char) * 150, IPC_CREAT | 0666);
        int shared_memory_2 = shmget(2007, sizeof(char) * 150, IPC_CREAT | 0666);

        char shared_memory_1_string[20] = {0};
        char shared_memory_2_string[20] = {0};
        sprintf(shared_memory_1_string, "%d", shared_memory_1);
        sprintf(shared_memory_2_string, "%d", shared_memory_2);

        // Start processes:
        {
            // P1:
            pid_t p1_pid = start_proc("proc_p1", 2, pipe1w_string, pipe1r_string);

            // P2:
            pid_t p2_pid = start_proc("proc_p2", 2, pipe1w_string, pipe1r_string);

            // Pr:
            char p1_pid_string[10] = {0};
            sprintf(p1_pid_string, "%d", p1_pid);
            char p2_pid_string[10] = {0};
            sprintf(p2_pid_string, "%d", p2_pid);

            pid_t pr_pid = start_proc("proc_pr", 4, p1_pid_string, p2_pid_string, pipe1r_string, pipe2w_string);

            // Serv2:
            pid_t serv2_pid = start_proc("proc_serv2", 1, argv[2]);

            // Serv1:
            pid_t serv1_pid = start_proc("proc_serv1", 2, argv[1], argv[2]);

            // T:
            pid_t t_pid = start_proc("proc_t", 3, pipe2r_string, shared_memory_1_string, semaphore_1_string);

            // D:
            pid_t d_pid = start_proc("proc_d", 3, shared_memory_2_string, semaphore_2_string, argv[1]);

            // S:
            pid_t s_pid = start_proc("proc_s", 4, shared_memory_1_string, semaphore_1_string, shared_memory_2_string, semaphore_2_string);

            // Delay the termination of ourselves:
            sleep(1);

            // Cleanup:
            kill(p1_pid,    SIGUSR2);
            kill(p2_pid,    SIGUSR2);
            kill(pr_pid,    SIGUSR2);
            kill(serv2_pid, SIGUSR2);
            kill(serv1_pid, SIGUSR2);
            kill(t_pid,     SIGUSR2);
            kill(d_pid,     SIGUSR2);
            kill(s_pid,     SIGUSR2);

            // We don't know whether the already given programs have SIGUSR2 kill handling.
            // Let's make sure they all are killed:
            kill(p1_pid,    SIGKILL);
            kill(p2_pid,    SIGKILL);
            kill(pr_pid,    SIGKILL);
            kill(serv2_pid, SIGKILL);
            kill(serv1_pid, SIGKILL);
            kill(t_pid,     SIGKILL);
            kill(d_pid,     SIGKILL);
            kill(s_pid,     SIGKILL);
        }

        // Cleanup pipes:
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe1r);
        close(pipe1w);

        close(pipe2[0]);
        close(pipe2[1]);
        close(pipe2r);
        close(pipe2w);

        // Cleanup semaphores:
        semctl(semaphore_1, 0, IPC_RMID, 0);
        semctl(semaphore_2, 0, IPC_RMID, 0);

        // Cleanup shared memory:
        shmctl(shared_memory_1, IPC_RMID, NULL);
        shmctl(shared_memory_2, IPC_RMID, NULL);
    }

    return 0;
}