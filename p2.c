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
            {
                remove_block(mem_blocks, (size_t)n, 'm');
                printf("Deallocated %ld bytes\n", n);
            }
        }
        else
            printf("Invalid size\n");
        printf("\n");
        print_mem_list(mem_blocks);
        //print_malloc_blocks(mem_blocks);  //the ones that have type 'm'
        return;
    }
    printf("Memory blocks for process %d\n", getpid());
}

void *getMemoryShmget(key_t key, size_t size, mem_list *mem_blocks)
{
    void * p;
    int aux,id,flags=0777;
    struct shmid_ds s;

    if (size)     /*tam distito de 0 indica crear */
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
void sharedCreate(char *words[], mem_list *mem_blocks)
{
    key_t key;
    size_t size;
    void *p;

    // printf("Creating shared memory\n");
    if (words[3]==NULL) {
        print_mem_list(mem_blocks); //change later to print only the shared memory
        return;
    }
    // printf("Not returned\n");
    key=(key_t)  strtoul(words[2],NULL,10);
    size=(size_t) strtoul(words[3],NULL,10);
    if (size == 0) {
        printf ("No 0-byte blocks allocated\n");
        return;
    }
    if ((p = getMemoryShmget(key,size, mem_blocks)) != NULL)
    {
        printf("Assigned %lu bytes in %p\n",(unsigned long) size, p);
        print_mem_list(mem_blocks);             //remove later
    }
    else
        printf("Unable to allocate shared memory with key %lu: %s\n", (unsigned long) key, strerror(errno));
}

void cmd_shared(int word_num, char *words[], mem_list *mem_blocks)
{
    if (word_num < 3){
        print_mem_list(mem_blocks); //change later to print only the shared memory
        return;
    }
    if (!strcmp(words[1], "-create"))
        sharedCreate(words, mem_blocks);
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

    printf("protection&PROT_WRITE %d\n", protection&PROT_WRITE);
    if (protection&PROT_WRITE)
        modo = O_RDWR;
    printf("Modoooo %d\n", modo);
    if (stat(file, &s) == -1 || (df = open(file, modo)) == -1)
        return NULL;
    printf("s.st_size %lld\n", s.st_size);
    if ((p = mmap (NULL, s.st_size, protection, map, df, 0)) == MAP_FAILED){
        close(df);
        return NULL;
    }
    insert_mmap_block(list, p, s.st_size, file, s.st_size);
    return p;
}

void free_mmap(char *words[], mem_list *list)
{
    // if (words[2] == NULL) {
    //     print_mem_list(list);
    //     return;
    // }
    // mem_block *tmp = 
    // if (munmap(words[2], s.st_size) == -1) {
    //     perror("Error unmapping file");
    //     return;
    // }
    // printf("File unmapped sucessfully");
    // remove_mmap_block(tmp, list);        //Don't forget to close it and deallocate the file name
}

void cmd_mmap(char *arg[], mem_list *list)
{ 
     char *perm;
     void *p;
     int protection = 0;
     
    if (arg[1] == NULL)
        {print_mmap_blocks(list); return;}                   //print the mapped list
    if (!strcmp(arg[1], "-free"))
    {
         free_mmap(arg, list);
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

  printf ("parametro:%3d(%p) array %p, arr estatico %p\n",n,&n,automatico, estatico);

  if (n>0)
    recursive(n-1);
}

void fill_memory(void *p, size_t cont, unsigned char byte)
{
  unsigned char *arr=(unsigned char *) p;
  size_t i;

  for (i=0; i<cont;i++)
		arr[i]=byte;
}