#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../headers/element.h"


ElementArray init_element_array(){
    ElementArray ea;
    ea.array = malloc(sizeof(Element) * 16) ;
    ea.len = 0;
    ea.max_len = 16;

    return ea;
}

void add_element(ElementArray *ea, Element e){
    if(ea->len == ea->max_len){
        ea->max_len *= 2;
        ea->array = realloc(ea->array, ea->max_len * sizeof(Element));
    }
    ea->array[ea->len++] = e;
}

/**
* Initialise un nouvel Element
*
* @param char *key : la chaine de char pour la clé
* @param unsigned int count : la valeur initial de count
*
* @return Element : l'Element initialisé
**/
Element init_element(char *key, unsigned int count){
    Element e;
    //e.key = malloc(sizeof(char) * strlen(key));
    strcpy(e.key, key);
    e.count = count;

    return e;
}

void increment_element_count_by_n(ElementArray *ea, char *key, unsigned int n){
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
        Element e = init_element(key, n);
        add_element(ea, e);
    }
}

void increment_element_count(ElementArray *ea, char *key){
    increment_element_count_by_n(ea, key, 1);
}

void merge_element_count(ElementArray *ea, Element e){
    increment_element_count_by_n(ea, e.key, e.count);
}

void free_element_array(ElementArray *ea){
    free(ea->array);
    ea->array = NULL;
    ea->len = 0;
    ea->max_len = 0;
}

void display_element(Element e){
    printf("%s => %d\n", e.key, e.count);
}

void display_element_array(ElementArray ea){
    for(int i = 0; i < ea.len; i++){
        display_element(ea.array[i]);
    }
}
