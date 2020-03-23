oui : test.o hello.o hello_world.o
	gcc -o oui bin/test.o bin/hello.o bin/hello_world.o

hello_world.o : src/hello_world.c
	gcc -o bin/hello_world.o -c src/hello_world.c

hello.o : src/hello.c
	gcc -o bin/hello.o -c src/hello.c

test.o : src/test.c
	gcc -o bin/test.o -c src/test.c
