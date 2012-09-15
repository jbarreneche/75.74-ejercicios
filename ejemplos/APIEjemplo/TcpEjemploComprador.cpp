/*
 *  TcpEjemploComprador.cpp
 *  
 *  Created by Maria Feldgen on 3/11/12.
 *  Copyright 2012 Facultad de Ingenieria U.B.A. All rights reserved.
 *
 *  El programa usa las siguientes funciones de la API:
 * 
 *		int iniciarAplicacion(char *,char *,int, CONTROL *); 
 *	para inicializar la informacion de control y las comunicaciones 
 *		int enviarDatosCliente(void *, int, void *, int, CONTROL *);
 *	arma los datos de la empresa del comprador con nombre de la empresa, cuit 
 *	y tipo de factura
 *	Recibe la respuesta de aceptacion del vendedor (S: aceptado, N: rechazado)
 *		int pedirProducto(void *, int, void *, int, CONTROL *);
 *	arma los datos de cada producto: codigo y cantidad, e indicador si hay mas productos, y 
 *  espera la confirmacion de la cantidad por parte del vendedor
 *		int recibirFactura (void *, int, CONTROL *);
 * 	recibe la factura de la compra del vendedor 
 *		int cerrarAplicacion(CONTROL *);
 *	indicar que el programa termina, devolver los recursos y cerrar comunicaciones.
 *
 *  Termina
 *
 * Los parametros del programa son:
 *   ./ClienteTickets nroCliente
 */
#include "TcpEjemploAplicacion.h"
#include "TcpEjemploClienteAPI.h"

int main(int argc, char *argv[]) {
	char aplicacion[]="TcpEjemploComprador";
	char sistema[]="TcpEjemploVenta";

	CONTROL con;					/* Informacion de control de la API */
	CLIENTE compra;
	int i, j, n, nro, total;
	char mostrar[200];				/* mensaje para mostrar en pantalla */
	char *pname;
	pid_t  pid_pr;					/* pid este proceso */
	FILE* fp;	
	char nombre[]="TcpEjemploProductos";  /* contiene los productos a comprar */
	char archivo[30];				/* archivo */
	char primeraLinea[80];
	char resp[5000];
	
	COMPRA item;	
	CONFIRMACION prod;
	FACTURA fact;
	int longitud;
	char tipoCompra;
	
	pname = argv[0];		 /* nombre del programa que esta corriendo */
	pid_pr = getpid(); 		 /* pid del programa que esta corriendo */
	/*
	 * Verificar los parametros pasados en la execlp
	 */
	if (argc > 1)
		nro = atoi(argv[1]);
	else
	{   sprintf (mostrar,"%s FALTA EL NRO DE CLIENTE EN EL FORK\n", aplicacion); 
		write(fileno(stdout), mostrar, strlen(mostrar));
		exit(1);
	}	
	sprintf (mostrar,"%s %d (%d): Comienza el comprador\n", aplicacion, nro, pid_pr); 
	write(fileno(stdout), mostrar, strlen(mostrar));

	n=iniciarAplicacion(aplicacion, sistema, nro, &con);
	if (n == -1){
		sprintf (mostrar,"%s %d (%d): Error en abrirComunicacion, termina\n", aplicacion, nro, pid_pr); 
		write(fileno(stdout), mostrar, strlen(mostrar));
		cerrarAplicacion(&con);
		exit(1);
	}
/*
 * obtener del archivo los productos para comprar 
 */
	sprintf(archivo,"%s%d",nombre, nro);
	if( (fp=fopen(archivo,"r") ) == NULL) {
		sprintf (mostrar,"%s %d (%d): NO se puede abrir el archivo de productos: %s\n", aplicacion, nro, pid_pr, archivo); 
		write(fileno(stdout), mostrar, strlen(mostrar));
		cerrarAplicacion(&con);
		exit(1);
	}
	else{
		fgets(primeraLinea,80,fp);			/* Ignorar la primera linea. */
		/*
		 * Enviar datos de la empresa del comprador al vendedor
		 */
		fscanf(fp,"%s %s %c %d",				
			   (compra.razonSocial),
			   (compra.cuit),
			   &(compra.tipo),
			   &(total));
		tipoCompra = compra.tipo;
		sprintf (mostrar,"%s %d (%d): Datos del comprador: Razon social: %s, Cuit: %s, Tipo factura: %c\n", aplicacion, nro, pid_pr, compra.razonSocial, compra.cuit, compra.tipo); 
		write(fileno(stdout), mostrar, strlen(mostrar));
		if (enviarDatosCliente(&compra, sizeof(compra), resp, longitud, &con)==-1) {
			perror("TcpEjemploComprador: Error en la primitiva enviarDatosCliente: "); 
			cerrarAplicacion(&con);
			exit(1);
		}
		sprintf (mostrar,"%s %d (%d): Aceptacion del vendedor: %c\n", aplicacion, nro, pid_pr, resp[0]); 
		write(fileno(stdout), mostrar, strlen(mostrar));
		
		if (resp[0] =='S') {   /* datos aceptados, listo para comprar */
			sprintf (mostrar,"%s %d (%d): Comprar %d productos\n", aplicacion, nro, pid_pr, total); 
			write(fileno(stdout), mostrar, strlen(mostrar));
			
			for ( j=0; j<total; j++) {
				//while(!feof(fp) ){   /* transferir todo el archivo */
				
				if (fscanf(fp,"%d %d",
				   &(item.producto),
				   &(item.cantidad)) == -1) {
					perror("TcpEjemploComprador: Error en el fscanf ");
					cerrarAplicacion(&con);
					exit(1);
				}
				if ( j < total-1)
				   item.mas = 1;
				else
				   item.mas = 0;
				/*
				 * preguntar si hay stock suficiente 
				 */
				sprintf (mostrar,"%s %d (%d): Preguntar si hay stock para el Item Nro: %d, Producto con codigo: %d, Cantidad: %d, Marca hay mas: %d\n", aplicacion, nro, pid_pr, j+1, item.producto, item.cantidad, item.mas); 
				write(fileno(stdout), mostrar, strlen(mostrar));
				
				if (pedirProducto (&item, sizeof(item), &prod, longitud, &con)== -1) {
					perror("TcpEjemploComprador: Error en la primitiva pedirProducto: "); 
					cerrarAplicacion(&con);
					exit(1);
				}			
				sprintf (mostrar,"%s %d (%d): Respuesta del vendedor: Producto codigo: %d, Cantidad: %d\n", aplicacion, nro, pid_pr, prod.producto, prod.cantidad); 
				write(fileno(stdout), mostrar, strlen(mostrar));				
			}
			/* 
			 * esperar por la factura 
			 */
			sprintf(mostrar,"%s (%d) %d: Esperando la factura\n", aplicacion, pid_pr, nro);
			write(fileno(stdout), mostrar, strlen(mostrar));
		
			if (recibirFactura (&fact, longitud, &con)==-1) {
				perror("TcpEjemploComprador: Error en la primitiva pedirProducto: "); 
				cerrarAplicacion(&con);
				exit(1);
			   }				   
			sprintf(mostrar,"%s (%d) %d: Factura recibida: Empresa: %s Encabezado: %s %s %c\n", aplicacion, pid_pr, nro,
					fact.empresa, fact.clien.razonSocial, fact.clien.cuit, fact.clien.tipo);
			write(fileno(stdout), mostrar, strlen(mostrar));
			for (int j=0; j< fact.cantProductos; j++) {
				sprintf(mostrar,"%s (%d) %d:           Codigo de Producto: %d %s, Precio: %4.2f, Cantidad: %d\n", aplicacion, pid_pr, nro, fact.prod[j].codigo, fact.prod[j].denominacion, fact.prod[j].precio, fact.prod[j].cantidad);
				write(fileno(stdout), mostrar, strlen(mostrar));		   
			}
			if (tipoCompra =='A') 
			   sprintf(mostrar,"%s (%d) %d:      Totales: Bruto: %6.2f, Iva: %4.2f, Neto a pagar: %6.2f\n", aplicacion, pid_pr, nro,
							   fact.total, fact.iva, fact.neto);
			else 
			  sprintf(mostrar,"%s (%d) %d:       Totales: Neto a pagar: %6.2f\n", aplicacion, pid_pr, nro, fact.neto);
			write(fileno(stdout), mostrar, strlen(mostrar));
		}
   }
		/*
		 *	fin de la compra, el cliente termina
		 */
	cerrarAplicacion(&con);
	sprintf (mostrar,"%s %d (%d): TERMINA\n", aplicacion, nro, pid_pr);
	write(fileno(stdout), mostrar, strlen(mostrar));
	
    exit(0);
}


