#
# atmega328 build lib
#
MCU=atmega328
LIB_NAME=my_$(MCU)


SRC_DIR=../../src/$(MCU)
BUILD_DIR=./object_files
LIB_DIR=../../lib
LIBINC_DIR=../../include/$(MCU)

INCLUDES= -I$(SRC_DIR) 

# Sources to place in include file.
# This should be files that need to be compiled with the main.cpp project
INCLUDE_SRCS= $(SRC_DIR)/myInterrupt.cpp

# Compiler/MCU options
F_CPU=16000000
CC=avr-gcc
CFLAGS= -Wall -g -Os -mmcu=${MCU} -DF_CPU=${F_CPU} 

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
	echo $(INCLUDE_SRCS)
	@cp $(INCLUDE_SRCS) $(LIBINC_DIR)

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
	rm -rf $(LIBINC_DIR)/*.h
	rm $(LIB_DIR)/lib$(LIB_NAME).a

