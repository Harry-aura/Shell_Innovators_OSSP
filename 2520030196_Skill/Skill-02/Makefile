CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = shell_loop
SRC = src/shell_loop.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

