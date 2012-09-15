#include "constantes.h"
#include "lib/semaphore.h"
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
// #include <sys/resource.h>

int main(int argc, char *argv[]) {
	int shmid; //, compras1, compras2, despacho1, despacho2;
	char mostrar[120];	/* mensaje para mostrar en pantalla */
	char *pname;		/* nombre del programa */
	int pid_pr, childpid;
	int mutex;			/* file descriptor de los IPCs */
	key_t clave;

	TICKETS *shmem_tickets;
	int cantVendedores = NUM_VENDEDORES, cantClientes = NUM_CLIENTES;
	static char param1[10]; /* string parametros del execlp */

	pname = argv[0];
	pid_pr =getpid();
	struct stat st; /* dir stat */

	// static char param1[10] param2[10], param3[10]; /* string parametros del execlp */
	if(stat(FTOK_DIR,&st) != 0) {
		sprintf (mostrar,"%s (%d): Falta el directorio de referencia Cte FTOK_DIR: %s\n", pname, pid_pr, FTOK_DIR); 
		write(fileno(stdout), mostrar, strlen(mostrar));
		exit(1);
	}

	// Inicializacion de la SHM
	clave = ftok(FTOK_DIR,SHM);
	if ((shmid = shmget (clave, sizeof(TICKETS), IPC_CREAT|IPC_EXCL|0660)) == -1) { 
		perror ("TcpServerConcurrente: error al crear la shared memory "); 
		exit (1);
	}
	
	if ((shmem_tickets = (TICKETS *) shmat(shmid,0,0)) == (TICKETS *) -1 ) { 
		perror ("TcpServerConcurrente: error en el attach a shared memory "); 
		exit (1);
    }

	/* 
	 *	 crear e inicializar el IPC semaforo mutex 
	 */	 
	if ((mutex = creasem (MUTEX)) == -1) 	/* IPC para exclusion mutua */{   
		perror ("TcpServerConcurrente: error al crear el (IPC) semaforo mutex"); 
		exit (1);
    }
	inisem (mutex, 1);			/* inicializarlo */

	/*
	 * Crear los vendedores 
	 */
	for (int i = 0; i < cantVendedores; i++) {
		sprintf(param1, "%d\n",i+1); /* pasarle el nro de vendedor */

		if ( (childpid = fork()) < 0) { 
			perror("TcpServerConcurrente: error en el fork para TcpEjemploVendedor");
			exit(1);
		}
		else if (childpid == 0){	 
			/*
			 *	 	 PROCESO HIJO (child) Vendedor
			 */  	
			execlp("./vendedor", "vendedor", param1, (char *)0);
			perror("TcpServerConcurrente: error al lanzar el vendedor");
			exit(3);
		}
	}

	/*
	 * Crear los vendedores 
	 */
	for (int i = 0; i < cantClientes; i++) {
		sprintf(param1, "%d\n",i+1); /* pasarle el nro de vendedor */

		if ( (childpid = fork()) < 0) { 
			perror("TcpServerConcurrente: error en el fork para TcpEjemploVendedor");
			exit(1);
		}
		else if (childpid == 0){	 
			/*
			 *	 	 PROCESO HIJO (child) Vendedor
			 */  	
			execlp("./cliente", "cliente", param1, (char *)0);
			perror("TcpServerConcurrente: error al lanzar el cliente");
			exit(3);
		}
	}

	return 0;
}