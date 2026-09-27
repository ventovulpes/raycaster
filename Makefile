CFLAGS = -Wall -Wextra -Wpedantic -std=c11

TARGET = build/raycaster
TEST = tests/test_main.c src/player.c src/map.c src/ray.c

build:
	cmake -S . -B build
	cmake --build build

run: build
	./$(TARGET)

test: $(TEST)
	mkdir -p build
	cc $(CFLAGS) -Isrc $(TEST) -o build/test_main
	./build/test_main

clean:
	rm -rf build

.PHONY: build clean test run