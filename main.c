#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

typedef struct element{
    char key[51];
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
*
* @return element : l'element initialisé
**/
element init_element(char *key, unsigned int count){
    element e;
    //e.key = malloc(sizeof(char) * strlen(key));
    strcpy(e.key, key);
    e.count = count;

    return e;
}

void increment_element_count_by_n(element_array *ea, char *key, unsigned int n){
    int i = 0;
    int done = 0;
    while(i < ea->len && !done){
        if(strcmp(ea->array[i].key, key) == 0){
            ea->array[i].count += n;
            done = 1;
        }
        i++;
    }
    if(!done){
        element e = init_element(key, n);
        add_element(ea, e);
    }
}

void increment_element_count(element_array *ea, char *key){
    increment_element_count_by_n(ea, key, 1);
}

void merge_element_count(element_array *ea, element e){
    increment_element_count_by_n(ea, e.key, e.count);
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
*
* @return int : 1 si c'est un separateur, 0 sinon
**/
int is_separator(char c){
    char separator_list[] = {
        '\n', '\t', ' ', ',', ';',
        '!', '?', '.', ':', '\r'
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
**/
void fork_check(int fork_res){
    if(fork_res == -1){
        printf("Erreur de fork\n");
        exit(EXIT_FAILURE);
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
        exit(EXIT_FAILURE);
    }
    return file;
}

typedef struct FileManager{
    FILE *file;
    char **files_path;
    int nb_files;
    int index;
    int last_used_index;
    int is_done;
} FileManager;

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
* Return un bloc de donnée selon la taille souhaitée, à partir d'un fichier
*
* @param FILE *file : fichier contenant la donnée
* @param int nb_lg : taille du bloc (nombre de ligne par bloc)
*
* @return char * : bloc de donnée
**/

void get_data_block(FileManager *fm, char *data_block, int nb_lg, int line_size){
    int i = 0;
    int remaining_lines = nb_lg;
    char data_line[line_size];
    fm_update_file(fm);
    while(i < remaining_lines && !fm->is_done){
        while (i < remaining_lines && fgets(data_line, line_size, fm->file)) {
            strcat(data_block, data_line);
            i++;
        }
        if(i < remaining_lines){
            fm->index++;
            remaining_lines -= i;
            i = 0;
        }
        fm_update_file(fm);
    }
}

/**
* Compte le nombre d'occurrences d'un caractère dans une chaîne de caractère.
*
* @prama char *data : la chaîne de caractère
*
* @return element_array : element_array avec le nombre d'occurrences de chaque caractère
**/
element_array char_count(char *data){
    element_array ea = init_element_array();
    int i = 0;
    while (data[i] != '\0') {
        if(!ispunct(data[i]) && !isspace(data[i])){
            char string[] = {toupper(data[i])};
            increment_element_count(&ea, string);
        }
        i++;
    }

    return ea;
}

element_array word_count(char *data){
    // char separator_list[] = {
    //     '\n', '\t', ' ', ',', ';',
    //     '!', '?', '.', ':', '\r'
    // };

    char *separators = " \n\t,;!?.:\r";

    element_array ea = init_element_array();

    char *data_tok = strtok(data, separators);
    int i;
    while (data_tok != NULL) {
        i = 0;
        while (data_tok[i] != '\0' && i < 50) {
            data_tok[i] = toupper(data_tok[i]);
            i++;
        }
        if(i == 50){
            strncpy(data_tok, data_tok, i);
            data_tok[i] = '\0';
        }
        increment_element_count(&ea, data_tok);
        data_tok = strtok(NULL, separators);
    }

    return ea;
}

element_array separator_count(char *data){
    element_array ea = init_element_array();
    int i = 0;
    while (data[i] != '\0') {
        if(ispunct(data[i]) || isspace(data[i])){
            if(data[i] == ' '){
                increment_element_count(&ea, "space");
            }
            else if(data[i] == '\r'){
                increment_element_count(&ea, "\\r");
            }
            else if(data[i] == '\n'){
                increment_element_count(&ea, "\\n");
            }
            else if(data[i] == '\t'){
                increment_element_count(&ea, "\\t");
            }
            else{
                char string[] = {data[i]};
                increment_element_count(&ea, string);
            }
        }
        i++;
    }

    return ea;
}




/**
* send each element of an element_array array from a work to the collecteur
*
* @param element_array ea : the element_array that contains our data
* @int *pipe_wtoc : pipe to communicate from worker to collecteur
**/
void send_data_wtoc(element_array ea, int *pipe_wtoc){
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



int is_type_valid(const char *type){
    int is_valid = 0;
    if(strlen(type) != 1){
        return is_valid;
    }

    char valid_types[] = "cws";
    int i = 0;
    while(i < strlen(valid_types) && !is_valid){
        if(type[0] == valid_types[i]){
            is_valid = 1;
        }
        i++;
    }

    return is_valid;
}


int is_number(const char *nb_enf){
    int is_number = 1;
    int i = 0;
    while(i < strlen(nb_enf) && is_number){
        if(!isdigit(nb_enf[i])){
            is_number = 0;
        }
        i++;
    }
    return is_number;
}

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

        element_array ea_count = init_element_array();

        element e;

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
        element_array data_block;

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
