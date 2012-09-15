/*
 * TcpComEntrantes
 *
 *  Created by Maria Feldgen on 3/10/12.
 *  Copyright 2012 Facultad de Ingenieria U.B.A. All rights reserved.
 *
 * Descripcion:	
 *		Recibe un mensaje sobre un canal establecido usando TCP  
 *		Envía el mensaje sobre la cola de recepcion de la API.
 *	Termina: cuando recibe el signal SIGUSR1
 */
#include	"TcpEjemploAplicacion.h"
#include	"TcpSocketsGral.h"

extern int TcpRecibir(int, void *, size_t );

void sig_handler(int signo) {
    char mostrar[100];
	sprintf (mostrar,"TcpComEntrantes: TERMINA\n");
	write(fileno(stdout), mostrar, strlen(mostrar));
	exit(0);
}

int main(int argc, char *argv[]){
	int	newsockfd;		/* filedescriptor de sockets */
	pid_t  pid;			/* pid del proces */
	char mostrar[200];	/* mensaje para mostrar en pantalla */
	char *pname;		/* nombre del programa */
	int colaR;			/* file descriptor cola de envio */
	PROTOCOLO recibe;	/* mensaje del socket y la cola de envio*/
	key_t clave1;

	long int canal;
	int n;
	int identif;
	struct sigaction sa;
	memset (&sa, 0, sizeof (sa)); /* initialization */
	sa.sa_handler = &sig_handler; /* specify the signal handler */
	sigaction (SIGUSR1, &sa, NULL); /* using the undefined signal number */
	pname = argv[0];
	/*
	 *		Verificar si se pasa el descriptor del socket
	 */
	if (argc > 3) {
		canal = atoi(argv[1]);		/* identificador del canal de comunicaciones */
		newsockfd = atoi(argv[2]);	/* convertir port ingresado a binario */
		if (argv[3][0]== 'S') 
			identif=COLASR;
		else 
			identif=COLACR;
	}
	else {
		perror("TcpComEntrantes: faltan parametros ( id. del canal, fd del socket y S/C) al invocar este programa");
		exit(1);
	 }
	pid = getpid();
	//signal(SIGUSR1, sig_handler);  /* formato viejo de tratamiento de seniales */

	sprintf (mostrar,"%s (%d): Parametros recibidos: canal: %s, fd del socket: %s, tipo: %s\n", pname, pid, argv[1], argv[2], argv[3]);
	write(fileno(stdout), mostrar, strlen(mostrar));

	/*
	 * adquirir la cola de recepcion de la aplicacion en el servidor
	 */
	clave1 = ftok(DIRECTORIO,identif);
	if ((colaR = msgget (clave1,0660)) == -1) { 
		perror ("TcpComEntrantes: error al hacer el get de la cola de recepcion "); 
		exit(1);
	}	
	for (; ; ) {
		/* 
		 * procesar el mensaje recibido sobre el socket
		 */
		n = TcpRecibir(newsockfd, &recibe, sizeof(recibe));
		if (n < 0){
			if (errno == EBADF) {
				sprintf (mostrar,"%s (%d): TERMINA\n", pname, pid);
				write(fileno(stdout), mostrar, strlen(mostrar));
				exit(0);
			}
			else {
				perror("TcpComEntrantes: error en el read sobre el socket ");
				exit(1);
			}
		}
		sprintf (mostrar,"%s (%d): Mensaje recibido sobre el socket: tipo: %d, Origen: %d, Destino: %d, TipoMsg: %d, Longitud: %d, Longitud total: %d, Canal que procesa: %d\n", pname, pid, recibe.tipo, recibe.origen, recibe.destino, recibe.tipoMsg, recibe.longitud, sizeof(recibe.mensaje), canal);
		write(fileno(stdout), mostrar, strlen(mostrar));
		/*
		 * modificar destino para indicar cual es el canal de salida 
		 */
		recibe.destino = canal;
		/*
		 * enviar sobre la cola  
		 */
		if (msgsnd(colaR, (PROTOCOLO *)&recibe, sizeof(recibe), 0) == -1){   
			perror ("TcpComEntrantes: Error en el envio cola recepcion ");
			return(-1);
		}
		sprintf (mostrar,"%s (%d): Mensaje enviado a la cola recepcion\n", pname, pid);
		write(fileno(stdout), mostrar, strlen(mostrar));
	}
  close(newsockfd);
  exit(0);
}
