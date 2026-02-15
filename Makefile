# Makefile for MDFourier for DOS
# Based on mdf-x68000 Makefile structure
# Compiler: DJGPP (GCC for DOS)

# Compiler settings
CC = gcc
CFLAGS = -Wall -O2 -std=c99 -march=i386
LDFLAGS = -lm

# Directories
BUILD_DIR = build
BIN_DIR = bin

# Target executable
TARGET = mdf
EXE = $(BIN_DIR)/$(TARGET).exe

# Source files
SOURCES = main.c mdfourier.c opl.c sbdsp.c key.c crc.c vga.c

# Object files
OBJECTS = $(BUILD_DIR)/main.o \
          $(BUILD_DIR)/mdfourier.o \
          $(BUILD_DIR)/opl.o \
          $(BUILD_DIR)/sbdsp.o \
          $(BUILD_DIR)/key.o \
          $(BUILD_DIR)/crc.o \
          $(BUILD_DIR)/vga.o

# Build rules
all: dirs $(EXE)

dirs:
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)

$(EXE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(EXE)
	@echo Build complete: $(EXE)

$(BUILD_DIR)/main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o $(BUILD_DIR)/main.o

$(BUILD_DIR)/mdfourier.o: mdfourier.c
	$(CC) $(CFLAGS) -c mdfourier.c -o $(BUILD_DIR)/mdfourier.o

$(BUILD_DIR)/opl.o: opl.c
	$(CC) $(CFLAGS) -c opl.c -o $(BUILD_DIR)/opl.o
	
$(BUILD_DIR)/sbdsp.o: sbdsp.c
	$(CC) $(CFLAGS) -c sbdsp.c -o $(BUILD_DIR)/sbdsp.o

$(BUILD_DIR)/key.o: key.c
	$(CC) $(CFLAGS) -c key.c -o $(BUILD_DIR)/key.o

$(BUILD_DIR)/crc.o: crc.c
	$(CC) $(CFLAGS) -c crc.c -o $(BUILD_DIR)/crc.o

$(BUILD_DIR)/vga.o: vga.c
	$(CC) $(CFLAGS) -c vga.c -o $(BUILD_DIR)/vga.o

clean:
	@if exist $(BUILD_DIR)\*.o del $(BUILD_DIR)\*.o
	@if exist $(EXE) del $(EXE)
	@echo Clean complete

.PHONY: all dirs clean
