all: 1730sh.o 1730sh

1730sh: 1730sh.o
	g++ -Wall -std=c++14 -g -O0 -pedantic-errors -o 1730sh 1730sh.o

1730sh.o: 1730sh.cpp
	g++ -Wall -std=c++14 -g -O0 -pedantic-errors -c 1730sh.cpp
clean:
	rm 1730sh.o
	rm 1730sh