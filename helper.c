//helper code

// #include <unistd.h>
// /*las funciones entre puntos hay que implementarlas */

// void Cmd_open (char * tr[])
// {
//     int i,df, mode=0;
    
//     if (tr[0]==NULL) { /*no hay parametro*/
//       // ..............ListarFicherosAbiertos...............
//         return;
//     }
//     for (i=1; tr[i]!=NULL; i++)
//       if (!strcmp(tr[i],"cr")) mode|=O_CREAT;
//       else if (!strcmp(tr[i],"ex")) mode|=O_EXCL;
//       else if (!strcmp(tr[i],"ro")) mode|=O_RDONLY; 
//       else if (!strcmp(tr[i],"wo")) mode|=O_WRONLY;
//       else if (!strcmp(tr[i],"rw")) mode|=O_RDWR;
//       else if (!strcmp(tr[i],"ap")) mode|=O_APPEND;
//       else if (!strcmp(tr[i],"tr")) mode|=O_TRUNC; 
//       else break;
      
//     if ((df=open(tr[0],mode,0777))==-1)
//         perror ("Imposible abrir fichero");
//     else{
//         //...........AnadirAFicherosAbiertos (descriptor...modo...nombre....)....
//         printf ("Anadida entrada a la tabla ficheros abiertos..................",......);
// }

// void Cmd_close (char *tr[])
// { 
//     int df;
    
//     if (tr[0]==NULL || (df=atoi(tr[0]))<0) { /*no hay parametro*/
//       //..............ListarFicherosAbiertos............... /*o el descriptor es menor que 0*/
//         return;
//     }

    
//     if (close(df)==-1)
//         perror("Inposible cerrar descriptor");
//     //else'' ........EliminarDeFicherosAbiertos......
// }

// void Cmd_dup (char * tr[])
// { 
//     int df, duplicado;
//     char aux[MAXNAME],*p;
    
//     if (tr[0]==NULL || (df=atoi(tr[0]))<0) { /*no hay parametro*/
//         ListOpenFiles(-1);                 /*o el descriptor es menor que 0*/
//         return;
//     }
    
 
//     p=.....NombreFicheroDescriptor(df).......;
//     sprintf (aux,"dup %d (%s)",df, p);
//     .......AnadirAFicherosAbiertos......duplicado......aux.....fcntl(duplicado,F_GETFL).....;
// } 


// char LetraTF (mode_t m)
// {
//      switch (m&S_IFMT) { /*and bit a bit con los bits de formato,0170000 */
//         case S_IFSOCK: return 's'; /*socket */
//         case S_IFLNK: return 'l'; /*symbolic link*/
//         case S_IFREG: return '-'; /* fichero normal*/
//         case S_IFBLK: return 'b'; /*block device*/
//         case S_IFDIR: return 'd'; /*directorio */ 
//         case S_IFCHR: return 'c'; /*char device*/
//         case S_IFIFO: return 'p'; /*pipe*/
//         default: return '?'; /*desconocido, no deberia aparecer*/
//      }
// }
// /*las siguientes funciones devuelven los permisos de un fichero en formato rwx----*/
// /*a partir del campo st_mode de la estructura stat */
// /*las tres son correctas pero usan distintas estrategias de asignaciÃ³n de memoria*/

// char * ConvierteModo (mode_t m, char *permisos)
// {
//     strcpy (permisos,"---------- ");
    
//     permisos[0]=LetraTF(m);
//     if (m&S_IRUSR) permisos[1]='r';    /*propietario*/
//     if (m&S_IWUSR) permisos[2]='w';
//     if (m&S_IXUSR) permisos[3]='x';
//     if (m&S_IRGRP) permisos[4]='r';    /*grupo*/
//     if (m&S_IWGRP) permisos[5]='w';
//     if (m&S_IXGRP) permisos[6]='x';
//     if (m&S_IROTH) permisos[7]='r';    /*resto*/
//     if (m&S_IWOTH) permisos[8]='w';
//     if (m&S_IXOTH) permisos[9]='x';
//     if (m&S_ISUID) permisos[3]='s';    /*setuid, setgid y stickybit*/
//     if (m&S_ISGID) permisos[6]='s';
//     if (m&S_ISVTX) permisos[9]='t';
    
//     return permisos;
// }


// char * ConvierteModo2 (mode_t m)
// {
//     static char permisos[12];
//     strcpy (permisos,"---------- ");
    
//     permisos[0]=LetraTF(m);
//     if (m&S_IRUSR) permisos[1]='r';    /*propietario*/
//     if (m&S_IWUSR) permisos[2]='w';
//     if (m&S_IXUSR) permisos[3]='x';
//     if (m&S_IRGRP) permisos[4]='r';    /*grupo*/
//     if (m&S_IWGRP) permisos[5]='w';
//     if (m&S_IXGRP) permisos[6]='x';
//     if (m&S_IROTH) permisos[7]='r';    /*resto*/
//     if (m&S_IWOTH) permisos[8]='w';
//     if (m&S_IXOTH) permisos[9]='x';
//     if (m&S_ISUID) permisos[3]='s';    /*setuid, setgid y stickybit*/
//     if (m&S_ISGID) permisos[6]='s';
//     if (m&S_ISVTX) permisos[9]='t';
    
//     return permisos;
// }

// char * ConvierteModo3 (mode_t m)
// {
//     char *permisos;

//     if ((permisos=(char *) malloc (12))==NULL)
//         return NULL;
//     strcpy (permisos,"---------- ");
    
//     permisos[0]=LetraTF(m);
//     if (m&S_IRUSR) permisos[1]='r';    /*propietario*/
//     if (m&S_IWUSR) permisos[2]='w';
//     if (m&S_IXUSR) permisos[3]='x';
//     if (m&S_IRGRP) permisos[4]='r';    /*grupo*/
//     if (m&S_IWGRP) permisos[5]='w';
//     if (m&S_IXGRP) permisos[6]='x';
//     if (m&S_IROTH) permisos[7]='r';    /*resto*/
//     if (m&S_IWOTH) permisos[8]='w';
//     if (m&S_IXOTH) permisos[9]='x';
//     if (m&S_ISUID) permisos[3]='s';    /*setuid, setgid y stickybit*/
//     if (m&S_ISGID) permisos[6]='s';
//     if (m&S_ISVTX) permisos[9]='t';
    
//     return permisos;
// } 