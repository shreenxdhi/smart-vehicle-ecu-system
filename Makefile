CC     = gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror -Wpedantic -g -Isrc

TARGET = vehicle_ecu
SRCS   = src/master_node.c \
         src/ecu_airbag.c \
         src/ecu_obstacle.c \
         src/ecu_fuel.c \
         src/ecu_antitheft.c \
         src/protocol_can.c \
         src/protocol_uart.c \
         src/protocol_i2c.c \
         src/dtc_logger.c src/vehicle_logic.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET) dtc_log.txt
	rm -rf build

run: all
	./$(TARGET) 90 10 8 1

test: all test-unit test-sanitized
	python3 tests/test_vehicle_system.py
	python3 tests/test_fault_injection.py
	python3 tests/test_stress.py
	python3 tests/test_starvation.py
	python3 tests/test_cli.py

.PHONY: all clean run test

build:
	mkdir -p build
build/test-core: tests/test_core.c src/protocol_can.c src/vehicle_logic.c src/common.h src/protocol_can.h src/vehicle_logic.h | build
	$(CC) $(CFLAGS) -fsanitize=address,undefined tests/test_core.c src/protocol_can.c src/vehicle_logic.c -o $@
test-unit: build/test-core
	./build/test-core
.PHONY: test-unit

build/vehicle-sanitized: $(SRCS) $(wildcard src/*.h) | build
	$(CC) $(CFLAGS) -fsanitize=address,undefined $(SRCS) -o $@
test-sanitized: build/vehicle-sanitized
	python3 tests/test_sanitized.py
.PHONY: test-sanitized
