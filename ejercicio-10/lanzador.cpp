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
	int shmControl; 
	int semSet;	
	key_t clave;

	char* pname = argv[0];
	int  pid_pr = getpid(), childpid = 0;

	char mostrar[300];	/* mensaje para mostrar en pantalla */
	struct stat st; /* dir stat */

	static char param1[10], param2[10]; /* string parametros del execlp */
	CONTROL *shmem_control;
	int cantAEncontrarse = CANTIDAD_A_ENCONTRARSE;
	int tiempo;

	if(stat(FTOK_DIR,&st) != 0) {
		sprintf (mostrar,"%s (%d): Falta el directorio de referencia Cte FTOK_DIR: %s\n", pname, pid_pr, FTOK_DIR); 
		write(fileno(stdout), mostrar, strlen(mostrar));
		exit(1);
	}

	// Inicializacion de la SHM_CONTROL
	clave = ftok(FTOK_DIR,SHM_CONTROL);
	if ((shmControl = shmget (clave, sizeof(CONTROL), IPC_CREAT|IPC_EXCL|0660)) == -1) { 
		perror ("Lanzador: error al crear la shared memory "); 
		exit (1);
	}
	
	if ((shmem_control = (CONTROL *) shmat(shmControl,0,0)) == (CONTROL *) -1 ) { 
		perror ("Lanzador: error en el attach a shared memory "); 
		exit (1);
    }
    shmem_control->enLugarDeEncuentro = 0;

	/* 
	 *	 crear e inicializar el IPC semaforo mutex 
	 */	 
	if ((semSet = creasem (SEM_SET, cantAEncontrarse + SEM_CANT_EXTRA)) == -1) 	/* IPC para exclusion mutua */{   
		perror ("Lanzador: error al crear el (IPC) semaforo mutex shared memory"); 
		exit (1);
    }

	for(int i = 0; i++; i < cantAEncontrarse) 
		if (inisem(semSet, 0, i) < 0) {
			perror("Lanzador: no pudo iniciar semaforo de encuentro");
			exit(1);
		};
	sprintf (mostrar,"%s (%d): sems de encuentro\n", pname, pid_pr); 
	write(fileno(stdout), mostrar, strlen(mostrar));
	inisem(semSet, 1, cantAEncontrarse + SEM_MUTEX_OFFSET);/* inicializarlo */
	sprintf (mostrar,"%s (%d): sems de mutex\n", pname, pid_pr); 
	write(fileno(stdout), mostrar, strlen(mostrar));
	inisem(semSet, 0, cantAEncontrarse + SEM_RENDEZVOUS_OFFSET);/* inicializarlo */
	sprintf (mostrar,"%s (%d): sems de rendez\n", pname, pid_pr); 
	write(fileno(stdout), mostrar, strlen(mostrar));

	/*
	 * Crear procesos de encuentro
	 */
	for (int i = 0; i < cantAEncontrarse; i++) {
		sprintf(param1, "%d\n",i+1); /* pasarle el nro de encuentro */
		sprintf(param2, "%d\n", cantAEncontrarse); /* pasarle el nro de encuentro */

		if ( (childpid = fork()) < 0) { 
			perror("Lanzador: error en el fork para encuentro");
			exit(1);
		}
		else if (childpid == 0){	 
			/*
			 *	 	 PROCESO HIJO (child) Vendedor
			 */  	
			execlp("bin/encuentro", "encuentro", param1, param2, (char *)0);
			perror("Lanzador: error al lanzar el encuentro");
			exit(3);
		}
	}

	for(;;) {
		sprintf (mostrar,"%s (%d): Esperando nuevo encuentro \n", pname, pid_pr); 
		write(fileno(stdout), mostrar, strlen(mostrar));

		p(semSet, cantAEncontrarse + SEM_RENDEZVOUS_OFFSET);

		sprintf (mostrar,"%s (%d): Encuentro completo! \n", pname, pid_pr); 
		write(fileno(stdout), mostrar, strlen(mostrar));

		tiempo = rand() % 10;
		sleep(tiempo); 

		p(semSet, cantAEncontrarse + SEM_MUTEX_OFFSET);
		shmem_control->enLugarDeEncuentro = 0;
		v(semSet, cantAEncontrarse + SEM_MUTEX_OFFSET);


		for(int i = 0; i < cantAEncontrarse; i++) {
			sprintf (mostrar,"%s (%d): Reiniciando procesos de encuentro %d\n", pname, pid_pr, i); 
			write(fileno(stdout), mostrar, strlen(mostrar));

			if (v(semSet, i) < 0) {
				perror("No se pudo levantar el semaforo");
				exit(1);
			}
		}
	}
	return 0;
}