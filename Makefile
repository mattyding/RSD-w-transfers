CC=g++

engine.o: engine.cpp engine.h
	$(CC) -std=c++11 -c engine.cpp -o engine.o
	$(CC) agent.o matching.o engine.o -o engine

all : agent.cpp agent.h matching.cpp matching.h engine.cpp engine.h
	$(CC) -std=c++11 -c matching.cpp -o matching.o
	$(CC) -std=c++11 -c agent.cpp -o agent.o
	$(CC) -std=c++11 -c engine.cpp -o engine.o
	$(CC) agent.o matching.o engine.o -o engine

clean:
	rm -f *.o
	rm -f engine