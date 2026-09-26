CFLAGS = -Wall -Wextra -Wpedantic -std=c11

TARGET = build/raycaster
AUX = src/player.c src/map.c src/ray.c
SRC = src/main.c $(AUX)
OBJ = $(SRC:src/%.c=build/%.o)

$(TARGET): $(OBJ)
	cc $(CFLAGS) $(OBJ) -o $(TARGET)

build/%.o: src/%.c
	mkdir -p build
	cc $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

test: tests/test_main.c $(AUX)
	mkdir -p build
	cc $(CFLAGS) -Isrc tests/test_main.c $(AUX) -o build/test_main
	./build/test_main

clean:
	rm -rf build

.PHONY: clean test run