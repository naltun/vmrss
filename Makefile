BIN    := build/vmrss
CC     := $(shell which tcc || echo gcc)
CFLAGS := -Wall -Werrors
SRC    := vmrss.c

all: $(BIN)

$(BIN): $(SRC)
	mkdir --parents build/
	$(CC) $(CFLAGS) -o $(BIN) $(SRC)

clean:
	rm --recursive $(BIN)

debug:
	$(CC) $(CFLAGS) -g -o $(BIN) $(SRC)
