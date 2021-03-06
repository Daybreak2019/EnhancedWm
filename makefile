######################################################################
#  Author: Wen Li 
#  Date: 4/5/2020
# Description: dpi module
######################################################################
export CC = gcc 
export LD = gcc

SOURCE_DIR = source
HEADER_DIR = include
OBJ_PATH = obj

export CFLAGS     = -std=c++11 -g -I$(HEADER_DIR)
export LD_CFLAGES = -lstdc++

SOURCE_FILES = $(wildcard $(SOURCE_DIR)/*.cpp)
OBJ_FILES = $(addprefix $(OBJ_PATH)/, $(addsuffix .o,$(notdir $(basename $(SOURCE_FILES)))))

TARGET = EnWm

.PHONY:all clean

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(LD) -o $@ $^ $(LD_CFLAGES)
	
$(OBJ_PATH)/%.o: $(SOURCE_DIR)/%.cpp
	@if [ ! -d $(OBJ_PATH) ];then mkdir $(OBJ_PATH); fi
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -rf $(OBJ_PATH) $(TARGET)


