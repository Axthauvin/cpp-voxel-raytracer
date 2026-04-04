CXX := g++

SRC_DIRS := src lib
BUILD_DIR := build

SRC_CPP_FILES := $(shell find $(SRC_DIRS) \( -name '*.cpp' -o -name '*.cc' \) | sort)
OBJ_FILES := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(patsubst %.cc,$(BUILD_DIR)/%.o,$(SRC_CPP_FILES)))
TEST_OBJ := $(BUILD_DIR)/test.o
DEP_FILES := $(OBJ_FILES:.o=.d) $(TEST_OBJ:.o=.d)

CPPFLAGS += -Isrc -Ilib -Ilib/utils
CXXFLAGS += -Wall -Wextra -O3 -g -std=c++20 -march=native
LDFLAGS += -fopenmp
CXXFLAGS += -fopenmp
LDLIBS += -lm

all: test

test: $(OBJ_FILES) $(TEST_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $(TEST_OBJ) $(OBJ_FILES) $(LDLIBS)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c -o $@ $<

$(BUILD_DIR)/%.o: %.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c -o $@ $<

clean:
	rm -rf $(BUILD_DIR) test

-include $(DEP_FILES)

.PHONY: all clean test
