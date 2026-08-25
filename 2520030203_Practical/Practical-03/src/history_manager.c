#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#define INITIAL_CAPACITY 4
#define PROMPT "ForgeOS-History$ "

// Linked list node for dynamic command history tracking
typedef struct HistoryNode {
    char *command;
    struct HistoryNode *prev;
    struct HistoryNode *next;
} HistoryNode;

typedef struct {
    HistoryNode *head;
    HistoryNode *tail;
    int size;
} HistoryList;

void init_history(HistoryList *list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

void add_history(HistoryList *list, const char *cmd) {
    if (cmd == NULL || strlen(cmd) == 0) return;

    HistoryNode *node = (HistoryNode *)malloc(sizeof(HistoryNode));
    node->command = strdup(cmd);
    node->next = NULL;
    node->prev = list->tail;

    if (list->tail != NULL) {
        list->tail->next = node;
    } else {
        list->head = node;
    }
    list->tail = node;
    list->size++;
}

void free_history(HistoryList *list) {
    HistoryNode *curr = list->head;
    while (curr != NULL) {
        HistoryNode *temp = curr;
        curr = curr->next;
        free(temp->command);
        free(temp);
    }
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

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

// Dynamically allocated input line buffer with automatic resizing
char *read_dynamic_line(HistoryList *history) {
    struct termios orig_termios;
    enable_raw_mode(&orig_termios);

    size_t capacity = INITIAL_CAPACITY;
    size_t length = 0;
    char *buffer = (char *)malloc(capacity * sizeof(char));
    buffer[0] = '\0';

    HistoryNode *current_history_node = NULL;
    char ch;

    while (read(STDIN_FILENO, &ch, 1) == 1) {
        if (ch == '\n' || ch == '\r') {
            printf("\n");
            buffer[length] = '\0';
            break;
        } else if (ch == 27) { // ANSI Escape Sequence
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) == 1 && read(STDIN_FILENO, &seq[1], 1) == 1) {
                if (seq[0] == '[') {
                    if (seq[1] == 'A') { // Up Arrow (Previous command)
                        if (current_history_node == NULL) {
                            current_history_node = history->tail;
                        } else if (current_history_node->prev != NULL) {
                            current_history_node = current_history_node->prev;
                        }

                        if (current_history_node != NULL) {
                            // Clear current visual line
                            while (length > 0) {
                                printf("\b \b");
                                length--;
                            }
                            length = strlen(current_history_node->command);
                            if (length >= capacity) {
                                capacity = length + 16;
                                buffer = (char *)realloc(buffer, capacity);
                            }
                            strcpy(buffer, current_history_node->command);
                            printf("%s", buffer);
                            fflush(stdout);
                        }
                    } else if (seq[1] == 'B') { // Down Arrow (Next command)
                        if (current_history_node != NULL) {
                            current_history_node = current_history_node->next;
                            while (length > 0) {
                                printf("\b \b");
                                length--;
                            }
                            if (current_history_node != NULL) {
                                length = strlen(current_history_node->command);
                                if (length >= capacity) {
                                    capacity = length + 16;
                                    buffer = (char *)realloc(buffer, capacity);
                                }
                                strcpy(buffer, current_history_node->command);
                                printf("%s", buffer);
                            } else {
                                buffer[0] = '\0';
                                length = 0;
                            }
                            fflush(stdout);
                        }
                    }
                }
            }
        } else if (ch == 127 || ch == '\b') { // Backspace
            if (length > 0) {
                length--;
                buffer[length] = '\0';
                printf("\b \b");
                fflush(stdout);
            }
        } else if (ch == 4) { // Ctrl+D
            disable_raw_mode(&orig_termios);
            free(buffer);
            return NULL;
        } else {
            // Dynamically double capacity if buffer limit is reached
            if (length + 2 >= capacity) {
                capacity *= 2;
                buffer = (char *)realloc(buffer, capacity);
            }
            buffer[length++] = ch;
            buffer[length] = '\0';
            putchar(ch);
            fflush(stdout);
        }
    }

    disable_raw_mode(&orig_termios);
    return buffer;
}

int main() {
    HistoryList history;
    init_history(&history);

    printf("=== ForgeOS Dynamic Memory & Command History System ===\n");
    printf("Supports: Dynamic Buffer Resizing, Up/Down Arrow Navigation, 'history', 'exit'\n\n");

    while (1) {
        printf("%s", PROMPT);
        fflush(stdout);

        char *line = read_dynamic_line(&history);
        if (line == NULL) {
            break;
        }

        // Trim leading spaces
        char *cmd = line;
        while (*cmd == ' ') cmd++;

        if (strlen(cmd) > 0) {
            add_history(&history, cmd);

            if (strcmp(cmd, "exit") == 0) {
                printf("[ForgeOS] Exiting session and freeing allocated memory.\n");
                free(line);
                break;
            } else if (strcmp(cmd, "history") == 0) {
                printf("--- Stored Command History ---\n");
                int idx = 1;
                HistoryNode *curr = history.head;
                while (curr != NULL) {
                    printf(" %d: %s\n", idx++, curr->command);
                    curr = curr->next;
                }
            } else {
                printf("[Executed]: %s\n", cmd);
            }
        }
        free(line); // Deallocate dynamically read line
    }

    free_history(&history); // Deallocate all linked list nodes
    return 0;
}
