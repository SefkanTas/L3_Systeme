#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../headers/element.h"
#include "../headers/file_manager.h"
#include "../headers/pwclib.h"


/**
* check_nb_params
*
* Vérifie si le nombre de paramètres est correcte, quitte le programme sinon.
*
* @param int nb_param : Nombre de paramètres
* @param int nb_required : Nombre de paramètres requis
* @param const char *name : Nom du programme pour l'affichage de l'erreur
**/
void check_nb_params(int nb_param, int nb_required, const char *name){
    if(nb_param < nb_required){
        printf("Nombre de parametre inccorecte.\n");
        printf("%s type_comptage nb_enfants nb_lg liste_fichiers\n", name);
        exit(EXIT_FAILURE);
    }
}


/**
* check_type_comptage
*
* Vérifie que le type de comptage spécifié par l'utilisateur est correcte,
*   quitte le programme sinon.
*
* @param const char *param : Type de comptage entré par l'utilisateur
**/
void check_type_comptage(const char *param){
    if(!is_type_valid(param)){
        printf("Le type de comptage est invalide (c, w, s).\n");
        exit(EXIT_FAILURE);
    }
}

/**
* check_nb_enfants
*
* Vérifie que le nombre d'enfants spécifié par l'utilisateur est correcte,
*   quitte le programme sinon.
*
* @param const char *param : Nombre d'enfants entré par l'utilisateur
**/
void check_nb_enfants(const char *param){
    if(!is_number(param)){
        printf("Le nombre d'enfants est invalide.\n");
        exit(EXIT_FAILURE);
    }
}

/**
* check_nb_lg
*
* Vérifie que le nombre de lignes spécifié par l'utilisateur est correcte,
*   quitte le programme sinon.
*
* @param const char *param : Nombre de lignes entré par l'utilisateur
**/
void check_nb_lg(const char *param){
    if(!is_number(param)){
        printf("Le nombre de lignes est invalide.\n");
        exit(EXIT_FAILURE);
    }
}

/**
* check_files
*
* Vérifie que les fichiers spécifié par l'utilisateur existent et sont
*   accessible en écriture, quitte le programme sinon.
*
* @param int argc : nombre de paramètres du programme.
* @param const char *argv[] : Paramètres du programme.
**/
void check_files(int argc, const char *argv[]){
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
}

/**
* check_params
*
* Vérifie que tous les paramètres du programme sont correcte,
*    quitte le programme sinon.
*
* @param int argc : nombre de paramètres du programme.
* @param const char *argv[] : Paramètres du programme.
**/
void check_params(int argc, const char *argv[]){
    check_nb_params(argc, 5, argv[0]);
    check_type_comptage(argv[1]);
    check_nb_enfants(argv[2]);
    check_nb_lg(argv[3]);
    check_files(argc, argv);
}

/**
* Retourne le nombre d'enfants en respectant les règles définies.
*
* @param int nb_enfants : Le nombre d'enfants entré par l'utilisateur.
*
* @return int : Le nombre d'enfants respectant les règles définies.
**/
int set_nb_enfants(int nb_enfants){
    const int NB_ENFANT_MIN = 2;
    const int NB_ENFANT_MAX = 8;

    if(nb_enfants == 0){
        nb_enfants = sysconf(_SC_NPROCESSORS_ONLN) - 2;
    }
    if(nb_enfants < NB_ENFANT_MIN){
        nb_enfants = NB_ENFANT_MIN;
    }
    else if (nb_enfants > NB_ENFANT_MAX) {
        nb_enfants = NB_ENFANT_MAX;
    }
    return nb_enfants;
}

/**
* Check si un fork a bien fonctionné,
* quitte le processus sinon.
*
* @param int fork_res : Resultat du fork().
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
* @param int pid_father : Le pid du processus père.
* @param int *pid_son : Le pid du fils créer (producteur ou collecteur).
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
* Return un bloc de donnée selon la taille souhaitée, à partir d'un fichier
*
* @param FileManager *fm : Contient les informations sur les fichiers.
* @param char *data_block : Le buffer pour la données.
* @param int nb_lg : Nombre de ligne par bloc de données.
* @param int line_size : Ma taille d'une ligne.
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
* Compte le nombre d'occurrences de chaque caractères dans une chaîne de caractères.
*
* @param char *data : la chaîne de caractère
* @return ElementArray : ElementArray avec le nombre d'occurrences de chaque caractères
**/
ElementArray char_count(char *data){
    ElementArray ea = init_element_array();
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

/**
* Compte le nombre d'occurrences de chaque mots dans une chaîne de caractères.
*
* @param char *data : la chaîne de caractère
* @return ElementArray : ElementArray avec le nombre d'occurrences de chaque mots
**/
ElementArray word_count(char *data){
    char *separators = " \n\t\r!\"#$%&()*+,-./:;<=>?@[\\]^_`{|}~";

    ElementArray ea = init_element_array();

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

/**
* Compte le nombre d'occurrences de chaque séparateurs dans une chaîne de caractères.
*
* @param char *data : la chaîne de caractère.
* @return ElementArray : ElementArray avec le nombre d'occurrences de chaque séparateurs.
**/
ElementArray separator_count(char *data){
    ElementArray ea = init_element_array();
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
* Retourne le nombre d'occurence de chaque éléments dans une chaîne de
* caractères, en fonction d'un type.
*
* @param char type : Le type.
* @param char *data : la chaîne de caractère.
**/
ElementArray get_count_by_type(char type, char *data){
    ElementArray ea;
    switch (type) {
        case 'c':
            ea = char_count(data);
            break;
        case 'w':
            ea = word_count(data);
            break;
        case 's':
            ea = separator_count(data);
            break;
    }
    return ea;
}

/**
* send each Element of an ElementArray array from a worker to the collecteur
*
* @param ElementArray ea : the ElementArray that contains our data.
* @int *pipe_wtoc : pipe to communicate from worker to collecteur.
**/
void send_data_wtoc(ElementArray ea, int *pipe_wtoc){
    for(int i = 0; i < ea.len; i++){
        write(pipe_wtoc[1], &ea.array[i], sizeof(Element));
    }
}

/**
* Vérifie si un type est valide.
*
* @param const char *type : Le type.
* @return int : 1 si type valide, 0 sinon.
**/
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

/**
* Vérifie si une chaîne de caractères est un nombre.
*
* @param const char *str : La chaîne de caractères.
* @return int : 1 si la chaine est un nombre, 0 sinon.
**/
int is_number(const char *str){
    int is_number = 1;
    int i = 0;
    while(i < strlen(str) && is_number){
        if(!isdigit(str[i])){
            is_number = 0;
        }
        i++;
    }
    return is_number;
}
