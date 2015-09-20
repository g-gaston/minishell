all: shell

shell: main.o program.o
	clang++ main.o program.o -o shell

main.o: main.cpp
	clang++ -c main.cpp

program.o: program.cpp
	clang++ -c program.cpp

clean:
	rm *o shell