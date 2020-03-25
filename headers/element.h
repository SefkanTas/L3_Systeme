#ifndef ELEMENT_H
#define ELEMENT_H

/**
* Element
*
* @char key[51] : un mot de 50 char max (+ '\0')
* @unsigned int count : Le nombre d'occurrences de la clé.
**/
typedef struct Element{
    char key[51];
    unsigned int count;
} Element;


/**
* ElementArray : Tableau dynamique de Element
*
* @Element *array : le tableau
* @int len : Le nombre d'éléments dans le tableau
* @int max_len : Le nombre d'élements maximum possible pour le tableau au vu de
*   la mémoire allouée pour ce dernier.
*
**/
typedef struct ElementArray{
    Element *array;
    int len;
    int max_len;
} ElementArray;

ElementArray init_element_array();
void add_element(ElementArray *ea, Element e);
Element init_element(char *key, unsigned int count);
int get_element_index_by_key(ElementArray ea, char *key);
void increment_element_count_by_n(ElementArray *ea, char *key, long n);
void increment_element_count(ElementArray *ea, char *key);
void merge_element_count(ElementArray *ea, Element e);
void free_element_array(ElementArray *ea);
void display_element(Element e);
void display_element_array(ElementArray ea);

#endif
