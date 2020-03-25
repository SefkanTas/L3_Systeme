#ifndef PWCLIB_H
#define PWCLIB_H

void check_nb_params(int nb_param, int nb_required, const char *name);
void check_type_comptage(const char *param);
void check_nb_enfants(const char *param);
void check_nb_lg(const char *param);
void check_files(int argc, const char *argv[]);
void check_params(int argc, const char *argv[]);
int set_nb_enfants(int nb_enfants);
void fork_check(int fork_res);
void init_p_or_c(int pid_father, int *pid_son);
void get_data_block(FileManager *fm, char *data_block, int nb_lg, int line_size);
ElementArray char_count(char *data);
ElementArray word_count(char *data);
ElementArray separator_count(char *data);
ElementArray get_count_by_type(char type, char *data);
void send_data_wtoc(ElementArray ea, int *pipe_wtoc);
int is_type_valid(const char *type);
int is_number(const char *str);
void launch_producteur(int *pipe_cltop);

#endif
