/*
 * Procedimiento tcpservertrans()
 * Descripcion:	Transferencia de datos entre server y cliente
 *               Recibe pedido de compra de un cliente y devuelve una respuesta 
 * Sintaxis: tcpservertrans(sockfd, server, pid, nro)
 *	sockfd:	socket del accept
 *	server:	nombre del programa server
 *  pid:    pid del programa que atiende al cliente
 *  nro:	nro del vendedor
 * Usa:		funcion recibir()
 *			funcion enviar()
 */

#include	"inet.h"
#include   <time.h>

extern int recibir(int, void *, size_t );
extern int enviar(int, void *, size_t);

void tcpservertrans(int sockfd, char server[], pid_t pid, int nro){
	int	n;
	COMPRA item;
	char texto[]="OK";
	CONFIRMACION resp;
	char mostrar[200];
	time_t  ticks;
	int tiempo;

	srand ( time(NULL) );
	do {
		n = recibir(sockfd, &item, sizeof(item));
		if (n == 0)
		    return;		/* fin de la conexion */
		else
		    if (n < 0) {
		      sprintf(mostrar,"%s (%d) %d: error en recibir\n", server, pid, nro);
			  write(fileno(stdout), mostrar, strlen(mostrar));
			  return;
			}
		if (item.producto == 999) {
			sprintf(mostrar,"%s (%d) %d: Fin de la compra del Cliente: %d \n", server, pid, nro, item.cliente);
			write(fileno(stdout), mostrar, strlen(mostrar));
		}
		else {
			sprintf(mostrar,"%s (%d) %d: Compra recibida: Cliente: %d Producto: %d cantidad: %d comentario: %s\n", server, pid, nro, item.cliente, item.producto, item.cantidad, item.comentario);
			write(fileno(stdout), mostrar, strlen(mostrar));		
			ticks = time(NULL);
			tiempo = rand() % 10;
			sleep(tiempo); 
			/* 
			 * armar la respuesta
			 */
			resp.cliente=item.cliente;
			resp.vendedor = nro;
			resp.producto = item.producto;
			sprintf(resp.respuesta,"%.24s %s",ctime(&ticks),texto);
			sprintf(mostrar,"%s (%d) %d: Respuesta: Cliente: %d Vendedor: %d Producto: %d Respuesta: %s\n",  server, pid, nro, resp.cliente, resp.vendedor, resp.producto, resp.respuesta);
			write(fileno(stdout), mostrar, strlen(mostrar));
			/* 
			 *	  enviar respuesta
			 */
			if (enviar(sockfd, &resp, sizeof(resp)) != sizeof(resp)) {
				sprintf(mostrar,"%s (%d) %d: error en el envio \n", server, pid, nro);
				write(fileno(stdout), mostrar, strlen(mostrar));
				return;
			}
		} 
	} while (item.producto != 999);
}
