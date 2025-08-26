# Makefile

BIN_DIR := bin
SRC_INIT := src/init
SRC_MOUNTALL := src/mountall
SRC_RECTL := src/rectl
SRC_REHALT := src/rehalt

INIT_SRCS := $(wildcard $(SRC_INIT)/*.c)
MOUNTALL_SRCS := $(wildcard $(SRC_MOUNTALL)/*.c)
RECTL_SRCS := $(wildcard $(SRC_RECTL)/*.c)
REHALT_SRCS := $(wildcard $(SRC_REHALT)/*.c)

INIT_HDRS := $(wildcard $(SRC_INIT)/include/*.h)
MOUNTALL_HDRS := $(wildcard $(SRC_MOUNTALL)/include/*.h)
RECTL_HDRS := $(wildcard $(SRC_RECTL)/include/*.h)

INIT_BIN := $(BIN_DIR)/re
MOUNTALL_BIN := $(BIN_DIR)/mountall
RECTL_BIN := $(BIN_DIR)/rectl
REHALT_BIN := $(BIN_DIR)/rehalt

all: $(BIN_DIR) $(INIT_BIN) $(MOUNTALL_BIN) $(RECTL_BIN) $(REHALT_BIN)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(INIT_BIN): $(INIT_SRCS) $(INIT_HDRS)
	gcc $(INIT_SRCS) -o $@

$(MOUNTALL_BIN): $(MOUNTALL_SRCS) $(MOUNTALL_HDRS)
	gcc $(MOUNTALL_SRCS) -o $@

$(RECTL_BIN): $(RECTL_SRCS) $(RECTL_HDRS)
	gcc $(RECTL_SRCS) -o $@

$(REHALT_BIN): $(REHALT_SRCS)
	gcc $(REHALT_SRCS) -o $@

clean:
	rm -rf $(BIN_DIR)
