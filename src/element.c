#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/element.h"

/**
* init_element_array
*
* Initialise un ElementArray.
*
* @return ElementArray : Un ElementArray initialisé.
**/
ElementArray init_element_array(){
    ElementArray ea;
    ea.array = malloc(sizeof(Element) * 16) ;
    ea.len = 0;
    ea.max_len = 16;

    return ea;
}

/**
* init_element
*
* Initialise un nouvel Element
*
* @param char *key : la chaine de char pour la clé.
* @param unsigned int count : la valeur initial de count.
*
* @return Element : Un Element initialisé.
**/
Element init_element(char *key, unsigned int count){
    Element e;
    strcpy(e.key, key);
    e.count = count;

    return e;
}


/**
* free_element_array
*
* Libère la mémoire d'un ElementArray
*
* @param ElementArray *ea : L'ElementArray dont la mémoire doit être libérée.
**/
void free_element_array(ElementArray *ea){
    free(ea->array);
    ea->array = NULL;
    ea->len = 0;
    ea->max_len = 0;
}

/**
* add_element
*
* Ajoute un Element dans un ElementArray.
*
* @param ElementArray *ea : L'ElementArray cible.
* @param Element e : L'Element a ajouter.
**/
void add_element(ElementArray *ea, Element e){
    if(ea->len == ea->max_len){
        ea->max_len *= 2;
        ea->array = realloc(ea->array, ea->max_len * sizeof(Element));
    }
    ea->array[ea->len++] = e;
}


/**
* get_element_index_by_key
*
* Retourne l'index d'un Element dans un ElementArray en fonction de sa clé.
*
* @param ElementArray ea : L'ElementArray cible.
* @param char *key : La clé
*
* @return int : L'index de l'Element dans l'ElementArray.
**/
int get_element_index_by_key(ElementArray ea, char *key){
    int found = -1;
    int i = 0;
    while(i < ea.len && found == -1){
        if(strcmp(ea.array[i].key, key) == 0){
            found = i;
        }
        i++;
    }
    return found;
}

/**
* increment_element_count_by_n
*
* Incremente le compteur d'un Element de n.
*
* @param ElementArray *ea : ElementArray qui contient l'Element
* @param char *key : La clé de l'Element.
* @param long n : La valeur d'incrémentation.
*
**/
void increment_element_count_by_n(ElementArray *ea, char *key, long n){
    int index = get_element_index_by_key(*ea, key);

    if(index == -1){
        if(n >= UINT_MAX){
            n = UINT_MAX;
        }
        Element e = init_element(key, n);
        add_element(ea, e);
    }
    else{
        if(ea->array[index].count + n > UINT_MAX){
            ea->array[index].count = UINT_MAX;
        }
        else{
            ea->array[index].count += n;
        }
    }
}


/**
* increment_element_count
*
* Incremente le compteur d'un Element de 1.
*
* @param ElementArray *ea : ElementArray qui contient l'Element
* @param char *key : La clé de l'Element.
*
**/
void increment_element_count(ElementArray *ea, char *key){
    increment_element_count_by_n(ea, key, 1);
}

/**
* merge_element_count
*
* Fusionne les compteurs d'un Element dans un ElementArray avec celui d'un autre
*   Element qui a la même clé. Si la fusion n'est pas possible, l'Element
*   extérieur est ajouté à l'ElementArray.
*
* @param ElementArray *ea
* @param Element e
*/
void merge_element_count(ElementArray *ea, Element e){
    increment_element_count_by_n(ea, e.key, e.count);
}

/**
* display_element
*
* Affiche un Element.
*
* @param Element e : Element à afficher.
**/
void display_element(Element e){
    if(e.count == UINT_MAX){
        printf("%s => %d+\n", e.key, e.count);
    }
    else{
        printf("%s => %d\n", e.key, e.count);
    }
}

/**
* display_element_array
*
* Affiche les Element d'un ElementArray.
*
* @param ElementArray ea : ElementArray dont les Element sont a afficher.
**/
void display_element_array(ElementArray ea){
    for(int i = 0; i < ea.len; i++){
        display_element(ea.array[i]);
    }
}
