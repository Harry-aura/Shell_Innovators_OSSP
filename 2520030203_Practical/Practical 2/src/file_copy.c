#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source_file> <destination_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // open() system call transitions to kernel space to retrieve file descriptor
    int src_fd = open(argv[1], O_RDONLY);
    if (src_fd < 0) {
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }

    // open() with creation, truncation, and read/write user permissions (0644)
    int dest_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd < 0) {
        perror("Error opening/creating destination file");
        close(src_fd);
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;
    size_t total_bytes = 0;

    // Control transitions repeatedly via read() and write() system calls
    while ((bytes_read = read(src_fd, buffer, BUFFER_SIZE)) > 0) {
        bytes_written = write(dest_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("Error writing to destination file");
            close(src_fd);
            close(dest_fd);
            exit(EXIT_FAILURE);
        }
        total_bytes += bytes_written;
    }

    if (bytes_read < 0) {
        perror("Error reading source file");
    }

    // close() system call releases file descriptors from kernel file table
    close(src_fd);
    close(dest_fd);

    printf("File copy successful. Transferred %zu bytes from '%s' to '%s'.\n",
           total_bytes, argv[1], argv[2]);

    return 0;
}
