CC=gcc
CFLAGS:=-Wall -Werror -pedantic
VALGRIND=valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all -s --leak-check=full
BUILD_DIR=bld
SRC_DIR=src


all: clean build_dir build

build: $(BUILD_DIR)/memory_impl $(BUILD_DIR)/pile_impl $(BUILD_DIR)/naive_impl $(BUILD_DIR)/concurrent_impl
	rm -fv ./src/naive/*.o
	rm -fv ./src/memory/*.o
	rm -fv ./src/pile/*.o
	rm -fv ./src/concurrent/*.o

build_dir:
	mkdir -p "bld"

$(BUILD_DIR)/memory_impl: $(SRC_DIR)/memory/Node_memory.o $(SRC_DIR)/memory/Game_memory.o main.c
	$(CC) $(CFLAGS) $? -o $@

$(BUILD_DIR)/pile_impl: $(SRC_DIR)/pile/Node_pile.o $(SRC_DIR)/pile/Game_pile.o main.c
	$(CC) $(CFLAGS) $? -o $@

$(BUILD_DIR)/naive_impl: $(SRC_DIR)/naive/Node_naive.o $(SRC_DIR)/naive/Game_naive.o main.c
	$(CC) $(CFLAGS) $? -o $@

$(BUILD_DIR)/concurrent_impl: $(SRC_DIR)/concurrent/Node_concurrent.o $(SRC_DIR)/concurrent/Game_concurrent.o main.c
	$(CC) $(CFLAGS) $? -o $@ -lpthread


.PHONY: clean

clean:
	rm -rf bld

