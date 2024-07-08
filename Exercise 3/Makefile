# Compiler
CC = g++ -std=c++11

# Compiler flags
CFLAGS = -Wall -Wextra -g -pthread

# Source files
SRCS = $(wildcard *.cpp)

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable
TARGET = ex3.out

# Default target
all: $(TARGET)

# Compile source files into object files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files into the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)

