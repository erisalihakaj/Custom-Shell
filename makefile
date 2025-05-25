# Makefile
# Author: Eris Alihakaj
# Student_ID: 23305593
# I Acknowledge DCU's Academic Integrity Policy 

# Configuration
COMPILER = gcc
COMPILER_FLAGS = -Wall -Wextra -g

# Directory paths
SOURCE_DIR = ./src
OUTPUT_DIR = ./bin

# Files to compile
SOURCE_FILES = $(SOURCE_DIR)/customshell.c \
               $(SOURCE_DIR)/environment.c \
               $(SOURCE_DIR)/execute_commands.c \
               $(SOURCE_DIR)/io_redirection.c \
               $(SOURCE_DIR)/utility.c

INCLUDE_FILES = $(SOURCE_DIR)/customshell.h

# Target executable
EXECUTABLE = $(OUTPUT_DIR)/customshell

# Default target: build the executable
build: $(EXECUTABLE)

# Ensure the output directory exists
$(OUTPUT_DIR):
	@mkdir -p $(OUTPUT_DIR)

# Compile the source files into the executable
$(EXECUTABLE): $(SOURCE_FILES) $(INCLUDE_FILES) | $(OUTPUT_DIR)
	$(COMPILER) $(COMPILER_FLAGS) $(SOURCE_FILES) -o $(EXECUTABLE)

# Clean up compiled files
remove:
	rm -f $(EXECUTABLE)