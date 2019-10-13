all: shell

shell: simple_shell.cpp
	g++ -o simple_shell simple_shell.cpp

clean:
	rm simple_shell
