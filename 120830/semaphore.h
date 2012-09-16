#include <sys/sem.h>
#include <sys/ipc.h>

#ifndef SEMAPHORE
#define SEMAPHORE

int creasem(int); // int cant
int creasem(int, int); // int cant
int getsem(int);
int getsem(int, int);
int inisem(int, int); // int num
int inisem(int, int, int); // int num
int p(int); // int num
int p(int, int); // int num
int v(int); // int num
int v(int, int); // int num
int elisem(int);

/*
 *	  Funciones de semaforos
 *
 *		crear el set de semaforos (si no existe)
 */
int creasem(int identif)
{
	return creasem(identif, 1);
}
int creasem(int identif, int cantidad)
{
    key_t clave;
    clave = ftok(FTOK_DIR, identif);
    return( semget(clave, cantidad, IPC_CREAT | IPC_EXCL | 0660));  /* da error si ya existe */
}
/*
 *	  	adquirir derecho de acceso al set de semaforos existentes
 */
int getsem(int identif)
{
	return getsem(identif, 1);
}
int getsem(int identif, int num)
{
    key_t clave;
    clave = ftok(FTOK_DIR, identif);
    return( semget(clave, num, 0660));
}
/*
 *		inicializar al semaforo del set de semaforos
 */
int inisem(int semid, int val)
{
	return inisem(semid, val, 0);
}

int inisem(int semid, int val, int sem_num)
{
	union semun {
		int              val;		/* Value for SETVAL */
		struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
		unsigned short  *array;  /* Array for GETALL, SETALL */
		struct seminfo  *__buf;  /* Buffer for IPC_INFO
								  (Linux specific) */
	} arg;
	arg.val = val;
	return( semctl(semid, sem_num, SETVAL, arg));
}
/*
 *		ocupar al semaforo  (p) WAIT
 */
int p(int semid) {
	return p(semid, 0);
}
int p(int semid, int sem_num)
{
	struct sembuf oper;
	oper.sem_num = sem_num; 		/* nro de semaforo del set */
	oper.sem_op =  -1;		/* p(sem) */
	oper.sem_flg = 0;
	return (semop (semid, &oper, 1));
}
/*
 * 		liberar al semaforo  (v) SIGNAL
 */
int v(int semid) {
	return v(semid, 0);
}
int v(int semid, int sem_num)
{
	struct sembuf oper;
	oper.sem_num = sem_num; 		/* nro de semaforo */
	oper.sem_op = 1;			/* v(sem) */
	oper.sem_flg = 0;
	return (semop (semid, &oper, 1));
}
/*
 *		 eliminar el set de semaforos
 */
int elisem(int semid)
{
	return (semctl (semid, 0, IPC_RMID, (struct semid_ds *) 0));
}

#endif

