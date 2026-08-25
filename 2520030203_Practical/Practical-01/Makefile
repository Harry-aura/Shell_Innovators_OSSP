CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = command_runner
SRC = src/command_runner.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
