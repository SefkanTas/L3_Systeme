#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>


typedef struct element{
    char key[50];
    unsigned int count;
} element;

typedef struct element_array{
    element *array;
    int len;
    int max_len;
} element_array;

element_array init_element_array(){
    element_array ea;
    ea.array = malloc(sizeof(element) * 16) ;
    ea.len = 0;
    ea.max_len = 16;

    return ea;
}

void add_element(element_array *ea, element e){
    if(ea->len == ea->max_len){
        ea->max_len *= 2;
        ea->array = realloc(ea->array, ea->max_len * sizeof(element));
    }
    ea->array[ea->len++] = e;
}

/**
* Initialise un nouvel element
*
* @param char *key : la chaine de char pour la clé
* @param unsigned int count : la valeur initial de count
* @return element : l'element initialisé
**/
element init_element(char *key, unsigned int count){
    element e;
    //e.key = malloc(sizeof(char) * strlen(key));
    strcpy(e.key, key);
    e.count = count;

    return e;
}

void increment_element_count(element_array *ea, char *key){
    int i = 0;
    int done = 0;
    while(i < ea->len && !done){
        //printf("LA COMPARAISON %d :: %s %s %d\n", strcmp(ea->array[i].key, key), ea->array[i].key, key, i);
        if(strcmp(ea->array[i].key, key) == 0){
            ea->array[i].count++;
            done = 1;
        }
        i++;
    }
    if(!done){
        element e = init_element(key, 1);
        add_element(ea, e);
    }
}

void free_element_array(element_array *ea){
    free(ea->array);
    ea->array = NULL;
    ea->len = 0;
    ea->max_len = 0;
}


/**
* Check si un char est un separateur
*
* @param char *c : char a tester
* @return int
*/
int is_separator(char c){
    char separator_list[] = {
        '\n', '\t', ' ', ',', ';',
        '!', '?', '.', ':'
    };
    int list_size = sizeof(separator_list) / sizeof(char);

    int i = 0;
    int is_separator = 0;
    while ((i<list_size) && !is_separator) {
        if(c == separator_list[i]){
            is_separator = 1;
        }
        i++;
    }
    return is_separator;
}


/**
* Check si un fork a bien fonctionné,
* quitte le processus sinon.
*/
void fork_check(int fork_res){
    if(fork_res == -1){
        printf("Erreur de fork\n");
        exit(1);
    }
}

/**
* Initialise les processus producteur ou collecteur
*
* @param pid_father *c : char a tester
*/
void init_p_or_c(int pid_father, int *pid_son){
    if(pid_father == getpid()){
        int tmp_fork_res = fork();
        fork_check(tmp_fork_res);
        if(tmp_fork_res == 0){
            *pid_son = getpid();
        }
    }
}

/**
* Récupère le fichier, arrête le programme s'il y a une erreur.
* return le fichier
*/
FILE * get_file(char *path){
    FILE *file = fopen(path, "r");
    if (file == NULL){
        printf("Erreur de fichier\n");
        exit(2);
    }
    return file;
}

/**
* Return un bloc de donnée selon la taille souhaitée, à partir d'un fichier
*
* @param FILE *file : fichier contenant la donnée
* @param int nb_lg : taille du bloc (nombre de ligne par bloc)
* @return char * : bloc de donnée
*/
char * get_data_block(FILE *file, int nb_lg, int line_size){
    char data_line[line_size];
    char *data_block = malloc(sizeof(char) * line_size * nb_lg);

    int i = 0;
    while (i < nb_lg && fgets(data_line, line_size, file)) {
        strcat(data_block, data_line);
        i++;
    }

    return data_block;
}

/**
* Envoie la première vagues de données vers les workers
*/
void send_data_to_workers(FILE *file, int **pipes_ptow, int nb_enfants, int nb_lg, int line_size){
    char *block;
    int i = 0;
    int file_has_data = 1;
    while (i<nb_enfants && file_has_data) {
        block = get_data_block(file, nb_lg, line_size);
        if(block[0] == '\0'){
            /* Si le fichier est vide */
            file_has_data = 0;
            write(pipes_ptow[i][1], "DONE", sizeof(char) * strlen("DONE") + 1);
        }
        else{
            write(pipes_ptow[i][1], block, sizeof(char) * strlen(block) + 1);
        }
        i++;
    }
}

element_array char_count(char *data){
    int i = 0;
    element_array ea = init_element_array();
    while (data[i] != '\0') {
        if(!is_separator(data[i])){
            char string[] = {toupper(data[i])};
            increment_element_count(&ea, string);
        }
        i++;
    }

    return ea;
}

// a voir si c'est bien
void send_count_data_wtoc(element_array ea, int *pipe_wtoc){
    for(int i = 0; i < ea.len; i++){
        write(pipe_wtoc[1], &ea.array[i], sizeof(element));
    }
}

void display_element(element e){
    printf("%s => %d\n", e.key, e.count);
}

void display_element_array(element_array ea){
    for(int i = 0; i < ea.len; i++){
        display_element(ea.array[i]);
    }
}

int main(int argc, char const *argv[]) {

    int nb_enfants = 2;
    int nb_lg = 4;
    int line_size = 1024;

    int worker_id;
    int pid_producteur = -1;
    int pid_collecteur = -1;
    int pid_client = getpid();
    int status;

    int **pipes_ptow = malloc(sizeof(int *) * nb_enfants);
    int pipe_wtop[2];
    int pipe_wtoc[2];

    pipe(pipe_wtop);
    pipe(pipe_wtoc);

    //creation worker
    int worker_created = 0;
    while((pid_client == getpid()) && (worker_created < nb_enfants)){
        worker_id = worker_created;
        pipes_ptow[worker_id] = malloc(sizeof(int)*2);
        pipe(pipes_ptow[worker_id]);
        worker_created++;
        fork();
    }

    if(pid_client == getpid()){
        worker_id = -1;
    }

    init_p_or_c(pid_client, &pid_producteur);
    init_p_or_c(pid_client, &pid_collecteur);

    if(pid_producteur == getpid()){
        //Do producteur stuff
        //Fermer les pipes_ptow en lecteur [0]
        //printf("Producteur OK\n");

        FILE *file = get_file("extra_mini_lorem.txt");

        send_data_to_workers(file, pipes_ptow, nb_enfants, nb_lg, line_size);

    }

    if(pid_collecteur == getpid()){
        //Do collecteur stuff
        //printf("Collecteur OK\n");

        // char msg[255];
        // read(pipe_wtoc[0], msg, sizeof(msg));
        // printf("COLLECTEUR : %s\n", msg);
        //
        // read(pipe_wtoc[0], msg, sizeof(msg));
        // printf("COLLECTEUR : %s\n", msg);

        element e;
        read(pipe_wtoc[0], &e, sizeof(e));
        printf("%s => %d\n", e.key, e.count);
        read(pipe_wtoc[0], &e, sizeof(e));
        printf("%s => %d\n", e.key, e.count);
        read(pipe_wtoc[0], &e, sizeof(e));
        printf("%s => %d\n", e.key, e.count);
        read(pipe_wtoc[0], &e, sizeof(e));
        printf("%s => %d\n", e.key, e.count);
        read(pipe_wtoc[0], &e, sizeof(e));
        printf("%s => %d\n", e.key, e.count);
        read(pipe_wtoc[0], &e, sizeof(e));
        printf("%s => %d\n", e.key, e.count);
        read(pipe_wtoc[0], &e, sizeof(e));
        printf("%s => %d\n", e.key, e.count);
        read(pipe_wtoc[0], &e, sizeof(e));
        printf("%s => %d\n", e.key, e.count);
        read(pipe_wtoc[0], &e, sizeof(e));
        printf("%s => %d\n", e.key, e.count);


    }

    if(worker_id >= 0){
        char data[line_size * nb_lg];
        read(pipes_ptow[worker_id][0], data, sizeof(data));

        //printf("%s\n", data);

        element_array element_cout = char_count(data);
        //display_element_array(element_cout);

        send_count_data_wtoc(element_cout, pipe_wtoc);

        // sprintf(data, "OUI");
        // write(pipe_wtoc[1], data, sizeof(data));
    }


    if(pid_client == getpid()){
        wait(&status);
        wait(&status);
        wait(&status);
        wait(&status);

        printf("\n----- CLIENT : fermeture du programme -----\n");
    }


    return 0;
}
