CC = gcc
LINK_FLAGS = `pkg-config --libs --cflags raylib`
INCLUDE_FLAGS = -Iinclude -I.
PROJECT_NAME = gtf
all: $(PROJECT_NAME).c
	$(CC) $(LINK_FLAGS) $(INCLUDE_FLAGS) -ggdb -Wall -Wextra -fsanitize=address $(PROJECT_NAME).c -o ./main

release: $(PROJECT_NAME).c
	$(CC) $(LINK_FLAGS) -O2 -o ./main
run: all 
	./main
