CC      := gcc
CFLAGS_COMMON := -Wall -Wextra -Wpedantic -I./src -I./deps
CFLAGS_DEBUG  := -O0 -g
CFLAGS_RELEASE := -O2

CFLAGS_ASAN := -fsanitize=address -g -O0 -fno-omit-frame-pointer
LDFLAGS_ASAN := -fsanitize=address

CFLAGS  := $(CFLAGS_COMMON) $(CFLAGS_DEBUG)
CFLAGS_BENCH := $(CFLAGS_COMMON) $(CFLAGS_RELEASE) -DNDEBUG
LDFLAGS := -lm -fopenmp

SRC := $(shell find src -name '*.c' ! -name 'main.c' ! -name 'benchmark.c' ! -name 'benchmark_queue.c')
APP_SRC := $(SRC) src/main.c
BENCH_SRC := $(SRC) src/benchmark.c
BENCH_QUEUE_SRC := $(SRC) src/benchmark_queue.c

HDR := $(shell find src -name '*.h') $(shell find tests -name '*.h' 2>/dev/null || true)

TARGET  := build/convol
BENCH_TARGET := build/benchmark
BENCH_QUEUE_TARGET := build/benchmark_queue

TEST_SRC := $(shell find tests -name '*.c' 2>/dev/null || true)
TEST_SUPPORT := tests/test_utils.c
TEST_MAINS := $(filter-out $(TEST_SUPPORT), $(TEST_SRC))
TEST_BINS := $(patsubst tests/%.c, build/%.tests, $(TEST_MAINS))

.PHONY: build clean fmt test bench bench-queue help asan

help:
	@echo "Available targets:"
	@echo "  build         - Build main program"
	@echo "  test          - Run tests"
	@echo "  bench         - Run single image benchmark (FILTER, IMAGE, THREADS)"
	@echo "  bench-queue   - Run pipeline benchmark (FILTER_Q, MODE_Q, IMAGE, THREADS)"
	@echo "  asan          - Build with AddressSanitizer"
	@echo "  clean         - Remove build directory"
	@echo "  fmt           - Format code"

build: $(TARGET)

$(TARGET): $(APP_SRC) $(HDR)
	@mkdir -p build
	$(CC) $(CFLAGS) $(APP_SRC) -o $@ $(LDFLAGS)

$(BENCH_TARGET): $(BENCH_SRC) $(HDR)
	@mkdir -p build
	$(CC) $(CFLAGS_BENCH) $(BENCH_SRC) -o $@ $(LDFLAGS)

$(BENCH_QUEUE_TARGET): $(BENCH_QUEUE_SRC) $(HDR)
	@mkdir -p build
	$(CC) $(CFLAGS_BENCH) $^ -o $@ $(LDFLAGS)

$(TEST_BINS): build/%.tests: tests/%.c $(TEST_SUPPORT) $(SRC) $(HDR)
	@mkdir -p build
	$(CC) $(CFLAGS) $< $(TEST_SUPPORT) $(SRC) -o $@ $(LDFLAGS) -lcmocka

bench: $(BENCH_TARGET)
	@./$(BENCH_TARGET) $(FILTER) $(IMAGE) $(THREADS)

bench-queue: $(BENCH_QUEUE_TARGET)
	@./$(BENCH_QUEUE_TARGET) $(FILTER_Q) $(MODE_Q) $(IMAGE) --threads=$(THREADS)

test: $(TEST_BINS)
	@for t in $(TEST_BINS); do \
		echo "=== $$t ==="; \
		./$$t || exit 1; \
	done

asan: clean
	@mkdir -p build
	$(CC) $(CFLAGS) $(CFLAGS_ASAN) $(APP_SRC) -o $(TARGET)_asan $(LDFLAGS) $(LDFLAGS_ASAN)
	
clean:
	rm -rf build

fmt:
	clang-format -i $(SRC) $(APP_SRC) $(HDR) $(TEST_SRC) $(BENCH_SRC) $(BENCH_QUEUE_SRC)

# Default values for benchmark
FILTER ?= motion
IMAGE ?= ./images/test_image.jpg
THREADS ?= 4

# Default values for queue benchmark
MODE_Q ?= --mode=block
FILTER_Q ?= --filter=motion
