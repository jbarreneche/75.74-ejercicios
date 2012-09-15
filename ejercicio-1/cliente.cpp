#include "constantes.h"
#include "lib/semaphore.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/msg.h>
#include <errno.h>

int main(int argc, char *argv[]) {
	char mostrar[120];	/* mensaje para mostrar en pantalla */
	key_t clave;
	char* pname = argv[0];
	int  pid_pr = getpid(), childpid = 0;
	
	int qCompras, qVentas; 
	PROTOCOLO envio, recepcion;
	COMPRA compra;
	TICKET resCompra;

	int clientNumber;
	char archivo[15];  /* contiene los productos a vender */

	FILE* 	fp;					/* archivo y estructura de compras */
	char	primeraLinea[80];

	if (!argc) {
		sprintf (mostrar, "Missing arguments"); 
		write(fileno(stdout), mostrar, strlen(mostrar));
		exit(1);
	}

	clientNumber = atoi(argv[1]);
	sprintf (mostrar, "Launched client %d\n", clientNumber); 
	write(fileno(stdout), mostrar, strlen(mostrar));

	sprintf(archivo, "cliente_%d.txt", clientNumber);
	if( ( fp = fopen(archivo,"r") ) == NULL) {  
		sprintf (mostrar,"%s (%d): NO se puede abrir el archivo con las compras para el cliente %s\n", pname, pid_pr, archivo); 
		write(fileno(stdout), mostrar, strlen(mostrar));
		exit(1);
	}
	/*   
	 *	abrir la cola compras
	 */ 
	clave = ftok(FTOK_DIR, Q_COMPRAS);
	if ((qCompras = msgget (clave, 0660)) == -1) {  
		perror ("Cliente: error al hacer el get de la cola Compras "); 
		exit (1);
	}

	/*   
	 *	abrir la cola ventas
	 */ 
	clave = ftok(FTOK_DIR, Q_VENTAS);
	if ((qVentas = msgget (clave, 0660)) == -1) {  
		perror ("Cliente: error al hacer el get de la cola Ventas "); 
		exit (1);
	}

	envio.origen = clientNumber;

	fgets(primeraLinea,80,fp);			/* Ignorar la primera linea. */

	int cantidad;
	while(!feof(fp)){   /* descargar todo el archivo */
		fscanf(fp,"%d %d", &cantidad, &compra.monto);

		sprintf (mostrar,"%s (%d): cantidad a comprar: %d dinero disponible %d\n", pname, pid_pr, cantidad, compra.monto); 
		write(fileno(stdout), mostrar, strlen(mostrar));

		// Solicitar vendedor
		envio.tipo = SOLICITAR_ATENCION;
		if (msgsnd(qCompras, (PROTOCOLO*)&envio, sizeof(envio), 0) == -1){   
			perror ("Cliente: Error en el envio cola compras");
			exit(2);
		}

		if (msgrcv(qVentas, (PROTOCOLO *)&recepcion, sizeof(recepcion), clientNumber, 0) == -1){ 
			if (errno == EINVAL || errno == EIDRM) {	/* verifica si se destruyeron los IPC */
				sprintf (mostrar,"%s (%d): TERMINA\n", pname, pid_pr); 
				write(fileno(stdout), mostrar, strlen(mostrar));
				exit(0);
			} else {  
				perror ("Cliente: Error en la recepcion en cola envio ");
				exit (1);
			}
		}

		do {
			cantidad--;
			compra.mas = !cantidad;

			if (resCompra.noHayMas) {
				sprintf (mostrar,"%s (%d): No hay mas tickets\n", pname, pid_pr); 
				write(fileno(stdout), mostrar, strlen(mostrar));
				exit(0);
			}

			if (resCompra.numero > 0) {
				sprintf (mostrar,"%s (%d): Adquiri el ticket: %d y me sobra %d\n", pname, pid_pr, resCompra.numero, resCompra.vuelto); 
				write(fileno(stdout), mostrar, strlen(mostrar));
			} else {
				sprintf (mostrar,"%s (%d): No pude adquirir el ticket\n", pname, pid_pr, resCompra.numero, resCompra.vuelto); 
				write(fileno(stdout), mostrar, strlen(mostrar));
			}
			compra.monto = resCompra.vuelto;
		} while(cantidad > 0 && resCompra.numero > 0);
	}
	
	fclose(fp);

	return 0;
}
