# Compiler to use
CC = gcc

# Compiler flags (optional, can include warnings or optimization flags)
CFLAGS = -Wall -Werror

SRCS = main.c shell.c string.c path.c

# Object files
OBJS = main.o shell.o string.o path.o

# Target executable
TARGET = witsshell

# Default target (first target is the default one)
all: $(TARGET)

# Rule to create the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Rule to create object files (.o) from source files (.c)
%.o: %.c Include.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up compiled files
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets (they don't represent actual files)
.PHONY: all clean
