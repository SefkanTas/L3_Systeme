pwc : bin/main.o bin/element.o bin/file_manager.o bin/pwclib.o
	gcc -o pwc bin/main.o bin/element.o bin/file_manager.o bin/pwclib.o -Wall

bin/main.o : main.c
	gcc -o bin/main.o -c main.c -Wall

bin/element.o : src/element.c
	gcc -o bin/element.o -c src/element.c -Wall

bin/file_manager.o : src/file_manager.c
	gcc -o bin/file_manager.o -c src/file_manager.c -Wall

bin/pwclib.o : src/pwclib.c
	gcc -o bin/pwclib.o -c src/pwclib.c
