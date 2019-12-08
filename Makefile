EXNAME = cacheSim
CC = g++
FLAGS = -g -Wall
OBJS = main.o cache.o

$(EXNAME): $(OBJS)
	$(CC) $(FLAGS) -o $(EXNAME) $(OBJS)

main.o: main.cpp main.h
	$(CC) $(FLAGS) -c main.cpp

cache.o: cache.cpp cache.h
	$(CC) $(FLAGS) -c cache.cpp

clean:
	rm *.o *.out $(EXNAME)