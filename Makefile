SRC := yote.cpp
BIN := yote

WARN := -Wall -Wextra -Wpedantic
BASE := -std=c++20 $(WARN)

RELEASE_FLAGS := -O3 -march=native -mtune=native -flto -fomit-frame-pointer -DNDEBUG
DEBUG_FLAGS := -O0 -g3

.PHONY: all release debug clean

all: release

release:
	g++ $(BASE) $(RELEASE_FLAGS) $(SRC) -o $(BIN)

debug:
	g++ $(BASE) $(DEBUG_FLAGS) $(SRC) -o $(BIN)

clean:
	rm -f $(BIN)