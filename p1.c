#include "shell.h"

//Authors: Anahit Apresyan - anahit.apresyan
//		   Selma Djozic - selma.djozic

int main(int argc, char **argv)
{
	char line[MAXLINE];
	char *words[MAXLINE/2];
	t_list *hist = create_list();
	t_list *open_files = create_list();
	mem_list *mem_blocks = create_mem_list();

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
		if (process_command(line, words, hist, open_files, mem_blocks) == 0)
			break;
	}
	destroy_list(hist, 0);
	destroy_list(open_files, 1);
	destroy_mem_list(mem_blocks);
	return (EXIT_SUCCESS);
}

int process_command(char *line, char * words[], t_list *hist, t_list *open_files, mem_list *mem_blocks)
{
	// printf("LINEEEE: %s", line);
	int word_num = TrocearCadena(line, words);

	if (word_num > 0){
		if (!strcmp(words[0], "authors"))
			cmd_authors(word_num, words);
		if (!strcmp(words[0],"pid"))
			cmd_pid(word_num, words);
		if (!strcmp(words[0],"chdir"))
			cmd_chdir(word_num, words);
		if (!strcmp(words[0],"date"))
			cmd_date_time(word_num, words);
		if (!strcmp(words[0],"time"))
			cmd_date_time(word_num, words);
		if (!strcmp(words[0],"hist"))
			cmd_hist(word_num, words, hist);
		if (!strcmp(words[0],"command"))
			 cmd_command(word_num, words, hist, open_files, mem_blocks);
		if (!strcmp(words[0], "open"))
			 cmd_open(word_num, words, open_files);
		if (!strcmp(words[0], "close"))
			 cmd_close(word_num, words, open_files);
		if (!strcmp(words[0],"dup"))
			 cmd_dup(word_num, words, open_files);
		if (!strcmp(words[0], "listopen"))
			 cmd_listopen(word_num, words, open_files);
		if (!strcmp(words[0],"infosys"))
			 cmd_infosys();
		// if (!strcmp(words[0],"help"))
		//	 cmd_help(word_num, words);
		if (!strcmp(words[0],"quit") || !strcmp(words[0],"exit") || !strcmp(words[0],"bye"))
			return (0);
		if (!strcmp(words[0], "create"))
			cmd_create(word_num, words);
		if (!strcmp(words[0], "stat"))
			cmd_stat(word_num, words);
		if (!strcmp(words[0], "list"))
			cmd_list(word_num, words);
		if (!strcmp(words[0], "delete"))
			cmd_delete(word_num, words);
		if (!strcmp(words[0], "deltree"))
			cmd_deltree(word_num, words);
		if (!strcmp(words[0], "malloc"))
			cmd_malloc(word_num, words, mem_blocks);
		if (!strcmp(words[0], "shared"))
			cmd_shared(word_num, words, mem_blocks);
		if (!strcmp(words[0], "mmap"))
			cmd_mmap(words, mem_blocks);
		if (!strcmp(words[0], "recurse"))
			cmd_recurse(words);
		if (!strcmp(words[0], "memfill"))
			cmd_memfill(word_num, words);
	}
	return 1;
}

void cmd_authors(int word_num, char * words[])
{
	char *authors_logins[2] = {"anahit.apresyan", "selma.djozic"};
	char *authors_names[2] = {"Anahit Apresyan", "Selma Djozic"};
	if (word_num == 1)
		for (int i = 0; i < 2; i++)
			printf("%s: %s\n", authors_names[i], authors_logins[i]);
	else if (word_num > 1)
	{
		if (!strcmp(words[1], "-l"))
			for (int i = 0; i < 2; i++)
				printf("%s\n", authors_logins[i]);
		else if (!strcmp(words[1], "-n"))
			for (int i = 0; i < 2; i++)
				printf("%s\n", authors_names[i]);
	}
}

void cmd_pid(int word_num, char *words[])
{
	if (word_num > 1 && !strcmp(words[1], "-p"))
		printf("Pid of the parent of the shell: %d\n", getppid());
	else
		printf("Pid of the shell: %d\n", getpid());
}

void cmd_chdir(int word_num, char *words[])
{
	if (word_num > 1)
	{
		if (chdir(words[1]))
			perror("Impossible to change the directory");
	}
	else
		current_directory();
}

void cmd_date_time(int word_num, char *words[])
{
	time_t t;
	struct tm *curr_time;
	char time_str[50];

	t = time(NULL);
	curr_time = localtime(&t);

	if (!strcmp(words[0], "date"))
	{
		strftime(time_str, sizeof(time_str), "%d/%m/%Y", curr_time);
		printf("%s\n", time_str);
	}
	else
		printf("%02d:%02d:%02d\n", curr_time->tm_hour, curr_time->tm_min, curr_time->tm_sec);
}

void cmd_hist(int word_num, char *words[], t_list *hist)
{
	t_node *node = hist->top;
	int i = 1;
	int n = -1;

	if (word_num > 1)
	{
		if(!strcmp(words[1], "-c"))
		{
			while (hist->top != NULL)
				remove_top(hist);
			return;
		}
		if (words[1][0] == '-' && words[1][1])
			n = strtol(words[1] + 1, NULL, 10);
	}
	while (node && n--)
	{
		printf("%d: %s",i++, (char *)node->data);
		node = node->next;
	}
}

void cmd_command(int word_num, char *words[], t_list *hist, t_list *open_files, mem_list *mem_blocks)
{
	
	int i = 1;
	long n = 0;
	t_node *node = hist->top;

	if (word_num > 1)
		n = strtol(words[1], NULL, 10);
	if (n > 0 && node != NULL)
	{
		// printf("Node info %s\n", (char *)node->data);
		// if (node->next != NULL)
		// 	printf("Node has next\n");
		while (i++ != n && node->next != NULL)
		{
			// printf("in command - %d: %s\n", i - 1, (char *)node->data);
			node = node->next;
		}
		// printf("in command - %d: %s\n", i - 1, (char *)node->data);

		// printf("i: %d\nn: %ld\n", i, n);
		if (i != n + 1 || (i== n+1 && node->next == NULL))
			return;
		char *aux_words[MAXNAME];
		char *aux = strdup((char*)node->data);
		int aux_num = TrocearCadena(aux, aux_words);
		if (aux && word_num == aux_num)
		{
			int j = 0;
			for(j = 0; j < word_num; j++)
				if (strcmp(words[j], aux_words[j]))
					break;
			if (j == word_num)
			{
				printf("You are trying to create a recursion:D\n");
				free(aux);
				return;
			}
		}
		if (aux)
			free(aux);
		char *line = strdup((char *)node->data);

		if (line != NULL)
		{
			process_command(line, words, hist, open_files, mem_blocks);
			free(line);
		}
		else
			perror("Memory allocation failed.");
	}
}

void cmd_open (int word_num, char * words[], t_list *open_files)
{
    int df, mode=0;
    
    if (word_num == 1) { /*no hay parametro*/
    //    ..............ListarFicherosAbiertos...............
		// t_node *tmp = open_files->top;
		// while (tmp != NULL)
		// {
		// 	t_file *file = (t_file *)tmp->data;
		// 	if (file != NULL)
		// 	{
		// 		printf("descriptor: %d: %s\n", file->fd, file->file_name);
		// 		tmp = tmp->next;
		// 	}
		// }
		cmd_listopen(word_num, words, open_files);
        return;
    }

    for (int i=2; words[i]!=NULL; i++)
	{
		// printf("in the for loop\n");
		if (!strcmp(words[i],"cr")) mode|=	O_CREAT;
		else if (!strcmp(words[i],"ex")) mode|=O_EXCL;
		else if (!strcmp(words[i],"ro"))mode|=O_RDONLY;
		else if (!strcmp(words[i],"wo")) mode|=O_WRONLY;
		else if (!strcmp(words[i],"rw")) mode|=O_RDWR;
		else if (!strcmp(words[i],"ap")) mode|=O_APPEND;
		else if (!strcmp(words[i],"tr")) mode|=O_TRUNC; 
		else break;
	}
    
	// printf("The mode is: %d\n", mode);
    if ((df=open(words[1],mode,0777))==-1)
        perror ("Impossible to open the file");
    else
	{
		printf("File %s open with file descriptor %d\n", words[1], df);
		insert_element(open_files, new_tfile(df, words[1]));
	}
        // ...........AnadirAFicherosAbiertos (descriptor...modo...nombre....)....
        // printf ("Anadida entrada a la tabla ficheros abiertos..................",......);
}

void cmd_close (int word_num, char * words[], t_list *open_files)
{ 
    int df;
	errno = 0;

	if (word_num > 1)
		df = strtol(words[1], NULL, 10);
	if (word_num == 1 || errno != 0)
	{
		cmd_listopen(word_num, words, open_files);
		return;
	}
	// printf("About to try to close file descriptor %d\n", df);
    if (close(df)==-1)
        perror("Impossible to close the descriptor");
    else
	{
		printf("File descriptor %d closed\n", df);
		remove_open_file(open_files, df);
	}
}

void cmd_dup(int word_num, char * words[], t_list *open_files)
{ 
	int df;
	char aux[MAXNAME],*p;

	errno = 0;

	if (word_num > 1)
		df = strtol(words[1], NULL, 10);
	if (word_num == 1 || errno != 0)
	{
		cmd_listopen(word_num, words, open_files);
		return;
	}

	p=file_name(open_files, df);
	if (!p)
	{
		cmd_listopen(word_num, words, open_files);
		return;
	}
	sprintf (aux,"dup %d (%s)",df, p);
	int new_fd = dup(df);
	if (new_fd >= 0)
		insert_open_file(open_files, new_tfile(new_fd, aux));
	else
		perror("dup");
}

void cmd_listopen(int word_num, char *words[], t_list *open_files)
{
	t_node *tmp = open_files->top;
	while (tmp != NULL)
	{
		t_file *file = (t_file *)tmp->data;
		if (file != NULL)
		{
			int flags = fcntl(file->fd, F_GETFL);
			if (flags == -1) {
				perror("fcntl");
				return;
    		}
			int fl = flags & O_ACCMODE;
			if (fl == O_RDONLY)
				printf("descriptor: %d: %s %s\n", file->fd, file->file_name, "O_RDONLY");
			else if (fl == O_WRONLY)
				printf("descriptor: %d: %s %s\n", file->fd, file->file_name, "O_WRONLY");
			else if (fl == O_RDWR)
				printf("descriptor: %d: %s %s\n", file->fd, file->file_name, "O_RDWR");
			else
				printf("descriptor: %d: %s %s\n", file->fd, file->file_name, "file mode");
		}
		else
		{
			perror("casating");
			return;
		}
		tmp = tmp->next;
	}
}

void cmd_infosys()
{
	struct utsname systemInfo;

    if (uname(&systemInfo) != 0) {
        perror("uname");
		return;
    }
    printf("%s %s %s %s\n", systemInfo.nodename, systemInfo.sysname, systemInfo.release, systemInfo.version);
}

void cmd_create(int word_num, char *words[])
{
	int flag = 0;

	if (word_num > 1 && !strcmp(words[1], "-f"))
		flag = 1;
	if (word_num == 1 || (word_num == 2 && flag))
	{
		current_directory();
		return;
	}
	
	if (flag)
	{
		int fd = open(words[2], O_CREAT | O_WRONLY, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

		if (fd == -1)
			perror("open");
		else
		{
			printf("File '%s' created successfully.\n", words[2]);
			close(fd);
		}
	}
	else
	{
		int status = mkdir(words[1], S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

		if (!status)
			printf("Directory '%s' created successfully.\n", words[1]);
		else 
			perror("mkdir");
	}
}

void cmd_stat(int word_num, char *words[])
{
	int i;
	int l = 0;
	int acc = 0;
	int link = 0;

	for (i = 1; words[i] != NULL; i++)
	{
		if (strncmp(words[i], "-", 1))
			break;
		stat_flags(&l, &acc, &link, words[i]);
	}

	if (words[i] == NULL)
		current_directory(); 
	for (; words[i] != NULL; i++)
		stat_helper(words[i], l, acc, link);
}

void cmd_list(int word_num, char *words[])
{
	int i;
	int hid = 0;
	int reca = 0;
	int recb = 0;
	int l = 0;
	int acc = 0;
	int link = 0;

	for (i = 1; words[i] != NULL; i++)
	{
		if (strncmp(words[i], "-", 1))
			break;
		if (!strcmp(words[i], "-hid"))
			hid = 1;
		else if (!strcmp(words[i], "-reca") && !recb)
			reca = 1;
		else if (!strcmp(words[i], "-recb") && !reca)
			recb = 1;
		stat_flags(&l, &acc, &link, words[i]);
	}
	if (words[i] == NULL)
		current_directory();
	for (; words[i] != NULL; i++)
		list_dir(words[i], l, acc, link, reca, recb, hid);
	
}

void cmd_delete(int word_num, char *words[])
{
	if (word_num == 1)
		current_directory();
	else
	{
		for(int i = 1; words[i] != NULL; i++)
			delete(words[i]);
	}
}

void cmd_deltree(int word_num, char *words[])
{
	if (word_num == 1)
		current_directory();
	else
		for(int i = 1; words[i] != NULL; i++)
			delete_dir(words[i]);
}

