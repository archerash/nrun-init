CC = gcc
CFLAGS = -Wall -Wextra -I.

SRC_INIT = $(wildcard src/init/*.c)
SRC_MOUNT = $(wildcard src/init/mount/*.c)
SRC_RECTL = $(wildcard src/rectl/*.c)

HDR_INIT = $(wildcard src/init/include/*.h)
HDR_MOUNT = $(wildcard src/init/mount/include/*.h)
HDR_RECTL = $(wildcard src/rectl/include/*.h)

BIN = bin
RE = $(BIN)/re
MOUNTALL = $(BIN)/mountall
RECTL = $(BIN)/rectl

all: $(RE) $(MOUNTALL) $(RECTL)

$(RE): $(SRC_INIT) $(HDR_INIT)
	$(CC) $(SRC_INIT) -o $@

$(MOUNTALL): $(SRC_MOUNT) $(HDR_MOUNT)
	$(CC) $(SRC_MOUNT) -o $@

$(RECTL): $(SRC_RECTL) $(HDR_RECTL)
	$(CC) $(SRC_RECTL) -o $@

clean:
	rm -f $(RE) $(MOUNTALL) $(RECTL)
