#ifndef CONSTANTES

#define CONSTANTES

#define FTOK_DIR "/home/knoppix/Distribuidos/ejercicio-10"

typedef struct {		/* stock de tickets */
	int enLugarDeEncuentro;	/* Cantidad de tickets a vender */
} CONTROL;

// General messages

#define CANTIDAD_A_ENCONTRARSE 5
#define SEM_MUTEX_OFFSET 0
#define SEM_RENDEZVOUS_OFFSET 1
#define SEM_CANT_EXTRA 2

// IPCS Indexes
#define SEM_SET 101

#define SHM_CONTROL 201

// COMMON

#endif