#include "shell.h"

void cmd_malloc(int word_num, char *words[], mem_list *mem_blocks)
{
	long n = 0;
	int flag = 0;
	int i = 1;

	if (word_num > 1)
	{
		if (!strcmp(words[i], "-free"))
		{
			flag = 1;
			i++;
			if (words[i] == NULL) {
				printf("Wrong number of arguments\n");
				return;
			}
		}
		n = strtol(words[i], NULL, 10);
		if (n > 0)
		{
			if (!flag)
			{
				void *addr = malloc(n);
				if (addr != NULL)
				{
					insert_block(mem_blocks, addr, (size_t)n, 'm');
					printf("Allocatied %ld bytes at address %p\n", n, addr);
				}
				else
					perror("malloc");
			}
			else
				remove_block(mem_blocks, (size_t)n, 'm');
		}
		else
			printf("Invalid size\n");
		return;
	}
	print_malloc_blocks(mem_blocks);
	// printf("Memory blocks for process %d\n", getpid());
}

void *get_memory_shmget(key_t key, size_t size, mem_list *mem_blocks)
{
	void * p;
	int aux,id,flags=0777;
	struct shmid_ds s;

	if (size)	 /*tam distito de 0 indica crear */
		flags = flags | IPC_CREAT | IPC_EXCL;
	if (key == IPC_PRIVATE)  /*no nos vale*/
		{errno = EINVAL; return NULL;}
	if ((id = shmget(key, size, flags))==-1)
		return (NULL);
	if ((p = shmat(id,NULL,0)) == (void*) -1){
		aux = errno;
		if (size)
			 shmctl(id,IPC_RMID,NULL);
		errno = aux;
		return (NULL);
	}
	shmctl(id,IPC_STAT,&s);
	insert_shared_block(mem_blocks, p, (size_t)size, key);
	return (p);
}
void shared_create(char *words[], mem_list *mem_blocks)
{
	key_t key;
	size_t size;
	void *p;

	if (words[3]==NULL) {
		print_shared_blocks(mem_blocks); //change later to print only the shared memory
		return;
	}
	key=(key_t)  strtoul(words[2],NULL,10);
	size=(size_t) strtoul(words[3],NULL,10);
	if (size == 0) {
		printf ("No 0-byte blocks allocated\n");
		return;
	}
	if ((p = get_memory_shmget(key,size, mem_blocks)) != NULL)
		printf("Assigned %lu bytes in %p\n",(unsigned long) size, p);
	else
		printf("Unable to allocate shared memory with key %lu: %s\n", (unsigned long) key, strerror(errno));
}

void cmd_shared(int word_num, char *words[], mem_list *mem_blocks)
{
	if (word_num < 3){
		print_shared_blocks(mem_blocks);
		return;
	}
	if (!strcmp(words[1], "-create"))
		shared_create(words, mem_blocks);
	else if (!strcmp(words[1], "-free"))
		shared_free(words, mem_blocks);
	else if (!strcmp(words[1], "-delkey"))
		shared_delkey(words);
}

void shared_delkey(char *words[])
{
   key_t clave;
   int id;
   char *key = words[2];

   if (key == NULL || (clave = (key_t)strtoul(key,NULL,10)) == IPC_PRIVATE){
		printf ("delkey needs a valid_key\n");
		return;
   }
   if ((id = shmget(clave,0,0666)) == -1){
		perror ("shmget: impossible to get shared memory");
		return;
   }
   if (shmctl(id,IPC_RMID,NULL) == -1)
		perror ("shmctl: impossible to delete shared memory id\n");
}

void shared_free(char *words[], mem_list *list)
{
	key_t clave;
	char *key = words[2];
	void *p;

	if (key == NULL || (clave = (key_t)strtoul(key,NULL,10)) == IPC_PRIVATE){
		printf ("shared -free needs a valid_key\n");
		return;
	}
	if ((p = get_sharedmem_addr(clave, list)) != NULL) {
		if (shmdt(p) == -1)
			perror ("shmctl: impossible to delete shared memory id\n");
		else
			remove_shared_block(list, clave);
	}
	else
		printf("No block of memory with that key mapped in this process.");
}

void *map_file(char *file, int protection, mem_list *list)
{
	int df, map = MAP_PRIVATE, modo = O_RDONLY;
	struct stat s;
	void *p;

	if (protection&PROT_WRITE)
		modo = O_RDWR;
	if (stat(file, &s) == -1 || (df = open(file, modo)) == -1)
		return NULL;
	if ((p = mmap (NULL, s.st_size, protection, map, df, 0)) == MAP_FAILED){
		close(df);
		return NULL;
	}
	insert_mmap_block(list, p, s.st_size, file, df);
	return p;
}

void cmd_mmap(char *arg[], mem_list *list)
{ 
	 char *perm;
	 void *p;
	 int protection = 0;
	 
	if (arg[1] == NULL)
		{print_mmap_blocks(list); return;}				   //print the mapped list
	if (!strcmp(arg[1], "-free"))
	{
		// printf("Freeing\n");
		remove_mmap_block(arg[2], list);
		return;
	}
	if ((perm = arg[2]) != NULL && strlen(perm) < 4) {
		if (strchr(perm,'r') != NULL)
			protection|=PROT_READ;
		if (strchr(perm,'w') != NULL)
			protection|=PROT_WRITE;
		if (strchr(perm,'x') != NULL)
			protection|=PROT_EXEC;
	}
	if ((p = map_file(arg[1], protection, list)) == NULL)
		perror ("Impossible to map the file");
	else
		printf ("File %s mapped in %p\n", arg[1], p);
}

void cmd_recurse(char *words[])
{
	long n;
	if (words[1] != NULL) {
		n = strtoul(words[1], NULL, 10);
		recursive(n);
	}
}

void recursive(int n)
{
  char automatico[TAMANO];
  static char estatico[TAMANO];

  printf ("Parameter:%3d(%p) array %p, arr static %p\n",n,&n,automatico, estatico);

  if (n>0)
	recursive(n-1);
}

void fill_memory(void *p, size_t cont, unsigned char byte)
{
	unsigned char *arr=(unsigned char *) p;
	size_t i;

	printf("Filling %zu bytes of memory with byte (%02x) starting at address %p\n", cont, byte, p);
	for (i=0; i < cont; i++)
		arr[i]=byte;
}

void cmd_memfill(int word_num, char *words[])
{
	if (word_num < 4) 
	{
		printf("Wrong arguments\n");
		return;
	}
	void *addr = (void *)strtoul(words[1], NULL, 16);
	long s = strtol(words[2], NULL, 10);
	if (s < 0)
	{
		printf("Wrong size\n");
		return;
	}
	size_t cont = (size_t)s;
	unsigned char byte = (unsigned char)strtoul(words[3], NULL, 10);
	fill_memory(addr, cont, byte);
}

void cmd_memdump(int word_num, char *words[])
{
	if (word_num < 3) 
	{
		printf("Wrong arguments\n");
		return;
	}
	void *p = (void *)strtoul(words[1], NULL, 16);
	size_t cont = (size_t)strtoul(words[2], NULL, 10);
	unsigned char *arr=(unsigned char *) p;
	size_t i;

	printf("Dumping %zu bytes from address %p\n", cont, p);
	for (i=0; i < cont; i++)
		printf("%2c ", arr[i]);
	if (cont > 0)
		printf("\n");
	for (i=0; i < cont; i++)
		printf("%02x ", arr[i]);
	if (cont > 0)
		printf("\n");
}

ssize_t read_file(char *f, void *p, size_t cont)
{
	struct stat s;
	ssize_t  n;  
	int df,aux;

	if (stat (f,&s) == -1 || (df = open(f,O_RDONLY)) == -1)
		return -1;	 
	if (cont == -1)   /* si pasamos -1 como bytes a leer lo leemos entero*/
		cont = s.st_size;
	if ((n = read(df, p, cont)) == -1){
		aux = errno;
		close(df);
		errno = aux;
		return -1;
	}
	close(df);
	return n;
}

void cmd_read(char *words[])
{
	void *p;
	size_t cont = -1;  /* -1 indicates reading the entire file*/
	ssize_t n;
	if (words[1] == NULL || words[2] == NULL){
		printf ("Wrong number of arguments\n");
		return;
	}
	p = (void *)strtoul(words[2], NULL, 16);  /*convert from string to pointer*/
	if (words[3] != NULL)
		cont = (size_t)atoll(words[3]);
	if ((n = read_file(words[1], p, cont)) == -1)
		perror ("Impossible to read the file");
	else
		printf ("Reading %lld bytes from %s into %p\n",(long long)n, words[1], p);
}

ssize_t write_file(char *f, void *p, size_t cont, int overwrite)
{
	ssize_t  n;
	int df, aux, flags = O_CREAT | O_EXCL | O_WRONLY;

	if (overwrite)
		flags = O_CREAT | O_WRONLY | O_TRUNC;

	if ((df = open(f,flags,0777)) == -1)
		return -1;

	if ((n = write(df, p, cont)) == -1){
		aux = errno;
		close(df);
		errno = aux;
		return -1;
	}
	close(df);
	return n;
}

void cmd_write(int word_num, char *words[])
{
	void *p;
	size_t cont = -1;  /* -1 indicates reading the entire file*/
	ssize_t n;
	int o = 0;
	if (word_num < 4){
		printf ("Wrong number of arguments\n");
		return;
	}
	if (!strcmp(words[1], "-o"))
		o = 1;
	p = (void *)strtoul(words[2 + o], NULL, 16);  /*convert from string to pointer*/
	if (words[3 + o] != NULL)
		cont = (size_t)atoll(words[3 + o]);
	if ((n = write_file(words[1 + o], p, cont, o)) == -1)
		perror ("Impossible to write to file");
	else
		printf ("Writing %lld bytes from %p into %s\n",(long long)n, p, words[1 + o]);
}

void cmd_mem(char *words[], mem_list *list)
{
	if (words[1] == NULL || !strcmp(words[1], "-all"))
	{
		print_vars();
		print_functions();
		print_mem_list(list);
	}
	else if (!strcmp(words[1], "-blocks"))
		print_mem_list(list);
	else if (!strcmp(words[1], "-vars"))
		print_vars();
	else if (!strcmp(words[1], "-funcs"))
		print_functions();
	else if (!strcmp(words[1], "-pmap"))
		do_mem_pmap();
}

int d1;
char d2;
long d3;

int e1 = 0;
char e2 = 'a';
long e3 = 0;

void print_vars()
{
	int a1;
	char a2;
	long a3;

	static int b1 = 0;
	static char b2 = 'a';
	static long b3 = 1;

	static int c1;
	static char c2;
	static long c3;

	printf("Local variables          %16p, %16p, %16p\n", &a1, &a2, &a3);
	printf("Global variables         %16p, %16p, %16p\n", &e1, &e2, &e3);
	printf("Global N.I. variables    %16p, %16p, %16p\n", &d1, &d2, &d3);
	printf("Static variables         %16p, %16p, %16p\n", &b1, &b2, &b3);
	printf("Static N.I. variables    %16p, %16p, %16p\n", &c1, &c2, &c3);

}

void print_functions()
{
	printf("Functions of the program %16p, %16p, %16p\n", (void *)cmd_read, (void *)cmd_write, (void *)cmd_mem);
	printf("Library functions        %16p, %16p, %16p\n", (void *)shmat, (void *)shmdt, (void *)shmget);
}

void do_mem_pmap(void) /*sin argumentos*/
 { 
	pid_t pid;	   /*hace el pmap (o equivalente) del proceso actual*/
	char elpid[32];
	char *argv[4]={"pmap",elpid,NULL};

	sprintf (elpid,"%d", (int) getpid());
	if ((pid = fork()) == -1){
		perror ("Imposible crear proceso");
		return;
	}	
	if (pid==0){ /*proceso hijo*/
		if (execvp(argv[0],argv)==-1)
			perror("cannot execute pmap (linux, solaris)");
		
		argv[0]="vmmap"; argv[1]="-interleave"; argv[2]=elpid;argv[3]=NULL;
		if (execvp(argv[0],argv)==-1) /*probamos vmmap Mac-OS*/
			perror("cannot execute vmmap (Mac-OS)");		  
		
		argv[0]="procstat"; argv[1]="vm"; argv[2]=elpid; argv[3]=NULL;   
		if (execvp(argv[0],argv)==-1)/*No hay pmap, probamos procstat FreeBSD */
			perror("cannot execute procstat (FreeBSD)");
			
		argv[0]="procmap",argv[1]=elpid;argv[2]=NULL;	
			if (execvp(argv[0],argv)==-1)  /*probamos procmap OpenBSD*/
			perror("cannot execute procmap (OpenBSD)");
			
		exit(1);
	}
	waitpid (pid,NULL,0);
}