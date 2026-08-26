CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = file_copy
SRC = src/file_copy.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

