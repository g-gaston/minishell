all: shell

shell: main.o
	clang++ main.o -o shell

main.o: main.cpp
	clang++ -c main.cpp

clean:
	rm *o shell