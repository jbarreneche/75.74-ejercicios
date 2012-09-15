/*
 * Programa: Ejemplo de server iTERATIVO usando protocolo TCP (tcpservi).
 * Descripcion:
 *	Hacer un open pasivo (tcpoppas)
 *	Esperar por un cliente 
 *	Por cada cliente:
 *		Recibir un pedido
 *		Responder al pedido
 *	Para terminarlo (kill -9 nro de proceso)
 * Sintaxis: tcpserv [port]
 *	port: 	port sobre el cual atiende este servidor
 *	Si no se ingresa nro de port: se usa el PORT_FIJO (ver inet.h)
 * Usa:	Funcion  tcpoppas() 
 *		Procedimiento tcpservertrans().
 */

#include	"inet.h"
extern void tcpservertrans(int, char *, pid_t, int);
extern int tcpoppas(int);

int main(int argc, char *argv[])
{
 int			sockfd,		/* socket que sirve como template */ 
			newsockfd;	/* socket con el cliente */
 unsigned		clilen,		/* longitud dir. cliente */
			childpid;	        /* nro proceso hijo */
 struct sockaddr_in	cli_addr;
 char mostrar[80];                      /* mensaje para pantalla */
 pid_t pid_si;                                /* pid del server iterativo */
 char	*pname;         /* nombre del programa */
 int 	port; /* port de conexion */
 int nro = 1;

 pname = argv[0];
 pid_si = getpid();

 /*			INICIALIZACION DEL SERVER
  * 		Crear un socket para TCP (un Internet stream socket).
  */
/*
 *		Verificar si se informo un port o se debe usar el default
 */
 if (argc > 1)
	port = atoi(argv[1]);	/* convertir port ingresado a binario */
 else
	port = PORT_FIJO;	/* usar el port por default (ver inet.h) */
 if (! (port > 0))			/* validarlo */
	{
	 sprintf(mostrar,"%s (%d): nro. de port invalido %s\n", pname, pid_si, argv[1]);
	 write(fileno(stdout), mostrar, sizeof(mostrar));
	 exit(1);
	}
 /*
  * 		Hacer el open pasivo 
  */
 if ((sockfd = tcpoppas(port)) < 0)
	{
 	 perror ("server: no se puede hacer el open pasivo ");
 	 exit(1);
 	 }
 /*
  *			PROCESAMIENTO DEL SERVER
  */

 for ( ; ; )
	 {	/* no termina nunca */
 /*
  * 		ESTE ES UN EJEMPLO DE UN SERVER ITERATIVO
  * 	   Espera por un cliente en el accept:
  *        Cuando llega un cliente lo atiende
  *        y a continuacion vuelve al accept por otro cliente.
  */

	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0)
	       {
       		perror ("server: error en el accept");
         	exit(1); 
         	}
/*
 *	 	 Atiende al cliente
 */
	tcpservertrans(newsockfd, pname, pid_si, nro);    /* procesar el pedido */
	close(newsockfd);                       /* cerrar el socket con el cliente */
	}
}
