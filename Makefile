CFLAGS=-Wall -ggdb3 -O3
CC=g++ $(CFLAGS)
EXEC=M3D
SRCS= $(wildcard *.cpp)
OBJS=$(SRCS:.cpp=.o)

all: M3D

M3D: $(OBJS)
	$(CC) $(OBJS) -o $@

.PHONY: clean
clean:
	rm -f *.o M3D core 
