/*
 *  TcpEjemploServerAPI.h
 *  
 *  Created by Maria Feldgen on 3/10/12.
 *  Copyright 2012 Facultad de Ingenieria U.B.A. All rights reserved.
 *
 * Primitivas de la API para la aplicacion (los vendedores) en el servidor:
 *		int iniciarAplicacion(char *, char *,int, CONTROL *);
 *  para adquirir los IPCs e inicializar la informacion de control,
 *  identificadores y port de TCP que obtiene del archivo servicios.txt.
 *		int obtenerCliente(void *, int, CONTROL *);
 *	entrega los datos del comprador que recibe de la cola de recepcion 
 *	con un identificador general 
 *		int aceptacionCliente(void *, int, CONTROL *);
 *	recibe un mensaje, agrega los datos del protocolo (longitud e identificadores)
 *	y lo envia sobre la cola de envio con  identificador = canal
 *		int compraProducto(void *, int, CONTROL *);
 *	por cada producto entrega el mensaje que recibio de la cola de recepcion 
 *		con identificador = vendedor
 *		int actualizarStock(int, int, PRODUCTO *, CONTROL *);
 *	y actualiza el stock con la cantidad y devuelve los datos completos del producto
 *		int confirmacionProducto(void *, int, CONTROL *);
 * recibe el mensaje,agrega los datos del protocolo y lo envia a la cola de envio
 * con identificador = canal.
 *		int entregarFactura(void *, int, CONTROL *);
 * recibe el mensaje,agrega los datos del protocolo y lo envia a la cola de envio
 * con identificador = canal.
 *		int cerrarVenta(CONTROL *);
 * cierra el socket y avisa para que terminen los procesos de comunicaciones 
 */

#include "TcpSocketsGral.h"
#include "semaforos.h"

typedef struct      {	/* Estado con el sistema de comunicaciones */
/* IPCs de concurrencia */
	int mutex;			/* semaforo de exclusion mutua */
	int shmid;			/* file descriptor shm */
	STOCK *shmem;       /* puntero a la shared memory */
/* IPCs y conectores con la comunicacion */	
	int recepcion;		/* file descriptor cola recepcion */
	int envio;			/* file descriptor cola envio */
	int vendedor;		/* nro de vendedor*/
	int conectorVendedor; /* conector del vendedor = nro vendedor */
	int conectorVendedorGral; 
	int conectorCliente; /* conector del cliente conectado */
	int conectorCanal;	/* conector del canal de comunicaciones con el cliente */
/* datos de la aplicacion que usa la API */	
	int pidAplicacion;
	char *aplicacion;
} CONTROL;

typedef struct		{	/* devolucion de resultados */
	int longitud;		/* longitud del mensaje */
	char mensaje[5000];	/* mensaje recibido */
} RETORNO;

int iniciarAplicacion(char *, char *,int, CONTROL *);
int aceptacionCliente(void *, int, CONTROL *);
int confirmacionProducto(void *, int, CONTROL *);
int entregarFactura(void *, int, CONTROL *);
int obtenerCliente(void *, int, CONTROL *);
int compraProducto(void *, int, CONTROL *);
int cerrarVenta(CONTROL *);
/* 
 * de concurrencia 
 */
int actualizarStock(int, int, PRODUCTO *, CONTROL *);

/*		Primitiva:
 *		Adquirir los IPC para las comunicaciones
 */
int iniciarAplicacion(char *aplicacion, char* sistema, int ident, CONTROL *control) {
	key_t clave;
	struct stat st;
	char mostrar[200];
	int pid;	
	FILE *fp;
	char nombre[]="                                     ";
	int port, vendedorInicial;
	char server[]="                                    ";
	char primeraLinea[80];
	
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
	control->vendedor = ident;
	control->conectorVendedor = vendedorInicial;
	control->conectorVendedorGral = vendedorInicial;
	control->conectorCliente=0;
	control->aplicacion = aplicacion;
	control->pidAplicacion = pid;
	/*   
	 *	acceder a la memoria compartida con los productos
	 */ 
	clave = ftok(DIRECTORIO,SHM);
	if ((control->shmid = shmget (clave,sizeof (STOCK),0660)) == -1) { 
		perror ("abrirComunicacion: error al hacer el get a la shared memory "); 
		return (-1);
	}
	if ((control->shmem = (STOCK *) shmat(control->shmid,0,0)) == (STOCK *) -1 ) {
		perror ("abrirComunicacion: error en el attach a shared memory "); 
		return (-1);
    }
	/* 
	 *	 obtener el mutex 
	 */	 
	if ((control->mutex = getsem (MUTEX)) == -1) {
			/* IPC para exclusion mutua */ 
		perror ("abrirComunicacion: error al hacer el get del (IPC) semaforo mutex"); 
		return (-1);
    }	
	/*   
	 *	acceder a la cola recepcion
	 */ 
	clave = ftok(DIRECTORIO,COLASR);
	if ((control->recepcion = msgget (clave,0660)) == -1) { 
		perror ("abrirComunicacion: error al hacer el get de la cola de recepcion "); 
		return (-1);
    }
	/*   
	 *	acceder a la cola envio
	 */ 
	clave = ftok(DIRECTORIO,COLASE);
	if ((control->envio = msgget (clave,0660)) == -1) { 
		perror ("abrirComunicacion: error al hacer el get de la cola de envio "); 
		return  (-1);
    }
	return 0;
	}

	/* Funcion interna
	 * Enviar mensaje sobre la cola enviar 
	 */
	int enviarMensaje(int tipoMsg, void *mensaje, int longitud, CONTROL *control) {
	PROTOCOLO enviar;
	char mostrar[200];
	if (longitud > sizeof(enviar.mensaje)) {
		sprintf (mostrar,"%s %d (%d) API-->enviarMensaje: la longitud del mensaje supera el maximo: %d\n", control->aplicacion, control->vendedor, control->pidAplicacion, sizeof(enviar.mensaje));
		write(fileno(stdout), mostrar, strlen(mostrar));
		return(-1);
	}
	enviar.longitud = longitud;
	enviar.tipo = control->conectorCanal;
	enviar.destino = control->conectorCliente;
	enviar.origen = control->conectorVendedor;
	enviar.tipoMsg = tipoMsg;
	memcpy (enviar.mensaje, mensaje, longitud+1);
	enviar.tipoMsg = tipoMsg;  /* tipo para la aplicacion  */
			 
	sprintf (mostrar,"%s %d (%d) API-->enviarMensaje: API--> Envia mensaje s/ cola envios Tipo: %d, Origen: %d, Destino: %d, TipoMsg: %d, Longitud: %d, Longitud total: %d\n", control->aplicacion, control->vendedor, control->pidAplicacion, enviar.tipo, enviar.origen, enviar.destino, enviar.tipoMsg, enviar.longitud, sizeof(enviar.mensaje));
	 write(fileno(stdout), mostrar, strlen(mostrar));
	 if (msgsnd(control->envio, (PROTOCOLO *)&enviar, sizeof(enviar), 0) == -1) {
		 /* datos del cliente enviada a algun vendedor */
	     perror ("API-->enviarMensaje: Error en el envio cola envios ");
		 return(-1);
	 }
	return 0;
}
				 
 /* Funcion interna
  * Enviar mensaje sobre la cola enviar 
  */
int recibirMensaje(int tipoMsg, void *resultado, int longitud, CONTROL *control ) {
	 PROTOCOLO recibir;
	 char mostrar[200];
	 long int msg_a_recibir = control->conectorVendedor; 
	 sprintf (mostrar,"%s %d (%d) API-->recibirMensaje: API: --> Espera mensaje con tipo %d\n",control->aplicacion, control->vendedor, control->pidAplicacion, msg_a_recibir);
	 write(fileno(stdout), mostrar, strlen(mostrar));
	 if (msgrcv(control->recepcion,(PROTOCOLO *)&recibir, sizeof(recibir), msg_a_recibir, 0) == -1) {
		 perror ("API:-->recibirMensaje: Error en la recepcion del mensaje ");
		 return (-1);
	 }
	  control->conectorCliente = recibir.origen;
	  control->conectorCanal = recibir.destino;
	  longitud = recibir.longitud;
	  memcpy (resultado, recibir.mensaje, recibir.longitud+1);
	  sprintf (mostrar,"%s %d (%d) API-->recibirMensaje: API: --> Recibe mensaje Tipo: %d, Origen: %d, Destino: %d, TipoMsg: %d, Longitud: %d, Longitud total: %d\n",control->aplicacion, control->vendedor, control->pidAplicacion, recibir.tipo,recibir.origen, recibir.destino, recibir.tipoMsg, longitud, sizeof(recibir));
	  write(fileno(stdout), mostrar, strlen(mostrar));
	  return 0;
  }
				 
 /* Primitiva:
  * confirmar datos del cliente  
  */
 int aceptacionCliente(void *mensaje, int longitud, CONTROL *control) {
	 int retorno;
	 char mostrar[200];
	 control->conectorVendedor = control->vendedor;   /* atiende este vendedor */
	 retorno =  enviarMensaje(ACEP, mensaje, longitud, control);
	 sprintf (mostrar,"%s %d (%d) API-->aceptacionCliente: mensaje enviado\n", control->aplicacion, control->vendedor, control->pidAplicacion);
	 write(fileno(stdout), mostrar, strlen(mostrar));
	 return retorno;
 }
				 
 /* Primitiva:
  * confirmacion del producto 
  */
int confirmacionProducto(void *mensaje, int longitud, CONTROL *control ) {
	int retorno;
	char mostrar[200];
	
	retorno =  enviarMensaje(CONF, mensaje, longitud, control);
	sprintf (mostrar,"%s %d (%d) API-->confirmacionProducto: mensaje enviado\n", control->aplicacion, control->vendedor, control->pidAplicacion);
	write(fileno(stdout), mostrar, strlen(mostrar));
	return retorno;
}
 /* Primitiva:
  * entregar factura 
  */
 int entregarFactura(void *mensaje, int longitud, CONTROL *control ) {
	 int retorno;
	 char mostrar[200];

	 retorno =  enviarMensaje(FACT, mensaje, longitud, control);
	 sprintf (mostrar,"%s %d (%d) API-->entregarFactura: mensaje enviado\n", control->aplicacion, control->vendedor, control->pidAplicacion);
	 write(fileno(stdout), mostrar, strlen(mostrar));
	 return retorno;
	 
 }				 
 /* Primitiva:
  * recibir datos del cliente  
  */
 int obtenerCliente (void *mensaje, int longitud, CONTROL *control) {
	 int retorno;
	 char mostrar[200];

	 control->conectorVendedor = control->conectorVendedorGral;  /* atiende un vendedor que esta libre */
	 retorno = recibirMensaje(PEDI, mensaje, longitud, control);
	 sprintf (mostrar,"%s %d (%d) API-->obtenerCliente: mensaje enviado\n", control->aplicacion, control->vendedor, control->pidAplicacion);
	 write(fileno(stdout), mostrar, strlen(mostrar));
	 return retorno;
	 
 }				 
 /* Primitiva:
  * pedido  del producto  
  */
 int compraProducto (void *mensaje, int longitud, CONTROL *control) {
	 int retorno;
	 char mostrar[200];

	 retorno = recibirMensaje(PROD, mensaje, longitud, control);
	 sprintf (mostrar,"%s %d (%d) API-->compraProducto: mensaje enviado\n", control->aplicacion, control->vendedor, control->pidAplicacion);
	 write(fileno(stdout), mostrar, strlen(mostrar));
	 return retorno;
	 
 }
/* Primitiva
 * Cerrar la aplicacion: Destruir las aplicaciones de comunicaciones
 */
int cerrarVenta(CONTROL *control) {
	int retorno, longitud;
	char mensaje[]="Fin venta";
	char mostrar[200];
	sprintf (mostrar,"%s %d (%d) API-->cerrarAplicacion: Eliminar canal saliente\n", control->aplicacion, control->vendedor, control->pidAplicacion);
	write(fileno(stdout), mostrar, strlen(mostrar));	
	retorno =  enviarMensaje(FINV, mensaje, strlen(mensaje), control);
	sprintf (mostrar,"%s %d (%d) API-->cerrarAplicacion: mensaje enviado para eliminar los canales\n", control->aplicacion, control->vendedor, control->pidAplicacion);
	write(fileno(stdout), mostrar, strlen(mostrar));
	return retorno;
}	
 /* Primitiva:
  * para actualizar stock, devuelve la cantidad de productos que se pueden vender  
  */				 
 int actualizarStock(int producto, int cantidad, PRODUCTO *art, CONTROL *control ) {
	 char mostrar[200];
	 STOCK *puntero;
	 int resultado, i;

	 p(control->mutex);
	 puntero=control->shmem;	
	 i=0;
	 while (i < puntero->cantProductos + 1 && puntero->prod[i].codigo != producto) 
			i++;
	
	 if (puntero->prod[i].cantidad > cantidad) {
	 	puntero->prod[i].cantidad = puntero->prod[i].cantidad - cantidad;
		resultado = cantidad;
	 } 
	 else {
		resultado = puntero->prod[i].cantidad;
		puntero->prod[i].cantidad = 0;
	 }
	 strcpy(art->denominacion, puntero->prod[i].denominacion);
	 art->precio = puntero->prod[i].precio;
	 art->codigo = puntero->prod[i].codigo;

	 art->cantidad=resultado;
	 v(control->mutex);
	 sprintf (mostrar,"%s %d (%d) API-->actualizarStock: stock actualizado\n", control->aplicacion, control->vendedor, control->pidAplicacion);
	 write(fileno(stdout), mostrar, strlen(mostrar));	 
	 return resultado;
 }
				 
 	 

				 

 

