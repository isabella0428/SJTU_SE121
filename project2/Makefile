CC := g++
CXXFLAGS = -std=c++17 -Wall
SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %cpp,%o,$(SRCS))
TARGET := Dijkstra
all:$(TARGET)
%.o:%.cpp
	$(CC) $(CXXFLAGS) -c -g -Ddebug $<
$(TARGET):$(OBJS)
	$(CC) $(CXXFLAGS) -o $@ $^
clean:
	rm -rf $(TARGET) *.o