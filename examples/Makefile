
CC = gcc

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
EXE = $(SRC:.c=)

ifeq ($(MAKECMDGOALS),debug)
CFLAGS+=-g
endif

all: $(EXE) $(OBJ)

debug: all

%: %.o
	$(CC) $(CFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(OBJ) $(EXE)