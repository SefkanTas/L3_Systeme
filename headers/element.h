#ifndef ELEMENT_H
#define ELEMENT_H

typedef struct Element{
    char key[51];
    unsigned int count;
} Element;

typedef struct ElementArray{
    Element *array;
    int len;
    int max_len;
} ElementArray;

ElementArray init_element_array();
void add_element(ElementArray *ea, Element e);
Element init_element(char *key, unsigned int count);
void increment_element_count_by_n(ElementArray *ea, char *key, unsigned int n);
void increment_element_count(ElementArray *ea, char *key);
void merge_element_count(ElementArray *ea, Element e);
void free_element_array(ElementArray *ea);
void display_element(Element e);
void display_element_array(ElementArray ea);

#endif
