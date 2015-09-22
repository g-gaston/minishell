all: shell

shell: main.o program.o command.o
	clang++ main.o program.o command.o -o shell

main.o: main.cpp
	clang++ -c main.cpp

program.o: program.cpp
	clang++ -c program.cpp

command.o: command.cpp
	clang++ -c command.cpp

clean:
	rm *o shell
