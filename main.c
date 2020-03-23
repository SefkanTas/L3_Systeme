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

    const int NB_ENFANT_MIN = 2;
    const int NB_ENFANT_MAX = 8;

    int nb_enfants = 2;
    int nb_lg = 2;
    int line_size = 1024;
    char *files_list[] = {"data/text.txt", "data/extra_mini_lorem.txt"};
    int nb_files = 2;
    FileManager fm;

    if(argc < 5){
        printf("Nombre de parametre inccorecte.\n");
        printf("%s type_comptage nb_enfants nb_lg liste_fichiers\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if(!is_type_valid(argv[1])){
        printf("Le type de comptage est invalide (c, w, s).\n");
        exit(EXIT_FAILURE);
    }

    if(!is_number(argv[2])){
        printf("Le nombre d'enfants est invalide.\n");
        exit(EXIT_FAILURE);
    }

    if(!is_number(argv[3])){
        printf("Le nombre de lignes est invalide.\n");
        exit(EXIT_FAILURE);
    }

    char type_comptage = argv[1][0];
    nb_enfants = atoi(argv[2]);

    if(nb_enfants == 0){
        nb_enfants = sysconf(_SC_NPROCESSORS_ONLN) - 2;
    }
    if(nb_enfants < NB_ENFANT_MIN){
        nb_enfants = NB_ENFANT_MIN;
    }
    else if (nb_enfants > NB_ENFANT_MAX) {
        nb_enfants = NB_ENFANT_MAX;
    }

    nb_lg = atoi(argv[3]);

    char *filess[argc - 4];
    int file_error = 0;
    for(int i = 4; i < argc; i++){
        if(access(argv[i], R_OK) == -1){
            printf("Le fichier \"%s\" n'existe pas.\n", argv[i]);
            file_error = 1;
        }
    }

    if(file_error){
        exit(EXIT_FAILURE);
    }

    fm.nb_files = argc - 4;
    fm.files_path = malloc(sizeof(char) * nb_files);
    char *oui[nb_files];
    for(int i = 4; i < argc; i++){
        fm.files_path[i-4] = malloc(sizeof(char) * strlen(argv[i]) + 1);
        strcpy(fm.files_path[i-4], argv[i]);
    }


    int worker_id;
    int pid_producteur = -1;
    int pid_collecteur = -1;
    int pid_client = getpid();
    int status;

    int pipes_ptow[nb_enfants][2];
    int pipe_wtop[2];
    int pipe_wtoc[2];

    pipe(pipe_wtop);
    pipe(pipe_wtoc);

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

    if(pid_producteur == getpid()){

        close(pipe_wtop[1]);
        close(pipe_wtoc[0]);
        close(pipe_wtoc[1]);

        for (int i = 0; i < nb_enfants; i++) {
            close(pipes_ptow[i][0]);
        }

        fm.index = 0;
        fm.last_used_index = -1;
        fm.nb_files = 2;
        fm.is_done = 0;


        int file_index = 0;
        int old_file_index = -1;
        int remaining_lines = nb_lg;
        int i = 0;

        FILE *file;

        int j = 0;
        while(j < nb_enfants){
            char *data_block = malloc(sizeof(char) * line_size * nb_lg);
            get_data_block(&fm, data_block, nb_lg, line_size);
            write(pipes_ptow[j][1], data_block, sizeof(char) * strlen(data_block) + 1);
            j++;
            //free(data_block);
        }

        int worker_id_requesting;

        while(!fm.is_done){
            char *data_block = malloc(sizeof(char) * line_size * nb_lg);
            read(pipe_wtop[0], &worker_id_requesting, sizeof(int));
            get_data_block(&fm, data_block, nb_lg, line_size);
            write(pipes_ptow[worker_id_requesting][1], data_block, sizeof(char) * strlen(data_block) + 1);
            //free(data_block);
        }

        close(pipe_wtop[0]);
        printf("END producteur\n");

    }

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

        while (read(pipe_wtoc[0], &e, sizeof(e)) > 0) {
            merge_element_count(&ea_count, e);
        }

        display_element_array(ea_count);

        free_element_array(&ea_count);
    }


    if(worker_id >= 0){
        close(pipe_wtop[0]);
        close(pipe_wtoc[0]);

        for (int i = 0; i < nb_enfants; i++) {
            if(i != worker_id){
                close(pipes_ptow[i][0]);
            }
            close(pipes_ptow[i][1]);
        }

        char data[line_size * nb_lg];
        ElementArray data_block;

        while (read(pipes_ptow[worker_id][0], data, sizeof(data)) > 0) {
            switch (type_comptage) {
                case 'c':
                    data_block = char_count(data);
                    break;
                case 'w':
                    data_block = word_count(data);
                    break;
                case 's':
                    data_block = separator_count(data);
                    break;
            }
            send_data_wtoc(data_block, pipe_wtoc);
            write(pipe_wtop[1], &worker_id, sizeof(int));
        }
        printf("end worker : %d\n", worker_id);

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
            wait(&status);
        }

        printf("\n----- CLIENT : fermeture du programme -----\n");
    }


    return 0;
}
