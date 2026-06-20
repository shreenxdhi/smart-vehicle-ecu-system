CC     = gcc
CFLAGS = -std=c99 -Wall -Wextra -g -Isrc

TARGET = vehicle_ecu
SRCS   = src/master_node.c \
         src/ecu_airbag.c \
         src/ecu_obstacle.c \
         src/ecu_fuel.c \
         src/ecu_antitheft.c \
         src/protocol_can.c \
         src/protocol_uart.c \
         src/protocol_i2c.c \
         src/dtc_logger.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET) dtc_log.txt

run: all
	./$(TARGET) 90 10 8 1

test: all
	python3 tests/test_vehicle_system.py
	python3 tests/test_fault_injection.py
	python3 tests/test_stress.py
	python3 tests/test_starvation.py

.PHONY: all clean run test
