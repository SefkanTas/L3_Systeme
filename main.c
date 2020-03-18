#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

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

int main(int argc, char const *argv[]) {

    int nb_enfants = 2;
    int nb_lg = 20;
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

        char msg[255];
        read(pipe_wtoc[0], msg, sizeof(msg));
        //printf("COLLECTEUR : %s\n", msg);

        read(pipe_wtoc[0], msg, sizeof(msg));
        //printf("COLLECTEUR : %s\n", msg);
    }

    if(worker_id >= 0){
        //Do worker stuff
        //printf("Worker %d OK\n", worker_id);
        char data[line_size * nb_lg];
        read(pipes_ptow[worker_id][0], data, sizeof(data));
        printf("--------------------- Message pour %d ---------------------\n%s\n--------------------- FIN ---------------------\n", worker_id, data);

        sprintf(data, "OUI");
        write(pipe_wtoc[1], data, sizeof(data));
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
