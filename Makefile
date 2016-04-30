# names of the basic deriectories
SRC_DIR = src
INC_DIR = include
LIB_DIR = libs
BUILD_DIR = build

LIB_INCLUDE = -I$(LIB_DIR)/tclap-1.2.1/include
LIB_LINK = -L/usr/lib64 -Wl,-Bstatic -llua -Wl,-Bdynamic

#list all source files in SRC_DIR
SRC_FILES += $(wildcard $(SRC_DIR)/*.cpp)

# set the flags for the compilers
CXX_FLAGS := -g -O0 -Wall -pedantic -std=c++11
CC_FLAGS  := -g -O0 -Wall -pedantic

ifdef OPT
  CXX_FLAGS := -O3 -Wall -pedantic -std=c++11
  CC_FLAGS  := -O3 -Wall -pedantic
endif

CXX   := g++
CC    := gcc

#cpu dco
OBJ_FILES_T  = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES))
OBJ_FILES    = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(OBJ_FILES_T))


INC  = -I$(INC_DIR) $(LIB_INCLUDE)

FactorioViewer: $(OBJ_FILES)
	$(CXX) $(CXX_FLAGS) -o FactorioViewer $(OBJ_FILES) $(LIB_LINK)

$(BUILD_DIR)/%.o : $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CC_FLAGS) -c $< -o $@ $(INC)
	@$(CC) $(CC_FLAGS) -MM $< -MP -MT $@ -MF $(@:.o=.d) $(INC)

$(BUILD_DIR)/%.o : $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXX_FLAGS) -c $< -o $@ $(INC)
	@$(CXX) $(CXX_FLAGS) -MM $< -MP -MT $@ -MF $(@:.o=.d) $(INC)

.PHONY: clean
clean:
	rm -f FactorioViewer
	rm -r build/*

#include the dependencie files
-include $(OBJ_FILES:.o=.d)
