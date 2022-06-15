#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAXCOM 1000
#define MAXLIST 100

#define clear() printf("\033[H\033[J")

void init_shell(void);
int take_input(char *str);
void print_dir(void);
void exec_args(char **parsed);
void exec_args_piped(char **parsed, char **parsedpipe);
int own_cmd_handler(char **parsed);
int parse_pipe(char *str, char **stripped);
void parse_space(char *str, char **parsed);
int process_string(char *str, char **parsed, char **parsed_pipe);
#endif
