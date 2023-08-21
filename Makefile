
CC = gcc
CFLAGS = -Wall -Wunused -Werror -Wformat-security
MAKEFLAGS += --no-print-directory

SRC_PATH = src
EXAMPLE_PATH = examples
INSTALL_PATH = /usr/local/sbin

SRC = $(wildcard $(SRC_PATH)/*.c)
OBJ = $(SRC:.c=.o)
EXE = $(SRC:.c=)

CP_FORMAT = "[cp]\t%-20s -> %s\n"
MV_FORMAT = "[mv]\t%-20s -> %s\n"

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

install:
	@cp -v -i $(EXE) $(INSTALL_PATH) | awk -v format=$(CP_FORMAT) '{printf format, $$1, $$3}'

uninstall:
	rm $(INSTALL_PATH)/$(subst $(SRC_PATH)/,,$(EXE))

test:
	@$(MAKE) clean && $(MAKE)
	@python test.py