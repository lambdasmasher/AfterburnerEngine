SRCS := $(wildcard src/*.cpp)
OBJS := $(patsubst src/%.cpp,build/%.o,$(SRCS))

LDFLAGS := -lGL -lGLEW -lglfw
CXXFLAGS := -std=c++17 -O2 -Isrc -I/usr/include

all: build/engine
build/engine: $(OBJS)
	g++ $(LDFLAGS) -o $@ $^
build/%.o: src/%.cpp
	g++ $(CXXFLAGS) -c -MMD -o $@ $<
-include $(OBJS:.o=.d)