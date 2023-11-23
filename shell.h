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
# include <sys/ipc.h>
# include <sys/shm.h>
# include <sys/mman.h>
# define MAXLINE 2048
# define MAXNAME 2048
# define TAMANO 2048

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


typedef struct m_block
{
	void *addr;
	size_t size;
	char *time;
	char type;
	key_t key;
	char *file_name;
	int fd;
	struct m_block *next;
} mem_block;

typedef struct m_list
{
	mem_block *top;
}	mem_list;


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
int 	process_command(char *line, char *words[], t_list *hist, t_list *open_files, mem_list *mem_blocks);
void	cmd_authors(int word_num, char * words[]);
void	cmd_pid(int word_num, char *words[]);
void	cmd_chdir(int word_num, char *words[]);
void	cmd_date_time(int word_num, char *words[]);
void	cmd_hist(int word_num, char *words[], t_list *hist);
void 	cmd_command(int word_num, char *words[], t_list *hist, t_list *open_files, mem_list *mem_blocks);
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
void	*create_mem_list();
mem_block *create_block(void *addr, size_t size, char type_alloc);
int 	insert_block(mem_list *list, void *addr, size_t size, char type_alloc);
void 	destroy_mem_list(mem_list *list);
void	cmd_malloc(int word_num, char *words[], mem_list *mem_blocks);
void	remove_block(mem_list *list, size_t size, char type);
void	print_mem_list(mem_list *list);
char	*current_time();
void	cmd_shared(int word_num, char *words[], mem_list *mem_blocks);
void 	sharedCreate(char *words[], mem_list *mem_blocks);
void	*getMemoryShmget(key_t key, size_t size, mem_list *mem_blocks);
void	shared_delkey(char *words[]);
void	shared_free(char *words[], mem_list *list);
void	insert_shared_block(mem_list *mem_blocks, void *p, size_t size, key_t key);
void	remove_shared_block(mem_list *list, key_t key);
void	insert_mmap_block(mem_list *mem_blocks, void *p, size_t size, char *file, int fd);
void	*map_file(char *file, int protection, mem_list *list);
void	cmd_mmap(char *arg[], mem_list *list);
void	free_mmap(char *words[], mem_list *list);
// void 	remove_mmap_block(mem_block *block, mem_list *list);
void	recursive(int n);
void 	cmd_recurse(char *words[]);
void 	fill_memory(void *p, size_t cont, unsigned char byte);
void 	*get_sharedmem_addr(key_t key, mem_list *list);
void 	print_mmap_blocks(mem_list *list);
void	print_shared_blocks(mem_list *list);
void	print_malloc_blocks(mem_list *list);

#endif