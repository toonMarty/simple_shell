#include "main.h"

/**
 * init_shell - function that initializes
 * a shell
 * Return: nothing
 */

void init_shell(void)
{
	clear();
	sleep(1);
	clear();
}

/**
 * take_input - takes input from
 * user and reads the line
 * @str: the string entered
 * Return: Success (0), Failure (1)
 */

int take_input(char *str)
{
	char *buf;

	buf = readline("$ ");

	if (strlen(buf) != 0)
	{
		add_history(buf);
		strcpy(str, buf);

		return (0);
	}
	else
	{
		return (1);
	}
}

/**
 * print_dir - prints current
 * working directory
 * Return: void
 */

void print_dir(void)
{
	char cwd[1024];

	getcwd(cwd, sizeof(cwd));
	printf("\nDir: %s", cwd);
}

/**
 * exec_args - Function that executes
 * Command line arguments
 * @parsed: args_list
 * Return: nothing
 */

void exec_args(char **parsed)
{
	pid_t pid;

	pid = fork();

	if (pid == -1)
	{
		perror("Error:\n");
		return;
	}
	else if (pid == 0)
	{
		if (execvp(parsed[0], parsed) < 0)
		{
			printf("\nCould not execute command..");
		}

		exit(0);
	}
	else
	{
		wait(NULL);
		return;
	}
}

/**
 * exec_args_piped - function that
 * executes piped arguments
 * @parsed: args_list
 * @parsedpipe: piped args_list to be
 * analyzed/tokenized
 * Return: void
 */

void exec_args_piped(char **parsed, char **parsedpipe)
{
	int pipefd[2];
	pid_t p1, p2;

	if (pipe(pipefd) < 0)
	{
		printf("\nPipe could not be initialized");
		return;
	}

	p1 = fork();

	if (p1 < 0)
	{
		printf("\nCould not fork");
		return;
	}

	if (p1 == 0)
	{
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		if (execvp(parsed[0], parsed) < 0)
		{
			printf("\nCould not execute command");
			exit(0);
		}
	}
	else
	{
		p2 = fork();

		if (p2 < 0)
		{
			printf("\nFork failed");
			return;
		}
		if (p2 == 0)
		{
			close(pipefd[1]);
			dup2(pipefd[0], STDIN_FILENO);
			close(pipefd[0]);
			if (execvp(parsedpipe[0], parsedpipe) < 0)
			{
				printf("\nCommand execution failed");
			}
		}
		else
		{
			wait(NULL);
			wait(NULL);
		}
	}
}

/**
 * own_cmd_handler - Function that
 * handles built-in commands i.e
 * cd, and exit
 * @parsed: args_list to built-ins
 * Return: Success (0)
 */

int own_cmd_handler(char **parsed)
{
	int i;

	int switch_own_arg = 0;
	char *list_of_own_cmds[2];

	list_of_own_cmds[0] = "exit";
	list_of_own_cmds[1] = "cd";
	for (i = 0; i < 2; i++)
	{
		if (strcmp(parsed[0], list_of_own_cmds[i]) == 0)
		{
			switch_own_arg = i + 1;
			break;
		}
	}

	switch (switch_own_arg)
	{
		case 1:
			printf("\nlogout\n");
			exit(0);
		case 2:
			chdir(parsed[1]);
			return (1);
		default:
			break;
	}

	return (0);
}

/**
 * parse_pipe - Function that
 * analyzes/tokenizes piped args
 * @str: string to be parsed
 * @strpiped: piped string to be parsed
 * Return: Success (0) or (1) Failure
 */

int parse_pipe(char *str, char **strpiped)
{
	int i;

	for (i = 0; i < 2; i++)
	{
		strpiped[i] = strsep(&str, " | ");

		if (strpiped[i] == NULL)
		{
			break;
		}
	}

	if (strpiped[1] == NULL)
	{
		return (0);
	}
	else
	{
		return (1);
	}
}

/**
 * parse_space - tokenizes spaces &
 * separators
 * @str: the string to parse
 * @parsed: args_list
 * Return: void
 */

void parse_space(char *str, char **parsed)
{
	int i;

	for (i = 0; i < MAXLIST; i++)
	{
		parsed[i] = strsep(&str, " ");

		if (parsed[i] == NULL)
		{
			break;
		}

		if (strlen(parsed[i]) == 0)
		{
			i--;
		}
	}
}

/**
 * process_string - processes a string
 * @str: the string to be processed
 * @parsed: args_array
 * @parsed_pipe: piped args tokenization
 * Return: piped processed string
 */

int process_string(char *str, char **parsed, char **parsed_pipe)
{
	char *strpiped[2];

	int piped = 0;

	piped = parse_pipe(str, strpiped);

	if (piped)
	{
		parse_space(strpiped[0], parsed);
		parse_space(strpiped[1], parsed_pipe);
	}
	else
	{
		parse_space(str, parsed);
	}

	if (own_cmd_handler(parsed))
	{
		return (0);
	}
	else
	{
		return (1 + piped);
	}
}

/**
 * main - Entry point
 *
 * Return: Always 0 (Success)
 */
int main(void)
{
	char input_string[MAXCOM];
	char *parsed_args[MAXLIST];
	char *parsed_args_piped[MAXLIST];
	int exec_flag = 0;

	init_shell();

	while (1)
	{
		print_dir();

		if (take_input(input_string))
		{
			continue;
		}

		exec_flag = process_string(input_string, parsed_args, parsed_args_piped);

		if (exec_flag == 1)
		{
			exec_args(parsed_args);
		}

		if (exec_flag == 2)
		{
			exec_args_piped(parsed_args, parsed_args_piped);
		}
	}

	return (0);
}
