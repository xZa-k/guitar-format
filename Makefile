CC = gcc
LINK_FLAGS = `pkg-config --libs --cflags raylib`
INCLUDE_FLAGS = -Iinclude -I.
PROJECT_NAME = gtf
all: $(PROJECT_NAME).c
	$(CC) $(LINK_FLAGS) $(INCLUDE_FLAGS) -ggdb -Wall -Wextra -fsanitize=address $(PROJECT_NAME).c -o ./main

test: $(PROJECT_NAME).c
	$(CC) $(LINK_FLAGS) $(INCLUDE_FLAGS) -DTEST_MODE -ggdb -Wall -Wextra -fsanitize=address $(PROJECT_NAME).c -o ./main
	./main

release: $(PROJECT_NAME).c
	$(CC) $(LINK_FLAGS) -O2 -o ./main
run: release
	./main
