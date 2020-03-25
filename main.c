#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#include "headers/element.h"
#include "headers/file_manager.h"
#include "headers/pwclib.h"

int main(int argc, char const *argv[]) {

    const int LINE_SIZE = 1024;

    check_params(argc, argv);

    char type_comptage = argv[1][0];
    int nb_enfants = set_nb_enfants(atoi(argv[2]));
    int nb_lg = atoi(argv[3]);
    const int BLOCK_SIZE = LINE_SIZE * nb_lg;

    int worker_id;
    int pid_producteur = -1;
    int pid_collecteur = -1;
    int pid_client = getpid();

    int pipes_ptow[nb_enfants][2];
    int pipe_wtop[2];
    int pipe_wtoc[2];

    pipe(pipe_wtop);
    pipe(pipe_wtoc);

    /*Création des workers*/
    int worker_created = 0;
    while((pid_client == getpid()) && (worker_created < nb_enfants)){
        worker_id = worker_created;
        pipe(pipes_ptow[worker_id]);
        worker_created++;
        fork();
    }

    if(pid_client == getpid()){
        worker_id = -1;
    }

    init_p_or_c(pid_client, &pid_producteur);
    init_p_or_c(pid_client, &pid_collecteur);


    /* Code du producteur */
    if(pid_producteur == getpid()){
        close(pipe_wtop[1]);
        close(pipe_wtoc[0]);
        close(pipe_wtoc[1]);
        for (int i = 0; i < nb_enfants; i++) {
            close(pipes_ptow[i][0]);
        }

        FileManager fm;
        fm.nb_files = argc - 4;
        fm.files_path = malloc(sizeof(char) * fm.nb_files);

        /*Ajoute les path des fichier dans le FileManager*/
        for(int i = 4; i < argc; i++){
            fm.files_path[i-4] = malloc(sizeof(char) * strlen(argv[i]) + 1);
            strcpy(fm.files_path[i-4], argv[i]);
        }

        fm.index = 0;
        fm.last_used_index = -1;
        fm.is_done = 0;

        /*Envoie la première vague de données vers les tous les workers*/
        int j = 0;
        char data_block[BLOCK_SIZE];
        while(j < nb_enfants){
            data_block[0] = '\0';
            get_data_block(&fm, data_block, nb_lg, LINE_SIZE);
            write(pipes_ptow[j][1], data_block, sizeof(char) * strlen(data_block) + 1);
            j++;
        }

        /*Attend qu'un worker réalise une demande pour lui envoyer la donnée*/
        int worker_id_requesting;
        while(!fm.is_done){
            data_block[0] = '\0';
            read(pipe_wtop[0], &worker_id_requesting, sizeof(int));
            get_data_block(&fm, data_block, nb_lg, LINE_SIZE);
            write(pipes_ptow[worker_id_requesting][1], data_block, sizeof(char) * strlen(data_block) + 1);
        }

        fm_free_files_path(&fm);
        close(pipe_wtop[0]);
    }


    /* Code du collecteur */
    if(pid_collecteur == getpid()){
        close(pipe_wtop[0]);
        close(pipe_wtop[1]);
        close(pipe_wtoc[1]);
        for (int i = 0; i < nb_enfants; i++) {
            close(pipes_ptow[i][0]);
            close(pipes_ptow[i][1]);
        }

        ElementArray ea_count = init_element_array();
        Element e;

        /*Reçoit la donnée des workers puis les fusionnent*/
        while (read(pipe_wtoc[0], &e, sizeof(e)) > 0) {
            merge_element_count(&ea_count, e);
        }

        display_element_array(ea_count);
        free_element_array(&ea_count);
    }


    /* Code des workers */
    if(worker_id >= 0){
        close(pipe_wtop[0]);
        close(pipe_wtoc[0]);
        for (int i = 0; i < nb_enfants; i++) {
            if(i != worker_id){
                close(pipes_ptow[i][0]);
            }
            close(pipes_ptow[i][1]);
        }

        char data[BLOCK_SIZE];
        ElementArray element_count;

        /*
        * Reçoit la donnée du producteur,
        * réalise les opérations de comptage,
        * envoie la donnée au collecteur,
        * redemande de la donnée au producteur
        */
        while (read(pipes_ptow[worker_id][0], data, sizeof(data)) > 0) {
            element_count = get_count_by_type(type_comptage, data);
            send_data_wtoc(element_count, pipe_wtoc);
            write(pipe_wtop[1], &worker_id, sizeof(int));
        }

        printf("END worker : %d\n", worker_id);

        close(pipe_wtoc[1]);
        close(pipe_wtop[1]);
    }


    if(pid_client == getpid()){
        close(pipe_wtop[0]);
        close(pipe_wtop[1]);
        close(pipe_wtoc[0]);
        close(pipe_wtoc[1]);
        for (int i = 0; i < nb_enfants; i++) {
            close(pipes_ptow[i][0]);
            close(pipes_ptow[i][1]);
        }

        for(int i = 0; i < nb_enfants + 2; i++){
            wait(NULL);
        }

        printf("\n----- CLIENT : fermeture du programme -----\n");
    }

    return 0;
}
