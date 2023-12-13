#include "shell.h"

process_list *pr_list;
extern char **environ;

int main(int argc, char **argv, char **env)
{
	char line[MAXLINE];
	char *words[MAXLINE/2];
	t_list *hist = create_list();
	t_list *open_files = create_list();
	mem_list *mem_blocks = create_mem_list();
	pr_list = create_process_list();

	if (!hist || !open_files)
		return -1;

	if (is_fd_open(0))
		insert_open_file(open_files, new_tfile(0, "Standard input"));
	if (is_fd_open(1))
		insert_open_file(open_files, new_tfile(1, "Standard output"));
	if (is_fd_open(2))
		insert_open_file(open_files, new_tfile(2, "Standard error"));
	
	while (1)
	{
		printf("\001\033[1;35m\002Shell> \001\033[1;0m\002");
		if (!fgets(line, sizeof(line), stdin))
		{
			perror("exit\n");
			return (EXIT_FAILURE);
		}
		insert_element(hist, strdup(line));
		if (process_command(line, words, hist, open_files, mem_blocks, env) == 0)
			break;
	}
	destroy_list(hist, 0);
	destroy_list(open_files, 1);
	destroy_mem_list(mem_blocks);
	destroy_process_list(pr_list);
	return (EXIT_SUCCESS);
}

void cmd_uid(int word_num, char *words[])
{
	int l = 0;
	uid_t uid = getuid();
	uid_t euid = geteuid();
	struct passwd *u_info = getpwuid(uid);
	struct passwd *eu_info = getpwuid(euid);

	if (word_num == 1 || !strcmp(words[1], "-get")) {
		printf("Real credential: %ld, (%s)\n", (long)uid, u_info ? u_info->pw_name : "unknown");
		printf("Effective credential: %ld, (%s)\n", (long)euid, eu_info ? eu_info->pw_name : "unknown");
		return;
	}
	if (!strcmp(words[1], "-set"))
	{
		
		if (words[2] != NULL && !strcmp(words[2], "-l"))
			l = 1;
		uid_t id;
		if (l)
		{
			struct passwd *pwd = getpwnam(words[2]);
			if (pwd)
				id = pwd->pw_uid;
			else
			{
				perror("getpwnam");
				return;
			}
		}
		else
			id  = strtoul(words[2], NULL, 10); //ul?
		if (setuid(id) == -1)
			perror("setuid");
	}
}

int search_var(char *var, char *e[])
{
	int pos = 0;
	char aux[strlen(var) + 1];

	strcpy(aux, var);
	strcat(aux, "=");

	while (e[pos] != NULL)
		if (!strncmp(e[pos], aux, strlen(aux)))
			return(pos);
		else 
			pos++;
	errno = ENOENT;
	return(-1);
}

void cmd_showvar(char *words[], char *env[])
{
	int i;

	if (words[1] == NULL) 
	{
		showenv(env, "arg3");
		return;
	}
	if ((i = search_var(words[1], env)) != -1)
		printf("arg3 main: %s (%p) @%p\n", env[i], env[i], &env[i]);
	if ((i = search_var(words[1], environ)) != -1)
		printf("environ: %s (%p) @%p\n", environ[i], environ[i], &environ[i]);
	char *var = getenv(words[1]);
	// if (var != NULL)
		printf("getenv: %s (%p)\n", var, var);
}

void showenv(char *env[], char *name)
{
	int i = 0;
	while (env[i] != NULL) {
		printf("%p->%s[%d]=(%p) %s\n", &env[i], name, i,env[i],env[i]);
		i++;
	}
}

void cmd_showenv(char *words[], char *env[])
{
	if (words[1] == NULL)
		showenv(env, "arg3");
	else if (!strcmp(words[1], "-environ"))
		showenv(environ, "environ");
	else if (!strcmp(words[1], "-addr"))
	{
		printf("arg3: %p (stored in %p)\n", env, &env);
		printf("environ: %p (stored in %p)\n", environ, &environ);
	}
}

void changevar(char *var, char *value, char *e[])
{
	int i;
	if ((i = search_var(var, e)) != -1) {
		char *aux;
		if ((aux = (char *)malloc((strlen(var) + strlen(value) + 2) * sizeof(char *))) == NULL)
		{
			perror("malloc");
			return;
		}
		strcpy(aux, var);
		strcat(aux, "=");
		strcat(aux, value);
		e[i] = aux;
	}
	else
		printf("Impossible to change the variable value: No such variable\n");
}

void cmd_changevar(int word_num, char *words[], char *env[])
{
	if (word_num != 4)
	{
		printf("Usage: changevar [-a|-e|-p] var value\n");
		return;
	}
	if (!strcmp(words[1], "-a"))
		changevar(words[2], words[3], env);
	else if (!strcmp(words[1], "-e"))
		changevar(words[2], words[3], environ);
	else if (!strcmp(words[1], "-p")) {
		char *aux;
		if ((aux = (char *)malloc((strlen(words[2]) + strlen(words[3]) + 2) * sizeof(char *))) == NULL)
		{
			perror("malloc");
			return;
		}
		strcpy(aux, words[2]);
		strcat(aux, "=");
		strcat(aux, words[3]);
		printf("AUX: %s\n", aux);
		if (putenv(aux) == -1)
			perror("putenv");
	}
	else
		printf("Usage: changevar [-a|-e|-p] var value\n");
}

void subsvar(char *var1, char *var2, char *value, char *e[])
{
	int i;
	if ((i = search_var(var1, e)) != -1) {
		char *aux = (char *)malloc((strlen(var2) + strlen(value) + 1) * sizeof(char *));
		strcpy(aux, var2);
		strcat(aux, "=");
		strcat(aux, value);
		e[i] = aux;
	}
	else
		printf("Impossible to change the variable value: No such variable\n");
}

void cmd_subsvar(int word_num, char *words[], char *env[])
{
	if (word_num != 5)
	{
		printf("Usage: subsvar [-a|-e] var value\n");
		return;
	}
	if (!strcmp(words[1], "-a"))
		subsvar(words[2], words[3], words[4], env);
	else if (!strcmp(words[1], "-e"))
		subsvar(words[2], words[3], words[4], environ);
	else
		printf("Usage: subsvar [-a|-e] var value\n");
}

void cmd_fork(char *tr[])
{
	pid_t pid;
	if ((pid = fork()) == 0){
		empty_process_list(pr_list);
		printf("Running process %d\n", getpid());
	}
	else if (pid != -1)
		waitpid(pid, NULL, 0);
}

void cmd_exec(char *args[])
{
	if (args[0] == NULL)
	{
		printf("Usage: exec pr args\n");
		return;
	}
	if (execvp(args[0], args) == -1)
		perror("execvp");
}

void cmd_execute(int word_num, char *words[])
{
	int bg = 0;
	pid_t pid;

	if (word_num != 1 && !strcmp(words[word_num - 1], "&"))
	{
		words[word_num - 1] = NULL;
		bg = 1;
	}
	if ((pid = fork()) == 0)
	{
		if (execvp(words[0], words) == -1)
		{
			perror("execvp");
			exit(255);
		}
	}
	else
	{
		if (!bg)
			waitpid(pid, NULL, 0);
		else
			insert_process_block(pr_list, pid, words);
	}
}

void cmd_job(char *words[])
{
	int fg = 0;
	long long pid;

	if (words[1] != NULL && !strcmp(words[1], "-fg"))
		fg = 1;
	if (words[1 + fg] != NULL)
	{
		pid = strtol(words[1 + fg], NULL, 10);
		process_block *block;
		if ((block = find_process_block(pr_list, pid)) != NULL)
		{
			check_status(block);
			if (fg)
			{
				if (block->status == ACTIVE)
				{
					int status;
					waitpid(block->pid, &status, WUNTRACED | WCONTINUED);
					update_status(status, block);
					print_state(block);
					printf("\n");
					remove_process_block(pr_list, block);
				}
				else
					printf("The process %d is not active\n", block->pid);
			}
			else
				process_info(block);
		}
		else
			printf("No process with pid %lld\n", pid);
	}
	else
		printf("Usage: job [-fg] pid\n");
}

void cmd_deljobs(char *words[])
{
	int term = 0;
	int sig = 0;
	int i = 0;

	while (words[i] != NULL)
	{
		if (!strcmp(words[i], "-term"))
			term = 1;
		if (!strcmp(words[i], "-sig"))
			sig = 1;
		i++;
	}
	if (!term && !sig)
		printf("Usage: deljobs [-term] [-sig]\n");
	else
		remove_process_blocks(pr_list, term, sig);
}