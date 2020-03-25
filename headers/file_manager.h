#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

/**
* FileManager
**/
typedef struct FileManager{
    FILE *file;
    char **files_path;
    int nb_files;
    int index;
    int last_used_index;
    int is_done;
} FileManager;

FILE * get_file(char *path);
void fm_update_file(FileManager *fm);
void fm_free_files_path(FileManager *fm);

#endif
