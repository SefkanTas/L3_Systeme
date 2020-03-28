wcp : bin/main.o bin/element.o bin/file_manager.o bin/wcplib.o
	gcc -o wcp bin/main.o bin/element.o bin/file_manager.o bin/wcplib.o -Wall

bin/main.o : src/main.c
	gcc -o bin/main.o -c src/main.c -Wall

bin/element.o : src/element.c
	gcc -o bin/element.o -c src/element.c -Wall

bin/file_manager.o : src/file_manager.c
	gcc -o bin/file_manager.o -c src/file_manager.c -Wall

bin/wcplib.o : src/wcplib.c
	gcc -o bin/wcplib.o -c src/wcplib.c
