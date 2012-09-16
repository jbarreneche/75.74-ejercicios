#include "constantes.h"
#include "lib/semaphore.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <errno.h>

int main(int argc, char *argv[]) {
	int shmControl; 
	int semSet;	
	key_t clave;
	CONTROL *shmem_control;

	int procNumber, cantAEncontrarse, tiempo;
	char mostrar[120];	/* mensaje para mostrar en pantalla */

	char* pname = argv[0];
	int  pid_pr = getpid(), childpid = 0;

	if (argc < 3) {
		sprintf (mostrar, "Missing arguments"); 
		write(fileno(stdout), mostrar, strlen(mostrar));
		exit(1);
	}

	procNumber = atoi(argv[1]);
	cantAEncontrarse = atoi(argv[2]);
	sprintf (mostrar,"%s-%d (%d): Iniciando proceso encuentro %d \n", pname, procNumber, pid_pr, procNumber); 
	write(fileno(stdout), mostrar, strlen(mostrar));

	// Inicializacion de la SHM_CONTROL
	clave = ftok(FTOK_DIR,SHM_CONTROL);
	if ((shmControl = shmget (clave, sizeof(CONTROL), 0660)) == -1) { 
		perror ("Vendedor: error al crear la shared memory "); 
		exit (1);
	}
	if ((shmem_control = (CONTROL *) shmat(shmControl,0,0)) == (CONTROL *) -1 ) { 
		perror ("Lanzador: error en el attach a shared memory "); 
		exit (1);
    }
	/* 
	 *	  Buscar mutex
	 */	 
	if ((semSet = getsem (SEM_SET, cantAEncontrarse + SEM_CANT_EXTRA)) == -1) 	/* IPC para exclusion mutua */{   
		perror ("Vendedor: error al encontrar el (IPC) semaforo mutex shared memory"); 
		exit (1);
    }

	for(;;) {
		sprintf (mostrar,"%s-%d/%d (%d): Iniciando travesia hacia el lugar de encuentro\n", pname, procNumber, cantAEncontrarse, pid_pr); 
		write(fileno(stdout), mostrar, strlen(mostrar));

		tiempo = rand() % 10;
		sleep(tiempo);

		p(semSet, cantAEncontrarse + SEM_MUTEX_OFFSET);

		if (++shmem_control->enLugarDeEncuentro == cantAEncontrarse) {
			sprintf (mostrar,"%s-%d (%d): Ultimo en llegar exitosamente al lugar de encuentro\n", pname, procNumber, pid_pr); 
			write(fileno(stdout), mostrar, strlen(mostrar));

			v(semSet, cantAEncontrarse + SEM_RENDEZVOUS_OFFSET);
		} else {
			sprintf (mostrar,"%s-%d (%d): Soy el %d en llegar al lugar de encuentro\n", pname, procNumber, pid_pr, shmem_control->enLugarDeEncuentro); 
			write(fileno(stdout), mostrar, strlen(mostrar));
		}

		v(semSet, cantAEncontrarse + SEM_MUTEX_OFFSET);

		sprintf (mostrar,"%s-%d (%d): Esperando se reinicie una travesia %d\n", pname, procNumber, pid_pr, procNumber - 1); 
		write(fileno(stdout), mostrar, strlen(mostrar));

		if (p(semSet, procNumber - 1) < 0) {
			perror("Encuentro: No se pudo esperar en mi semaforo");
			exit(1);
		};
	}

	return 0;

}
