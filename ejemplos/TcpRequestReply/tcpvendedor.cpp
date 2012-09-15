/*
 * Programa: Ejemplo de server CONCURRENTE usando protocolo TCP (tcpservc).
 *                    Programa que atiende a un cliente en particular (tcpecos).
 * Descripcion:	
 *		Recibir un mensaje
 *		Devolver el mensaje al cliente anteponiendo la hora de recepci�n
 *	
 * Usa:		Procedimiento tcpservertrans().
 */

#include	"inet.h"
extern void tcpservertrans(int, char *, pid_t, int);

int main(int argc, char *argv[])

{
 int newsockfd;		/* socket conectado al cliente */
 pid_t pid;	        /* pid del eco que atiende al cliente */
 char mostrar[120];	/* mensaje para mostrar en pantalla */
 char *pname;		/* nombre del programa */
 int port;		/* port de conexion */
 int nro;
                      
 pname = argv[0];
/*
 *		Verificar los parametros
 */
 if (argc > 2) {
	newsockfd = atoi(argv[1]);	/* convertir port ingresado a binario */
	 nro = atoi(argv[2]);
 }
 else {
	 perror("tcpventa: faltas los parametros de invocacion");
	 exit(1);
	 }
  pid = getpid();
  sprintf (mostrar,"%s (%d) %d: atiendo al cliente en el descriptor %d\n ", pname, pid, newsockfd, nro);
  write(fileno(stdout), mostrar, strlen(mostrar));  
  tcpservertrans(newsockfd, pname, pid, nro);	/* procesar las compras del cliente */
  close(newsockfd);
  exit(0);
}
