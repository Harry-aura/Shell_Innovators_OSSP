CC = gcc
CFLAGS = -Wall -Wextra -g -O0
TARGET = history_manager
SRC = src/history_manager.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
