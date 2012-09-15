/*
 * Funcion TcpEnviar()
 *	Escribe n bytes sobre un descriptor:
 *		si no se escribieron n bytes
 *			repetir hasta que se hayan escrito los n bytes
 *	(Se debe usar esta funcion cuando el descriptor es un stream socket 
 * Sintaxis: int enviar(sockfd, void *datos, size_t nbytes)
             sockfd: descriptor del socket a usar .
	    datos: datos a enviar.
	    nbytes: cantidad datos a enviar (en bytes).
 */
#include	"../TcpSocketsGral.h"

int TcpEnviar(int sockfd, void *datos, size_t nbytes)
{
	/* Variables. */
	char mostrar[80];
	int enviados;			/* Cantidad de bytes enviados. */
	int ult_envio;			/* Cantidad de bytes enviados en el
					   ultimo envio. */
	/* Envia los datos. */
	enviados = 0;
	
	 sprintf(mostrar," envio, bytes a enviar %d sobre el socket %d\n", nbytes, sockfd);
	 write(fileno(stdout), mostrar, strlen(mostrar));
	 
	while(nbytes)
	{
		ult_envio = write(sockfd, ((char *) datos) + enviados,
				nbytes);
		if (ult_envio <= 0) {
			perror("TcpEnviar: Error en el write sobre el socket ");
			return enviados;    /* error */
		}
		enviados += ult_envio;
		nbytes -= ult_envio;
	}

	/* Devuelve la cantidad de bytes enviados. */
	
	 sprintf(mostrar," envio, bytes enviados %d\n", enviados);
	 write(fileno(stdout), mostrar, strlen(mostrar));
	 

	return enviados;
}
