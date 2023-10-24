#include "shell.h"

//Authors: Anahit Apresyan - anahit.apresyan
//		  Salma


int TrocearCadena(char * str,   char * words[])
{ 
	int i=1;
	if ((words[0]=strtok(str," \n\t"))==NULL)
		return 0;
	while ((words[i]=strtok(NULL," \n\t"))!=NULL)
		i++;
	return i; 
}

int main(int argc, char **argv)
{
	char line[MAXLINE];
	char *words[MAXLINE/2];
	t_list *hist = create_list();
	t_list *open_files = create_list();

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
		if (process_command(line, words, hist, open_files) == 0)
			break;
	}
	destroy_list(hist);
	destroy_list(open_files);
	return (EXIT_SUCCESS);
}

int process_command(char *line, char * words[], t_list *hist, t_list *open_files)
{
	insert_element(hist, strdup(line));
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
		if (!strcmp(words[0],"comand"))
			 cmd_command(word_num, words, hist, open_files);
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
	}
	return 1;
}

void cmd_authors(int word_num, char * words[])
{
	char *authors_logins[2] = {"anahit.apresyan", "Selma"};
	char *authors_names[2] = {"Anahit Apresyan", "Selma Surname"};
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
	char *buff;
	long size;

	if (word_num > 1)
	{
		if (chdir(words[1]))
			perror("Impossible to change the directory");
	}
	else
	{
		size = pathconf(".", _PC_PATH_MAX);
		if ((buff = (char *)malloc((size_t)size))) {
			if (getcwd(buff, size))
				printf("Current working directory: %s\n", buff);
			else
				perror("getcwd");
			free(buff);
		}
		else
			perror("malloc");
	}
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

void cmd_command(int word_num, char *words[], t_list *hist, t_list *open_files)
{
	
	int i = 1;
	int n = 0;
	t_node *node = hist->top;

	
	if (word_num > 1)
		if (words[1][0] == '-' && words[1][1])
			n = strtol(words[1] + 1, NULL, 10);
	if (n > 0 && node)
	{
		while (i++ != n && node->next)
			node = node->next;
		char * line = strdup(node->data);
		if (line != NULL)
		{
			process_command(line, words, hist, open_files);
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
		printf("in the for loop\n");
		if (!strcmp(words[i],"cr")) mode|=	O_CREAT;
		else if (!strcmp(words[i],"ex")) mode|=O_EXCL;
		else if (!strcmp(words[i],"ro"))mode|=O_RDONLY;
		else if (!strcmp(words[i],"wo")) mode|=O_WRONLY;
		else if (!strcmp(words[i],"rw")) mode|=O_RDWR;
		else if (!strcmp(words[i],"ap")) mode|=O_APPEND;
		else if (!strcmp(words[i],"tr")) mode|=O_TRUNC; 
		else break;
	}
    
	printf("The mode is: %d\n", mode);
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