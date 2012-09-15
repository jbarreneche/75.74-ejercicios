/*
 *  TcpEjemploClienteAPI.h
 *  
 *  Created by Maria Feldgen on 3/10/12.
 *  Copyright 2012 Facultad de Ingenieria U.B.A. All rights reserved.
 *
 * Primitivas de la API para la aplicacion en el cliente del sistema
 * 
 *		int iniciarAplicacion(char *,char *,int, CONTROL *); 
 *	Adquiere los IPCs, inicializa la informacion de control desde
 *  el archivo de servicios estableciendo el canal de comunicaciones
 *	lanza los procesos de comunicaciones TcpComEntrantes y TcpComSalientes
 *	para el canal establecido.
 *		int enviarDatosCliente(void *, int, void *, int, CONTROL *);
 *	recibe un mensaje, arma los datos del protocolo (identificadores y longitud) 
 *	y lo envia sobre la cola de envios con identificador = canal.
 *	Espera y devuelve el mensaje de respuesta sobre la cola de recepcion 
 *		con identificador = comprador.
 *		int pedirProducto(void *, int, void *, int, CONTROL *);
 *	recibe un mensaje, arma los datos del protocolo (identificadores y longitud) 
 *	y lo envia sobre la cola de envios con identificador = canal.
 *	Espera y devuelve el mensaje de respuesta que obtiene de la cola de recepcion 
 *		con identificador = comprador.
 *		int recibirFactura (void *, int, CONTROL *);
 * 	Espera y devuelve el mensaje que obtuvo de la cola de recepcion 
 *		int cerrarAplicacion(CONTROL *);
 *	envia una señal SIGUSR! a los procesos TcpComEntrantes y TcpComSalientes 
 *  para indicar que el comprador termina y destruye los IPCs.
 */

#include "TcpSocketsGral.h"

typedef struct      {		/* Informacion de Control */
	/* de las comunicaciones */
	int recepcion;			/* file descriptor cola recepcion */
	int envio;				/* file descriptor cola envio */
	int socketfd;			/* file descriptor del socket*/
	
	int cliente;			/* id del cliente */
	int conectorCliente;	/* conector del cliente = nro de cliente en este caso */
	int conectorVendedor;	/* conector del vendedor asignado */

	int pidCanalEntrante;	/* nro proceso del canal de comunicaciones entrante */
	int pidCanalSaliente;	/* nro proceso del canal de comunicaciones saliente */
	/* de la aplicacion */
	int pidAplicacion;      /* nro de proceso de la aplicacion */
	char *aplicacion;		/* nombre de la aplicacion que usa la API */
} CONTROL;

typedef struct{			/* mensaje para la aplicacion */
	int longitud;
	char mensaje[5000];
} RESPUESTA;
extern int TcpOpenActivo(char *, int);

int iniciarAplicacion(char *,char *,int, CONTROL *); 
int enviarDatosCliente(void *, int, void *, int, CONTROL *);
int pedirProducto(void *, int, void *, int, CONTROL *);
int recibirFactura (void *, int, CONTROL *);
int cerrarAplicacion(CONTROL *);

/*		Primitiva:
 *		Iniciar Aplicacion:	inicializar la shm de control,
 *							crear las colas de envio y recepcion
 *							buscar port y nombre del servidor de la aplicacion,
 *							hacer un open activo con el servidor
 *							lanzar los procesos de comunicaciones con el socket
 */
int iniciarAplicacion(char *aplicacion, char * sistema, int ident, CONTROL *control) {
	key_t clave;
	struct stat st;
	char mostrar[200];
	int pid, childpid;

	int port;
	char server[]="                         ";
	int sockfd;
	FILE *fp;
	int vendedorInicial;
	static char param1[10], param2[10], param3[10]; /* string parametros del execlp */
	char primeraLinea[80];
	char nombre[]="                          ";

	
	pid = getpid();
	sprintf (mostrar,"%s %d (%d) API-->iniciarAplicacion: empieza\n", aplicacion, ident, pid);
	write(fileno(stdout), mostrar, strlen(mostrar));
	/* 
	 * Verificar que el DIRECTORIO para el ftok, esta creado
	 */	
	if(stat(DIRECTORIO,&st) != 0) {
		sprintf (mostrar,"%s %d (%d) API-->iniciarAplicacion: Falta el directorio de referencia para el ftok: %s\n", aplicacion, ident, pid, DIRECTORIO); 
		write(fileno(stdout), mostrar, strlen(mostrar));
		return(-1);
	}
	/*
	 * Buscar sistema en el archivo de servicios 
	 */
	if( (fp=fopen(ARCHIVO,"r") ) == NULL){  
		sprintf (mostrar,"%s %d (%d) API-->iniciarAplicacion: NO se puede abrir el archivo de servicios: %s\n", aplicacion, ident, pid, ARCHIVO); 
		write(fileno(stdout), mostrar, strlen(mostrar));
		exit(1);
	}
	else{
		fgets(primeraLinea,80,fp);			/* Ignorar la primera linea: aplicacion port nombre destino vendedor inicial */
		int i=0;
		do {
			fscanf(fp,"%s %d %s %d",
				   nombre,
				   &port,
				   server,
				   &vendedorInicial);		
		} while (( strcmp( nombre, sistema ) == 0 ) || ( strcmp( nombre, "FIN" ) == 0 ));
		if( strcmp(nombre, "FIN" ) == 0 ) {
			sprintf (mostrar,"%s %d (%d) API-->iniciarAplicacion: El sistema no existe en el archivo de servicios %s\n", aplicacion, ident, pid, sistema); 
			write(fileno(stdout), mostrar, strlen(mostrar));
			return(-1);
		}
	}
	
	control->cliente = ident;
	control->conectorCliente = ident;
	control->conectorVendedor = vendedorInicial;
	control->aplicacion = aplicacion;
	control->pidAplicacion = pid;
	/*   
	 *	Crear la cola recepcion
	 */ 
	clave = ftok(DIRECTORIO,COLACR);
	if ((control->recepcion = msgget (clave,IPC_CREAT|IPC_EXCL|0660)) == -1) {
		perror ("API-->iniciarAplicacion: error al crear la cola de recepcion "); 
		return (-1);
    }
	/*   
	 *	Crear la cola envio
	 */ 
	clave = ftok(DIRECTORIO,COLACE);
	if ((control->envio = msgget (clave,IPC_CREAT|IPC_EXCL|0660)) == -1) {
		perror ("API-->iniciarAplicacion: error al crear la cola de envio "); 
		return  (-1);
    }
	/*
	 *	Hacer un open activo y obtener el socket.
	 */
	sockfd = TcpOpenActivo(server, port); 
	if ( sockfd < 0 )
	{		/* Hay un error: aviso y terminar */
		if (sockfd == -2) 
	    {
			sprintf(mostrar,"%s %d (%d) API-->iniciarAplicacion: Nombre del servidor no existe %s\n", aplicacion, ident, pid, server);
            write(fileno(stdout), mostrar, strlen(mostrar));
			return(-1);
	    }
		else
	    {
			perror("API-->iniciarAplicacion: error en el open activo");
			return(-1);
	    }
	}
	
	sprintf (mostrar,"%s %d (%d) API-->iniciarAplicacion: se hizo el open activo, socket %d\n", aplicacion, ident, pid, sockfd);
	write(fileno(stdout), mostrar, strlen(mostrar));  
	
	/* armar los parametros para los execlp */
	sprintf(param1, "%d\n",ident); /* pasarle el nro de cliente */
	sprintf(param2, "%d\n",sockfd); /* pasarle el socket a los procesos de comunicaciones */
	/* comunicaciones entrantes */
	if ( (childpid = fork()) < 0) { 
		perror("API-->iniciarAplicacion: error en el fork para TcpComEntrantes");
		return(-1);
	}
	else if (childpid == 0){	 
		/*
		 *	 	 PROCESO HIJO (child) para las comunicaciones entrantes
		 */  	
		execlp("./TcpComEntrantes", "TcpComEntrantes", param1, param2, "C", (char *)0);
		perror("API-->iniciarAplicacion: error al lanzar el TcpComEntrantes");
		return(-1);
	}
	control->pidCanalEntrante = childpid;
	sprintf(param3, "%d\n",childpid); /* pasarle el nro de proceso del canal entrante */
	/* comunicaciones salientes */
	if ( (childpid = fork()) < 0) { 
		perror("API-->iniciarAplicacion: error en el fork para TcpComSalientes");
		return(-1);
	}
	else if (childpid == 0){	 
		/*
		 *	 	 PROCESO HIJO (child) para las comunicaciones salientes
		 */  	
		execlp("./TcpComSalientes", "TcpComSalientes", param1, param2, "C", param3, (char *)0);
		perror("API-->iniciarAplicacion: error al lanzar el TcpComSalientes");
		return(-1);
	}
	control->pidCanalSaliente = childpid;
	sprintf (mostrar,"%s %d (%d) API-->iniciarAplicacion: Fin inicializacion\n", aplicacion, ident, pid);
	write(fileno(stdout), mostrar, strlen(mostrar));
	return 0;
}

	/* Funcion interna
	 * Enviar mensaje sobre la cola enviar 
	 */
int enviarMensaje(int tipoMsg, void *mensaje, int longitud, CONTROL *control ) {
	PROTOCOLO enviar;
	char mostrar[300];
	if (longitud > sizeof(enviar.mensaje)) {
		sprintf (mostrar,"%s %d (%d) API-->enviarMensaje: la longitud del mensaje supera el maximo: %d\n", control->aplicacion, control->cliente, control->pidAplicacion, sizeof(enviar.mensaje));
		write(fileno(stdout), mostrar, strlen(mostrar));
		return(-1);
	}
	enviar.longitud = longitud;
	enviar.tipo = control->conectorCliente;
	enviar.origen = control->conectorCliente;
    enviar.destino = control->conectorVendedor;
	memcpy (enviar.mensaje, (char *)mensaje, longitud+1);
	enviar.tipoMsg = tipoMsg;  /* datos del cliente */
			 
	sprintf (mostrar,"%s %d (%d) API-->enviarMensaje: Envia mensaje Tipo: %d, Origen: %d, Destino: %d, TipoMsg: %d, Longitud: %d, Longitud total: %d\n",control->aplicacion, control->cliente, control->pidAplicacion, enviar.tipo, enviar.origen, enviar.destino, enviar.tipoMsg, enviar.longitud, sizeof(enviar.mensaje));
	 write(fileno(stdout), mostrar, strlen(mostrar));
	 if (msgsnd(control->envio, (PROTOCOLO *)&enviar, sizeof(enviar), 0) == -1) {
		 /* datos del cliente enviados a algun vendedor */
		 perror("API-->enviarMensaje: error en el send sobre la cola de envio "); 
		 return  (-1);
	 }
	return 0;
}
				 
 /* Funcion interna
  * Recibir mensaje sobre la cola recibir 
  */
 int recibirMensaje(int tipoMsg, void *mensaje, int longitud, CONTROL *control ) {
	 PROTOCOLO recibir;
	 char mostrar[300];
	 long int msg_a_recibir = control->conectorCliente; 
	 if (msgrcv(control->recepcion, (PROTOCOLO *)&recibir, sizeof(recibir), msg_a_recibir, 0) == -1){
		 perror("API-->recibirMensaje: error en el receive sobre la cola de recepcion "); 
		 return  (-1);
	 }
	  control->conectorVendedor = recibir.destino;
	  longitud= recibir.longitud;
	  memcpy ((char *)mensaje, recibir.mensaje, recibir.longitud+1);
	  sprintf (mostrar,"%s %d (%d) API-->recibirMensaje: Recibe mensaje Tipo: %d, Origen: %d, Destino: %d, TipoMsg: %d, Longitud %d, Longitud total: %d\n", control->aplicacion, control->cliente, control->pidAplicacion, recibir.tipo, recibir.origen, recibir.destino, recibir.tipoMsg, longitud, sizeof(recibir.mensaje));
	  write(fileno(stdout), mostrar, strlen(mostrar));
	  return 0;
	  }
				 
 /* Primitiva:
  * Enviar datos del cliente al vendedor 
  */
 int enviarDatosCliente(void *mensaje, int longitud, void *respuesta, int longresp, CONTROL *control ) {
	 int retorno;
	 char mostrar[200];
	 sprintf (mostrar,"%s %d (%d) API-->enviarDatosCliente: iniciar la compra\n", control->aplicacion, control->cliente, control->pidAplicacion);
	 write(fileno(stdout), mostrar, strlen(mostrar));
	 if ((retorno = enviarMensaje(PEDI, mensaje, longitud, control))==0) 
		 retorno =  recibirMensaje(ACEP, respuesta, longresp, control);
	 return retorno;
 }
				 
 /* Primitiva:
  * Pedir el producto a comprar 
  */
int pedirProducto(void *mensaje, int longitud, void *respuesta, int longresp, CONTROL *control ) {
	int retorno;
	char mostrar[200];
	sprintf (mostrar,"%s %d (%d) API-->pedirProducto\n", control->aplicacion, control->cliente, control->pidAplicacion);
	write(fileno(stdout), mostrar, strlen(mostrar));
	if((retorno = enviarMensaje(PROD, mensaje, longitud, control))==0)
		retorno = recibirMensaje(CONF, respuesta, longresp, control);
	return retorno;
}

 /* Primitiva:
  * recibe factura de la compra  
  */
int recibirFactura (void *respuesta, int longresp, CONTROL *control) {
	char mostrar[200];
	int retorno;
	sprintf (mostrar,"%s %d (%d) API-->recibirFactura\n", control->aplicacion, control->cliente, control->pidAplicacion);
	write(fileno(stdout), mostrar, strlen(mostrar));
	retorno = recibirMensaje(FACT, respuesta, longresp, control) ;
	return retorno;
}

/* Primitiva
 * Cerrar la aplicacion: Cerrar el socket y Destruir los IPCs cuando termina 
 */
int cerrarAplicacion(CONTROL *control) {
	char mostrar[200];
	close(control->socketfd);					/* cerrar el socket */
	sprintf (mostrar,"%s %d (%d) API-->cerrarAplicacion: Eliminar canal saliente: %d\n", control->aplicacion, control->cliente, control->pidAplicacion, control->pidCanalSaliente);
	write(fileno(stdout), mostrar, strlen(mostrar));
	kill (control->pidCanalSaliente, SIGUSR1);  /* avisarle que termina la aplicacion */
	sprintf (mostrar,"%s %d (%d) API-->cerrarAplicacion: Eliminar canal entrante: %d\n", control->aplicacion, control->cliente, control->pidAplicacion, control->pidCanalEntrante);
	write(fileno(stdout), mostrar, strlen(mostrar));
	kill (control->pidCanalEntrante, SIGUSR1);  /* eliminar el canal entrante */
	sprintf (mostrar,"%s %d (%d) API-->cerrarAplicacion: Eliminar las colas\n", control->aplicacion, control->cliente, control->pidAplicacion);
	write(fileno(stdout), mostrar, strlen(mostrar));
	msgctl(control->recepcion, IPC_RMID, NULL); /* eliminar la cola de recepcion */        
	msgctl(control->envio, IPC_RMID, NULL);		/* eliminar la cola de envio */
	sprintf (mostrar,"%s %d (%d) API-->cerrarAplicacion\n", control->aplicacion, control->cliente, control->pidAplicacion);
	write(fileno(stdout), mostrar, strlen(mostrar));
	return 0;
}	


 

