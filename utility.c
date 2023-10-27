#include "shell.h"

int ft_perror(char *str, int ret)
{
	perror(str);
	return ret;
}

int is_fd_open(int fd) {
	int flags = fcntl(fd, F_GETFL);
	if (flags == -1) {
		// Error occurred, handle it accordingly
		perror("fcntl");
		return -1; // Or any other error code
	}

	// Check if the file descriptor is open for reading or writing
	if (flags & O_RDONLY || flags & O_WRONLY || flags & O_RDWR) {
		return 1; // File descriptor is open
	} else {
		return 0; // File descriptor is not open
	}
}

t_file *new_tfile(int fd, char *file_name)
{
	t_file *file = (t_file *)malloc(sizeof(t_file));
	file->fd = fd;
	file->file_name = strdup(file_name);
	return file;
}

void current_directory()
{
	char *buff;
	long size;

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

int TrocearCadena(char * str,   char * words[])
{ 
	int i=1;
	if ((words[0]=strtok(str," \n\t"))==NULL)
		return 0;
	while ((words[i]=strtok(NULL," \n\t"))!=NULL)
		i++;
	return i; 
}

char file_type_letter(mode_t m)
{
	 switch (m&S_IFMT) { /*and bit a bit con los bits de formato,0170000 */
		case S_IFSOCK: return 's'; /*socket */
		case S_IFLNK: return 'l'; /*symbolic link*/
		case S_IFREG: return '-'; /* fichero normal*/
		case S_IFBLK: return 'b'; /*block device*/
		case S_IFDIR: return 'd'; /*directorio */ 
		case S_IFCHR: return 'c'; /*char device*/
		case S_IFIFO: return 'p'; /*pipe*/
		default: return '?'; /*desconocido, no deberia aparecer*/
	 }
}

char *convert_mode(mode_t m)
{
	static char permissions[12];
	strcpy (permissions,"----------@");
	
	permissions[0]=file_type_letter(m);
	if (m&S_IRUSR) permissions[1]='r';	/*propietario*/
	if (m&S_IWUSR) permissions[2]='w';
	if (m&S_IXUSR) permissions[3]='x';
	if (m&S_IRGRP) permissions[4]='r';	/*grupo*/
	if (m&S_IWGRP) permissions[5]='w';
	if (m&S_IXGRP) permissions[6]='x';
	if (m&S_IROTH) permissions[7]='r';	/*resto*/
	if (m&S_IWOTH) permissions[8]='w';
	if (m&S_IXOTH) permissions[9]='x';
	if (m&S_ISUID) permissions[3]='s';	/*setuid, setgid y stickybit*/
	if (m&S_ISGID) permissions[6]='s';
	if (m&S_ISVTX) permissions[9]='t';
	
	return permissions;
}

void stat_helper(char *file, int l, int acc, int link)
{
	struct stat file_stat;

	if (lstat(file, &file_stat))
	{
		perror("lstat");
		return;
	}
	if (acc)
	{
		struct tm *access_time = localtime(&file_stat.st_atime);
		char access_date[50];
		strftime(access_date, sizeof(access_date), "%Y/%m/%d %H:%M", access_time);
		printf("%s ", access_date);
	}
		// printf("%ld ", file_stat.st_atimespec);
	if (l)
	{
		printf("%s ", convert_mode(file_stat.st_mode));
		printf("%d ", file_stat.st_nlink);
		struct passwd *owner = getpwuid(file_stat.st_uid);
		printf("%s ", owner ? owner->pw_name : "unknown");
		struct group *gr = getgrgid(file_stat.st_gid);
		printf("%s ", gr ? gr->gr_name : "unknown");
		// printf("%ld ", file_stat.st_birthtimespec); //format later
		struct tm *creation_time = localtime(&file_stat.st_ctime);
		char creation_date[50];
		strftime(creation_date, sizeof(creation_date), "%Y/%m/%d %H:%M", creation_time);
		printf("%s ", creation_date);
	}
	printf("%lld %s", (long long)file_stat.st_size, get_file_name(file));
	if (link && S_ISLNK(file_stat.st_mode))
	{
		char linked_to[1024];
		ssize_t len = readlink(file, linked_to, sizeof(linked_to) - 1);
		if (len != -1) {
			linked_to[len] = '\0';
			printf("->%s", linked_to);
		}
		else
			perror("readlink");
	}
	printf("\n");
}

void stat_flags(int *l, int *acc, int *link, char *word)
{
	if (!strcmp(word, "-long"))
		*l = 1;
	else if (!strcmp(word, "-acc"))
		*acc = 1;
	else if (!strcmp(word, "-link"))
		*link = 1;
}

void delete(char *file)
{
	char *dir = dirname(file);
	if (!access(dir, W_OK))
	{
		if (remove(file) == -1)
			printf("File/directory named - %s - not deleted\n", file); // check errno say why
	}
	else
		printf("Deletetion of - %s - failed due to permission issues\n", file);
}

void list_dir(char * dir_name, int l, int acc, int link, int reca, int recb, int hid)
{
	DIR *dir;
	struct dirent *entry;
	struct stat file_stat;

	if (reca || (!reca && !recb))
		list_files(dir_name, l, acc, link, hid);
	if (!reca && !recb)
		return;

	if ((dir = opendir(dir_name)) != NULL)
	{
		while ((entry = readdir(dir)) != NULL)
		{
			if (!strncmp(entry->d_name, ".", 1) && !hid)
				continue;
			char full_name[1024];
			sprintf(full_name, "%s/%s", dir_name, entry->d_name);
			if (lstat(full_name, &file_stat) == 0)
			{
				if (S_ISDIR(file_stat.st_mode) && strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
					list_dir(full_name, l, acc, link, reca, recb, hid);
			}
			else
				perror("lstat");
		}
	}

	if (recb)
		list_files(dir_name, l, acc, link, hid);
	closedir(dir);
}

void list_files(char *dir_name, int l, int acc, int link, int hid)
{
	DIR *dir;
	struct dirent *entry;
	printf("\nDIRECTORY ------ %s ------\n", dir_name);
	// stat_helper(dir_name, l, acc, link);
	if ((dir = opendir(dir_name)) != NULL)
	{
		while ((entry = readdir(dir)) != NULL)
		{
			if (!strncmp(entry->d_name, ".", 1) && !hid)
				continue;
			char full_name[1024];
			sprintf(full_name, "%s/%s", dir_name, entry->d_name);
			stat_helper(full_name, l, acc, link);
		}
	}
	else
		stat_helper(dir_name, l, acc, link);
	closedir(dir);
}

void delete_dir(char * dir_name)
{
	DIR *dir;
	struct dirent *entry;
	struct stat file_stat;

	if ((dir = opendir(dir_name)) == NULL)
	{
		delete(dir_name);
		return;
	}
	while ((entry = readdir(dir)) != NULL)
	{
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
			continue;
		char full_name[1024];
		sprintf(full_name, "%s/%s", dir_name, entry->d_name);
		if (lstat(full_name, &file_stat) == 0) {
        	if (S_ISDIR(file_stat.st_mode))
				delete_dir(full_name);
			else
				delete(full_name);
		}
		else
			perror("lstat");
	}
	closedir(dir);
	delete(dir_name);
}

char *get_file_name(char *path)
{
    char *file_name = strrchr(path, '/');
    
    if (file_name != NULL)
        return file_name + 1;
    else
        return path;
}