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
	int qACKS; 
	key_t clave;

	char* pname = argv[0];
	int  pid_pr = getpid(), childpid = 0;

	char mostrar[300];	/* mensaje para mostrar en pantalla */
	struct stat st; /* dir stat */

	PROTOCOLO envio, recepcion;
	int cantAEncontrarse = CANTIDAD_A_ENCONTRARSE;
	int tiempo;

	static char param1[10], param2[10];

	if(stat(FTOK_DIR,&st) != 0) {
		sprintf (mostrar,"%s (%d): Falta el directorio de referencia Cte FTOK_DIR: %s\n", pname, pid_pr, FTOK_DIR); 
		write(fileno(stdout), mostrar, strlen(mostrar));
		exit(1);
	}

	/*   
	 *	crear la cola compras
	 */ 
	clave = ftok(FTOK_DIR, Q_ACKS);
	if ((qACKS = msgget (clave, IPC_CREAT | IPC_EXCL | 0660)) == -1) {  
		perror ("Lanzador: error al hacer el get de la cola Compras "); 
		exit (1);
	}


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

		for(int i = 0; i < cantAEncontrarse; i++) {
			if (msgrcv(qACKS, &recepcion, sizeof(envio) - sizeof(long), RENDEZVOUS, 0) == -1){   
				perror ("Rendezvous: Error en recepcion cola ACKS");
				exit(2);
			}
		}

		sprintf (mostrar,"%s (%d): Encuentro completo! \n", pname, pid_pr); 
		write(fileno(stdout), mostrar, strlen(mostrar));

		tiempo = rand() % 10;
		sleep(tiempo); 

		for(int i = 0; i < cantAEncontrarse; i++) {
			sprintf (mostrar,"%s (%d): Reiniciando procesos de encuentro %d\n", pname, pid_pr, i); 
			write(fileno(stdout), mostrar, strlen(mostrar));

			envio.tipo = i + 1;
			if (msgsnd(qACKS, &envio, sizeof(envio) - sizeof(long), 0) == -1){   
				perror ("Rendezvous: Error en el envio cola ACKS");
				exit(2);
			}

		}
	}
	return 0;
}