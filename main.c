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

void send_data(int *pipe, char *data){
    char msg[255];
    sprintf(msg, "Tu est le worker 50");
    write(*pipe, msg, sizeof(msg));
}

int main(int argc, char const *argv[]) {

    int nb_enfants = 2;

    int worker_id;
    int state;

    int pid_producteur = -1;
    int pid_collecteur = -1;

    int **pipes_ptow = malloc(sizeof(int *) * nb_enfants);
    int pipe_wtop[2];
    int pipe_wtoc[2];

    pipe(pipe_wtop);
    pipe(pipe_wtoc);

    int pid_client = getpid();

    //creation worker
    int i = 0;
    while((pid_client == getpid()) && (i < nb_enfants)){
        pipes_ptow[i] = malloc(sizeof(int)*2);
        worker_id = i;
        pipe(pipes_ptow[i]);
        i++;
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
        printf("Producteur OK\n");


        FILE *file = fopen("extra_mini_lorem.txt", "r");
        

        /*envoie donnée aux workers*/
        for(int i = 0; i<nb_enfants; i++){
            char msg[255];
            sprintf(msg, "Tu est le worker %d", i);
            write(pipes_ptow[i][1], msg, sizeof(msg));

            //send_data(&pipes_ptow[i][1]);
        }

    }

    if(pid_collecteur == getpid()){
        //Do collecteur stuff
        printf("Collecteur OK\n");

        char msg[255];
        read(pipe_wtoc[0], msg, sizeof(msg));
        printf("%s\n", msg);

        read(pipe_wtoc[0], msg, sizeof(msg));
        printf("%s\n", msg);
    }

    if(worker_id >= 0){
        //Do worker stuff
        printf("Worker %d OK\n", worker_id);
        char msg[255];
        read(pipes_ptow[worker_id][0], msg, sizeof(msg));
        printf("Message pour %d : %s\n", worker_id, msg);

        sprintf(msg, "OUI");
        write(pipe_wtoc[1], msg, sizeof(msg));
    }


    if(pid_client == getpid()){
        wait(&state);
        wait(&state);
        wait(&state);
        wait(&state);

        printf("\n----- CLIENT : fermeture du programme -----\n");
    }


    return 0;
}
