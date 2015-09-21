
CC=clang++ -DDEBUG=1

all: shell

shell: main.o program.o alias.o
	$(CC) main.o program.o alias.o -o shell

main.o: main.cpp
	$(CC) -c main.cpp -std=c++11

program.o: program.cpp
	$(CC) -c program.cpp

alias.o: alias.cpp
	$(CC) -c alias.cpp -std=c++11

clean:
	$(RM) *o *~ shell
