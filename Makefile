CFLAGS = -Wall -Wextra -Wpedantic -std=c11

TARGET = raycaster
AUX = src/player.c src/map.c src/ray.c
SRC = src/main.c $(AUX)
OBJ = $(SRC:.c=.o)

$(TARGET): $(OBJ)
	cc $(CFLAGS) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

.PHONY: clean test

test: tests/test_main.c $(AUX)
	cc $(CFLAGS) -Isrc tests/test_main.c $(AUX) -o tests/test_main
	./tests/test_main

clean:
	rm -f $(OBJ) $(TARGET) tests/test_main