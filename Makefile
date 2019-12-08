EXNAME = cacheSim
FLAGS = -g -Wall

all: main.o cache.o
	g++ $(FLAGS) -o $(EXNAME) main.o

main.o: main.cpp main.h
	g++ $(FLAGS) -c main.cpp

cache.o: cache.cpp cache.h
	g++ $(FLAGS) -c cache.cpp

clean:
	rm *.o *.out $(EXNAME)