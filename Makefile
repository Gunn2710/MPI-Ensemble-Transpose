# Placeholder build file; targets get filled in during implementation.
CC      ?= mpicc
CFLAGS  ?= -O2 -std=c11 -Wall -Wextra -Iinclude
BUILD   := build

.PHONY: all clean test
all:
	@echo "Nothing to build yet. Add sources under src/ and targets here."

test:
	@echo "No tests yet. Add correctness tests under tests/."

clean:
	rm -rf $(BUILD)
