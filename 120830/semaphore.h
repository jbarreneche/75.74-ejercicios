#include "./constants.h"

int creasem(int); # int cant
int getsem(int);
int inisem(int, int); # int num
int p(int); # int num
int v(int); # int num
int elisem(int);

int creasem(int identif) { # Cant = 1
  key_t clave;
  clave = ftok(DIRECTORIO, identif);
  return (semget(clave, 1 , IPC_CREAT (pipe) IPC_EXCL (pipe) 0660));
}

int getsem(int identif) {
  key_t clave;
  clave = ftok(DIRECTORIO, identif);
  return (semget(clave, 1 , 0660));
}

int inisem(int semfd, int valor) {
  union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *_buf;
  } arg;
  arg.val = valor;
  return (semctl(semfd, 0, SETVAL, arg));
}
  
int p(int semfd) {
  struct sembuf oper;
  oper.sem_num = 0;
  oper.sem_op = -1; 
  oper.flag = 0;
  
  return (semop(semfd, &oper, 1));
}

int p(int semfd) {
  struct sembuf oper;
  oper.sem_num = 0;
  oper.sem_op = 1; 
  oper.flag = 0;

  return (semop(semfd, &oper, 1));
}

int elisem(int semfd) {
  return (semctl(semfd, 0, IPC_RMID, (struct semid_ds*)0));
}
