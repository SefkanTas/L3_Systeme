#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../headers/element.h"
#include "../headers/file_manager.h"


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
* @return ElementArray : ElementArray avec le nombre d'occurrences de chaque caractère
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

ElementArray word_count(char *data){
    char *separators = " \n\t,;!?.:\r";

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
* send each Element of an ElementArray array from a work to the collecteur
*
* @param ElementArray ea : the ElementArray that contains our data
* @int *pipe_wtoc : pipe to communicate from worker to collecteur
**/
void send_data_wtoc(ElementArray ea, int *pipe_wtoc){
    for(int i = 0; i < ea.len; i++){
        write(pipe_wtoc[1], &ea.array[i], sizeof(Element));
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
