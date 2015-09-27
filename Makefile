
CC=clang++ -DDEBUG=1
RM=rm -f

all: shell

shell: main.o program.o alias.o command.o
	$(CC) main.o program.o alias.o command.o -o shell

main.o: main.cpp
	$(CC) -c main.cpp -std=c++11

program.o: program.cpp
	$(CC) -c program.cpp

alias.o: alias.cpp
	$(CC) -c alias.cpp -std=c++11

command.o: command.cpp
	$(CC) -c command.cpp

clean:
	$(RM) *.o *~ shell

