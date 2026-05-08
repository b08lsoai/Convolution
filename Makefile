CC      := gcc
CFLAGS_COMMON := -Wall -Wextra -Wpedantic -I./src -I./deps
CFLAGS_DEBUG  := -O0 -g
CFLAGS_RELEASE := -O2

CFLAGS  := $(CFLAGS_COMMON) $(CFLAGS_DEBUG)
CFLAGS_BENCH := $(CFLAGS_COMMON) $(CFLAGS_RELEASE) -DNDEBUG
LDFLAGS := -lm -fopenmp

SRC := $(shell find src -name '*.c' ! -name 'main.c' ! -name 'benchmark.c')
APP_SRC := $(SRC) src/main.c
BENCH_SRC := $(SRC) src/benchmark.c
HDR := $(shell find src -name '*.h') $(shell find tests -name '*.h' 2>/dev/null || true)

TARGET  := build/convol
BENCH_TARGET := build/benchmark
TEST_SRC := $(shell find tests -name '*.c' 2>/dev/null || true)
TEST_BIN := build/tests

.PHONY: build clean fmt test bench help

help:
	@echo "Available targets:"
	@echo "  build    - Build the main program"
	@echo "  test     - Build and run tests"
	@echo "  bench    - Build and run benchmark"
	@echo "  clean    - Remove build directory"
	@echo "  fmt      - Format code with clang-format"

build: $(TARGET)

$(TARGET): $(APP_SRC)
	@mkdir -p build
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BENCH_TARGET): $(BENCH_SRC)
	@mkdir -p build
	$(CC) $(CFLAGS_BENCH) $^ -o $@ $(LDFLAGS)

$(TEST_BIN): $(SRC) $(TEST_SRC)
	@mkdir -p build
	$(CC) $(CFLAGS) $(SRC) $(TEST_SRC) -o $@ $(LDFLAGS) -lcmocka

bench: $(BENCH_TARGET)
	@./$(BENCH_TARGET) $(FILTER) $(IMAGE)

test: $(TEST_BIN)
	@./$(TEST_BIN)

clean:
	rm -rf build

fmt:
	clang-format -i $(SRC) $(APP_SRC) $(HDR) $(TEST_SRC) $(BENCH_SRC)

# Default values for benchmark
FILTER ?= motion
IMAGE ?= ./images/test_image.jpg
