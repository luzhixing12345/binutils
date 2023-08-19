
CC = gcc
CFLAGS = -Wall -Wunused -Werror -Wformat-security
MAKEFLAGS += --no-print-directory

SRC_PATH = src
EXAMPLE_PATH = examples

SRC = $(wildcard $(SRC_PATH)/*.c)
OBJ = $(SRC:.c=.o)
EXE = $(SRC:.c=)

ifeq ($(MAKECMDGOALS),debug)
CFLAGS+=-g
endif

all: $(EXE) $(OBJ)
	@$(MAKE) -C $(EXAMPLE_PATH)

debug: all

%: %.o
	$(CC) $(CFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(OBJ) $(EXE)
	@$(MAKE) -C $(EXAMPLE_PATH) clean