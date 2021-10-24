# Directories
SRC_DIR=../../src
BUILD_DIR=./object_files

INCLUDES= -I$(SRC_DIR) 

# Compiler options
MCU=atmega328
F_CPU=16000000
CC=avr-gcc
CFLAGS= -Wall -g -Os -mmcu=${MCU} -DF_CPU=${F_CPU} -I.

# Get list of source files
SRC_FILES=$(shell cd $(SRC_DIR) && find *.cpp)

# Get list of object files
SRC_OBJ=$(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))

.PHONY: all
all: dirs $(SRC_OBJ)
	@echo "Complete..."

# Create necessary directories
dirs: 
	@mkdir -p object_files
	@echo "Object file directory created..."

# Create object files from .cpp files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp	
	@echo
	@echo "--------------------Compiling $@----------------------"
	$(CC) -c $(INCLUDES) $(CFLAGS) -o $@ $<
	@echo "-----------------Finished Compiling $@----------------"
	@echo


#clean:
