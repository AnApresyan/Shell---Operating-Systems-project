#ifndef SHELL_H
#define SHELL_H

# include "stdlib.h"
# include "stdio.h"
# include "string.h"
# include "unistd.h"
# include "fcntl.h"
# include "time.h"
# include "errno.h"
# include <sys/utsname.h>
# include <sys/stat.h>
# include <sys/types.h>
# include "pwd.h"
# include "grp.h"
# include "dirent.h"
# include <libgen.h>
# define MAXLINE 2048
# define MAXNAME 2048

typedef struct s_node
{
	void            *data;
	struct s_node	*next;
}	t_node;

typedef struct s_list
{
	t_node	*top;
}	t_list;

typedef struct s_file
{
	int		fd;
	char	*file_name;
}	t_file;

int		TrocearCadena(char * str,   char * words[]);
int     ft_perror(char *str, int ret);
t_node  *create_node(void *data);
void	*create_list();
void	insert_element(t_list *list, void *data);
void 	insert_open_file(t_list *list, t_file *file);
void	remove_element(t_list *list, char *data);
// void 	remove_element(t_list *list, int fd);
void 	remove_open_file(t_list *list, int fd);
void	remove_top(t_list *list);
void	destroy_list(t_list *list, int str);
int		is_fd_open(int fd);
t_file	*new_tfile(int fd, char *file_name);
int 	process_command(char *line, char *words[], t_list *hist, t_list *open_files);
void	cmd_authors(int word_num, char * words[]);
void	cmd_pid(int word_num, char *words[]);
void	cmd_chdir(int word_num, char *words[]);
void	cmd_date_time(int word_num, char *words[]);
void	cmd_hist(int word_num, char *words[], t_list *hist);
void 	cmd_command(int word_num, char *words[], t_list *hist, t_list *open_files);
void	cmd_open (int word_num, char * words[], t_list *open_files);
void	cmd_close (int word_num, char * words[], t_list *open_files);
void 	cmd_listopen(int word_num, char *words[], t_list *open_files);
void	cmd_dup(int word_num, char * words[], t_list *open_files);
char	*file_name(t_list *list, int fd);
void	cmd_infosys();
void	cmd_create(int word_num, char *words[]);
void	current_directory();
void 	cmd_stat(int word_num, char *words[]);
char 	file_type_letter (mode_t m);
char	*convert_mode(mode_t m);
void	stat_helper(char *file, int l, int acc, int link);
void	stat_flags(int *l, int *acc, int *link, char *word);
void	cmd_list(int word_num, char *words[]);
void	cmd_delete(int word_num, char *words[]);
void	delete(char *file);
void	delete_dir(char * dir_name);
void	cmd_deltree(int word_num, char *words[]);
void	list_dir(char * dir_name, int l, int acc, int link,int reca, int recb, int hid);
char	*get_file_name(char *path);
void 	list_files(char *dir_name, int l, int acc, int link, int hid);

#endif