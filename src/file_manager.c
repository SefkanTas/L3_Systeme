#include <stdio.h>
#include <stdlib.h>

#include "../headers/file_manager.h"


/**
* Récupère le fichier, arrête le programme s'il y a une erreur.
* return le fichier
*/
FILE * get_file(char *path){
    FILE *file = fopen(path, "r");
    if (file == NULL){
        printf("Erreur de fichier\n");
        exit(EXIT_FAILURE);
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
