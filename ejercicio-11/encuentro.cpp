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
	int qACKS; 
	key_t clave;

	PROTOCOLO envio, recepcion;
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

	/*   
	 *	abrir la cola ACKS
	 */ 
	clave = ftok(FTOK_DIR, Q_ACKS);
	if ((qACKS = msgget (clave, 0660)) == -1) {  
		perror ("Cliente: error al hacer el get de la cola Compras "); 
		exit (1);
	}

	for(;;) {
		sprintf (mostrar,"%s-%d/%d (%d): Iniciando travesia hacia el lugar de encuentro\n", pname, procNumber, cantAEncontrarse, pid_pr); 
		write(fileno(stdout), mostrar, strlen(mostrar));

		tiempo = rand() % 10;
		sleep(tiempo);

		envio.tipo = RENDEZVOUS;
		if (msgsnd(qACKS, &envio, sizeof(envio) - sizeof(long), 0) == -1){   
			perror ("Cliente: Error en el envio cola ACKS");
			exit(2);
		}

		sprintf (mostrar,"%s-%d (%d): Esperando se reinicie una travesia %d\n", pname, procNumber, pid_pr, procNumber - 1); 
		write(fileno(stdout), mostrar, strlen(mostrar));

		if (msgrcv(qACKS, &recepcion, sizeof(envio) - sizeof(long), procNumber, 0) == -1){   
			perror ("Cliente: Error en recepcion cola ACKS");
			exit(2);
		}
	}

	return 0;

}
