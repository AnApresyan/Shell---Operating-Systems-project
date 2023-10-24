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