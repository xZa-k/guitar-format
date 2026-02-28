CC = gcc
# LINK_FLAGS = `pkg-config --libs --cflags`
INCLUDE_FLAGS = -Iinclude -I.
PROJECT_NAME = gtf
SRC_PATH = ./src/$(PROJECT_NAME)
BUILD_PATH = ./build/$(PROJECT_NAME)
all: $(SRC_PATH).c
	$(CC) $(LINK_FLAGS) $(INCLUDE_FLAGS) -ggdb -Wall -Wextra $(SRC_PATH).c -o $(BUILD_PATH)

test: $(SRC_PATH).c
	$(CC) $(LINK_FLAGS) $(INCLUDE_FLAGS) -DTEST_MODE -ggdb -Wall -Wextra $(SRC_PATH).c -o $(BUILD_PATH)
	$(BUILD_PATH)

release: $(SRC_PATH).c
	$(CC) $(LINK_FLAGS) $(SRC_PATH).c -O2 -o $(BUILD_PATH)
	
run: release
	./build/$(PROJECT_NAME)
