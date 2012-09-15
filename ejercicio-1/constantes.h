#ifndef CONSTANTES

#define CONSTANTES

#define FTOK_DIR "/home/knoppix/Distribuidos/ejercicio-1"

typedef struct {		/* stock de tickets */
	int cantTickets;	/* Cantidad de tickets a vender */
} TICKETS;

#define NUM_VENDEDORES 3
#define NUM_CLIENTES 3

// IPCS Indexes
#define MUTEX_SHM 101

#define SHM_TICKETS 201

#define Q_VENTAS  301
#define Q_COMPRAS 302

#endif