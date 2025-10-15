# MySPIM MIPS Processor Simulator Makefile
# CDA3103 Computer Logic and Organization Project

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = spimcore
SRCDIR = src
SOURCES = $(SRCDIR)/spimcore.c $(SRCDIR)/project.c
OBJECTS = $(SOURCES:.c=.o)

# Default target
all: $(TARGET)

# Build the simulator
$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

# Clean build artifacts
clean:
	rm -f $(TARGET) $(OBJECTS)

# Run with test file (replace test.asc with your file)
run: $(TARGET)
	./$(TARGET) test/test.asc

# Debug build with additional flags
debug: CFLAGS += -DDEBUG -O0
debug: $(TARGET)

# Release build with optimization
release: CFLAGS += -O2 -DNDEBUG
release: clean $(TARGET)

# Create test directory if it doesn't exist
setup:
	mkdir -p test
	@echo "Test directory created. Place your .asc files in the test/ directory."

# Help target
help:
	@echo "Available targets:"
	@echo "  all      - Build the simulator (default)"
	@echo "  clean    - Remove build artifacts"
	@echo "  run      - Build and run with test.asc"
	@echo "  debug    - Build with debug flags"
	@echo "  release  - Build optimized version"
	@echo "  setup    - Create test directory"
	@echo "  help     - Show this help"

.PHONY: all clean run debug release setup help
