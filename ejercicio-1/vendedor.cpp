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
	char mostrar[120];	/* mensaje para mostrar en pantalla */
	char *pname;		/* nombre del programa */
	int pid_pr;
	key_t clave;

	int qCompras, qVentas; 
	int mtxShm;	
	int shmTickets; 
	TICKETS *shmem_tickets;
	PROTOCOLO envio, recepcion;
	COMPRA compra;
	TICKET resCompra;

	int salemanNumber, tiempo;

	pname = argv[0];
	pid_pr =getpid();

	if (!argc) {
		sprintf (mostrar, "Missing arguments"); 
		write(fileno(stdout), mostrar, strlen(mostrar));
		exit(1);
	}

	salemanNumber = atoi(argv[1]);
	sprintf (mostrar, "Launched vendedor %d\n", salemanNumber); 
	write(fileno(stdout), mostrar, strlen(mostrar));

	/*   
	 *	abrir la cola compras
	 */ 
	clave = ftok(FTOK_DIR, Q_COMPRAS);
	if ((qCompras = msgget (clave, 0660)) == -1) {  
		perror ("Vendedor: error al hacer el get de la cola Compras "); 
		exit (1);
	}

	/*   
	 *	abrir la cola ventas
	 */ 
	clave = ftok(FTOK_DIR, Q_VENTAS);
	if ((qVentas = msgget (clave, 0660)) == -1) {  
		perror ("Vendedor: error al hacer el get de la cola Ventas "); 
		exit (1);
	}
	// Inicializacion de la SHM_TICKETS
	clave = ftok(FTOK_DIR,SHM_TICKETS);
	if ((shmTickets = shmget (clave, sizeof(TICKETS), 0660)) == -1) { 
		perror ("Vendedor: error al crear la shared memory "); 
		exit (1);
	}
	if ((shmem_tickets = (TICKETS *) shmat(shmTickets,0,0)) == (TICKETS *) -1 ) { 
		perror ("Lanzador: error en el attach a shared memory "); 
		exit (1);
    }
	/* 
	 *	  Buscar mutex
	 */	 
	if ((mtxShm = getsem (MUTEX_SHM)) == -1) 	/* IPC para exclusion mutua */{   
		perror ("Vendedor: error al encontrar el (IPC) semaforo mutex shared memory"); 
		exit (1);
    }

	envio.origen = salemanNumber;

	for(;;) {
		sprintf (mostrar,"%s (%d): Esperando nuevo cliente\n", pname, pid_pr, recepcion.origen); 
		write(fileno(stdout), mostrar, strlen(mostrar));

		if (msgrcv(qCompras, (PROTOCOLO *)&recepcion, sizeof(recepcion) - sizeof(long), SOLICITAR_ATENCION, 0) == -1){ 
			if (errno == EINVAL || errno == EIDRM) {	/* verifica si se destruyeron los IPC */
				sprintf (mostrar,"%s (%d): TERMINA\n", pname, pid_pr); 
				write(fileno(stdout), mostrar, strlen(mostrar));
				exit(0);
			} else {  
				perror ("Vendedor: Error en la recepcion en cola compras");
				exit (1);
			}
		}

		sprintf (mostrar,"%s (%d): Atendiendo al cliente %d\n", pname, pid_pr, recepcion.origen); 
		write(fileno(stdout), mostrar, strlen(mostrar));

		envio.tipo = recepcion.origen;
		if (msgsnd(qVentas, (PROTOCOLO*)&envio, (sizeof(PROTOCOLO) - 4), 0) == -1){   
			perror ("Vendedor: Error en el envio cola ventas");
			exit(2);
		}

		do {
			sprintf (mostrar,"%s (%d): Esperando nuevo pedido de ticket del cliente %d\n", pname, pid_pr, recepcion.origen); 
			write(fileno(stdout), mostrar, strlen(mostrar));

			if (msgrcv(qCompras, (PROTOCOLO *)&recepcion, sizeof(recepcion) - sizeof(long), salemanNumber, 0) == -1){ 
				if (errno == EINVAL || errno == EIDRM) {	/* verifica si se destruyeron los IPC */
					sprintf (mostrar,"%s (%d): TERMINA\n", pname, pid_pr); 
					write(fileno(stdout), mostrar, strlen(mostrar));
					exit(0);
				} else {  
					perror ("Vendedor: Error en la recepcion en cola compras");
					exit (1);
				}
			}
			memcpy ((char *)&compra, recepcion.mensaje, sizeof(compra));

			sprintf (mostrar,"%s (%d): Atendiendo al cliente %d, pago con %d\n", pname, pid_pr, recepcion.origen, compra.monto); 
			write(fileno(stdout), mostrar, strlen(mostrar));
			// Acceder a SHM
			p(mtxShm);
			sprintf (mostrar,"%s (%d): Consultando disponibilidad de tickets\n", pname, pid_pr, recepcion.origen, compra.monto); 
			write(fileno(stdout), mostrar, strlen(mostrar));
			
			tiempo = rand() % 10;
			sleep(tiempo); 

			if (shmem_tickets->cantTickets == 0) {
				resCompra.numero = -1;
				resCompra.vuelto = compra.monto;
				resCompra.noHayMas = true;
			} else if (compra.monto < 100) {
				resCompra.numero = -1;
				resCompra.vuelto = compra.monto;
				resCompra.noHayMas = false;
			} else {
				resCompra.numero = shmem_tickets->cantTickets;
				shmem_tickets->cantTickets--;
				resCompra.vuelto = compra.monto - 100;
				resCompra.noHayMas = false;
			}
			v(mtxShm);

			sprintf (mostrar,"%s (%d): enviando... tipo %d orig %d\n", pname, pid_pr, envio.tipo, envio.origen); 
			memcpy (envio.mensaje, (char *)&resCompra, sizeof(resCompra));
			if (msgsnd(qVentas, (PROTOCOLO*)&envio, sizeof(envio) - sizeof(long), 0) == -1){   
				perror ("Vendedor: Error en el envio cola ventas");
				exit(2);
			}

		} while(compra.mas && resCompra.numero > 0);

	}
	return 0;

}
