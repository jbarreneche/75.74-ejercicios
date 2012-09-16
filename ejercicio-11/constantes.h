#ifndef CONSTANTES

#define CONSTANTES

#define FTOK_DIR "/home/knoppix/Distribuidos/ejercicio-11"

// General messages
#define RENDEZVOUS 9999

#define CANTIDAD_A_ENCONTRARSE 5

// IPCS Indexes
#define Q_ACKS  301

// COMMON
typedef struct      {	/* mensaje para la cola de recepcion/envio */
	long int tipo;		/* tipo de mensaje para la cola/receptor */
} PROTOCOLO;

#endif