/*
 * Definiciones para programas client-server para TCP y UDP.
 */

#include	<stdio.h>
#include   <stdlib.h>
#include   <unistd.h>
#include	<signal.h>
#include	<sys/wait.h>
#include   <sys/resource.h>
#include 	<string.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<netdb.h>

#define	SERV_UDP_PORT	5000
#define	PORT_FIJO	 5000		/* port por defecto */

typedef struct {
	int cliente;
	int producto;
	int cantidad;
	char comentario[100];
}COMPRA;

typedef struct {
	int cliente;
	int vendedor;
	int producto;
	char respuesta[80];
} CONFIRMACION;



