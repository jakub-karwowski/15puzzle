CXX=g++
CXXFLAGS=-Wall -Wextra -std=c++20

BUILD_DIR=./build
SRC_DIR = ./src
INC_DIR = ./include

TARGET=solve.out
LIBS=

SRCS := $(shell find $(SRC_DIR) -name *.cpp)
OBJS := $(addprefix $(BUILD_DIR)/,$(addsuffix .o, $(basename $(notdir $(SRCS)))))
DEPS := $(OBJS:.o=.d)

$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) -o $@ $^ $(CXXFLAGS) 
#$^ OBJS
#$@ TARGET $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@ 
#$@ ./BUILD_DIR/%.o
#$< %.cpp

.PHONY: clean

clean:
	rm $(OBJS) $(DEPS) $(TARGET)

-include $(DEPS)
