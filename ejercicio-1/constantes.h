#ifndef CONSTANTES

#define CONSTANTES

#define FTOK_DIR "/home/knoppix/Distribuidos/ejercicio-1"

typedef struct {		/* stock de tickets */
	int cantTickets;	/* Cantidad de tickets a vender */
} TICKETS;

typedef struct {
	int monto;
	bool mas;
} COMPRA;

typedef struct {
	bool noHayMas;
	int numero;
	int vuelto;
} TICKET;

// General messages
#define SOLICITAR_ATENCION 9999

#define NUM_VENDEDORES 3
#define NUM_CLIENTES 3

// IPCS Indexes
#define MUTEX_SHM 101

#define SHM_TICKETS 201

#define Q_VENTAS  301
#define Q_COMPRAS 302

// COMMON
typedef struct      {	/* mensaje para la cola de recepcion/envio */
	long int tipo;		/* tipo de mensaje para la cola/receptor */
	int origen;			/* conector cliente o vendedor */
	char mensaje[5000];	/* mensaje del/para la aplicacion */
} PROTOCOLO;

#endif