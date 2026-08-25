#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_CMD_LEN 256
#define MAX_ARGS 32

int main() {
    char input[MAX_CMD_LEN];
    char *args[MAX_ARGS];

    printf("=== Linux OS Command Execution System ===\n");
    printf("Enter a Linux command to execute (e.g., 'ls -l', 'uname -a'): ");
    fflush(stdout);

    if (fgets(input, sizeof(input), stdin) == NULL) {
        perror("Error reading input");
        exit(EXIT_FAILURE);
    }

    input[strcspn(input, "\n")] = '\0';

    if (strlen(input) == 0) {
        printf("No command entered. Exiting.\n");
        return 0;
    }

    int i = 0;
    char *token = strtok(input, " ");
    while (token != NULL && i < MAX_ARGS - 1) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;

    printf("\n[Parent Process] PID: %d is preparing to fork child...\n", getpid());

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        printf("[Child Process] PID: %d | Parent PPID: %d\n", getpid(), getppid());
        printf("[Child Process] Executing command '%s' via execvp()...\n\n", args[0]);

        if (execvp(args[0], args) == -1) {
            perror("Execution failed (execvp error)");
            exit(EXIT_FAILURE);
        }
    } else {
        printf("[Parent Process] Waiting for Child PID: %d to finish...\n", pid);
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            printf("\n[Parent Process] Child process terminated with status code: %d\n", WEXITSTATUS(status));
        }
    }

    return 0;
}
