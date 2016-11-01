# Made by ValeriyKr #
CC = clang
CFLAGS = -c -std=c99 -g -Wall -Wextra -pedantic 
SOURCES = main.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = tree

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f ./*.o
