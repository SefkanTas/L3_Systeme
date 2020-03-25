#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "../headers/file_manager.h"


/**
* Récupère le fichier, arrête le programme s'il y a une erreur.
* return le fichier
*/
FILE * get_file(char *path){
    FILE *file = fopen(path, "r");
    if (file == NULL){
        printf("Erreur de fichier : %s\n", path);
        killpg(getpgrp(), SIGKILL);
    }
    return file;
}

void fm_update_file(FileManager *fm){
    if(fm->index >= fm->nb_files){
        fm->is_done = 1;
        return;
    }
    if((fm->last_used_index != fm->index)){
        fm->file = get_file(fm->files_path[fm->index]);
        fm->last_used_index = fm->index;
    }
}

void fm_free_files_path(FileManager *fm){
    for(int i = 0; i< fm->nb_files; i++){
        free(fm->files_path[i]);
    }
    free(fm->files_path);
    fm->files_path = NULL;
}
