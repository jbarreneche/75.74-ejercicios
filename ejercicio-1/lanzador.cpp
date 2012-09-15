#include "constantes.h"
#include "lib/semaphore.h"

#include <errno.h>
#include <limits.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
	int shmTickets; 
	int qCompras, qVentas; 
	int mtxShm;	
	key_t clave;

	char* pname = argv[0];
	int  pid_pr = getpid(), childpid = 0;

	char mostrar[300];	/* mensaje para mostrar en pantalla */
	struct stat st; /* dir stat */

	static char param1[10]; /* string parametros del execlp */
	TICKETS *shmem_tickets;
	int cantVendedores = NUM_VENDEDORES, cantClientes = NUM_CLIENTES;

	if(stat(FTOK_DIR,&st) != 0) {
		sprintf (mostrar,"%s (%d): Falta el directorio de referencia Cte FTOK_DIR: %s\n", pname, pid_pr, FTOK_DIR); 
		write(fileno(stdout), mostrar, strlen(mostrar));
		exit(1);
	}

	// Inicializacion de la SHM_TICKETS
	clave = ftok(FTOK_DIR,SHM_TICKETS);
	if ((shmTickets = shmget (clave, sizeof(TICKETS), IPC_CREAT|IPC_EXCL|0660)) == -1) { 
		perror ("Lanzador: error al crear la shared memory "); 
		exit (1);
	}
	
	if ((shmem_tickets = (TICKETS *) shmat(shmTickets,0,0)) == (TICKETS *) -1 ) { 
		perror ("Lanzador: error en el attach a shared memory "); 
		exit (1);
    }
    shmem_tickets->cantTickets = 5;

	/* 
	 *	 crear e inicializar el IPC semaforo mutex 
	 */	 
	if ((mtxShm = creasem (MUTEX_SHM)) == -1) 	/* IPC para exclusion mutua */{   
		perror ("Lanzador: error al crear el (IPC) semaforo mutex shared memory"); 
		exit (1);
    }
	inisem (mtxShm, 1);			/* inicializarlo */

	/*   
	 *	crear la cola compras
	 */ 
	clave = ftok(FTOK_DIR, Q_COMPRAS);
	if ((qCompras = msgget (clave, IPC_CREAT | IPC_EXCL | 0660)) == -1) {  
		perror ("Lanzador: error al hacer el get de la cola Compras "); 
		exit (1);
	}

	/*   
	 *	crear la cola ventas
	 */ 
	clave = ftok(FTOK_DIR, Q_VENTAS);
	if ((qVentas = msgget (clave, IPC_CREAT | IPC_EXCL | 0660)) == -1) {  
		perror ("Lanzador: error al hacer el get de la cola Ventas "); 
		exit (1);
	}

	/*
	 * Crear los vendedores 
	 */
	for (int i = 0; i < cantVendedores; i++) {
		sprintf(param1, "%d\n",i+1); /* pasarle el nro de vendedor */

		if ( (childpid = fork()) < 0) { 
			perror("Lanzador: error en el fork para vendedor");
			exit(1);
		}
		else if (childpid == 0){	 
			/*
			 *	 	 PROCESO HIJO (child) Vendedor
			 */  	
			execlp("bin/vendedor", "vendedor", param1, (char *)0);
			perror("Lanzador: error al lanzar el vendedor");
			exit(3);
		}
	}

	/*
	 * Crear los Clientes
	 */
	for (int i = 0; i < cantClientes; i++) {
		sprintf(param1, "%d\n",i+1); /* pasarle el nro de vendedor */

		if ( (childpid = fork()) < 0) { 
			perror("Lanzador: error en el fork para cliente");
			exit(1);
		}
		else if (childpid == 0){	 
			/*
			 *	 	 PROCESO HIJO (child) Cliente
			 */  	
			execlp("bin/cliente", "cliente", param1, (char *)0);
			perror("Lanzador: error al lanzar el cliente");
			exit(3);
		}
	}

	return 0;
}