CC = g++
CFLAGS = -std=c++14
SRCS = aho_corasick.hpp main.cpp

compiled.o: $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o compiled.o

clean:
	Del "compiled.o"