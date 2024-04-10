BIN    := build/vmrss
CC     := $(shell which tcc || echo gcc)
CFLAGS := -Wall -Werror
SRC    := vmrss.c

all: $(BIN)

$(BIN): $(SRC)
	mkdir --parents build/
	$(CC) $(CFLAGS) -o $(BIN) $(SRC)

clean:
	rm --recursive $(BIN)

debug:
	gcc $(CFLAGS) -g -o $(BIN) $(SRC)
