#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../headers/file_manager.h"

/**
* Ouvre en lecture et retourne un fichier, quitte le programme s'il y a une erreur
*
* @param char *path : Chemin vers le fichier.
* @return FILE * : Le fichier.
**/
FILE * get_file(char *path){
    FILE *file = fopen(path, "r");
    if (file == NULL){
        printf("Erreur de fichier : %s\n", path);
        killpg(getpgrp(), SIGKILL);
    }
    return file;
}


/**
* Met à jour le fichier d'un FileManager en fonction de l'index de ce dernier.
*
* @param FileManager *fm : Le FileManager.
**/
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

/**
* Libère la mémoire des chaînes de caractères contenant les liens vers des
* fichiers d'un FileManager.
*
* @param FileManager *fm : Le FileManager.
**/
void fm_free_files_path(FileManager *fm){
    for(int i = 0; i< fm->nb_files; i++){
        free(fm->files_path[i]);
    }
    free(fm->files_path);
    fm->files_path = NULL;
}
