/*
 * TcpSocketsGral.h
 *  
 *  Created by Maria Feldgen on 3/11/12.
 *  Copyright 2012 Facultad de Ingenieria U.B.A. All rights reserved.
 *
 * Definiciones para los programas de comunicaciones cliente/servidor usando TCP.
 */
#include <sys/resource.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#define	SERV_UDP_PORT	5000
#define	PORT_FIJO	 5000		/* port por defecto */
/* 
 * SHM; memoria compartida tipo STOCK
 * MUTEX: semaforo mutex para la memoria compartida
 * COLASR y COLASE: colas recepcion y envio del servidor TCP
 * COLACR y COLACE: colas recepcion y envio de los clientes TCP
 */
#define CONTROLS 300
#define SHM 201
#define MUTEX 202
#define COLASR 203
#define COLASE 204

#define CONTROLC 400
#define COLACR 101
#define COLACE 102

#define ARCHIVO "servicios.txt"

typedef struct      {	/* mensaje para la cola de recepcion/envio */
	long int tipo;		/* tipo de mensaje para la cola/receptor */
	int origen;			/* conector cliente o vendedor */
	int destino;		/* conector del cliente o vendedor o canal de comunicaciones */
	int tipoMsg;		/* tipoMsg para la API */ 
	int longitud;		/* longitud del mensaje */
	char mensaje[5000];	/* mensaje del/para la aplicacion */
} PROTOCOLO;




