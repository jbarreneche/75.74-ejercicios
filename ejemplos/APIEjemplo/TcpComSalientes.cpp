/*
 * TcpComSalientes
 *
 *  Created by Maria Feldgen on 3/10/12.
 *  Copyright 2012 Facultad de Ingenieria U.B.A. All rights reserved.
 *
 * Descripcion:	
 *		Recibe un mensaje de la cola de envio de la API  
 *		Envía el mensaje sobre canal establecido usando TCP.
 *	Termina: cuando recibe el signal SIGUSR1 o se destruye la cola o recibe el mensaje de FINV.
 *	En el caso de recibir el mensaje de FINV: destruye el proceso de comunicaciones entrantes.
 * 
 *	Los parametros del programa son:
 *   ./TcpComSalientes nroCanal, fd del socket, pid del proceso TcpComEntrantes
 *
 */

#include	"TcpEjemploAplicacion.h"
#include	"TcpSocketsGral.h"

extern int TcpEnviar(int, void *, size_t);

void sig_handler(int signo) {
    char mostrar[100];
	sprintf (mostrar,"TcpComSalientes: TERMINA\n");
	write(fileno(stdout), mostrar, strlen(mostrar));
	exit(0);
}

int main(int argc, char *argv[]){
	int	newsockfd;		/* socket conectado al cliente */
	pid_t  pid;			/* pid del proces */
	char mostrar[200];	/* mensaje para mostrar en pantalla */
	char *pname;		/* nombre del programa */
	int port;			/* port de conexion */
	int pidComEntrantes; /* pid del proceso TcpComEntrantes */
	
	int colaE;			/* file descriptor cola de envio */
	PROTOCOLO envio;	/* mensaje saliente de la cola y del socket */
	key_t clave1;

	int identif;
	long int canal;
	int n;				
	pname = argv[0];
	/*
	 *		Verificar si se pasan todos los parametros
	 */
	if (argc > 4) {
		canal=atoi(argv[1]);
		newsockfd = atoi(argv[2]);	/* convertir port ingresado a binario */
		if (argv[3][0] == 'S') 
			identif=COLASE;
		else 
			identif=COLACE;
		pidComEntrantes = atoi(argv[4]); /* pid del proceso TcpComEntrantes */
	}
	else {
		sprintf (mostrar,"%s (%d): Faltan parametros al invocar este programa: canal, fd del socket, tipo y pid de TcpComEntrantes\n", pname, pid);
		write(fileno(stdout), mostrar, strlen(mostrar));
		exit(1);
	 }
	pid = getpid();
	signal(SIGUSR1, sig_handler);

	sprintf (mostrar,"%s (%d): Parametros recibidos: canal: %s, fd del socket: %s, tipo: %s, proceso: %s\n", pname, pid, argv[1], argv[2], argv[3], argv[4]);
	write(fileno(stdout), mostrar, strlen(mostrar));
	/*
	 * adquirir la cola de envios de la aplicacion
	 */
	clave1 = ftok(DIRECTORIO,identif);
	if ((colaE = msgget (clave1,0660)) == -1) { 
		perror ("TcpComSalientes: error al hacer el get de la cola de envio "); 
		exit(1);
	}	
	for (; ; ) {
		/* 
		 * procesar el pedido de la cola
		 */
		if (msgrcv(colaE, (PROTOCOLO *)&envio, sizeof(envio), canal, 0) == -1){ 
			if (errno == EINVAL || errno == EIDRM) {	/* verifica si se destruyeron los IPC */
				sprintf (mostrar,"%s (%d): TERMINA\n", pname, pid); 
				write(fileno(stdout), mostrar, strlen(mostrar));
				exit(0);
			}
			else {  
				perror ("TcpComSalientes: Error en la recepcion en cola envio ");
				exit (1);
			}
		}
		sprintf (mostrar,"%s (%d): Recibe mensaje de cola envio Tipo: %d, Origen: %d, Destino: %d, TipoMsg: %d, Longitud: %d, Longitud total: %d, canal: %d\n", pname, pid, envio.tipo, envio.origen, envio.destino, envio.tipoMsg, envio.longitud, sizeof(envio.mensaje), canal);
		write(fileno(stdout), mostrar, strlen(mostrar));
		if (envio.tipoMsg == FINV) {
			sprintf(mostrar,"%s (%d): Fin de la Venta: avisar al proceso TcpComEntrantes %d\n ", pname, pid, pidComEntrantes);
			write(fileno(stdout), mostrar, strlen(mostrar));
			close(newsockfd);
			kill (pidComEntrantes, SIGUSR1);  /* eliminar el canal entrante */
			sprintf(mostrar,"%s (%d): TERMINA\n ", pname, pid);
			write(fileno(stdout), mostrar, strlen(mostrar));
			exit(0);
		}
		/*
		 * Agregar el id. del destino final en el tipo 
		 */
		envio.tipo = envio.destino;
		/*
		 * enviar sobre el socket 
		 */
		if (TcpEnviar(newsockfd, (PROTOCOLO *) &envio, sizeof(envio)) != sizeof(envio)) {
			sprintf(mostrar,"%s (%d): error en el envio por el socket\n", pname, pid);
			write(fileno(stdout), mostrar, strlen(mostrar));
			exit(1);
		}
		sprintf (mostrar,"%s (%d): Mensaje enviado al socket\n", pname, pid);
		write(fileno(stdout), mostrar, strlen(mostrar));
	}
  close(newsockfd);
  exit(0);
}
