# Makefile for Paint for DOS
# Supports DJGPP compiler

# Compiler settings
CC = gcc
CFLAGS = -Wall -O2 -std=c99 -I./include
LDFLAGS = -lm

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
BIN_DIR = bin

# Target executable
TARGET = $(BIN_DIR)/paint.exe

# Source files
SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/vga.c \
          $(SRC_DIR)/input.c \
          $(SRC_DIR)/adlib.c

# Object files
OBJECTS = $(BUILD_DIR)/main.o \
          $(BUILD_DIR)/vga.o \
          $(BUILD_DIR)/input.o \
          $(BUILD_DIR)/adlib.o

# Build rules
all: dirs $(TARGET)

dirs:
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(TARGET)
	@echo Build complete: $(TARGET)

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main.c -o $(BUILD_DIR)/main.o

$(BUILD_DIR)/vga.o: $(SRC_DIR)/vga.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/vga.c -o $(BUILD_DIR)/vga.o

$(BUILD_DIR)/input.o: $(SRC_DIR)/input.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/input.c -o $(BUILD_DIR)/input.o

$(BUILD_DIR)/adlib.o: $(SRC_DIR)/adlib.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/adlib.c -o $(BUILD_DIR)/adlib.o

clean:
	@if exist $(BUILD_DIR)\*.o del $(BUILD_DIR)\*.o
	@if exist $(TARGET) del $(TARGET)
	@echo Clean complete

.PHONY: all dirs clean
