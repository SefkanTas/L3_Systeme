#ifndef PWCLIB_H
#define PWCLIB_H

void fork_check(int fork_res);
void init_p_or_c(int pid_father, int *pid_son);
void get_data_block(FileManager *fm, char *data_block, int nb_lg, int line_size);
ElementArray char_count(char *data);
ElementArray word_count(char *data);
ElementArray separator_count(char *data);
void send_data_wtoc(ElementArray ea, int *pipe_wtoc);
int is_type_valid(const char *type);
int is_number(const char *nb_enf);

#endif
