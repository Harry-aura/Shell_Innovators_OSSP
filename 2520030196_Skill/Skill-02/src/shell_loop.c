#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#define BUFFER_SIZE 1024
#define PROMPT "ForgeOS$ "

// Function to configure terminal raw input for character-level control
void enable_raw_mode(struct termios *orig_termios) {
    struct termios raw;
    tcgetattr(STDIN_FILENO, orig_termios);
    raw = *orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disable_raw_mode(struct termios *orig_termios) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, orig_termios);
}

int read_command_line(char *buffer, size_t max_len) {
    struct termios orig_termios;
    enable_raw_mode(&orig_termios);

    size_t index = 0;
    char ch;
    memset(buffer, 0, max_len);

    while (read(STDIN_FILENO, &ch, 1) == 1) {
        if (ch == '\n' || ch == '\r') {
            printf("\n");
            buffer[index] = '\0';
            break;
        } else if (ch == 127 || ch == '\b') { // Handle Backspace
            if (index > 0) {
                index--;
                buffer[index] = '\0';
                printf("\b \b");
                fflush(stdout);
            }
        } else if (ch == 4) { // Handle EOF (Ctrl+D)
            disable_raw_mode(&orig_termios);
            return 0;
        } else {
            if (index < max_len - 1) {
                buffer[index++] = ch;
                putchar(ch);
                fflush(stdout);
            }
        }
    }

    disable_raw_mode(&orig_termios);
    return 1;
}

int main() {
    char input_buffer[BUFFER_SIZE];

    printf("=== ForgeOS Interactive Shell Interface ===\n");
    printf("Type commands, test backspace editing, or type 'exit' to terminate.\n\n");

    while (1) {
        printf("%s", PROMPT);
        fflush(stdout);

        if (!read_command_line(input_buffer, BUFFER_SIZE)) {
            printf("\n[ForgeOS] Exiting session.\n");
            break;
        }

        // Trim leading and trailing whitespaces
        char *cmd = input_buffer;
        while (*cmd == ' ') cmd++;

        if (strlen(cmd) == 0) {
            continue;
        }

        // Handle exit condition
        if (strcmp(cmd, "exit") == 0) {
            printf("[ForgeOS] Terminating interactive main loop. Goodbye!\n");
            break;
        }

        // Simulate internal multi-character command processing
        if (strcmp(cmd, "ls") == 0) {
            printf("[ForgeOS Command Output]: src  outputs  screenshots  Makefile  Documentation.txt\n");
        } else if (strcmp(cmd, "pwd") == 0) {
            char cwd[512];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                printf("[ForgeOS Command Output]: %s\n", cwd);
            }
        } else if (strcmp(cmd, "help") == 0) {
            printf("[ForgeOS Help]: Supported test commands -> ls, pwd, help, exit\n");
        } else {
            printf("[ForgeOS Execution]: Command '%s' captured and processed successfully.\n", cmd);
        }
    }

    return 0;
}

