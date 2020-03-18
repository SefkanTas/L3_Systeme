#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int testPipe(int argc, char const *argv[]) {
    printf("salut\n");

    //0 -> lecture
    //1 -> ecriture
    int p[2];
    int res = pipe(p);

    int state;

    char msg[50];

    if(res == -1){
        printf("Erreur avec le pipe, exit\n");
        exit(-1);
    }

    int f = fork();
    int f2;
    if(f > 0){
        f2 = fork();
    }


    if(f == 0){
        close(p[0]);

        printf("Le fils 1 ecrit :\n");
        sprintf(msg, "Le message 1");
        write(p[1], msg, sizeof(msg));

        //close(p[1]);
    }
    else if(f2 == 0){
        close(p[0]);

        printf("Le fils 2 ecrit :\n");
        sprintf(msg, "Le message 2");
        write(p[1], msg, sizeof(msg));
    }
    else if(f > 0){
        close(p[1]);

        printf("Le pere ecoute\n");
        read(p[0], msg, sizeof(msg));
        printf("Le fils dit : %s\n", msg);
        read(p[0], msg, sizeof(msg));
        printf("Le fils dit : %s\n", msg);

        close(p[0]);
        wait(&state);
    }

    printf("%d\n", res);

    return 0;
}

int readFileTest(int argc, char const *argv[]) {

    return 0;

    const int LINE_SIZE = 1024;

    FILE *file = fopen("extra_mini_lorem.txt", "r");
    char fileData[LINE_SIZE];
    // fgets(fileData, LINE_SIZE, file);
    //
    // for(int i = 0; i < LINE_SIZE - 1; i++){
    //     printf("%c", fileData[i]);
    // }

    int count = 0;
    for(char c = fgetc(file); c != EOF; c = fgetc(file)){
        printf("le char : '%c'\n", c);
        if(c != '\n'){
            count++;
        }
    }

    printf("strlen = %ld\n", strlen(fileData));
    printf("count : %d\n", count);

    // char niceData[strlen(fileData)];
    // strncpy(niceData, fileData, 6);
    // printf("%s", niceData);

    //printf("%s\n", fileData);
    fclose(file);
    return 0;
}

int maian(int argc, char const *argv[]) {

    int LINE_SIZE = 1024;
    int nb_lg = 11;
    FILE *file = fopen("extra_mini_lorem.txt", "r");
    char lineData[LINE_SIZE];
    char *blockData = malloc(sizeof(char) * LINE_SIZE * nb_lg);

    int i = 0;
    while (i < nb_lg && fgets(lineData, LINE_SIZE, file)) {
        strcat(blockData, lineData);
        i++;
    }

    printf("%s", blockData);
    printf("\nFINN\n");
    //strcat(dest, src);

    return 0;
}

unsigned int count_char(char *data){
    unsigned int count = 0;
    int i = 0;
    while (data[i] != '\0') {
        count++;
        i++;
    }
    return count;
}

int main(int argc, char const *argv[]) {
    char *c = "1234\n5";
    unsigned int count = count_char(c);

    printf("%d\n", count);
    return 0;
}
