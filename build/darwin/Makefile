# Directories
LIB_NAME=my_avr

SRC_DIR=../../src
BUILD_DIR=./object_files
LIB_DIR=../../lib
LIBINC_DIR=../../include

INCLUDES= -I$(SRC_DIR) 

# Compiler/MCU options
MCU=atmega328
F_CPU=16000000
CC=avr-gcc
CFLAGS= -Wall -g -Os -mmcu=${MCU} -DF_CPU=${F_CPU} -I.

# Get list of source files
SRC_FILES=$(shell cd $(SRC_DIR) && find *.cpp)

# Get list of object files
SRC_OBJ=$(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))

.PHONY: all
all: dirs $(SRC_OBJ) lib
	@cp $(SRC_DIR)/*.h $(LIBINC_DIR)
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

# Compile object files
compile: dirs $(SRC_OBJ)
	@cp $(SRC_DIR)/*.h $(LIBINC_DIR)

# Create shared library
lib:
	@echo
	@echo "--------------------Assembling lib$(LIB_NAME).a----------------------"
	avr-ar rcs $(LIB_DIR)/lib$(LIB_NAME).a $(SRC_OBJ)
	@echo "--------------------Finished Assembling -----------------------------"
	@echo

# Remove makefile produced files
clean:
	rm -rf $(BUILD_DIR)
	rm $(LIB_DIR)/lib$(LIB_NAME).a

