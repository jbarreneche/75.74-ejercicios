/*
 *  TcpEjemploVendedor.cpp
 *  
 *  Created by Maria Feldgen on 3/11/12.
 *  Copyright 2012 Facultad de Ingenieria U.B.A. All rights reserved.
 *
 *  Se lanza desde el programa TcpServerConcurrente
 *  Recibe por parametro el nro de vendedor. 
 *  Llama a las siguientes funciones de la API:
 *		int iniciarAplicacion(char *, char *,int, CONTROL *);
 *  para adquirir los IPCs e inicializar la informacion de control
 *		int obtenerCliente(void *, int, CONTROL *);
 *	recibe los datos del cliente y los verifica 
 *		int aceptacionCliente(void *, int, CONTROL *);
 *	devuelve la verificacion: aceptado = "S", rechazado = "N"
 *		int compraProducto(void *, int, CONTROL *);
 *	por cada producto que compra el comprador se verifica stock con:
 *		int actualizarStock(int, int, PRODUCTO *, CONTROL *);
 *	y actualiza con la compra cargando los datos en la factura
 *		int confirmacionProducto(void *, int, CONTROL *);
 *  devuelve la cantidad confirmada del producto comprado
 *		int entregarFactura(void *, int, CONTROL *);
 *	despues del ultimo producto, envia la factura.
 *		int cerrarVenta(CONTROL *);
 *	avisa que terminen los procesos de comunicaciones.
 *
 * Los parametros del programa son:
 *   ./TcpEjemploVendedor nroVendedor
 *
 * Usa la API: TcpEjemploServerAPI.h 
 */
#include "TcpEjemploAplicacion.h"
#include "TcpEjemploServerAPI.h"

int main(int argc, char *argv[]) {
	char aplicacion[]="TcpEjemploVendedor";
	char sistema[]="TcpEjemploVenta";
	
	CONTROL con;					/* datos que usa la API */
	CLIENTE compra;
	int i, n, conf;
	int nro;						/* nro de vendedor */
	char mostrar[200];				/* mensaje para mostrar en pantalla */
	char *pname; 
	pid_t  pid_pr;					/* pid este proceso */
	char empr[]="ALFA S.A.";
	char resp[1];
	COMPRA item;
	int cantidad;
	CONFIRMACION venta;
	FACTURA fact;
	int longitud;
	
	srand ( time(NULL) );
	pname = argv[0];		 /* nombre del programa que esta corriendo */
	pid_pr = getpid(); 		 /* pid del programa que esta corriendo */
	/*
	 * Verificar los parametros pasados en la execlp
	 */
	if (argc > 1)
		nro = atoi(argv[1]);
	else {
		sprintf (mostrar,"%s FALTA EL NRO DE VENDEDOR EN EL FORK\n", pname); 
		write(fileno(stdout), mostrar, strlen(mostrar));
		exit(1);
	}	
	sprintf (mostrar,"%s %d (%d): Comienza el vendedor\n", pname, nro, pid_pr); 
	write(fileno(stdout), mostrar, strlen(mostrar));
	
	if (iniciarAplicacion(aplicacion, sistema, nro, &con) == -1) {
		perror ("TcpEjemploVendedor: Error en la primitiva iniciarAplicacion: ");
		exit(1);
	}
	for (;;) {	
	/*
	 * Recibir datos de la empresa del comprador
	 */
	sprintf (mostrar,"%s %d (%d): Esperar por Datos de un Cliente\n", pname, nro, pid_pr); 
	write(fileno(stdout), mostrar, strlen(mostrar));
	if (obtenerCliente(&fact.clien, longitud, &con)==-1) {
		perror("TcpEjemploVendedor: Error en la primitiva obtenerCliente: ");
		cerrarVenta(&con);
		exit(1);
	}
	sprintf (mostrar,"%s %d (%d): Datos del Cliente razon social: %s cuit: %s tipo factura: %c\n", pname, nro, pid_pr, fact.clien.razonSocial, fact.clien.cuit, fact.clien.tipo); 
	write(fileno(stdout), mostrar, strlen(mostrar));
	conf = rand() % 10;
	resp[0]='S';
	if (conf > 8)
		resp[0]='N';
	sprintf (mostrar,"%s %d (%d): Aceptacion del vendedor: %c\n", pname, nro, pid_pr, resp[0]); 
	write(fileno(stdout), mostrar, strlen(mostrar));
	if (aceptacionCliente(&resp, sizeof(resp), &con)==-1) {
		perror("TcpEjemploVendedor: Error en la primitiva aceptacionCliente: ");
		cerrarVenta(&con);
		exit(1);
	}
	if (resp[0]=='S') {   /* datos aceptados, listo para vender */
		fact.cantProductos=0;
		fact.total = 0;
		strcpy(fact.empresa,empr);   /* completar el nombre de la empresa */
		do {   /* esperar por los productos */
			if (compraProducto (&item, sizeof(item), &con)== -1) {
				perror("TcpEjemploVendedor: Error en la primitiva pedirProducto: ");
				cerrarVenta(&con);
				exit(1);
			}			
			sprintf (mostrar,"%s %d (%d): Compra del cliente: producto: %d cantidad: %d mas?: %d\n", pname, nro, pid_pr, item.producto, item.cantidad, item.mas); 
			write(fileno(stdout), mostrar, strlen(mostrar));	
				/*
				 * preguntar si hay stock suficiente 
				 */
			cantidad = actualizarStock(item.producto, item.cantidad, &fact.prod[fact.cantProductos], &con);
			sprintf (mostrar,"%s %d (%d): Se vende producto: %d cantidad pedida: %d cantidad vendida: %d\n", pname, nro, pid_pr, item.producto, item.cantidad, cantidad); 
			write(fileno(stdout), mostrar, strlen(mostrar));
			
			fact.total+= fact.prod[fact.cantProductos].precio * fact.prod[fact.cantProductos].cantidad;
			venta.producto = fact.prod[fact.cantProductos].codigo;
			venta.cantidad = fact.prod[fact.cantProductos].cantidad;
			if (venta.cantidad > 0) { /* hay stock se agrega a la factura */
				fact.cantProductos++;
			}
			sprintf (mostrar,"%s %d (%d): Total Parcial de la factura %6.2f\n", pname, nro, pid_pr, fact.total); 
			write(fileno(stdout), mostrar, strlen(mostrar));
			if (confirmacionProducto(&venta, sizeof(venta), &con)== -1) {
					perror("TcpEjemploVendedor: Error en la primitiva confirmacionProducto: ");
					cerrarVenta(&con);
					exit (1);
				}			
		} while(item.mas == 1);
		
		/* 
		 * enviar la factura 
		 */
		/* calcular el iva y el total neto */
		fact.iva = fact.total * 21 / 100;
		fact.neto = fact.total + fact.iva;
		if (fact.clien.tipo != 'A') {
			fact.total = 0;
			fact.iva = 0;
		}
		sprintf(mostrar,"%s %d (%d): Factura a enviar: Empresa: %s Encabezado: %s %s %c\n", pname, nro, pid_pr,
					fact.empresa, fact.clien.razonSocial, fact.clien.cuit, fact.clien.tipo);
		write(fileno(stdout), mostrar, strlen(mostrar));
		for (int j=0; j< fact.cantProductos; j++) {
			sprintf(mostrar,"%s %d (%d):           Producto: %d %s precio: %4.2f cantidad: %d\n", pname, nro, pid_pr, 
						fact.prod[j].codigo, fact.prod[j].denominacion, fact.prod[j].precio, fact.prod[j].cantidad);
			write(fileno(stdout), mostrar, strlen(mostrar));		   
		}
		if (fact.clien.tipo == 'A')
			sprintf(mostrar,"%s %d (%d):      Totales: bruto: %6.2f iva: %4.2f neto a pagar: %6.2f\n", pname, nro, pid_pr, 
							   fact.total, fact.iva, fact.neto);
		else
			sprintf(mostrar,"%s %d (%d):      Totales: Neto a pagar: %6.2f\n", pname, nro, pid_pr, fact.neto);			
		write(fileno(stdout), mostrar, strlen(mostrar));
		if (entregarFactura (&fact, sizeof(fact), &con)==-1) {
			perror("TcpEjemploVendedor: Error en la primitiva entregarFactura: "); 
			exit(1);
		}
	}
	/*
	 *	fin de la compra, el vendedor termina
	 */
	sprintf (mostrar,"%s %d (%d): TERMINA de atender un cliente\n", pname, nro, pid_pr);
	write(fileno(stdout), mostrar, strlen(mostrar));
	cerrarVenta(&con);
  }
    exit(0);
}



