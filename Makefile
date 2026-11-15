CXXFLAGS ?= -O2 -std=c++20 -Wall -Wextra -Wpedantic -Iinclude
BUILD    := build
MPIRUN   ?= mpirun
TEST_NP  ?= 1 3 4

# Default to the MPI wrapper. `make CXX=...` still overrides it; a plain `?=`
# would not, because make already defines CXX as the system c++.
ifeq ($(origin CXX),default)
CXX := mpicxx
endif

# Shared code that every program links against.
LIB_SRCS := src/layout.cpp src/data.cpp
LIB_OBJS := $(LIB_SRCS:src/%.cpp=$(BUILD)/%.o)

.PHONY: all clean test
all: $(BUILD)/hello $(BUILD)/test_layout

# -MMD -MP writes .d files so editing a header rebuilds whatever includes it.
$(BUILD)/%.o: src/%.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(BUILD)/hello: src/hello.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -MMD -MP $< -o $@

$(BUILD)/test_layout: tests/test_layout.cpp $(LIB_OBJS) | $(BUILD)
	$(CXX) $(CXXFLAGS) -MMD -MP $^ -o $@

$(BUILD):
	mkdir -p $@

# Run each test at several rank counts; stop at the first failure.
test: $(BUILD)/test_layout
	@for np in $(TEST_NP); do $(MPIRUN) -np $$np $(BUILD)/test_layout || exit 1; done

clean:
	rm -rf $(BUILD)

-include $(wildcard $(BUILD)/*.d)
