/*
 * Procedimiento tcpclienttrans():
 * Descripcion: transferencia de datos entre cliente y servidor:
 *	Repetir hasta que fin del archivo  
 *		se envia un pedido de compra al vendedor (servidor)
 *		se espera por la respuesta del vendedor 
 *	Retorno.
 * Sintaxis: tcpclienttrans(int socketfd, char *cliente, pid_t pid_cl, int nro)
 *	socketfd = puntero al socket conectado con el server
 *	cliente = nombre del programa cliente
 *  pid_cl = pid del cliente
 *  nro = nro del cliente
 * Usa:		funcion enviar()
 *			funcion recibir()
 */

#include	"inet.h"

extern int recibir(int, void *, size_t );
extern int enviar(int, void *, size_t);

void tcpclienttrans (int sockfd, char *cliente, pid_t pid_cl, int nro)
{
	size_t	n;
	int	i;
    char nombre[]="TcpRPProd";  /* contiene los productos a comprar */
	char archivo[15];
	FILE* 	fp;					/* archivo y estructura de los montos */
	char	primeraLinea[80];
	char mostrar[180];			/* mensaje para la pantalla */
	COMPRA item;
	char texto[20];
	CONFIRMACION resp;
	/*
	 * obtener del archivo los productos para comprar 
	 */
	sprintf(archivo,"%s%d",nombre, nro);
	if( (fp=fopen(archivo,"r") ) == NULL)
	{   sprintf (mostrar,"%s %d (%d): NO se puede abrir el archivo de productos: %s\n", cliente, nro, pid_cl, archivo); 
		write(fileno(stdout), mostrar, strlen(mostrar));
		exit(1);
	}
	else{
		fgets(primeraLinea,80,fp);			/* Ignorar la primera linea. */
		while(!feof(fp) ){   /* transferir todo el archivo */
			item.cliente = nro;
			fscanf(fp,"%d %d %s",
				   &(item.producto),
				   &(item.cantidad),
				   (item.comentario));
		 /* 
		  * enviar cada pedido al vendedor 
		  */
			sprintf(mostrar,"%s (%d) %d: Pedido Producto: %d, cantidad: %d comentario: %s\n", cliente, pid_cl, nro,
					item.producto, item.cantidad, item.comentario);
			write(fileno(stdout), mostrar, strlen(mostrar));
			
			if (enviar(sockfd, &item, sizeof(item)) != sizeof(item))   {
				sprintf(mostrar,"%s (%d) %d: error en envio sobre el socket\n", cliente, pid_cl, nro);
				write(fileno(stdout), mostrar, strlen(mostrar));
				exit (1);
			   }
		 /*
		  *	recibir del server
		  *	mostrar el mensaje en pantalla (stdout)
		  */
			
			n = recibir(sockfd, &resp, sizeof(resp));
			if (n < 0){
				sprintf(mostrar,"%s (%d) %d: error en recibir\n", cliente, pid_cl, nro);
				write(fileno(stdout), mostrar, strlen(mostrar));
				}
			else {
				sprintf(mostrar,"%s (%d) %d: --> Respuesta: Cliente: %d Vendedor: %d Producto %d respuesta: %s\n", cliente, pid_cl, nro, resp.cliente, resp.vendedor, resp.producto, resp.respuesta);
				write(fileno(stdout), mostrar, strlen(mostrar));
				}
		   }
		   fclose(fp);
		   item.producto = 999;
		   sprintf(mostrar,"%s (%d) %d: Avisando que finalizo la compra\n", cliente, pid_cl, nro);
		   write(fileno(stdout), mostrar, strlen(mostrar));
			if (enviar(sockfd, &item, sizeof(item)) != sizeof(item))   {
				sprintf(mostrar,"%s (%d) %d: error en envio sobre el socket\n", cliente, pid_cl, nro);
				write(fileno(stdout), mostrar, strlen(mostrar));
				exit (1);
			}
		   }
	return;
	}
