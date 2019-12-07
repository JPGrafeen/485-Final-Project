EXNAME = cacheSim

all: main.o
	g++ -o $(EXNAME) main.o

clean:
	rm *.o *.out $(EXNAME)