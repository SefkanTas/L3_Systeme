pwc : bin/main.o bin/element.o bin/file_manager.o bin/pwclib.o
	gcc -o pwc bin/main.o bin/element.o bin/file_manager.o bin/pwclib.o

bin/main.o : main.c
	gcc -o bin/main.o -c main.c

bin/element.o : src/element.c
	gcc -o bin/element.o -c src/element.c

bin/file_manager.o : src/file_manager.c
	gcc -o bin/file_manager.o -c src/file_manager.c

bin/pwclib.o : src/pwclib.c
	gcc -o bin/pwclib.o -c src/pwclib.c
