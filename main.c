#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define YSH_RL_BUFSIZE 1024
#define YSH_TOK_BUFSIZE 64
#define YSH_TOK_DELIM " \t\r\n\a"	
int main(int argc, char **argv)
{
	/*char *ysh_read_line(void)
	{
		int bufsize = YSH_RL_BUFSIZE;
		int position = 0;
		char *buffer = malloc(sizeof(char) * bufsize);
		int c;

		if (!buffer) {
			fprintf(stderr, "ysh: allocation error\n");
			exit(EXIT_FAILURE);		
		}

		while (1) {
			c = getchar();
			if (c == EOF || c == \n) {
				buffer[position] = '\0';
				return buffer;
			} else {
				buffer[position] = c;
			}
			position++;

			if (position >= bufsize) {
				bufsize += YSH_RL_BUFSIZE;
				buffer = realloc(buffer, bufsize);
				if (!buffer) {
					fprintf(stderr, "ysh: allocation error\n");
				}

			}
		}

	}*/

	char *ysh_read_line(void)
	{
		char *line = NULL;
		ssize_t bufsize = 0;
 		getline(&line, &bufsize, stdin);
		return line;		
	}

	char **ysh_split_line(char *line)
	{
		int bufsize = YSH_TOK_BUFSIZE, position = 0;
		char **tokens = malloc(bufsize * sizeof(char*));
		char *token;

		if (!tokens) {
			fprintf(stderr, "ysh: allocation error");
			exit(EXIT_FAILURE);
		}

		token = strtok(line, YSH_TOK_DELIM);
		while (token != NULL) {
			tokens[position] = token;
			position++;
		

		if (position >= bufsize) {
			bufsize += YSH_TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if(!tokens) {
				fprintf(stderr, "ysh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, YSH_TOK_DELIM);
	}
		tokens[position] = NULL;
		return tokens;
	}

	int ysh_launch(char **args)
	{
		pid_t pid, wpid;
		int status;

		pid = fork();
		if (pid == 0) {
			//child
			if (execvp(args[0], args) == -1) {
				perror("ysh");
			}
			exit(EXIT_FAILURE);
		} else if (pid < 0) {
			//err forking
			perror("ysh");
		} else {
			//parent
			do {
				wpid = waitpid(pid, &status, WUNTRACED);	
			} while (!WIFEXITED(status) && !WIFSIGNALED(status));
		}
		return 1;
	}
	/*
	int ysh_cd(char **args);	
	int ysh_help(char **args);
	int ysh_exit(char **args);
	*/
	/*
	char *builtin_str[] = {
		"cd",
		"help",
		"exit"
	};

	int (*builtin_func[]) (char **) = {
		&ysh_cd,
		&ysh_help,
		&ysh_exit
	};

	int ysh_num_builtins() {
		return sizeof(builtin_str) / sizeof(char *);
	}
	*/

	int ysh_cd(char **args)
	{
		if (args[1] == NULL) {
			if (chdir(args[1]) == 0) {
				chdir("/home/ilya");
			}
		} else {
			if (chdir(args[1]) != 0) {
				perror("ysh");
			}
		}
		return 1;
	}

	int ysh_help(char **args)
	{
		int i;
		printf("Ilya Bilyk's Shell 0.2\nBeta prototype; written in C\n");
		return 1;
	}

	int ysh_exit(char **args) {return 0;}
	
	int ysh_cls(char **args) {
		write(STDOUT_FILENO, "\e[1;1H\e[2J", 16);
		// system("clear &");
	}

	char *builtin_str[] = {
		"cd",
		"help",
		"exit",
		"cls"
	};

	int (*builtin_func[]) (char **) = {
		&ysh_cd,
		&ysh_help,
		&ysh_exit,
		&ysh_cls
	};

	int ysh_num_builtins() {
		return sizeof(builtin_str) / sizeof(char *);
	}

	int ysh_execute(char **args)
	{
		int i;

		if (args[0] == NULL) {
			return 0;
		}

		for (i = 0; i < ysh_num_builtins(); i++) {
			if (strcmp(args[0], builtin_str[i]) == 0) {
				return (*builtin_func[i])(args);
			}
		}
		return ysh_launch(args);
	}

	void ysh_loop(void)
	{
		char *line;
		char **args;
		int status;
		char cwd[1024];

		do {
			getcwd(cwd, sizeof(cwd));
			printf("\x1B[32m" "["); printf(cwd); printf("]" "\x1B[37m" "--ysh-> ");
			line = ysh_read_line();
			args = ysh_split_line(line);
			status = ysh_execute(args);

			free(line);
			free(args);
		} while (status);
	}	
	// Run command loop
	ysh_loop();
	
	// Perform any shutdown/cleanup
	
	return EXIT_SUCCESS;
}	
