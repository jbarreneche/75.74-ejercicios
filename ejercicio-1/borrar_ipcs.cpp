/*
 * Programa: TcpBorrarIPC
 *  
 *  Created by Maria Feldgen on 3/10/12.
 *  Copyright 2012 Facultad de Ingenieria U.B.A. All rights reserved.
 *
 * Borra los IPCs del servidor para terminar los programas.
 */
#include "constantes.h"
#include "lib/semaphore.h"

#include <sys/shm.h>
#include <sys/msg.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	int shmTickets; 
	int qCompras, qVentas; 
	int mtxShm;	
	key_t clave;

	clave = ftok(FTOK_DIR, SHM_TICKETS);
	shmTickets = shmget (clave, sizeof(TICKETS), 0660);
	shmctl(shmTickets,IPC_RMID,(struct shmid_ds *)  0);

	mtxShm = getsem (MUTEX_SHM);
	elisem(mtxShm);
	
	clave    = ftok(FTOK_DIR, Q_COMPRAS);
	qCompras = msgget(clave,0660);
	msgctl(qCompras, IPC_RMID, NULL);
	
	clave   = ftok(FTOK_DIR, Q_VENTAS);
	qVentas = msgget(clave,0660);
	msgctl(qVentas, IPC_RMID, NULL);
	
	return 0;
}

