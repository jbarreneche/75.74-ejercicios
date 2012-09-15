/*
 * Programa: TcpServerConcurrente: Servidor concurrente que crea los 
 *			 canales de comunicaciones cuando se conecta un nuevo comprador 
 *			 usando protocolo TCP 
 *
 *  Created by Maria Feldgen on 3/11/12.
 *  Copyright 2012 Facultad de Ingenieria U.B.A. All rights reserved.
 *
 * Descripcion:	
 *	Crea los IPCs (colas, mutex y share memory para los productos)
 *  Carga los productos en la shared memory
 *	Lanza los vendedores (TcpEjemploVendedor)
 *  Hace un open pasivo 
 *	Espera por un comprador.
 *	Por cada comprador  lanza dos procesos para las comunicaciones:
 *		TcpComEntrantes y TCPComSalientes
 *	Para terminarlo [enter] y correr TCPBorrarIPC
 *
 * Sintaxis: TcpServerConcurrente [cantidad de vendedores] [port]
 *	cantidad de vendedores: cantidad de procesos vendedores concurrentes
 *	port: 	port sobre el cual atiende este servidor
 *	Si no se ingresa cantidad de vendedores y/o nro de port: se usan los
 *		valores por defecto en los archivos .h
 */
#include	"TcpEjemploAplicacion.h"
#include	"TcpSocketsGral.h"
#include	"semaforos.h"
extern int TcpOpenPasivo(int);

int main(int argc, char *argv[]) {
	int sockfd,			/* socket que sirve como template */ 
		newsockfd;		/* socket conectado al cliente */
	unsigned clilen,	/* longitud dir. cliente */
		  childpid;		/* pid del hijo */
	struct sockaddr_in	cli_addr;
	int shmid1, compras, despacho;
	char mostrar[300];	/* mensaje para mostrar en pantalla */
	char *pname;		/* nombre del programa */
	int pid_pr;
	int  port;			/* port de conexion */
	int canal;			/* Nro de canal de comunicaciones abierto con un cliente */
	int cantVendedores;
 
	int mutex, shmid;	/* file descriptor de los IPCs */
	STOCK *shmem;
	key_t clave;
	char archivo[]="TcpEjProductos.txt";  /* contiene los productos a vender */
	int i;
	FILE* 	fp;					/* archivo y estructura de los productos */
	char	primeraLinea[80];
	struct stat st;
	static char param1[10], param2[10], param3[10]; /* string parametros del execlp */
	pname = argv[0];
	pid_pr =getpid();
	
	/*
	 *	Verificar si se informo la cantidad de vendedores que se deben crear
	 */
	if (argc > 2) {  /* se informo la cantidad de vendedores */
		cantVendedores = atoi(argv[1]);	/* convertir a binario */
		port = atoi(argv[2]);	/* convertir port ingresado a binario */
	}
	else{
		if (argc > 1) {
			cantVendedores = atoi(argv[1]);	/* convertir a binario */
			port = PORT_FIJO;	/* usar el port por default */
		}
		else 
			cantVendedores = VENDEDORES;	/* default de la aplicacion */
	}
	/* 
	 * Verificar que el DIRECTORIO esta creado
	 */
	
	if(stat(DIRECTORIO,&st) != 0) {
		sprintf (mostrar,"%s (%d): Falta el directorio de referencia Cte DIRECTORIO: %s\n", pname, pid_pr, DIRECTORIO); 
		write(fileno(stdout), mostrar, strlen(mostrar));
		exit(1);
	}
	/*
	 *   crear la shared memory en la cual se mantiene el estado
	 *	hacer el attach e inicializar las variables compartidas
	 */ 
	clave = ftok(DIRECTORIO,SHM);
	if ((shmid1 = shmget (clave, sizeof(STOCK), IPC_CREAT|IPC_EXCL|0660)) == -1) { 
		perror ("TcpServerConcurrente: error al crear la shared memory "); 
		exit (1);
	}
	
	if ((shmem = (STOCK *) shmat(shmid1,0,0)) == (STOCK *) -1 ) { 
		perror ("TcpServerConcurrente: error en el attach a shared memory "); 
		exit (1);
    }
	/* 
	 * Cargar los datos en la memoria compartida
	 */
	if( (fp=fopen(archivo,"r") ) == NULL){  
		sprintf (mostrar,"%s (%d): NO se puede abrir el archivo con los productos %s\n", pname, pid_pr, archivo); 
		write(fileno(stdout), mostrar, strlen(mostrar));
		exit(1);
	}
	else{
		fgets(primeraLinea,80,fp);			/* Ignorar la primera linea. */
		int i=0;
		while(!feof(fp)){   /* descargar todo el archivo */
			fscanf(fp,"%d %20s %f %d",
				   &shmem->prod[i].codigo,
				   shmem->prod[i].denominacion,
				   &shmem->prod[i].precio,
				   &shmem->prod[i].cantidad);
			sprintf (mostrar,"%s (%d): producto a vender: codigo: %d, denominacion: %s, precio: %4.2f, stock: %d\n", pname, pid_pr,  shmem->prod[i].codigo, shmem->prod[i].denominacion, shmem->prod[i].precio, shmem->prod[i].cantidad); 
			write(fileno(stdout), mostrar, strlen(mostrar));
			i++;
			shmem->cantProductos++;
		}
		fclose(fp);
		shmem->cantProductos--; /* el ultimo es un terminador */		
	}
	/* 
	 *	 crear e inicializar el IPC semaforo mutex 
	 */	 
	if ((mutex = creasem (MUTEX)) == -1) 	/* IPC para exclusion mutua */{   
		perror ("TcpServerConcurrente: error al crear el (IPC) semaforo mutex"); 
		exit (1);
    }
	inisem (mutex, 1);			/* inicializarlo */
	/*   
	 *	crear la  cola Recepcion del servidor
	 */ 
	clave = ftok(DIRECTORIO,COLASR);
	if ((compras = msgget (clave,IPC_CREAT|IPC_EXCL|0660)) == -1) {   
		perror ("TcpServerConcurrente: error al hacer el get de la cola COMPRAS "); 
		exit (1);
    }
	
	/*   
	 *	crear la cola Envio del servidor
	 */ 
	clave = ftok(DIRECTORIO,COLASE);
	if ((despacho = msgget (clave,IPC_CREAT|IPC_EXCL|0660)) == -1) {  
		perror ("TcpServerConcurrente: error al hacer el get de la cola DESPACHO "); 
		exit (1);
	}
	/*
	 * Crear los vendedores 
	 */
	for (i=0; i<cantVendedores; i++) {
		sprintf(param1, "%d\n",i+1); /* pasarle el nro de vendedor */

		if ( (childpid = fork()) < 0) { 
			perror("TcpServerConcurrente: error en el fork para TcpEjemploVendedor");
			exit(1);
		}
		else if (childpid == 0){	 
			/*
			 *	 	 PROCESO HIJO (child) Vendedor
			 */  	
			execlp("./TcpEjemploVendedor", "TcpEjemploVendedor", param1, (char *)0);
			perror("TcpServerConcurrente: error al lanzar el TcpEjemploVendedor");
			exit(3);
		}
	}
	canal=1;
	/*			Inicializacion del servidor 
	 * 				OPEN PASIVO
	 */
	if ( (sockfd = TcpOpenPasivo(port)) < 0) {
			perror("TcpServerConcurrente: no se puede abrir el stream socket");
			exit(1);
		}
	sprintf (mostrar,"TcpServerConcurrente: se hizo el open pasivo, socket %d\n", sockfd);
	write(fileno(stdout), mostrar, strlen(mostrar));  
 /*
  *			PROCESAMIENTO DEL SERVER
  */
 for ( ; ; ) {	/* no termina nunca */
 /*
  * 		ESTE ES UN EJEMPLO DE UN SERVER CONCURRENTE
  * 	Espera por un cliente en el accept:
  *        Cuando llega un cliente crea los procesos para las comunicaciones (Entrante y saliente)  
  *        y el padre vuelve al accept por otro cliente.
  */
	 clilen = sizeof(cli_addr);
	 newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	 if (newsockfd < 0) {
		perror ("TcpServerConcurrente: error en el accept");
		exit(1); 
	 }
	 
	 sprintf (mostrar,"%s (%d): socket armado con un cliente: %d, armar canales de comunicaciones\n",pname, pid_pr, newsockfd);
	 write(fileno(stdout), mostrar, strlen(mostrar)); 
	 /* armar los parametros para los execlp */
	 sprintf(param1, "%d", canal); /* pasarle el nro de canal */
	 canal++;
	 sprintf(param2, "%d", newsockfd); /* pasarle el socket al hijo que atiende el fd del socket*/
	 /* comunicaciones entrantes */
	 if ( (childpid = fork()) < 0) { 
		 perror("TcpServerConcurrente: error en el fork para TcpComEntrantes");
		 exit(1);
	 }
	 else if (childpid == 0){	 
		 /*
		  *	 	 PROCESO HIJO (child) para las comunicaciones entrantes
		  */  	
		 close(sockfd);		/* cerrar socket original */
		 execlp("./TcpComEntrantes", "TcpComEntrantes", param1, param2, "S", (char *)0);
		 perror("TcpServerConcurrente: error al lanzar el TcpComEntrantes");
		 exit(3);
	 }
	 /* comunicaciones salientes */
	 sprintf(param3, "%d", childpid); /* pasarle el nro de proceso del canal entrante*/
	 if ( (childpid = fork()) < 0) { 
		perror("TcpServerConcurrente: error en el fork para TcpComSalientes");
		exit(1);
		}
	else if (childpid == 0){	 
/*
 *	 	 PROCESO HIJO (child) para las comunicaciones salientes
 */  	
		close(sockfd);		/* cerrar socket original */
		execlp("./TcpComSalientes", "TcpComSalientes", param1, param2,  "S", param3, (char *)0);
		perror("TcpServerConcurrente: error al lanzar el TcpComSalientes");
		exit(3);
	 }
		 /*
  *		 PROCESO PADRE, se prepara para recibir otro cliente
  */
	close(newsockfd);		/* cerrar el socket pasado al hijo */
	}
	exit(0);
 }

