/*
 * Programa: Ejemplo de cliente usando protocolo TCP : (tcpcli)
 * Descripcion:
 *	Verificar parametros ingresados
 *	Hacer un open activo (tcpopact)
 *      Repetir hasta el fin del archivo de productos (TcpRPProdn)
 *		   envia un pedido al servidor (vendedor)
 *		   espera por la respuesta del vendedor.
 *	
 * Sintaxis: tcpcli nro [servidor [port]]
 *  nro:		numero del cliente (OBLIGATORIO]
 *	servidor: 	nombre oficial del host server
 *	port:		nro de port
 *	servidor y port son parametros opcionales: 
 *		Si no se especifica port, se usa el PORT_FIJO (ver inet.h)
 *		Si no se especifica host, se usa el localhost 
 * Usa:	Funcion tcpopact(), 
 *		Procedimientos tcpclienttrans(), close()
 */

#include	"inet.h"
extern int tcpopact(char *, int);
extern void tcpclienttrans (int , char *, pid_t, int );

int main(int argc, char *argv[]){
 int sockfd, codigo_retorno;
 char *server;     /* nombre del server */
 char localhost[] =	"localhost"; /* nombre del server x defecto */	
 char mostrar[120];
 char *pname;	/* nombre del programa */
 int port;		/* port de conexion */
 int nro;
 pid_t pid_cl;	/* pid del cliente */
 
 pname = argv[0];	/* programa cliente */
 pid_cl = getpid();
	/*
	 * 	Paso 0: Verificar si se ingreso el nro de cliente
	 */
 if (argc > 1) {
	 nro = atoi(argv[1]);
 }
 else {
	 sprintf(mostrar,"%s (%d) Falta el nro del cliente (primer parametros y obligatorio)\n", pname, pid_cl);
	 write(fileno(stdout),mostrar,strlen(mostrar));
	 exit(1);
 }
 sprintf(mostrar,"\n%s (%d) %d: Cliente empieza a comprar\n", pname, pid_cl, nro);
 write(fileno(stdout),mostrar,strlen(mostrar));
 /*			
  *			Verificar nombre server y port
 /*
  * 	Paso 1: Verificar si se ingreso un numero de port o se usa el default
  */
  if (argc > 3) 
	port = atoi(argv[2]); /* se especifico port: convertir a binario */
  else
	port = PORT_FIJO;	/* usar el port por defecto (ver inet.h) */
  if ( ! (port > 0))	/* validarlo */
	{			/* valor incorrecto: aviso y terminar */
	sprintf(mostrar,"%s (%d): Nro. de port invalido %s\n", pname, pid_cl, argv[2]);
	write(fileno(stdout),mostrar,strlen(mostrar));
	exit(1);
	}
 /*
  *	Paso 2: Verificar si se ingreso nombre del server, o se usa el 
  *	        localhost 
  */
  if (argc > 2)
	server = argv[1];	/* se especifico nombre de server */
  else
	server = localhost;	/* ver inet.h */
 /*
  *	Paso 3: Hacer un open activo y obtener el socket.
  */
  sockfd = tcpopact(server, port); 
  if ( sockfd < 0 )
	{		/* Hay un error: aviso y terminar */
	if (sockfd == -2) 
	    {
	    sprintf(mostrar,"%s (%d) %d: Nombre de server no existe %s\n", pname, pid_cl,nro, server);
            write(fileno(stdout), mostrar, strlen(mostrar));
	    exit(1);
	    }
         else
	    {
	    perror("error en el open activo");
	    exit(1);
	    }
 }
 /*
  *		PROCESO DEL CLIENTE (hasta que el usuario ingrese EOF)
  */
 tcpclienttrans(sockfd, pname, pid_cl, nro);	/* transferencia de datos sobre la conexion*/

 /*
  *		CERRAR LA CONEXION Y TERMINAR
  */
 close(sockfd);    
 exit(0);
}
